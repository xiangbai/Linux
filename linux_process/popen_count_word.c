/*************************************************************************
	> File Name: count_word.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Thu 27 Mar 2014 10:53:10 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int argc , char *argv[])
{
	int count_word = 0 ;
	int count_num = 0 ;
	FILE *fp ;
	char word[128];
	fp = fopen(argv[1] , "r");
	if(fp == NULL)
	{
		perror("open");
	}

	while(bzero(word , 128) , fscanf(fp , "%s" , word)==1 )
	{
			++count_word ;
	}
	fclose(fp);
	printf("word : %d , num : %d\n",count_word,count_num);
}
