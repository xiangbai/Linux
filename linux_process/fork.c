/*************************************************************************
	> File Name: fork1.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Wed 26 Mar 2014 05:27:40 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(int argc , char *argv[])
{
	pid_t pid ;
	pid = fork();
	if(pid > 0)
	{
		printf("parent pid : %d ,parent ppid : %d , pid : %d \n",getpid() , getppid() , pid);
		while(1)
		{
			write(1 , "parent\n" , 7);
			sleep(1);
		}
	}
	else if(pid == 0)
	{

		printf("child pid : %d , ppid : %d , pid : %d \n",getpid() , getppid() , pid);
		while(1)
		{
			write(1 , "childen\n" , 8);
			sleep(2);
		}
	}
	return 0 ;
}
