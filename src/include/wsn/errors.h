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

#ifndef _WSN_INCL_ERRORS_H
#define _WSN_INCL_ERRORS_H

#include "wsn/defs.h"

typedef enum {
  WSN_ERR_NONE = 0,
  WSN_ERR_MALLOC,
  WSN_ERR_FILE_IO,
  WSN_ERR_CREATE_TLS_KEY,
  WSN_ERR_INVALID_CMD_OPTS,
  WSN_ERR_READ_CONF_FILE,
  WSN_ERR_PARSE_CONF_FILE,
  WSN_ERR_INVALID_CONF_FILE,
  WSN_ERR_GET_ADDR_INFO,
  WSN_ERR_BIND,
  WSN_ERR_LISTEN,
  WSN_ERR_CONNECT,
} wsn_err_code_t;

typedef struct {
  int last_err;
  char *last_err_str;
} wsn_err_ctx_t;

WSN_EXPORT int wsn_err_ctx_init(wsn_err_ctx_t *err_ctx);
WSN_EXPORT void wsn_err_ctx_cleanup(wsn_err_ctx_t *err_ctx);

WSN_EXPORT int wsn_last_err();
WSN_EXPORT const char* wsn_last_err_str();

void wsn_report_err(int err, const char *fmt, ...);
void wsn_clear_err();

#endif // _WSN_INCL_ERRORS_H