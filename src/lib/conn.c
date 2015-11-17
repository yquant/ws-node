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

wsn_conn_ctx_t* wsn_conn_create(wsn_server_listen_ctx_t *listen_ctx, int direction)
{
  wsn_conn_ctx_t *conn = malloc(sizeof(*conn));
  if (conn == NULL) {
    wsn_report_err(WSN_ERR_MALLOC, "Malloc conn failed for (\"%s\")",
                   listen_ctx->server->conf->host);
    return NULL;
  }
  conn->listen_ctx = listen_ctx;
  conn->direction = direction;
  return conn;
}

void wsn_conn_processing(wsn_conn_ctx_t *conn)
{
}
