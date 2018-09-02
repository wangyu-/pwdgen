#include "lib/md5.h"
#include "lib/pbkdf2-sha1.h"
#include "lib/pbkdf2-sha256.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include <string>
using namespace std;

const int it_count=1000*100;
const int inner_output_len=32*255; // this is limited by sha256 and hdkf

char inner_output_buf[inner_output_len+100];

bool char_table[256+100]={0};

char user_passwd[1000]="secret_password";
char hkdf_info[1000]="info_for_password_derive";

//char custom_str[1000]="1234567890abcdefghijklmnopqrstuvwxyz";
char custom_str[1000]="23456789abcdefghijkmnpqrstuvwxyz"; // without 0 o 1 l

int output_len=15;

int init_table()
{
	int custom_len=strlen(custom_str);
	for(int i=0;i<custom_len;i++)
	{
		unsigned char c= custom_str[i];
		char_table[c]=1;
	}
	return 0;
}
int main(int argc, char *argv[])
{
	if(argc!=3&&argc!=4&&argc!=5)
	{
		printf("usage:\n");
		printf(" ./this_program pwd info\n");
		printf(" ./this_program pwd info len\n");
		printf(" ./this_program pwd info len custom_char_list\n");
		exit(-1);
	}

	sscanf(argv[1],"%s",user_passwd);
	sscanf(argv[2],"%s",hkdf_info);
	if(argc>=4)
		sscanf(argv[3],"%d",&output_len);
	if(argc>=5)
		sscanf(argv[4],"%s",custom_str);

	printf("pwd=[%s]\n",user_passwd);
	printf("---------\n");

	printf("info=[%s] len=[%d] str=[%s]\n",hkdf_info,output_len,custom_str);

	int user_passwd_len=strlen(user_passwd);

	unsigned char salt[1000]="";
	char salt_text[1000]="wangyu_personal_salt1";
	md5((uint8_t*)(salt_text),strlen(salt_text),salt);

	unsigned char pbkdf2_output1[1000]="";
	PKCS5_PBKDF2_HMAC_SHA256((uint8_t*)user_passwd,user_passwd_len,salt,16,it_count, 32,pbkdf2_output1);  //TODO argon2 ?

	assert( hkdf_sha256_expand( pbkdf2_output1,32, (unsigned char *)hkdf_info,strlen(hkdf_info), (unsigned char *)inner_output_buf, inner_output_len )  ==0);

	init_table();

	string output="";


	printf("---------\n");

	for(int i=0;i<inner_output_len;i++)
	{
		unsigned char c=inner_output_buf[i];
		//if(char_table[c]) printf("%c",char(c));
		if(char_table[c]) output+=char(c);
	}
	assert(int(output.length())>=output_len);

	for(int i=0;i<output_len;i++)
	{
		printf("%c",output[i]);
	}
	printf("\n");

	printf("---------\n");

	for(int i=0;i<output_len;i++)
	{
		if(i!=0&&i%5==0) printf(" ");
		printf("%c",output[i]);
	}
	printf("\n");
	return 0;
}
