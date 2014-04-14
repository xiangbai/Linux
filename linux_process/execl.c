/*************************************************************************
  > File Name: testmain.c
  > Author: wang
  > Mail:xiangbai@qq.com 
  > Created Time: Thu 27 Mar 2014 09:30:02 AM CST
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
int main(int argc , char *argv[])
{
	int ired ;
	struct stat buf ;
	stat(argv[1] , &buf);
	if(S_ISDIR(buf.st_mode))
	{
		execl("/bin/cp" , "cp" , "-r" , argv[1] , argv[2] , NULL);
	}
	else
	{
		execl("/bin/cp" , "cp" , argv[1] , argv[2], NULL);
	}
	return 0 ;
}
