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

#ifndef _WSN_INCL_CONN_H
#define _WSN_INCL_CONN_H

#include "wsn/defs.h"
#include "wsn/server.h"

typedef enum {
  WSN_CONN_DIRECTION_IN = 0,
  WSN_CONN_DIRECTION_OUT = 1
} wsn_conn_dir_t;

typedef struct {
  uv_tcp_t tcp_handle;
  wsn_server_listen_ctx_t *listen_ctx;
  int direction;
} wsn_conn_ctx_t;

WSN_EXPORT wsn_conn_ctx_t* wsn_conn_create(wsn_server_listen_ctx_t *listen_ctx, int direction);
WSN_EXPORT void wsn_conn_processing(wsn_conn_ctx_t *conn);

#endif // _WSN_INCL_CONN_H