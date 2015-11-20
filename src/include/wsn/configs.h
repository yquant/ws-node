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

#ifndef _WSN_INCL_CONFIGS_H
#define _WSN_INCL_CONFIGS_H

#include "yajl/yajl_tree.h"

#include "wsn/defs.h"

#define WSN_CONF_DEFAULT_PROG_NAME "wsn-hub"

#define WSN_CONF_DEFAULT_CONF_FILE "wsn.conf"
#define WSN_CONF_DEFAULT_CONNECT_TIMEOUT 5000
#define WSN_CONF_DEFAULT_IDLE_TIMEOUT 3000
#define WSN_CONF_DEFAULT_HOST "127.0.0.1"

#ifdef _WIN32
  #define WSN_CONF_PIPENAME_PREFIX "\\\\?\\pipe\\"
#else
  #define WSN_CONF_PIPENAME_PREFIX "/tmp/"
#endif

enum {
  WSN_CONN_PROTOCOL_RAW = 0,
  WSN_CONN_PROTOCOL_TCP,
  WSN_CONN_PROTOCOL_WS,
  WSN_CONN_PROTOCOL_WSS,
};

enum {
  WSN_NODE_TYPE_SERVER = 0,
  WSN_NODE_TYPE_CLIENT = 1,
};

typedef struct {
  int type;
  char *host;
  unsigned short port;
  int conn_protocol;
} wsn_node_conf_t;

typedef struct {
  char *conf_file;
  int connect_timeout;
  int idle_timeout;
  int server_count;
  wsn_node_conf_t *servers_conf;
  int client_count;
  wsn_node_conf_t *clients_conf;
} wsn_all_configs_t;

#ifdef __cplusplus
extern "C" {
#endif

WSN_EXPORT void wsn_set_prog_name(char *name);
WSN_EXPORT char* wsn_prog_name();

WSN_EXPORT int wsn_node_conf_init(wsn_node_conf_t *node_conf, int type,
                                  char *host, unsigned short port, int conn_protocol);
WSN_EXPORT void wsn_node_conf_cleanup(wsn_node_conf_t *node_conf);

WSN_EXPORT int wsn_conn_protocol_from_str(const char *protocol);

wsn_node_conf_t* wsn_nodes_conf_find(int type, int *node_count, yajl_val js_configs);

#ifdef __cplusplus
}
#endif

#endif // _WSN_INCL_CONFIGS_H