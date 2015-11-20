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

#ifndef _WSN_INCL_CLIENT_H
#define _WSN_INCL_CLIENT_H

#include "wsn/defs.h"
#include "wsn/configs.h"

struct wsn_conn_ctx;

typedef struct {
  uv_getaddrinfo_t getaddrinfo_req;
  uv_connect_t connect_req;
  wsn_node_conf_t *conf;
  struct wsn_conn_ctx *conn;
  struct sockaddr host_addr;
  int connect_timeout;
  int idle_timeout;
  uv_timer_t timer_handle;
  uv_loop_t *loop;
} wsn_client_ctx_t;

#ifdef __cplusplus
extern "C" {
#endif

WSN_EXPORT int wsn_client_init(wsn_client_ctx_t *client, wsn_node_conf_t *conf, uv_loop_t *loop);
WSN_EXPORT int wsn_client_start(wsn_client_ctx_t *client);
WSN_EXPORT void wsn_client_cleanup(wsn_client_ctx_t *client);

#ifdef __cplusplus
}
#endif

#endif // _WSN_INCL_CLIENT_H