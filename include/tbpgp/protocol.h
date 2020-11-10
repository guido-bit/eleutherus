/*  
 *  TBPGP Blockchain Network and governance library
 *  Copyright (C) 2019-2025 Bhargav bhargav@tbpgp.org 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.  
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.  
 *
 *  You should have received a copy <license/lgpl.txt> of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 * 
 * */

#ifndef _PROTO_H_
#define _PROTO_H_
#include<event2/bufferevent.h>
#include<event2/buffer.h>
#include<event2/listener.h>
#include<event2/thread.h>
#include<event2/util.h>
#include<event2/event.h>
#include<stdint.h>
#define CHAINID_BYTES_LEN 32
/* FUNCS */
void conn_eventcb(struct bufferevent *, short, void *);
void conn_readcb(struct bufferevent *, void *);
void listener_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int, void *);
void *minion_func(void *);
void parse_hash(const char *, uint64_t *);
#endif
