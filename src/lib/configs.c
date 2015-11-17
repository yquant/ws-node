/*
 * ws-node - C library to support lightweight websocket based service node
 *
 * Copyright (C) 2015 yquant.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdlib.h>
#include <string.h>

#include "wsn/utils.h"
#include "wsn/errors.h"
#include "wsn/configs.h"

int wsn_server_conf_init(wsn_server_conf_t *server_conf,
                         char *host, unsigned short port, int conn_protocol)
{
  server_conf->host = host;
  server_conf->port = port;
  server_conf->conn_protocol = conn_protocol;
  return 0;
}

void wsn_server_conf_cleanup(wsn_server_conf_t *server_conf)
{
  if (server_conf->host) {
    free(server_conf->host);
  }
}

int wsn_conn_protocol_from_str(const char *protocol)
{
  if (strcmp(protocol, "wss") == 0) {
    return WSN_CONN_PROTOCOL_WSS;
  }
  return WSN_CONF_DEFAULT_CONN_PROTOCOL;
}

wsn_server_conf_t* wsn_servers_conf_find(int *server_count, yajl_val js_configs)
{
  yajl_val js_servers = wsn_yajl_tree_get(js_configs, "servers", yajl_t_array);
  if (js_servers == NULL) {
    wsn_report_err(WSN_ERR_INVALID_CONF_FILE, "Invalid config file, no servers config was found");
    return NULL;
  }
  
  int count = (int)js_servers->u.array.len;
  if (count == 0) {
    wsn_report_err(WSN_ERR_INVALID_CONF_FILE, "Invalid config file, no server config was found");
    return NULL;
  }
  
  wsn_server_conf_t* servers_conf = (wsn_server_conf_t*)malloc(sizeof(*servers_conf) * count);
  if (servers_conf == NULL) {
    wsn_report_err(WSN_ERR_MALLOC, "Malloc servers conf failed");
    return NULL;
  }

  for (int i = 0; i < count; i++) {
    yajl_val js_server = js_servers->u.array.values[i];
    yajl_val js_host = wsn_yajl_tree_get(js_server, "host", yajl_t_string);
    yajl_val js_port = wsn_yajl_tree_get(js_server, "port", yajl_t_number);
    yajl_val js_conn_protocol = wsn_yajl_tree_get(js_server, "connection_protocol", yajl_t_string);
    if (js_port == NULL) {
      for (int j = 0; j < i; j++) {
        wsn_server_conf_cleanup(&servers_conf[j]);
      }
      free(servers_conf);
      wsn_report_err(WSN_ERR_INVALID_CONF_FILE, "Invalid config file, no port config for server[%d]", i);
      return NULL;
    }
    unsigned short port = (unsigned short)YAJL_GET_INTEGER(js_port);
    char *host = NULL;
    if (js_host) {
      host = wsn_strdup(YAJL_GET_STRING(js_host));
    }
    int conn_protocol = WSN_CONF_DEFAULT_CONN_PROTOCOL;
    if (js_conn_protocol) {
      conn_protocol = wsn_conn_protocol_from_str(YAJL_GET_STRING(js_conn_protocol));
    }
    wsn_server_conf_init(&servers_conf[i], host, port, conn_protocol);
  }
  *server_count = count;
  return servers_conf;
}
