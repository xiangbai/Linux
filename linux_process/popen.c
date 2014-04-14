/*************************************************************************
	> File Name: testmain.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Thu 27 Mar 2014 11:17:53 AM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main(int argc , char *argv[])
{
	char filename[128] ;
	char cmd[128];
	char result[128];
	FILE *fp ;
	bzero(filename , 128);
	//表示从标准输入中读入操作的文件名
	while(fgets(filename , 128 , stdin) != NULL)
	{
		bzero(cmd , 128);
		//将命令进程进行格式输出
		sprintf(cmd , "./count_word %s", filename);
  		fp = popen(cmd , "r");
		bzero(result , 128);
		fgets(result , 128 , fp);
		puts(result);
		pclose(fp);
	}
	return 0 ;
}
