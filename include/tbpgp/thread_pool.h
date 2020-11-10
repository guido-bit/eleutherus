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

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include<tbpgp/exported.h>
#ifndef WIN32_LEAN_AND_MEAN
	#include<pthread.h>
#endif
#include<stdint.h>
enum ThreadStatus
{
	THREAD_STATUS_TERMINATE = 0x7fdc,
	THREAD_STATUS_SUSPENDED,
	THREAD_STATUS_RUN,
};
struct WorkLoad //job
{
	void *data;
	struct WorkLoad *next, *prev;
};
struct WorkQ //workqueue
{
	struct WorkLoad *work_load; //POINTER TO WORKLOAD
	struct ThreadPool *thread; //POINTER TO CURRENT THREAD
	THREAD_RW_MUTEX mutex;
	THREAD_COND cond;
};
struct ThreadPool //worker
{
        THREAD_T tid;
	int thread_status;
	struct WorkQ *work_q;
	uint32_t ret;
        struct ThreadPool *next, *prev;
};
/* FUNCS */
#ifdef _WIN32
struct ThreadPool *forge_thread(DWORD(*) (void *), void *, int);
#else
struct ThreadPool *forge_thread(void *(*) (void *), void *, int);
#endif
int push_work_load(void *);
int deploy_minions(void);
void terminate_minions(void);
/* GLOBALS */
#endif
