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

#define WSN_CONF_DEFAULT_CONF_FILE "wsn.conf"
#define WSN_CONF_DEFAULT_CONN_PROTOCOL 0

enum {
  WSN_CONN_PROTOCOL_WS = 0,
  WSN_CONN_PROTOCOL_WSS
};

typedef struct {
  char *host;
  unsigned short port;
  int conn_protocol;
} wsn_server_conf_t;

typedef struct {
  char *conf_file;
  int server_count;
  wsn_server_conf_t *servers_conf;
} wsn_all_configs_t;

WSN_EXPORT int wsn_server_conf_init(wsn_server_conf_t *server_conf,
                                    char *host, unsigned short port, int conn_protocol);
WSN_EXPORT void wsn_server_conf_cleanup(wsn_server_conf_t *server_conf);

WSN_EXPORT int wsn_conn_protocol_from_str(const char *protocol);

wsn_server_conf_t* wsn_servers_conf_find(int *server_count, yajl_val js_configs);

#endif // _WSN_INCL_CONFIGS_H