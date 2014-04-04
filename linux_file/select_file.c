/*************************************************************************
	> File Name: read_select.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Sat 29 Mar 2014 11:41:32 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/select.h>
int main(int argc , char *argv[])
{
	fd_set fd_rd , fd_rd_back ;
	int fd ;
	struct timeval tm ;
	//文件的打开
	fd = open(argv[1] , O_RDONLY);
	FD_ZERO(&fd_rd);
	//绑定检测输入流
	FD_SET(0 , &fd_rd);
	FD_SET(fd , &fd_rd);
	tm.tv_sec = 10 ;
	tm.tv_usec = 0 ;
	while(1)
	{
		fd_rd_back = fd_rd ;
		select(1024 , &fd_rd_back , NULL , NULL , &tm);
		//从标准输入流中读数据
		if(FD_ISSET(0 , &fd_rd_back))
		{
			char buf[128]="";
			int ired = read(0 , buf , 128);
			buf[ired] = '\0';
			write(1 , buf , strlen(buf));
		}
		//从文件中读数据
		else if(FD_ISSET(fd , &fd_rd_back))
		{
			char buf[128] = "";
			int ired = read(fd , buf , 128);
			buf[ired] = '\0';
			write(1 , buf , strlen(buf));
		}
	}
	return 0 ;
}

