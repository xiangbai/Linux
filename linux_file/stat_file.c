/*************************************************************************
	> File Name: filedir.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Mon 24 Mar 2014 11:37:36 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<unistd.h>
int main(int argc , char *argv[])
{
	DIR *pDir ;
	struct dirent *pDirent ;
	struct stat my_stat ;
	char line[255];
	pDir = opendir(argv[1]);
	if(pDir == NULL)
	{
		printf("dir open failed");
		exit(-1);
	}
    while((pDirent = readdir(pDir))!=NULL)
	{
		memset(line , 0 , 255*sizeof(char));
	//	sprintf(line , "pName : %s , pType : %c\n",pDirent->d_name,pDirent->d_type);
	//	puts(line);
		stat(pDirent->d_name , &my_stat);
		printf("st_name : %s , st_mode: %d , st_size : %d\n",pDirent->d_name , my_stat.st_mode,my_stat.st_size);
	}
	closedir(pDir);
}
