/*************************************************************************
	> File Name: findfile.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Mon 24 Mar 2014 03:40:13 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
int main(int argc , char *argv[])
{
	DIR *pDir ;
	struct dirent *pDirent ;
//	char filename[255] ;
	pDir = opendir(argv[1]);
	if(pDir == NULL)
	{
		printf("open failed");
		exit(-1);
	}
	while((pDirent = readdir(pDir)) != NULL)
	{
       //	memset(filename , 0  , 255);
	//	sprintf(filename , "%s" ,pDirent->d_name);
		if(strcmp(pDirent->d_name , argv[2]) == 0)
		{
			printf("find the %s file\n",argv[2]);
			break ;
		}
	}
        if(pDirent == NULL)
	{
		printf("not the %s file\n",argv[2]);
	}
	closedir(pDir);
	return 0 ;
}
