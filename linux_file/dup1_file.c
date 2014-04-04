/*************************************************************************
	> File Name: client1.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Tue 25 Mar 2014 04:55:40 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
int main(int argc , char *argv[])
{
	int fp = open(argv[1],O_RDONLY);
	char reve_buf[128] = " ";
	int ired ;
	if(fp == -1)
	{
		perror("open failed");
		exit(-1);
	}
	//将文件作为标准输入
	dup2(fp , 0);
	close(fp);
	//从文件中读出内容到数组中
	while(1)
	{
		ired = read(0 , reve_buf , 128);
		reve_buf[ired] = '\0' ;
        //将内容写入到标准输出
		write(1 , reve_buf , strlen(reve_buf));
	}
	return 0 ;
}
