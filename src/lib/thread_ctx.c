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
#include "wsn/thread_ctx.h"

static uv_key_t tls_key_;

int wsn_thread_ctx_key_init()
{
  int err = uv_key_create(&tls_key_);
  if (err) {
    wsn_report_err(WSN_ERR_CREATE_TLS_KEY, "Failed to create TLS key, %s", uv_strerror(err));
    err = WSN_ERR_CREATE_TLS_KEY;
  }
  return err;
}

int wsn_thread_ctx_init(wsn_thread_ctx_t *thread_ctx, wsn_err_ctx_t *err_ctx)
{
  thread_ctx->err_ctx = err_ctx;
  return 0;
}

wsn_thread_ctx_t* wsn_set_thread_ctx(wsn_thread_ctx_t *thread_ctx)
{
  wsn_thread_ctx_t *old_ctx = (wsn_thread_ctx_t*)uv_key_get(&tls_key_);
  uv_key_set(&tls_key_, thread_ctx);
  return old_ctx;
}

wsn_thread_ctx_t* wsn_thread_ctx()
{
  return (wsn_thread_ctx_t*)uv_key_get(&tls_key_);
}

void wsn_thread_ctx_cleanup(wsn_thread_ctx_t *thread_ctx)
{
  if (thread_ctx->err_ctx) {
    wsn_err_ctx_cleanup(thread_ctx->err_ctx);
  }
}