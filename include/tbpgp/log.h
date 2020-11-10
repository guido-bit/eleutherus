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

#ifndef _LOG_H_
#define _LOG_H_
#include<tbpgp/exported.h>
#ifndef WIN32_LEAN_AND_MEAN
#include <pthread.h>
#endif

enum LogLevels
{
	LOG_TRACE = 0,
	LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_FATAL
};
#define log_trace(...)	print_log(LOG_TRACE, __VA_ARGS__);
#define log_debug(...)	print_log(LOG_DEBUG, __VA_ARGS__);
#define log_info(...)	print_log(LOG_INFO, __VA_ARGS__);
#define log_warn(...)	print_log(LOG_WARN, __VA_ARGS__);
#define log_error(...)	print_log(LOG_ERROR, __VA_ARGS__);
#define log_fatal(...)	print_log(LOG_FATAL, __VA_ARGS__);
/* FUNCS */
void init_log(void);
void print_log(int level, const char *, ...);
#endif
