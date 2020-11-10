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
#include<event2/util.h>
#include<tbpgp/util.h>
#ifdef WIN32_LEAN_AND_MEAN
#include<ws2ipdef.h>
#else
#include<pthread.h>
#include<unistd.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include<signal.h>

void print_hash_bytes(unsigned char *hash, unsigned int n)
{
        int i = 0;

        printf("PRINT_HASH : ");
        for(i = 0; i < n; ++i)
        {
                printf("%02" PRIx8, hash[i]);
        }
        printf("\n");
}

void parse_hash(const char *str, uint64_t *b)
{
        unsigned int i, k;
        char str_temp[17];

        for(i = 0; i < 4; ++i)
        {
                for(k = 0; k < 16; ++k)
                {
                        str_temp[k] = str[k + (i*16)];
                }
                str_temp[16] = '\0';
                sscanf(str_temp, "%lx", &b[i]);
                b[i] = htobe64(b[i]);
        }
}
TBPGP_EXPORT_SYMBOL void tbpgp_signal_connect(int signum, void (*func)(int))
{
#ifndef _WIN32
        struct sigaction sa;
#else
        signal(signum, func);
#endif
#ifndef _WIN32
        sa.sa_handler = func;
        sa.sa_flags = SA_RESTART;
        sigfillset(&sa.sa_mask);
        if(sigaction(SIGINT, &sa, NULL) == -1)
        { 
                printf("Cannot handle SIGINT");
                exit(1);
        } 
#endif
}
const char *get_ip_str(void *addr, char *buf, size_t len, int type) //THERE SHOULD BE A BETTER WAY. BUT WORKS FOR NOW.
{
	const char *ip_str = NULL;
	struct sockaddr_in *ipv4;
	struct sockaddr_in6 *ipv6;
	struct evutil_addrinfo *res= NULL;
	struct sockaddr *sockaddr = NULL;

	switch(type)
	{
		case UTIL_ADDRINFO :
			res = (struct evutil_addrinfo *)addr;
			if(res->ai_family == AF_INET) //IPv4
			{
				ipv4 = (struct sockaddr_in *)res->ai_addr;
				ip_str  = evutil_inet_ntop(res->ai_family, &ipv4->sin_addr, buf, len);
			}
			else
			{
				ipv6 = (struct sockaddr_in6 *)res->ai_addr;
				ip_str  = evutil_inet_ntop(res->ai_family, &ipv6->sin6_addr, buf, len);
			}
			break;
		case UTIL_SOCKADDR :
			sockaddr = (struct sockaddr *)addr;
			if(sockaddr->sa_family == AF_INET)
			{
				ipv4 = (struct sockaddr_in *)sockaddr;
				ip_str = evutil_inet_ntop(ipv4->sin_family, &ipv4->sin_addr, buf, len);
			}
			else
			{
				ipv6 = (struct sockaddr_in6 *)sockaddr;
				ip_str = evutil_inet_ntop(ipv6->sin6_family, &ipv6->sin6_addr, buf, len);
			}
			break;
		default : 
			break;
	}

	
	return ip_str;
}
void mutex_lock(THREAD_MUTEX *lock)
{
#ifdef _WIN32
	WaitForSingleObject((*lock), INFINITE);
#else
	pthread_mutex_lock(lock);
#endif
}
void mutex_unlock(THREAD_MUTEX *lock)
{
#ifdef _WIN32
	ReleaseMutex(*lock);
#else
	pthread_mutex_unlock(lock);
#endif
}
long get_online_cores(void) 
{
	long num_procs = -1;
	long num_procs_max = -1;
#ifdef _WIN32
#	ifndef _SC_NPROCESSORS_ONLN
		SYSTEM_INFO info;
		GetSystemInfo(&info);
#		define sysconf(a) info.dwNumberOfProcessors
#	define _SC_NPROCESSORS_ONLN
#	endif
#endif
#ifdef _SC_NPROCESSORS_ONLN
	num_procs = sysconf(_SC_NPROCESSORS_ONLN);
	if (num_procs < 1)
	{
		return -1;
	}
	num_procs_max = sysconf(_SC_NPROCESSORS_CONF);
	if (num_procs_max < 1)
	{
		return -1;
	}
	return num_procs_max;
#else
	return -1;
#endif
}
