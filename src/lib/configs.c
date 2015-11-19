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

static char *prog_name_ = WSN_CONF_DEFAULT_PROG_NAME;

void wsn_set_prog_name(char *name)
{
  prog_name_ = name;
}

char* wsn_prog_name()
{
  return prog_name_;
}

int wsn_node_conf_init(wsn_node_conf_t *node_conf, int type,
                       char *host, unsigned short port, int conn_protocol)
{
  node_conf->type = type;
  if (host == NULL) {
    if (conn_protocol == WSN_CONN_PROTOCOL_RAW) {
      node_conf->host = wsn_strcat(WSN_CONF_PIPENAME_PREFIX, wsn_prog_name());
    } else {
      node_conf->host = wsn_strdup(WSN_CONF_DEFAULT_HOST);
    }
  } else {
    if (conn_protocol == WSN_CONN_PROTOCOL_RAW) {
      if (host[0] != '/' && host[0] != '\\') {
        node_conf->host = wsn_strcat(WSN_CONF_PIPENAME_PREFIX, host);
        free(host);
      } else {
        node_conf->host = host;
      }
    } else {
      node_conf->host = host;
    }
  }
  node_conf->port = port;
  node_conf->conn_protocol = conn_protocol;
  return 0;
}

void wsn_node_conf_cleanup(wsn_node_conf_t *node_conf)
{
  if (node_conf->host) {
    free(node_conf->host);
  }
}

int wsn_conn_protocol_from_str(const char *protocol)
{
  if (strcmp(protocol, "raw") == 0 || strcmp(protocol, "RAW") == 0) {
    return WSN_CONN_PROTOCOL_RAW;
  }
  if (strcmp(protocol, "tcp") == 0 || strcmp(protocol, "TCP") == 0) {
    return WSN_CONN_PROTOCOL_TCP;
  }
  if (strcmp(protocol, "ws") == 0 || strcmp(protocol, "WS") == 0) {
    return WSN_CONN_PROTOCOL_WS;
  }
  if (strcmp(protocol, "wss") == 0 || strcmp(protocol, "WSS") == 0) {
    return WSN_CONN_PROTOCOL_WSS;
  }
  return -1;
}

static void cleanup_initialized_node_conf_(wsn_node_conf_t* nodes_conf, int count)
{
  for (int i = 0; i < count; i++) {
    wsn_node_conf_cleanup(&nodes_conf[i]);
  }
  free(nodes_conf);
}

wsn_node_conf_t* wsn_nodes_conf_find(int type, int *node_count, yajl_val js_configs)
{
  const char *key = (type == WSN_NODE_TYPE_SERVER ? "servers" : "clients");
  yajl_val js_nodes = wsn_yajl_tree_get(js_configs, key, yajl_t_array);
  if (js_nodes == NULL) {
    wsn_report_err(WSN_ERR_MISSING_CONF_ITEM,
                   "Invalid config file, no %s node config was found", key);
    return NULL;
  }
  
  int count = (int)js_nodes->u.array.len;
  if (count == 0) {
    wsn_report_err(WSN_ERR_INVALID_CONF_FILE,
                   "Invalid config file, %s node config is empty", key);
    return NULL;
  }
  
  wsn_node_conf_t* nodes_conf = (wsn_node_conf_t*)malloc(sizeof(*nodes_conf) * count);
  if (nodes_conf == NULL) {
    wsn_report_err(WSN_ERR_MALLOC, "Malloc %s node config failed", key);
    return NULL;
  }

  for (int i = 0; i < count; i++) {
    yajl_val js_node = js_nodes->u.array.values[i];
    yajl_val js_conn_protocol = wsn_yajl_tree_get(js_node, "connection-protocol", yajl_t_string);
    yajl_val js_host = wsn_yajl_tree_get(js_node, "host", yajl_t_string);
    yajl_val js_port = wsn_yajl_tree_get(js_node, "port", yajl_t_number);
    if (js_conn_protocol == NULL) {
      cleanup_initialized_node_conf_(nodes_conf, i);
      wsn_report_err(WSN_ERR_INVALID_CONF_FILE,
                     "Invalid config file, no connection-protocol config for %s node[%d]", key, i);
      return NULL;
    }
    int conn_protocol = wsn_conn_protocol_from_str(YAJL_GET_STRING(js_conn_protocol));
    if (conn_protocol == -1) {
      cleanup_initialized_node_conf_(nodes_conf, i);
      wsn_report_err(WSN_ERR_INVALID_CONF_FILE,
                     "Invalid config file, invalid connection-protocol config for %s node[%d]", key, i);
    }
    unsigned short port = 0;
    if (js_port == NULL) {
      if (conn_protocol == WSN_CONN_PROTOCOL_TCP) {
        cleanup_initialized_node_conf_(nodes_conf, i);
        wsn_report_err(WSN_ERR_INVALID_CONF_FILE,
                       "Invalid config file, no port config for tcp %s node[%d]", key, i);
        return NULL;
      } else if (conn_protocol == WSN_CONN_PROTOCOL_WS) {
        port = 80;
      } else if (conn_protocol == WSN_CONN_PROTOCOL_WSS) {
        port = 443;
      } 
    } else {
      port = (unsigned short)YAJL_GET_INTEGER(js_port);
    }
    char *host = NULL;
    if (js_host) {
      host = wsn_strdup(YAJL_GET_STRING(js_host));
    }
    wsn_node_conf_init(&nodes_conf[i], type, host, port, conn_protocol);
  }
  *node_count = count;
  return nodes_conf;
}
