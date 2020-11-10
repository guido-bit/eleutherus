#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<ctype.h>
#include<sodium.h>
#include<tbpgp/util.h>

#define PP_BUF_LEN 1024

typedef union
{
	uint64_t unit64s[4];
	unsigned char chars[32];
}ChainID;
void read_input(char *buf, int len)
{
	while(fgets(buf, len, stdin) != NULL)
	{
		if(buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';
		break;
	}
}
void display_chainid(unsigned char *chainid)
{
	int i;
	for(i = 0; i < crypto_hash_sha256_BYTES; ++i)
		printf("%02x", chainid[i]);
	printf("\n");
}
void display_pow(const char **pow_list, long *pow_seq, int pow_seq_num)
{
	int i;
	printf("POW : ");
	for(i = 0; i < pow_seq_num; ++i)
	{
		printf("%s",pow_list[pow_seq[i]-1]);
		if(i != pow_seq_num-1)
			printf(" -> ");
	}
	printf("\n");
}
int main(int argc, char **argv)
{
	static const char *pow_list[] = { "OCCAM_00",
		"OCCAM_01",
		"OCCAM_10",
		"OCCAM_11",
		"OCCAM_20",
		"OCCAM_21",
		"OCCAM_22",
		"OCCAM_31",
		"OCCAM_32",
		"SHA2_256",
		"SHA3_256",
		"BLAKE2b",
		NULL };
	const char *pow_list_temp = NULL;
	long pp_num = 1;
	char pp_buf[100];
	int i;
	char **buf = NULL;
	char pow_buf[100];
	char *token, *saveptr;
	long *pow_seq = NULL;
	long pow_seq_num = 0;
	long pow_num = 0;
	unsigned char chainid[crypto_hash_sha256_BYTES];
	crypto_hash_sha256_state chainid_state;

	ChainID id;

	printf("Enter the number of passphrases you want to use (default:1)\n");
	read_input(pp_buf, 100);
	pp_num = strtol(pp_buf, NULL, 10);
	if(pp_num <= 0)
		pp_num = 1;
	buf = (char **)malloc(sizeof(char *)*pp_num);
	for(i = 0; i < pp_num; ++i)
		buf[i] = (char *)malloc(sizeof(char)*PP_BUF_LEN);
	for(i = 0; i < pp_num; ++i)
	{
		printf("Enter passphrase %d (maximum of %d characters allowed) : ", i + 1, PP_BUF_LEN-1);
		read_input(buf[i], PP_BUF_LEN);
	}
	printf("***** POW LIST *****\n");
	pow_list_temp = pow_list[0];
	i = 0;
	while(pow_list_temp != NULL)
	{
		printf("%d) %s\n", i+1, pow_list_temp);
		i += 1;
		pow_num += 1;
		pow_list_temp = pow_list[i];
	}
	printf("********************\n");
	printf("From the above list choose one or a sequence of POW functions for this blockchain (sequence should be separated by ',' or ' ').\n");
	read_input(pow_buf, 100);
	token = strtok_r(pow_buf, ", \n", &saveptr);
	i = 0;
	while(token != NULL)
	{
		pow_seq = realloc(pow_seq, sizeof(long)*(i+1));

		pow_seq[i] = strtol(token, NULL, 10);
		if((pow_seq[i] <= 0) || (pow_seq[i] > pow_num))
		{
			printf("Cannot find pow %ld\n", pow_seq[i]);
			return 0;
		}
		++i;
		token = strtok_r(NULL, ", \n", &saveptr);
	}
	pow_seq_num = i;
	if(pow_seq == NULL)
	{
		printf("POW not specified!!\n");
		return 1;
	}


	/* Calculate chain id SUPPORTS ONLY SHA256 for now*/
	crypto_hash_sha256_init(&chainid_state);
	for(i = 0; i < pp_num; ++i)
		crypto_hash_sha256_update(&chainid_state, (const unsigned char *)buf[i], strlen(buf[i]));
	crypto_hash_sha256_final(&chainid_state, chainid);

	/* PRETTY OUTPUT */
	printf("\n\n\n");
	printf("################################################################################\n");
	printf("ChainId(Hex) : ");
	display_chainid(chainid);
	display_pow(pow_list, pow_seq, pow_seq_num);
	printf("################################################################################\n");

	for(i = 0; i < pp_num; ++i)
		free(buf[i]);
	free(buf);
	free(pow_seq);

	parse_hash("88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589", id.unit64s);
	display_chainid(id.chars);
	
	return 0;
}
