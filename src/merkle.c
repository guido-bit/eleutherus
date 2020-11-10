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

#include<sodium.h>
#include<errno.h>
#include<stdio.h>
#include<stdint.h>
#include<inttypes.h>

void print_hash(unsigned char *hash)
{
	int i = 0;

	printf("FINAL_HASH : ");
	for(i = 0; i < crypto_hash_sha256_BYTES; ++i)
	{
		printf("%02" PRIx8, hash[i]);
	}
	printf("\n");
}

int merkle_hash(char *file_path)
{
	FILE *fp = NULL;
	int64_t nread = 0;
	uint8_t buf[8193];
	crypto_hash_sha256_state state;
	unsigned char hash[crypto_hash_sha256_BYTES];

	if(file_path == NULL)
		return 1;

	fp = fopen(file_path, "rb");
	if(!fp)
	{
		perror("Cannot open file");
		return 1;
	}
	crypto_hash_sha256_init(&state);
	while(1)
	{
		nread = fread(buf, 1, 8192, fp);
		crypto_hash_sha256_update(&state, buf, nread);
		if(nread < 8192)
			break;
	}
	crypto_hash_sha256_final(&state, hash);
	print_hash(hash);
	
	fclose(fp);

	return 0;
}
