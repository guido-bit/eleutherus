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

#ifndef _TBPGP_H_
#define _TBPGP_H_

#include <tbpgp/exported.h>
/* TYPEDEFS */
typedef struct Context *TbpGpContext;

TBPGP_EXPORT_SYMBOL TbpGpContext tbpgp_context_new(const char *);
TBPGP_EXPORT_SYMBOL void tbpgp_context_destroy(TbpGpContext);
TBPGP_EXPORT_SYMBOL int tbpgp_context_dispatch(TbpGpContext);
TBPGP_EXPORT_SYMBOL void tbpgp_quit(void);
TBPGP_EXPORT_SYMBOL void tbpgp_main(void);
TBPGP_EXPORT_SYMBOL int tbpgp_context_set_seednodes(struct Context *, char *);
TBPGP_EXPORT_SYMBOL void tbpgp_signal_connect(int , void (*)(int));

TBPGP_EXPORT_SYMBOL int merkle_hash(char *); //THIS IS JUST FOR TESTING DEL LATER

#endif
