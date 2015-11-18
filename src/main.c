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
#include <stdio.h>
#if HAVE_UNISTD_H
  #include <unistd.h>
#else
  #include "getopt.h"
#endif

#include "uv.h"

#include "wsn.h"

static const char *prog_name_ = "wsn-hub";

const char *get_prog_name_(void)
{
  return prog_name_;
}

static void usage_(int invalid)
{
  printf("Usage:\n"
         "\n"
         "  %s [-c <config file>] [-h]\n"
         "\n"
         "Options:\n"
         "\n"
         "  -c <config file>  Use given configuration file.\n"
         "                         Default: \"wsn.conf\"\n"
         "  -h                     Show this help message.\n"
         "",
         prog_name_);
  exit(invalid ? WSN_ERR_INVALID_CMD_OPTS : 0);
}

static void parse_opts_(wsn_all_configs_t *configs, int argc, char **argv)
{
  int opt;

  while (-1 != (opt = getopt(argc, argv, "c:h"))) {
    switch (opt) {
      case 'c':
        configs->conf_file = optarg;
        break;
      case 'h':
        usage_(0);
        break;
      default:
        usage_(1);
    }
  }
}

int main(int argc, char **argv)
{
  int err = 0;
  prog_name_ = wsn_path_file_part(argv[0]);

  wsn_thread_ctx_t thread_ctx;
  wsn_err_ctx_t err_ctx;
  
  wsn_err_ctx_init(&err_ctx);
  if (wsn_thread_ctx_key_init() != 0) {
    printf("WSN error: %s!\n", wsn_last_err_str());
    return wsn_last_err();
  }
  wsn_thread_ctx_init(&thread_ctx, &err_ctx);
  wsn_set_thread_ctx(&thread_ctx);
  
  wsn_conf_mgr_t *conf_mgr = wsn_get_conf_mgr();
  conf_mgr->init();
  
  wsn_all_configs_t *configs = conf_mgr->get_all_configs();
  parse_opts_(configs, argc, argv);
  if (conf_mgr->load() != 0) {
    printf("WSN error: %s!\n", wsn_last_err_str());
    err = wsn_last_err();
  }
  
  if (!err) {
    wsn_server_ctx_t *servers = NULL;
    wsn_client_ctx_t *clients = NULL;
    
    uv_loop_t *loop = uv_default_loop();
    
    if (configs->server_count) {
      servers = (wsn_server_ctx_t*)malloc(sizeof(*servers) * configs->server_count);
      if (servers == NULL) {
        printf("WSN error: Malloc servers failed!\n");
        err = WSN_ERR_MALLOC;
      } else {
        for (int i = 0; i < configs->server_count; i++) {
          err = wsn_server_init(&servers[i], &configs->servers_conf[i], loop);
          if (err) {
            printf("WSN error: %s!\n", wsn_last_err_str());
            break;
          }
          err = wsn_server_start(&servers[i]);
          if (err) {
            printf("WSN error: %s!\n", wsn_last_err_str());
            break;
          }
        }
      }
    }
    
    if (configs->client_count) {
      clients = (wsn_client_ctx_t*)malloc(sizeof(*clients) * configs->client_count);
      if (clients == NULL) {
        printf("WSN error: Malloc clients failed!\n");
        err = WSN_ERR_MALLOC;
      } else {
        for (int i = 0; i < configs->client_count; i++) {
          err = wsn_client_init(&clients[i], &configs->clients_conf[i], loop);
          if (err) {
            printf("WSN error: %s!\n", wsn_last_err_str());
            break;
          }
          err = wsn_client_start(&clients[i]);
          if (err) {
            printf("WSN error: %s!\n", wsn_last_err_str());
            break;
          }
        }
      }
    }
    
    if (!err) {
      if (uv_run(loop, UV_RUN_DEFAULT) != 0) {
        err = wsn_last_err();
        printf("WSN error: %s!\n", wsn_last_err_str());
      }
    }
    
    if (servers) {
      for (int i = 0; i < configs->server_count; i++) {
        wsn_server_cleanup(&servers[i]);
      }
      free(servers);
    }
    if (clients) {
      for (int i = 0; i < configs->client_count; i++) {
        wsn_client_cleanup(&clients[i]);
      }
      free(clients);
    }
    uv_loop_delete(loop);
  }
  
  conf_mgr->cleanup();
  wsn_thread_ctx_cleanup(&thread_ctx);

  return err;
}
