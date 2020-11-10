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

#ifndef _PEER_H_
#define _PEER_H_
#include<stdint.h>
#define PRIV_KEY_BYTES_LEN 64
#define PRIV_KEY_BITS_LEN (PRIV_KEY_BYTES_LEN*8)
#define PUB_KEY_BYTES_LEN 32
#define PUB_KEY_BITS_LEN (PUB_KEY_BYTES_LEN*8)
#define PEERID_BYTES_LEN PUB_KEY_BYTES_LEN
#define PEERID_BITS_LEN PUB_KEY_BITS_LEN
typedef union
{
        uint8_t uint8s[PRIV_KEY_BYTES_LEN];
        uint16_t uint16s[PRIV_KEY_BYTES_LEN/sizeof(uint16_t)];
        uint32_t uint32s[PRIV_KEY_BYTES_LEN/sizeof(uint32_t)];
        uint64_t uint64s[PRIV_KEY_BYTES_LEN/sizeof(uint64_t)];
        unsigned char k[PRIV_KEY_BYTES_LEN];
}PrivKey;
typedef union
{
        uint8_t uint8s[PUB_KEY_BYTES_LEN]; //32
        uint16_t uint16s[PUB_KEY_BYTES_LEN/sizeof(uint16_t)]; //16
        uint32_t uint32s[PUB_KEY_BYTES_LEN/sizeof(uint32_t)]; //8
        uint64_t uint64s[PUB_KEY_BYTES_LEN/sizeof(uint64_t)]; //4
        unsigned char k[PUB_KEY_BYTES_LEN]; //32
}PubKey;
struct IdKeys
{
	PrivKey priv_key;
	PubKey pub_key;
};
/* FUNCS */
int gen_id(void);
void display_ids(void);
void free_id(void);
#endif

