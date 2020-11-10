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

#include<tbpgp/exported.h>
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#ifdef WIN32_LEAN_AND_MEAN
#include<winsock2.h>
#include<ws2ipdef.h>
#else
#include<unistd.h>
#endif
#include<tbpgp/protocol.h>
#include<tbpgp/log.h>
#include<tbpgp/util.h>
#include<tbpgp/context.h>
#include<tbpgp/thread_pool.h>

#define TBPGP_TCP_PORT "9555"
#define TBPGP_UDP_PORT "8555"

static void udp_srv(evutil_socket_t sock, short which, void *data)
{
	char buf[1000];
	struct sockaddr_storage addr;
	ev_socklen_t addr_len;
	size_t rwbytes;
	char *buf_str = NULL;

	addr_len = sizeof(struct sockaddr_storage);

	if((rwbytes = recvfrom(sock, buf, 999, 0, (struct sockaddr *)&addr, &addr_len)) <= 0)
		return;

	buf_str = (char *)malloc(sizeof(char)*(rwbytes+1));
	memset(buf_str, 0, rwbytes+1);
	strncpy(buf_str, buf, rwbytes);
	log_info("UDP GOT : %s and bytes %d", buf_str, rwbytes);

	if(sendto(sock, buf, rwbytes, 0, (struct sockaddr *)&addr, addr_len) != rwbytes)
		log_error("UDP : DID NOT WRITE ALL");

	free(buf_str);
}
static void accept_error_cb(struct evconnlistener *listener, void *arg)
{
    struct event_base *base = evconnlistener_get_base(listener);
    // Cross-platform error handling
    int err = EVUTIL_SOCKET_ERROR();

    log_error("Got an error on the listener: %s", evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}
static void conn_writecb(struct bufferevent *bev, void *user_data)
{
        struct evbuffer *output = bufferevent_get_output(bev);
        if (evbuffer_get_length(output) == 0) {
                printf("flushed answer\n");
		sleep(20);
                //bufferevent_free(bev);
        }
}
static void signal_cb(int fd, short events, void *data)
{
        struct event_base *base = data;
        struct timeval delay = { 2, 0 };

        printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

        event_base_loopexit(base, &delay);
}
static void *p2p_start(void *data)
{
	struct Context *context = (struct Context *)data;
	struct evconnlistener *listener;
	struct event *udp_event;
	//struct event *signal_event;

	evutil_socket_t udp_sock;
	int err;

	struct evutil_addrinfo tcp_hints, *tcp_p, udp_hints, *udp_p;
	struct evutil_addrinfo *tcp_result = NULL, *udp_result = NULL;
	int tcp_status, udp_status;
	char ipstr[MAX_IP_STR_LEN];

	//log_info("LIBEVENT VERSION %s", LIBEVENT_VERSION);
#ifdef _WIN32
	evthread_use_windows_threads();
	context->mutex[P2P_MUTEX] = CreateMutex(NULL, FALSE, NULL);
#else
	evthread_use_pthreads();
	pthread_mutex_init(&context->mutex[P2P_MUTEX], NULL);
#endif
	mutex_lock(&context->mutex[P2P_MUTEX]);

#ifdef _WIN32
	WSADATA wsa_data;
	if(WSAStartup(MAKEWORD(2,2), &wsa_data))
	{
		log_error("WSAStartup Failed");
	}
#endif
	memset(&tcp_hints, 0, sizeof(struct evutil_addrinfo));
	memset(&udp_hints, 0, sizeof(struct evutil_addrinfo));

	tcp_hints.ai_family = udp_hints.ai_family = AF_UNSPEC;
	tcp_hints.ai_socktype = SOCK_STREAM;
	udp_hints.ai_socktype = SOCK_DGRAM;
	tcp_hints.ai_protocol = 0;
	udp_hints.ai_protocol = 0;
	tcp_hints.ai_flags = EVUTIL_AI_PASSIVE;
	udp_hints.ai_flags = EVUTIL_AI_PASSIVE | EVUTIL_AI_ADDRCONFIG; 
	tcp_hints.ai_canonname = udp_hints.ai_canonname = NULL;
	tcp_hints.ai_addr = udp_hints.ai_addr = NULL;
	tcp_hints.ai_next = udp_hints.ai_next = NULL;

	context->base = event_base_new();

	/* UDP */
	udp_status = evutil_getaddrinfo(NULL, TBPGP_UDP_PORT, &udp_hints, &udp_result);
	if(udp_status != 0)
	{
		log_error("UDP getaddrinfo : %s\n", evutil_gai_strerror(udp_status));
	}
	for(udp_p = udp_result; udp_p != NULL; udp_p = udp_p->ai_next)
	{
		udp_sock = socket(udp_p->ai_family, udp_p->ai_socktype, udp_p->ai_protocol);
		if(udp_sock == -1)
			continue;
		evutil_make_socket_nonblocking(udp_sock);
		evutil_make_listen_socket_reuseable(udp_sock);
		/*if(setsockopt(udp_sock, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off)) < 0) For later versions
			log_error("Cannot create dualstack");*/
		if(bind(udp_sock, udp_p->ai_addr, (int)udp_p->ai_addrlen) == 0)
		{
			log_info("UDP bound to address %s@%s", get_ip_str((void *)udp_p, ipstr, MAX_IP_STR_LEN,UTIL_ADDRINFO), TBPGP_UDP_PORT);
			break;
		}
		evutil_closesocket(udp_sock);
	}
	udp_event = event_new(context->base, udp_sock, EV_READ | EV_PERSIST, udp_srv, NULL);
	if (event_add(udp_event, NULL) < 0)
		log_error("Cannot add event");
	evutil_freeaddrinfo(udp_result);
	/* _END_UDP_ */
	
	/* TCP */
	tcp_status = evutil_getaddrinfo(NULL, TBPGP_TCP_PORT, &tcp_hints, &tcp_result);
	if(tcp_status != 0)
	{
		log_error("TCP : getaddrinfo : %s", evutil_gai_strerror(tcp_status));
	}
	for(tcp_p = tcp_result; tcp_p != NULL; tcp_p = tcp_p->ai_next)
	{
		if(tcp_p->ai_family == AF_INET) //SUPPORT IPV4 ONLY, FOR NOW..
		{
			listener = evconnlistener_new_bind(context->base, listener_cb, (void *)context->base, LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE|LEV_OPT_THREADSAFE, -1, (struct sockaddr *)tcp_p->ai_addr, sizeof(struct sockaddr));
			if(!listener)
				continue;
			break;
		}
	}
	if(!listener)
	{
		err = EVUTIL_SOCKET_ERROR();
		log_error("Cannot create TCP listener %s", evutil_socket_error_to_string(err));
	}
	else
	{
		log_info("TCP bound to address %s@%s", get_ip_str((void *)tcp_p, ipstr, MAX_IP_STR_LEN, UTIL_ADDRINFO), TBPGP_TCP_PORT);
	}

	evconnlistener_set_error_cb(listener, accept_error_cb);

	/* ATTACH SIGNALS */
	/* DEBUG UNIT */
	/*signal_event = evsignal_new(context->base, SIGINT, signal_cb, (void *)context->base);
	if (!signal_event || event_add(signal_event, NULL) < 0) 
	{
                log_error("Could not create/add a signal event!");
        }
	log_info("Dispatching event_base");*/
	/* END DEBUG UNIT */

	log_info("Dispatching base");
	if(evthread_make_base_notifiable(context->base))
		log_error("Connot make notifiable");
	event_base_dispatch(context->base); 

	evconnlistener_free(listener);
	//event_free(signal_event); //DELETE THIS LATER
	event_free(udp_event);
	event_base_free(context->base);
	evutil_closesocket(udp_sock);
	evutil_freeaddrinfo(tcp_result);

#ifdef _WIN32
	WSACleanup();
#endif
	mutex_unlock(&context->mutex[P2P_MUTEX]);
	return NULL;
}
int net_dispatch(struct Context *context)
{
	forge_thread(p2p_start, (void *)context, THREAD_STATUS_RUN);
	return 0;
}
