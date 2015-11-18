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
#include "wsn/client.h"

#ifndef INET6_ADDRSTRLEN
# define INET6_ADDRSTRLEN 63
#endif

int wsn_client_init(wsn_client_ctx_t *client, wsn_node_conf_t *conf, uv_loop_t *loop)
{
  client->conf = conf;
  client->loop = loop;
  client->connect_timeout = wsn_get_conf_mgr()->get_all_configs()->connect_timeout;
  client->idle_timeout = wsn_get_conf_mgr()->get_all_configs()->idle_timeout;
  uv_timer_init(loop, &client->timer_handle);
  return 0;
}

static void on_connect_timer_expire_(uv_timer_t *handle)
{
  wsn_client_ctx_t *client = CONTAINER_OF(handle, wsn_client_ctx_t, timer_handle);
  wsn_client_cleanup(client);
}

static void connect_timer_reset_(wsn_client_ctx_t *client)
{
  uv_timer_start(&client->timer_handle,
                 on_connect_timer_expire_,
                 (unsigned int)client->connect_timeout,
                 0);
}

static void on_connected_(uv_connect_t *req, int status)
{
  wsn_client_ctx_t *client = CONTAINER_OF(req, wsn_client_ctx_t, connect_req);
  
  uv_timer_stop(&client->timer_handle);

  if (status != 0) {
    return;
  }
  
  wsn_conn_ctx_t *conn = wsn_conn_create(NULL, client, client->idle_timeout,
                                         WSN_CONN_DIRECTION_OUT);
  if (conn) {
    wsn_conn_processing(conn);
  }
}

static int wsn_client_start_connect_(wsn_client_ctx_t *client)
{
  connect_timer_reset_(client);

  int err = uv_tcp_connect(&client->connect_req, &client->tcp_handle,
                           &client->host_addr, on_connected_);
  if (err) {
    wsn_report_err(WSN_ERR_CONNECT, "Connect to (\"%s\") failed: %s",
                   client->conf->host, uv_strerror(err));
    return WSN_ERR_CONNECT;
  }
  return 0;
}

static void on_get_host_addrs_(uv_getaddrinfo_t *req, int status, struct addrinfo *addrs)
{
  wsn_client_ctx_t *client = CONTAINER_OF(req, wsn_client_ctx_t, getaddrinfo_req);

  uv_timer_stop(&client->timer_handle);
  
  if (status < 0) {
    wsn_report_err(WSN_ERR_GET_ADDR_INFO, "Failed to getaddrinfo for client to (\"%s\"): %s",
                   client->conf->host, uv_strerror(status));
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
    wsn_report_err(WSN_ERR_GET_ADDR_INFO, "Remote host (\"%s\") has no IPv4/6 addresses",
                   client->conf->host);
    uv_freeaddrinfo(addrs);
    return;
  }

  union {
    struct sockaddr addr;
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
  } s;
  
  for (struct addrinfo *ai = addrs; ai != NULL; ai = ai->ai_next) {
    if (ai->ai_family == AF_INET) {
      s.addr4 = *(const struct sockaddr_in *) ai->ai_addr;
      s.addr4.sin_port = htons(client->conf->port);
    } else if (ai->ai_family == AF_INET6) {
      s.addr6 = *(const struct sockaddr_in6 *) ai->ai_addr;
      s.addr6.sin6_port = htons(client->conf->port);
    } else {
      continue;
    }
    client->host_addr = s.addr;
    uv_tcp_init(client->loop, &client->tcp_handle);
    break;
  }

  uv_freeaddrinfo(addrs);
  
  if (wsn_client_start_connect_(client) != 0) {
    uv_stop(client->loop);
    return;
  }
}

int wsn_client_start(wsn_client_ctx_t *client)
{
  int err = 0;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  connect_timer_reset_(client);

  err = uv_getaddrinfo(client->loop,
                       &client->getaddrinfo_req,
                       on_get_host_addrs_,
                       client->conf->host,
                       NULL,
                       &hints);
  if (err != 0) {
    wsn_report_err(WSN_ERR_GET_ADDR_INFO,
                   "Failed to start client to (\"%s\"), getaddrinfo error: %s",
                   client->conf->host, uv_strerror(err));
    err = WSN_ERR_GET_ADDR_INFO;
  }
  return err;
}

void wsn_client_cleanup(wsn_client_ctx_t *client)
{
  uv_close((uv_handle_t*)&client->tcp_handle, NULL);
}