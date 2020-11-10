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

#ifndef _EXPORTED_H_
#	define _EXPORTED_H_
#	pragma once
#	ifdef _WIN32
#		define WIN32_LEAN_AND_MEAN
#		define _WINSOCKAPI_
#		include<windows.h>
#		define TBPGP_EXPORT_SYMBOL __declspec( dllexport )
#		define THREAD_T HANDLE 
#		define THREAD_MUTEX HANDLE
#		define THREAD_RW_MUTEX SRWLOCK
#		define THREAD_COND CONDITION_VARIABLE
#		define THREAD_FUNC DWORD WINAPI
#	else
#		define TBPGP_EXPORT_SYMBOL
#		define THREAD_T pthread_t
#		define THREAD_MUTEX pthread_mutex_t
#		define THREAD_RW_MUTEX pthread_mutex_t
#		define THREAD_COND pthread_cond_t
#		define THREAD_FUNC void*
#	endif
#endif
