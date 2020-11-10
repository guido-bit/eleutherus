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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<tbpgp/log.h>
#include<event2/event.h>
#include<tbpgp/exported.h>
#include<tbpgp/util.h>
#include<tbpgp/context.h>
#include<tbpgp/thread_pool.h>
#include<tbpgp/p2p.h>
#include<tbpgp/id.h>
#include<event2/thread.h>
#include<event2/listener.h>

static struct Context *_g_context = NULL;

static void init_Context(struct Context *temp)
{
	temp->base = NULL;
	memset(temp->chain_id.uint8s, 0, CHAIN_ID_BYTES); 
	temp->seed_nodes = NULL;
	temp->next = temp->prev = NULL;
	return;
}
static void init_SeedNodes(struct SeedNodes *temp)
{
	temp->seed_node = NULL;
	temp->prev = temp->next = NULL;
	return;
}
static void free_SeedNodes(struct SeedNodes *temp)
{
	struct SeedNodes *temp1 = NULL;

	while(temp != NULL)
	{
		temp1 = temp->next;
		free(temp->seed_node);
		free(temp);
		temp = temp1;
	}
	return;
}
static struct Context *free_Context(struct Context *temp)
{
	struct Context *context_temp = temp->next;

	event_base_loopexit(temp->base, NULL);
	mutex_lock(&temp->mutex[P2P_MUTEX]);
	mutex_unlock(&temp->mutex[P2P_MUTEX]);
	free_SeedNodes(temp->seed_nodes);
	free(temp);

	return context_temp;

}
TBPGP_EXPORT_SYMBOL void tbpgp_context_destroy(struct Context *node) //API
{
	if(node == _g_context)
	{
		node->next->prev = _g_context->prev;
		_g_context = node->next;
		free_Context(node);
	}
	else if(node == _g_context->prev)
	{
		node->prev->next = NULL;
		_g_context->prev = node->prev;
		free_Context(node);
	}
	else
	{
		node->next->prev = node->prev;
		node->prev->next = node->next;
		free_Context(node);
	}
	free_Context(node);
	return;
}
TBPGP_EXPORT_SYMBOL int tbpgp_context_dispatch(struct Context *context)
{
	int ret = 1;

	ret = deploy_minions();
	ret = net_dispatch(context);

	return ret;
}
TBPGP_EXPORT_SYMBOL struct Context *tbpgp_context_new(const char *chain_id_str) //API
{
	struct Context *context_temp = NULL;

	if(chain_id_str != NULL)
	{
		if(!gen_id())
		{
			if(_g_context == NULL)
			{
				_g_context = (struct Context *)malloc(sizeof(struct Context));
				init_Context(_g_context);
				context_temp = _g_context;
			}
			else
			{
				context_temp = _g_context->prev;
				context_temp->next = (struct Context *)malloc(sizeof(struct Context));
				init_Context(context_temp->next);
				context_temp->next->prev = context_temp;
				context_temp = context_temp->next;
			}
			_g_context->prev = context_temp;
		}
		parse_hash(chain_id_str, context_temp->chain_id.uint64s); 

		print_hash_bytes(context_temp->chain_id.uchars, CHAIN_ID_BYTES);
	}
	return context_temp;
}
TBPGP_EXPORT_SYMBOL int tbpgp_context_set_seednodes(struct Context *context, char *seed_nodes)
{
	struct SeedNodes *seed_nodes_temp = NULL;
	char *token = NULL;
	char *saveptr = NULL;

	if((seed_nodes != NULL) && (context != NULL))
	{
		token = strtok_r(seed_nodes, ",", &saveptr);
		while(token != NULL)
		{
			if(token != NULL)
			{
				if(context->seed_nodes == NULL)
				{
					context->seed_nodes = (struct SeedNodes *)malloc(sizeof(struct SeedNodes));
					init_SeedNodes(context->seed_nodes);
					seed_nodes_temp = context->seed_nodes;
				}
				else
				{
					seed_nodes_temp = context->seed_nodes->prev;
					seed_nodes_temp->next = (struct SeedNodes *)malloc(sizeof(struct SeedNodes));
					init_SeedNodes(seed_nodes_temp->next);
					seed_nodes_temp->next->prev = seed_nodes_temp;
					seed_nodes_temp = seed_nodes_temp->next;
				}
				context->seed_nodes->prev = seed_nodes_temp;
				seed_nodes_temp->seed_node = strdup(token);
			}
			token = strtok_r(NULL, ",", &saveptr);
		}
		return 0;
	}
	return 1;
}
TBPGP_EXPORT_SYMBOL void tbpgp_quit(void)
{
	terminate_minions();
	free_id();
	while(_g_context != NULL)
	{
		_g_context = free_Context(_g_context);
	}
	libevent_global_shutdown();
}
