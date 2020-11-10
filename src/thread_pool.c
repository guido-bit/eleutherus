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

#include<tbpgp/thread_pool.h>
#include<tbpgp/util.h>
#include<tbpgp/protocol.h>
#ifndef WIN32_LEAN_AND_MEAN
#include<unistd.h>
#endif
#include<stdlib.h>
#include<stdio.h>

#include<tbpgp/log.h>

static struct ThreadPool *_g_thread_pool = NULL;
static struct WorkQ *_g_work_q = NULL;

static void free_ThreadPool(struct ThreadPool *temp)
{
        struct ThreadPool *temp1 = NULL;
        while(temp != NULL)
        {
                temp1 = temp->next;
                free(temp);
                temp = temp1;
        }
}
static void init_WorkLoad(struct WorkLoad *temp)
{
	temp->data = NULL;
	temp->next = temp->prev = NULL;
	return;
}
static void free_WorkLoad(struct WorkLoad *head)
{
	struct WorkLoad *temp= head;
	struct WorkLoad *temp1 = NULL;

	while(temp != NULL)
	{
		temp1 = temp->next;
		free(temp);
		temp = temp1;
	}
	return;
}
static void init_ThreadPool(struct ThreadPool *temp)
{
        temp->ret = 0;
	temp->thread_status = 0;
	temp->work_q = NULL;
        temp->next = temp->prev = NULL;
        return;
}
struct ThreadPool *get_thread(void)
{
        struct ThreadPool *thread_pool_temp;
        if(_g_thread_pool == NULL)
        {
                _g_thread_pool = (struct ThreadPool *)malloc(sizeof(struct ThreadPool));
                init_ThreadPool(_g_thread_pool);
                thread_pool_temp = _g_thread_pool;
        }
        else
        {
                thread_pool_temp = _g_thread_pool->prev;
                thread_pool_temp->next = (struct ThreadPool *)malloc(sizeof(struct ThreadPool));
                init_ThreadPool(thread_pool_temp->next);
                thread_pool_temp->next->prev = thread_pool_temp;
                thread_pool_temp = thread_pool_temp->next;
        }
        _g_thread_pool->prev = thread_pool_temp;
        thread_pool_temp->ret = -1;
        return thread_pool_temp;
}
#ifdef _WIN32
struct ThreadPool *forge_thread(DWORD (*thread_func) (void *), void *thread_arg, int thread_status)
#else
struct ThreadPool *forge_thread(void *(*thread_func) (void *), void *thread_arg, int thread_status)
#endif
{
	struct ThreadPool *thread_pool_temp = NULL;

	thread_pool_temp  = get_thread();
#ifdef _WIN32
	if((thread_pool_temp->tid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) thread_func, thread_arg, 0, NULL)) == NULL)
#else
	if(pthread_create(&thread_pool_temp->tid, NULL, thread_func, thread_arg))
#endif
	{
		perror("Cannot create thread");
		exit(EXIT_FAILURE);
	}

	return thread_pool_temp;
}
int push_work_load(void *data)
{
	struct WorkLoad *work_load_temp = NULL;

#ifdef _WIN32
	AcquireSRWLockExclusive(&_g_work_q->mutex);
#else
	mutex_lock(&_g_work_q->mutex);
#endif
	if(_g_work_q->work_load == NULL)
	{
		_g_work_q->work_load = (struct WorkLoad *)malloc(sizeof(struct WorkLoad));
		init_WorkLoad(_g_work_q->work_load);
		work_load_temp = _g_work_q->work_load;
	}
	else
	{
		work_load_temp = _g_work_q->work_load->prev;
		work_load_temp->next = (struct WorkLoad *)malloc(sizeof(struct WorkLoad));
		init_WorkLoad(work_load_temp->next);
		work_load_temp->next->prev = work_load_temp;
		work_load_temp = work_load_temp->next;
	}
	_g_work_q->work_load->prev = work_load_temp;
	work_load_temp->data = data;
#ifdef _WIN32
	WakeConditionVariable(&_g_work_q->cond);
#else
	pthread_cond_signal(&_g_work_q->cond);
#endif
#ifdef _WIN32
	ReleaseSRWLockExclusive(&_g_work_q->mutex); //SHOULD THIS BE EXCLUSIVE??
#else
	mutex_unlock(&_g_work_q->mutex);
#endif

	return 0;
}
void terminate_minions(void)
{
	struct ThreadPool *thread_pool_temp = NULL;

	if(_g_work_q != NULL)
	{
#ifdef _WIN32
		AcquireSRWLockExclusive(&_g_work_q->mutex);
#else
		mutex_lock(&_g_work_q->mutex);
#endif
		for(thread_pool_temp = _g_thread_pool; thread_pool_temp != NULL; thread_pool_temp = thread_pool_temp->next)
		{
			if(thread_pool_temp->work_q != NULL)
			{
				log_debug("NOT NULLL SHOULD SEE 8");
				thread_pool_temp->thread_status = THREAD_STATUS_TERMINATE;
			}
		}
		log_debug("SIGNALLING MINIONS TO SHUT DOWN");
#ifdef _WIN32
		WakeAllConditionVariable(&_g_work_q->cond);
#else
		if(pthread_cond_broadcast(&_g_work_q->cond))
			log_error("Error on broadcast");
#endif
#ifdef _WIN32
		ReleaseSRWLockExclusive(&_g_work_q->mutex);
#else
		mutex_unlock(&_g_work_q->mutex);
#endif
	}
}
int deploy_minions(void)
{
	long core_count = 1;
	struct ThreadPool *thread_pool_temp = NULL;
	int i;

	if(_g_work_q == NULL)
	{
		log_debug("DEPLOYING MININOS");
		_g_work_q = (struct WorkQ *)malloc(sizeof(struct WorkQ));
		_g_work_q->work_load = NULL;
#ifdef _WIN32
		InitializeSRWLock(&_g_work_q->mutex);
		//_g_work_q->mutex = CreateMutex(NULL, FALSE, NULL);
		InitializeConditionVariable(&_g_work_q->cond);
#else
		pthread_mutex_init(&_g_work_q->mutex, NULL);
		pthread_cond_init(&_g_work_q->cond, NULL);
#endif

		if((core_count = get_online_cores()) >= 8) //SHOULD THIS BE IN CONF?
		{
			core_count = 8; //SETTING THIS TO FOR NOW... FOR DEBUGGING
		}
		for(i = 0; i < core_count; ++i)
		{
			thread_pool_temp = get_thread();
			thread_pool_temp->work_q = _g_work_q;
#			ifdef _WIN32
			if((thread_pool_temp->tid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) minion_func, (void *)thread_pool_temp, 0, NULL)) == NULL)
#			else
			if(pthread_create(&thread_pool_temp->tid, NULL, minion_func, (void *)thread_pool_temp))
#			endif
			{
				perror("Cannot create thread");
				exit(EXIT_FAILURE);
			}
		}
	}
	return 0;
}
TBPGP_EXPORT_SYMBOL void tbpgp_main(void) //API
{
        struct ThreadPool *thread_pool_temp = _g_thread_pool;
        int i = 0;
	void *ret = NULL;

        while(thread_pool_temp != NULL)
        {
#ifdef _WIN32
		WaitForSingleObject(thread_pool_temp->tid, INFINITE);
		GetExitCodeThread(thread_pool_temp->tid, &thread_pool_temp->ret); //anything useful from return? TBD
		CloseHandle(thread_pool_temp->tid);
#else
		ret = (void *)&thread_pool_temp->ret;
                pthread_join(thread_pool_temp->tid, &ret); //anything useful from return? TBD
#endif
                thread_pool_temp = thread_pool_temp->next;
                i++;
        }
        free_ThreadPool(_g_thread_pool);
	if(_g_work_q != NULL)
	{
		free_WorkLoad(_g_work_q->work_load);
#ifndef _WIN32
		pthread_mutex_destroy(&_g_work_q->mutex);
		pthread_cond_destroy(&_g_work_q->cond);
#endif
		free(_g_work_q);
	}
        return;
}
