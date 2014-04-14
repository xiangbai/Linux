/*************************************************************************
	> File Name: testmain.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Wed 26 Mar 2014 04:35:48 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
int main(int argc , char *argv[])
{
	char cmd[128];
	char str[128];
	int ired ;
	while(memset(cmd , 0 , sizeof(cmd)) , (ired = read(0 , cmd , sizeof(cmd))) > 0)
	{
		cmd[ired] = '\0';
		memset(str , 0  , sizeof(str));
		sprintf(str , "./up_to_low %s",cmd) ;
	//	write(1 , str , strlen(str));
		system(str);
	}
	return 0 ;
}
