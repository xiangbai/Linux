/*************************************************************************
	> File Name: file_read.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Mon 24 Mar 2014 05:39:35 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

int main(int argc , char *argv[])
{
	char buffer[128] ;
	int nread ;
	nread = read(0 , buffer , 128);
	if(nread == -1)
	{
		write(2 , "a read error has occurred\n",26);
	}
	if(write(1 , buffer ,nread) != nread)
	{
		write(2 , "a read error has occurred\n",27);
	}
	return 0 ;
}
