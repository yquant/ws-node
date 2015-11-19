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

#ifndef _WSN_INCL_SERVER_H
#define _WSN_INCL_SERVER_H

#include "wsn/defs.h"
#include "wsn/configs.h"

struct wsn_server_ctx;

typedef struct {
  wsn_handles_t io_handle;
  struct sockaddr addr;
  struct wsn_server_ctx *server;
} wsn_server_listen_ctx_t;

typedef struct wsn_server_ctx {
  uv_getaddrinfo_t getaddrinfo_req;
  wsn_node_conf_t *conf;
  wsn_server_listen_ctx_t *listen_ctxs;
  int listen_ctx_count;
  int idle_timeout;
  uv_loop_t *loop;
} wsn_server_ctx_t;

WSN_EXPORT int wsn_server_listen_ctx_init(wsn_server_listen_ctx_t *listen_ctx,
                                          wsn_server_ctx_t *server, struct sockaddr *addr);
WSN_EXPORT void wsn_server_listen_ctx_cleanup(wsn_server_listen_ctx_t *listen_ctx);
WSN_EXPORT int wsn_server_init(wsn_server_ctx_t *server, wsn_node_conf_t *conf, uv_loop_t *loop);
WSN_EXPORT int wsn_server_start(wsn_server_ctx_t *server);
WSN_EXPORT void wsn_server_cleanup(wsn_server_ctx_t *server);

#endif // _WSN_INCL_SERVER_H