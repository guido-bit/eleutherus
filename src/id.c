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

#include<tbpgp/exported.h>
#include<tbpgp/log.h>
#include<tbpgp/id.h>
#include<sodium.h>
#include<string.h>
#include<stdio.h>

static struct IdKeys *_g_id_keys = NULL;

/* DEBUG UNIT */
void display_ids(void)
{
	int i;
	printf("PRIV KEY : ");
	for(i = 0; i < crypto_sign_SECRETKEYBYTES; ++i)
		printf("%02x", _g_id_keys->priv_key.uint8s[i]);
	printf("\nPUB KEY : ");
	for(i = 0 ; i< crypto_sign_PUBLICKEYBYTES; ++i)
		printf("%02x", _g_id_keys->pub_key.uint8s[i]);
	printf("\n");

}
int gen_id(void)
{
	if(_g_id_keys == NULL)
	{
		_g_id_keys = (struct IdKeys *)malloc(sizeof(struct IdKeys));
		memset(&_g_id_keys->priv_key, 0, crypto_sign_SECRETKEYBYTES);
		memset(&_g_id_keys->pub_key, 0, crypto_sign_PUBLICKEYBYTES);
		if(sodium_init() == -1)
		{
			free(_g_id_keys);
			log_error("OOPS 1");
			return 1;
		}
		if(crypto_sign_keypair(_g_id_keys->pub_key.k, _g_id_keys->priv_key.k) != 0)
		{
			log_error("OOPS 2");
			free(_g_id_keys);
			return 1;
		}
		display_ids();
	}
	return 0;
}
void free_id(void)
{
	if(_g_id_keys != NULL)
	{
		memset(&_g_id_keys->priv_key, 0, crypto_sign_SECRETKEYBYTES);
		memset(&_g_id_keys->pub_key, 0, crypto_sign_PUBLICKEYBYTES);
		free(_g_id_keys);
	}
}
