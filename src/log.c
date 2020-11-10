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

#include<tbpgp/log.h>
#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include<time.h>

static const char *log_levels[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
static struct
{
	int log_level;
	FILE *log_fp;
	THREAD_MUTEX mutex;
}tbpgp_log;

void init_log(void)
{
	tbpgp_log.log_fp = NULL;
#ifdef _WIN32
	tbpgp_log.mutex = CreateMutex(NULL, FALSE, NULL);
#else
	pthread_mutex_init(&tbpgp_log.mutex, NULL);
#endif
	return;
}
void print_log(int level, const char *fmt, ...)
{
	time_t t = time(NULL);
	struct tm *local_time = NULL;
#ifdef _WIN32
	struct tm ct;
	localtime_s(&ct, &t);
	local_time = &ct;
#else
	local_time = localtime(&t);
#endif
	va_list args;
	char buf[32];
#ifdef _WIN32
	WaitForSingleObject(tbpgp_log.mutex, INFINITE);
#else
	pthread_mutex_lock(&tbpgp_log.mutex);
#endif
	if(tbpgp_log.log_fp == NULL)
	{
		buf[strftime(buf, sizeof(buf), "[%Y-%m-%d (%H:%M:%S)]", local_time)] = '\0';
		fprintf(stderr, "%s %-5s : ", buf, log_levels[level]);
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
		fprintf(stderr, "\n");
		fflush(stderr);
	}
#ifdef _WIN32
	ReleaseMutex(tbpgp_log.mutex);
#else
	pthread_mutex_unlock(&tbpgp_log.mutex);
#endif
}
