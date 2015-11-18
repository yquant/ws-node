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
#include <stdio.h>
#include "wsn/errors.h"
#include "wsn/conf_mgr.h"
#include "wsn/conn.h"
#include "wsn/server.h"

#ifndef INET6_ADDRSTRLEN
# define INET6_ADDRSTRLEN 63
#endif

int wsn_server_listen_ctx_init(wsn_server_listen_ctx_t *listen_ctx,
                               wsn_server_ctx_t *server, struct sockaddr addr)
{
  listen_ctx->server = server;
  listen_ctx->addr = addr;
  uv_tcp_init(server->loop, &listen_ctx->tcp_handle);
  return 0;
}

void wsn_server_listen_ctx_cleanup(wsn_server_listen_ctx_t *listen_ctx)
{
  uv_close((uv_handle_t*)&listen_ctx->tcp_handle, NULL);
}

int wsn_server_init(wsn_server_ctx_t *server, wsn_node_conf_t *conf, uv_loop_t *loop)
{
  server->conf = conf;
  server->listen_ctxs = NULL;
  server->listen_ctx_count = 0;
  server->idle_timeout = wsn_get_conf_mgr()->get_all_configs()->idle_timeout;
  server->loop = loop;
  return 0;
}

static void on_new_connection_(uv_stream_t *stream, int status)
{
  wsn_server_listen_ctx_t *listen_ctx;
  wsn_conn_ctx_t *conn;

  if (status != 0) {
    return;
  }
  
  listen_ctx = CONTAINER_OF(stream, wsn_server_listen_ctx_t, tcp_handle);
  conn = wsn_conn_create(listen_ctx, NULL, listen_ctx->server->idle_timeout,
                         WSN_CONN_DIRECTION_IN);
  if (conn) {
    uv_tcp_init(listen_ctx->server->loop, &conn->tcp_handle);
    uv_accept(stream, (uv_stream_t*)&conn->tcp_handle);
    wsn_conn_processing(conn);
  }
}

static int wsn_server_start_listen_(wsn_server_listen_ctx_t *listen_ctx, uv_loop_t *loop)
{
  int err = uv_tcp_bind(&listen_ctx->tcp_handle, &listen_ctx->addr, 0);
  if (err) {
    wsn_server_listen_ctx_cleanup(listen_ctx);
    wsn_report_err(WSN_ERR_BIND, "Bind failed for server (\"%s\"): %s",
                   listen_ctx->server->conf->host, uv_strerror(err));
    return WSN_ERR_BIND;
  }
  err = uv_listen((uv_stream_t*)&listen_ctx->tcp_handle, 128, on_new_connection_);
  if (err) {
    wsn_server_listen_ctx_cleanup(listen_ctx);
    wsn_report_err(WSN_ERR_LISTEN, "Listen failed for server (\"%s\"): %s",
                   listen_ctx->server->conf->host, uv_strerror(err));
    return WSN_ERR_LISTEN;
  }
  return 0;
}

static void on_get_server_addrs_(uv_getaddrinfo_t *req, int status, struct addrinfo *addrs)
{
  wsn_server_ctx_t *server = CONTAINER_OF(req, wsn_server_ctx_t, getaddrinfo_req);

  if (status < 0) {
    wsn_report_err(WSN_ERR_GET_ADDR_INFO, "Failed to getaddrinfo for server (\"%s\"): %s",
                   server->conf->host, uv_strerror(status));
    uv_freeaddrinfo(addrs);
    return;
  }

  int ipv4_naddrs = 0;
  int ipv6_naddrs = 0;
  for (struct addrinfo *ai = addrs; ai != NULL; ai = ai->ai_next) {
    if (ai->ai_family == AF_INET) {
      ipv4_naddrs += 1;
    } else if (ai->ai_family == AF_INET6) {
      ipv6_naddrs += 1;
    }
  }

  if (ipv4_naddrs == 0 && ipv6_naddrs == 0) {
    wsn_report_err(WSN_ERR_GET_ADDR_INFO, "Server (\"%s\") has no IPv4/6 addresses",
                   server->conf->host);
    uv_freeaddrinfo(addrs);
    return;
  }

  server->listen_ctx_count = ipv4_naddrs + ipv6_naddrs;
  server->listen_ctxs = 
    (wsn_server_listen_ctx_t*)malloc(sizeof(server->listen_ctxs[0]) * server->listen_ctx_count);
  if (server->listen_ctxs == NULL) {
    wsn_report_err(WSN_ERR_MALLOC, "Malloc listen contexts for server (\"%s\") failed",
                   server->conf->host);
    uv_freeaddrinfo(addrs);
    return;
  }
  
  int n = 0;
  union {
    struct sockaddr addr;
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
  } s;
  
  for (struct addrinfo *ai = addrs; ai != NULL; ai = ai->ai_next) {
    if (ai->ai_family == AF_INET) {
      s.addr4 = *(const struct sockaddr_in *) ai->ai_addr;
      s.addr4.sin_port = htons(server->conf->port);
    } else if (ai->ai_family == AF_INET6) {
      s.addr6 = *(const struct sockaddr_in6 *) ai->ai_addr;
      s.addr6.sin6_port = htons(server->conf->port);
    } else {
      continue;
    }
    wsn_server_listen_ctx_init(&server->listen_ctxs[n], server, s.addr);
    n++;
  }

  uv_freeaddrinfo(addrs);
  
  for (int i = 0; i < server->listen_ctx_count; i++) {
    if (wsn_server_start_listen_(&server->listen_ctxs[i], server->loop) != 0) {
      uv_stop(server->loop);
      return;
    }
  }
}

int wsn_server_start(wsn_server_ctx_t *server)
{
  int err = 0;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  err = uv_getaddrinfo(server->loop,
                       &server->getaddrinfo_req,
                       on_get_server_addrs_,
                       server->conf->host,
                       NULL,
                       &hints);
  if (err != 0) {
    wsn_report_err(WSN_ERR_GET_ADDR_INFO,
                   "Failed to start server (\"%s\"), getaddrinfo error: %s",
                   server->conf->host, uv_strerror(err));
    err = WSN_ERR_GET_ADDR_INFO;
  }
  return err;
}

void wsn_server_cleanup(wsn_server_ctx_t *server)
{
  if (server->listen_ctxs) {
    for (int i = 0; i < server->listen_ctx_count; i++) {
      wsn_server_listen_ctx_cleanup(&server->listen_ctxs[i]);
    }
    free(server->listen_ctxs);
  }
}