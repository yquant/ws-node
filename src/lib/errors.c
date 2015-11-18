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
#include <stdarg.h>
#include <stdio.h>

#include "wsn/thread_ctx.h"
#include "wsn/utils.h"
#include "wsn/errors.h"

int wsn_err_ctx_init(wsn_err_ctx_t *err_ctx)
{
  err_ctx->last_err = 0;
  err_ctx->last_err_str = NULL;
  return 0;
}

void wsn_err_ctx_cleanup(wsn_err_ctx_t *err_ctx)
{
  if (err_ctx->last_err_str) {
    free(err_ctx->last_err_str);
  }
}

int wsn_last_err()
{
  wsn_err_ctx_t *ctx = wsn_thread_ctx()->err_ctx;
  return ctx->last_err;
}

const char* wsn_last_err_str()
{
  wsn_err_ctx_t *ctx = wsn_thread_ctx()->err_ctx;
  if (ctx->last_err_str == NULL) {
    char buf[1024];
    switch (ctx->last_err) {
      case 0:
        ctx->last_err_str = wsn_strdup("No error");
        break;
      default:
        snprintf(buf, sizeof(buf), "Unknown error %d", ctx->last_err);
        ctx->last_err_str = wsn_strdup(buf);
    }
  }
  return ctx->last_err_str;
}

void wsn_report_err(int err, const char *fmt, ...)
{
  va_list args;
  char buf[1024];
  
  wsn_err_ctx_t *ctx = wsn_thread_ctx()->err_ctx;
  
  ctx->last_err = err;
  if (ctx->last_err_str) {
    free(ctx->last_err_str);
    ctx->last_err_str = NULL;
  }
  if (fmt) {
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    ctx->last_err_str = wsn_strdup(buf);
  }
}

void wsn_clear_err()
{
  wsn_err_ctx_t *ctx = wsn_thread_ctx()->err_ctx;
  ctx->last_err = 0;
  if (ctx->last_err_str) {
    free(ctx->last_err_str);
  }
  ctx->last_err_str = NULL;
}