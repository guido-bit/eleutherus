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

#ifndef _CONTEXT_H_
#	define _CONTEXT_H_
#	include<tbpgp/exported.h>
#	ifndef _WIN32
#		include <pthread.h>
#	endif
#include<stdint.h>
#define CHAIN_ID_BYTES 32
typedef union
{
	unsigned char uchars[CHAIN_ID_BYTES];
	uint8_t uint8s[CHAIN_ID_BYTES];
	uint16_t uint16s[CHAIN_ID_BYTES/2];
	uint32_t uint32s[CHAIN_ID_BYTES/4];
	uint64_t uint64s[CHAIN_ID_BYTES/8];
}ChainId;
enum MUTEX_NAMES
{
	P2P_MUTEX = 0,
	TOTAL_MUTEXES
};
struct SeedNodes
{
	char *seed_node;
	struct SeedNodes *next, *prev;
};
struct Context
{
	struct event_base *base;
	THREAD_MUTEX mutex[TOTAL_MUTEXES];
	struct SeedNodes *seed_nodes;
	ChainId chain_id;
	struct Context *next, *prev;
};
#endif
