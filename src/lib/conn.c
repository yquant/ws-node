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

wsn_conn_ctx_t* wsn_conn_create(wsn_server_listen_ctx_t *listen_ctx,
                                wsn_client_ctx_t *client,
                                int idle_timeout, int direction)
{
  const char *type = (listen_ctx ? "server" : "client to");
  
  wsn_conn_ctx_t *conn = malloc(sizeof(*conn));
  if (conn == NULL) {
    wsn_report_err(WSN_ERR_MALLOC, "Malloc conn failed for %s (\"%s\")",
                   type, listen_ctx ? listen_ctx->server->conf->host : client->conf->host);
    return NULL;
  }
  conn->listen_ctx = listen_ctx;
  conn->client = client;
  conn->idle_timeout = idle_timeout;
  uv_loop_t *loop = (listen_ctx ? conn->listen_ctx->server->loop : client->loop);
  uv_timer_init(loop, &conn->timer_handle);
  conn->direction = direction;
  conn->state = WSN_CONN_STATE_CREATED;
  conn->nread = 0;
  return conn;
}

void wsn_conn_close(wsn_conn_ctx_t *conn)
{
  uv_timer_stop(&conn->timer_handle);
  uv_close((uv_handle_t*)&conn->tcp_handle, NULL);
  conn->state = WSN_CONN_STATE_CLOSED;
}

static void on_conn_timer_expire_(uv_timer_t *handle)
{
  wsn_conn_ctx_t *conn = CONTAINER_OF(handle, wsn_conn_ctx_t, timer_handle);
  wsn_conn_close(conn);
  free(conn);
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
  wsn_conn_ctx_t *conn = CONTAINER_OF(handle, wsn_conn_ctx_t, tcp_handle);
  buf->base = conn->buf + conn->nread;
  buf->len = sizeof(conn->buf) - conn->nread;
}

static void conn_process_data_(wsn_conn_ctx_t *conn)
{
}

static void on_conn_read_done_(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf)
{
  wsn_conn_ctx_t *conn = CONTAINER_OF(handle, wsn_conn_ctx_t, tcp_handle);
  if (nread <= 0) {
    wsn_conn_close(conn);
    return;
  }
  conn->nread += (int)nread;
  conn_process_data_(conn);
}

static void conn_read_(wsn_conn_ctx_t *conn)
{
  conn->state = WSN_CONN_READING;
  uv_read_start((uv_stream_t*)&conn->tcp_handle, conn_alloc_, on_conn_read_done_);
  conn_timer_reset_(conn);
}

void wsn_conn_processing(wsn_conn_ctx_t *conn)
{
  conn_read_(conn);
}
