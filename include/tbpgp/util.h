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

#ifndef _UTIL_H_
#define _UTIL_H_
#define MAX_IP_STR_LEN 46
#include<tbpgp/exported.h>
#ifndef _WIN32
	#include <pthread.h>
#endif
#include<event2/util.h>
enum AddrStructType
{
	UTIL_ADDRINFO = 0,
	UTIL_SOCKADDR,
};
/* FUNCS */
const char *get_ip_str(void *, char *, size_t, int);
void mutex_lock(THREAD_MUTEX *);
void mutex_unlock(THREAD_MUTEX *);
long get_online_cores(void);
void parse_hash(const char *, uint64_t *);
void print_hash_bytes(unsigned char *, unsigned int);

#endif
