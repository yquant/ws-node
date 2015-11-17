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

#ifndef _WSN_INCL_THREAD_CTX_H
#define _WSN_INCL_THREAD_CTX_H

#include "wsn/defs.h"
#include "wsn/errors.h"

typedef struct {
  wsn_err_ctx_t *err_ctx;
} wsn_thread_ctx_t;

WSN_EXPORT int wsn_thread_ctx_key_init();

WSN_EXPORT int wsn_thread_ctx_init(wsn_thread_ctx_t *thread_ctx, wsn_err_ctx_t *err_ctx);
WSN_EXPORT wsn_thread_ctx_t* wsn_set_thread_ctx(wsn_thread_ctx_t *thread_ctx);
WSN_EXPORT wsn_thread_ctx_t* wsn_thread_ctx();
WSN_EXPORT void wsn_thread_ctx_cleanup(wsn_thread_ctx_t *thread_ctx);

#endif // _WSN_INCL_THREAD_CTX_H