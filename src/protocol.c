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

#include<stdlib.h>
#include<string.h>
#include<tbpgp/thread_pool.h>
#include<tbpgp/log.h>
#include<tbpgp/util.h>
#include<event2/bufferevent.h>
#include<event2/buffer.h>
#include<event2/listener.h>
#include<event2/thread.h>
#include<event2/util.h>
#include<event2/event.h>
static struct WorkLoad *pop_work_load(struct WorkQ *work_q)
{
	struct WorkLoad *work_load_temp = NULL;
	if(work_q != NULL)
	{
		work_load_temp = work_q->work_load;
		work_q->work_load = work_load_temp->next;
		if(work_load_temp->next != NULL)
		{
			work_load_temp->next->prev = work_q->work_load->prev;
		}
	}
	return work_load_temp;
}
void conn_readcb(struct bufferevent *bev, void *user_data)
{
        struct evbuffer *input = bufferevent_get_input(bev);
	size_t nbytes = 0;
	char data[1024];
	
	nbytes = evbuffer_get_length(input);
	evbuffer_remove(input, data, nbytes);
	bufferevent_write(bev, data, nbytes);
}
void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
        if (events & BEV_EVENT_EOF) {
                printf("Connection closed.\n");
        } else if (events & BEV_EVENT_ERROR) {
                printf("Got an error on the connection: %s\n",
                    strerror(errno));/*XXX win32*/
        }
        /* None of the other events can happen here, since we haven't enabled
         * timeouts */
        bufferevent_free(bev);
}
void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{
        struct event_base *base = user_data;
	struct bufferevent *bev;
	char data[1024];
	int *mydata = NULL;
	char *ipstr;

	ipstr = (char *)malloc(sizeof(char)*MAX_IP_STR_LEN);
	log_info("Got connection from :%s", get_ip_str((void *)sa, ipstr, MAX_IP_STR_LEN, UTIL_SOCKADDR));

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	//bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, data);
        bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, data);
        bufferevent_enable(bev, EV_READ | EV_WRITE);

		/*DEBUG UNIT DELETE LATER*/
		mydata = (int *)malloc(sizeof(int));
		*mydata = 5647;
		log_debug("MYDATA is %d", *mydata);
	push_work_load((void *)mydata);

        //bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

void *minion_func(void *data)
{
	struct ThreadPool *thread_pool_temp = (struct ThreadPool *)data;
	struct WorkQ *work_q_temp = thread_pool_temp->work_q;
	struct WorkLoad *work_load_temp = NULL;
	int *mydata;

	while(1)
	{
		log_debug("LOCKING NOW");

#ifdef _WIN32
		AcquireSRWLockShared(&work_q_temp->mutex);
#else
		mutex_lock(&work_q_temp->mutex);
#endif
#ifdef _WIN32
		SleepConditionVariableSRW(&work_q_temp->cond, &work_q_temp->mutex, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
#else
		pthread_cond_wait(&work_q_temp->cond, &work_q_temp->mutex);
#endif
		log_debug("WAITING ON SIGNAL");

		if(thread_pool_temp->thread_status == THREAD_STATUS_TERMINATE)
		{
#ifdef _WIN32
			ReleaseSRWLockShared(&work_q_temp->mutex);
#else
			mutex_unlock(&work_q_temp->mutex);
#endif
			break;
		}

		log_debug("WTF 1");
		work_load_temp = pop_work_load(work_q_temp);
#ifdef _WIN32
		ReleaseSRWLockShared(&work_q_temp->mutex);
#else
		mutex_unlock(&work_q_temp->mutex);
#endif
		if(work_load_temp != NULL)
		{
			mydata = (int *)work_load_temp->data;
			log_debug("DOING SOME WORK %d", *mydata);
			/* do your stuff here */
			log_debug("DONE WITH WORK");
		}
		free(work_load_temp->data);
		free(work_load_temp);
	}
	log_debug("ENTIRING TO TERMINATE");
#ifndef _WIN32
	pthread_exit(NULL);
#endif
	return NULL;
}
