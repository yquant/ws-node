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

#ifndef _WSN_INCL_CONF_MGR_H
#define _WSN_INCL_CONF_MGR_H

#include "wsn/defs.h"
#include "wsn/configs.h"

typedef struct {
  void (*init)();
  wsn_all_configs_t* (*get_all_configs)();
  int (*load)();
  void (*cleanup)();
} wsn_conf_mgr_t;

WSN_EXPORT wsn_conf_mgr_t* wsn_get_conf_mgr();

#endif // _WSN_INCL_CONF_MGR_H