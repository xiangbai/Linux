/*************************************************************************
	> File Name: pipe.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Thu 27 Mar 2014 03:45:01 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
int main(int argc , char *argv[])
{
	int status ;
	int fds1[2] ;
	int fds2[2] ;
	pid_t pid ;
	pipe(fds1);
	pipe(fds2);
	pid = fork();
	if(pid > 0)
	{
		char buf[128];
		char *p = "hello world";
		printf("hello world\n");
		memset(buf ,0 , 128);
		//父进程向第一个管道写入数据
		write(fds1[1] , p , strlen(p));
		//父进程向第二个管道读数据
	    int ired = read(fds2[0] , buf , 128);
		printf("parent receive : %s\n" , buf);
		pid_t pid_wait = wait(&status);
		printf("pid_wait : %d , status : %d\n",pid_wait , status);

	}else if(pid == 0)
	{
		printf("---------");
		char buf[128] ;
		char *p = "goodbey" ;
		memset(buf ,0 , 128);
		//子进程向第一个管道读数据
		int ired = read(fds1[0] , buf , 128);
		printf("---------------\n");
		printf("from parent : %s\n",buf);
		//子进程开始向第二个管道写入数据
		write(fds2[1] , p , strlen(p)) ;
		//子进程退出
		exit(1);
	}
	return 0 ;
}
