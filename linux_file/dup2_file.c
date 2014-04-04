/*************************************************************************
  > File Name: client2.c
  > Author: wang
  > Mail:xiangbai@qq.com 
  > Created Time: Tue 25 Mar 2014 04:58:08 PM CST
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
	int fp = open(argv[1] , O_WRONLY | O_TRUNC);
	char buf[128] = " ";
	char send_buf[128]=" ";
	int ired ;
	if(fp == -1)
	{
		perror("open failed");
		exit(-1);
	}
	//将文件重定向到标准输出中
	dup2(fp , 1);
	close(fp);
	//从标准输入中读入数据放入buf当中
	while(1){
		ired = read(0 , buf , 128);
		buf[ired] = '\0';
		//格式化数据
		sprintf(send_buf , "client2:%s" , buf);
		//将数据写入到重定向如文件中
		write(1 , send_buf , strlen(send_buf)) ;
	}
	close(1);
	return 0 ;
}
