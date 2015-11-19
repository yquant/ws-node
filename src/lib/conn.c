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

#include "wsn/errors.h"
#include "wsn/conn.h"

static void conn_read_(wsn_conn_ctx_t *conn);
static void conn_write_(wsn_conn_ctx_t *conn, void *data, unsigned int len);

int wsn_conn_init(wsn_conn_ctx_t* conn, uv_loop_t *loop,
                  wsn_node_conf_t *conf,
                  int idle_timeout, int direction)
{
  conn->loop = loop;
  conn->conf = conf;
  conn->idle_timeout = idle_timeout;
  if (wsn_is_pipe(conn->conf)) {
    uv_pipe_init(loop, &conn->io_handle.pipe, 0);
  } else {
    uv_tcp_init(loop, &conn->io_handle.tcp);
  }
  uv_timer_init(loop, &conn->timer_handle);
  conn->direction = direction;
  conn->state = WSN_CONN_STATE_CREATED;
  conn->nread = 0;
  return 0;
}

static void on_closed_(uv_handle_t* handle)
{
  wsn_conn_ctx_t *conn = CONTAINER_OF(handle, wsn_conn_ctx_t, io_handle);
  conn->state = WSN_CONN_STATE_CLOSED;
  free(conn);
}

void wsn_conn_close(wsn_conn_ctx_t *conn)
{
  uv_timer_stop(&conn->timer_handle);
  uv_read_stop(&conn->io_handle.stream);
  uv_close(&conn->io_handle.handle, on_closed_);
  conn->state = WSN_CONN_STATE_CLOSING;
}

static void on_conn_timer_expire_(uv_timer_t *handle)
{
  wsn_conn_ctx_t *conn = CONTAINER_OF(handle, wsn_conn_ctx_t, timer_handle);
  wsn_conn_close(conn);
}

static void conn_timer_reset_(wsn_conn_ctx_t *conn)
{
  uv_timer_start(&conn->timer_handle,
                 on_conn_timer_expire_,
                 (unsigned int)conn->idle_timeout,
                 0);
}

static void conn_alloc_(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
  wsn_conn_ctx_t *conn = CONTAINER_OF(handle, wsn_conn_ctx_t, io_handle);
  buf->base = conn->buf + conn->nread;
  buf->len = sizeof(conn->buf) - conn->nread;
}

static void conn_process_data_(wsn_conn_ctx_t *conn)
{
  conn->nread = 0;
}

static void on_conn_read_done_(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf)
{
  wsn_conn_ctx_t *conn = CONTAINER_OF(handle, wsn_conn_ctx_t, io_handle);

  uv_timer_stop(&conn->timer_handle);

  if (nread < 0) {
    wsn_conn_close(conn);
    return;
  }
  
  if (nread == 0) {
    return;
  }
  
  conn->nread += (int)nread;
  conn_process_data_(conn);
  conn_read_(conn);
}

static void conn_read_(wsn_conn_ctx_t *conn)
{
  uv_read_start(&conn->io_handle.stream, conn_alloc_, on_conn_read_done_);
  conn_timer_reset_(conn);
}

static void on_conn_write_done_(uv_write_t *req, int status)
{
  wsn_conn_ctx_t *conn = CONTAINER_OF(req, wsn_conn_ctx_t, write_req);

  uv_timer_stop(&conn->timer_handle);

  if (status == UV_ECANCELED) {
    return;
  }

  conn_read_(conn);
}

static void conn_write_(wsn_conn_ctx_t *conn, void *data, unsigned int len)
{
  uv_buf_t buf;

  buf.base = (char*)data;
  buf.len = len;

  uv_write(&conn->write_req, &conn->io_handle.stream, &buf, 1, on_conn_write_done_);
  conn_timer_reset_(conn);
}

void wsn_conn_processing(wsn_conn_ctx_t *conn)
{
  conn->state = WSN_CONN_ACTIVE;
  conn_write_(conn, "hello\r\n\r\n", 9);
}
