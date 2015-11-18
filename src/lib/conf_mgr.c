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

#include "yajl/yajl_tree.h"

#include "wsn/conf_mgr.h"
#include "wsn/errors.h"
#include "wsn/utils.h"

static void init_();
static wsn_all_configs_t* get_all_configs_();
static int load_();
static void cleanup_();

static wsn_all_configs_t all_configs_;

static wsn_conf_mgr_t conf_mgr_ = {
  init_,
  get_all_configs_,
  load_,
  cleanup_,
};

wsn_conf_mgr_t* wsn_get_conf_mgr()
{
  return &conf_mgr_;
}

static void init_default_configs_()
{
  memset(&all_configs_, 0, sizeof(all_configs_));
  all_configs_.conf_file = WSN_CONF_DEFAULT_CONF_FILE;
  all_configs_.connect_timeout = WSN_CONF_DEFAULT_CONNECT_TIMEOUT;
  all_configs_.idle_timeout = WSN_CONF_DEFAULT_IDLE_TIMEOUT;
}

void init_()
{
  init_default_configs_();
}

wsn_all_configs_t* get_all_configs_()
{
  return &all_configs_;
}

static yajl_val parse_config_file_()
{
  NULL;
  int len;
  char errbuf[1024];
  
  char *content = wsn_read_all(all_configs_.conf_file, &len);
  if (content == NULL) {
    wsn_report_err(WSN_ERR_READ_CONF_FILE, "Failed to read config file %s", all_configs_.conf_file);
    return NULL;
  }

  yajl_val js_configs = yajl_tree_parse(content, errbuf, sizeof(errbuf));
  if (js_configs == NULL) {
    wsn_report_err(WSN_ERR_PARSE_CONF_FILE, "Failed to parse config file %s, %s", all_configs_.conf_file, errbuf);
  }
  
  free(content);
  return js_configs;
}

static int translate_configs_(yajl_val js_configs)
{
  yajl_val js_connect_timeout = wsn_yajl_tree_get(js_configs, "connect-timeout", yajl_t_number);
  if (js_connect_timeout) {
    all_configs_.connect_timeout = YAJL_GET_INTEGER(js_connect_timeout);
  }
  yajl_val js_idle_timeout = wsn_yajl_tree_get(js_configs, "idle-timeout", yajl_t_number);
  if (js_idle_timeout) {
    all_configs_.idle_timeout = YAJL_GET_INTEGER(js_idle_timeout);
  }
  all_configs_.servers_conf = wsn_nodes_conf_find(WSN_NODE_TYPE_SERVER, 
                                                  &all_configs_.server_count, js_configs);
  all_configs_.clients_conf = wsn_nodes_conf_find(WSN_NODE_TYPE_CLIENT,
                                                  &all_configs_.client_count, js_configs);
  if (all_configs_.servers_conf == NULL && all_configs_.clients_conf == NULL) {
    return wsn_last_err();
  }
  wsn_clear_err();
  return 0;
}

int load_()
{
  int err = 0;
  yajl_val js_configs = parse_config_file_();
  if (js_configs == NULL) {
    err = wsn_last_err();
  } else {
    err = translate_configs_(js_configs);
    yajl_tree_free(js_configs);
  }
  return err;
}

void cleanup_()
{
  for (int i = 0; i < all_configs_.server_count; i++) {
    wsn_node_conf_cleanup(&all_configs_.servers_conf[i]);
  }
}