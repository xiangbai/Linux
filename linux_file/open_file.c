/*************************************************************************
	> File Name: filefd.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Mon 24 Mar 2014 05:03:54 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc ,char *argv[])
{
	int fd_write ,fd_read;
	int ch;
	fd_write = open(argv[1] , O_WRONLY | O_CREAT);
	if(fd_write == -1)
	{
		printf("open failed!\n");
		exit(-1);
	}
	while((ch = fgetc(stdin)) != EOF)
	{		
		write(fd_write , &ch , sizeof(ch));
	}
	printf("write over\n");
	close(fd_write);
	fd_read = open(argv[1] , O_RDONLY);
	if(fd_read == -1)
	{
		printf("open failed!\n");
		exit(-1);
	}
	while(read(fd_read , &ch , sizeof(ch)))
	{
		write(1 , &ch , sizeof(ch));
	}
	printf("read over\n");
	close(fd_read);
	return 0 ;
}
