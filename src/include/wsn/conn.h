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
#include "wsn/configs.h"

typedef enum {
  WSN_CONN_DIRECTION_IN = 0,
  WSN_CONN_DIRECTION_OUT = 1
} wsn_conn_dir_t;

typedef enum {
  WSN_CONN_STATE_CREATED = 0,
  WSN_CONN_ACTIVE,
  WSN_CONN_STATE_CLOSING,
  WSN_CONN_STATE_CLOSED
} wsn_conn_state_t;

struct wsn_conn_ctx;

typedef void (*wsn_conn_close_cb_t)(struct wsn_conn_ctx *conn);

typedef struct wsn_conn_ctx {
  wsn_handles_t io_handle;
  uv_write_t write_req;
  int idle_timeout;
  uv_timer_t timer_handle;
  uv_loop_t *loop;
  wsn_node_conf_t *conf;
  int direction;
  int state;
  char buf[2048];
  int nread;
  wsn_conn_close_cb_t close_cb;
} wsn_conn_ctx_t;

#ifdef __cplusplus
extern "C" {
#endif

WSN_EXPORT int wsn_conn_init(wsn_conn_ctx_t* conn, uv_loop_t *loop,
                             wsn_node_conf_t *conf,
                             int idle_timeout, int direction,
                             wsn_conn_close_cb_t close_cb);
WSN_EXPORT void wsn_conn_close(wsn_conn_ctx_t *conn);
WSN_EXPORT void wsn_conn_start_processing(wsn_conn_ctx_t *conn);

#ifdef __cplusplus
}
#endif

#endif // _WSN_INCL_CONN_H