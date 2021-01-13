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
#include <stdarg.h>
using namespace std;

const int it_count=100*1000;
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
	memset(char_table,0,sizeof(char_table));
	int custom_len=strlen(custom_str);
	for(int i=0;i<custom_len;i++)
	{
		unsigned char c= custom_str[i];
		char_table[c]=1;
	}
	return 0;
}

string myprint_output;
void myprintf(const char* str, ...)
{
	char tmp[1000];
	tmp[0]=0;
	va_list vlist;
	va_start(vlist, str);
	vsprintf(tmp, str, vlist);
	va_end(vlist);
	myprint_output+=tmp;

}
extern "C" {
char * pwdgen(char * in_pwd,char * in_info, char * in_len, char * in_char_list)
{
	myprint_output.clear();
	if(strlen(in_pwd)==0||strlen(in_info)==0)
	{
		myprintf("usage:\n");
		myprintf(" ./this_program pwd info\n");
		myprintf(" ./this_program pwd info len\n");
		myprintf(" ./this_program pwd info len char_list\n");
		return (char *)myprint_output.c_str();
	}
	sscanf(in_pwd,"%s",user_passwd);
	sscanf(in_info,"%s",hkdf_info);

	if(strlen(in_len)!=0)
		sscanf(in_len,"%d",&output_len);
	if(strlen(in_char_list)!=0)
		sscanf(in_char_list,"%s",custom_str);

	myprintf("pwd=[%s]\n",user_passwd);
	myprintf("---------\n");

	myprintf("info=[%s] len=[%d] str=[%s]\n",hkdf_info,output_len,custom_str);

	int user_passwd_len=strlen(user_passwd);

	unsigned char salt[1000]="";
	char salt_text[1000]="wangyu_personal_salt1";
	md5((uint8_t*)(salt_text),strlen(salt_text),salt);

	unsigned char pbkdf2_output1[1000]="";
	PKCS5_PBKDF2_HMAC_SHA256((uint8_t*)user_passwd,user_passwd_len,salt,16,it_count, 32,pbkdf2_output1);  //TODO argon2 ?

	assert( hkdf_sha256_expand( pbkdf2_output1,32, (unsigned char *)hkdf_info,strlen(hkdf_info), (unsigned char *)inner_output_buf, inner_output_len )  ==0);

	init_table();

	string output="";


	myprintf("---------\n");

	for(int i=0;i<inner_output_len;i++)
	{
		unsigned char c=inner_output_buf[i];
		//if(char_table[c]) printf("%c",char(c));
		if(char_table[c]) output+=char(c);
	}
	assert(int(output.length())>=output_len);

	for(int i=0;i<output_len;i++)
	{
		myprintf("%c",output[i]);
	}
	myprintf("\n");

	myprintf("---------\n");

	for(int i=0;i<output_len;i++)
	{
		if(i!=0&&i%5==0) myprintf(" ");
		myprintf("%c",output[i]);
	}
	myprintf("\n");
	return (char *)myprint_output.c_str();

}
}
int main(int argc, char *argv[])
{
	if(argc!=3&&argc!=4&&argc!=5)
	{
		exit(-1);
	}
	char * res;
	char empty[5]={0};
	if(argc==3)
		res=pwdgen(argv[1],argv[2],empty,empty);
	if(argc==4)
		res=pwdgen(argv[1],argv[2],argv[3],empty);
	if(argc==5)
		res=pwdgen(argv[1],argv[2],argv[3],argv[4]);

	printf("%s",res);
	return 0;
}
