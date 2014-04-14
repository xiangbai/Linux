/*************************************************************************
	> File Name: up_to_low.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Wed 26 Mar 2014 04:30:38 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
void up_to_low(char *str)
{
	int i ;
	for(i = 0 ; i < strlen(str);  ++i)
	{
		if(isupper(str[i]))
		{
			str[i] = tolower(str[i]);
		}
	}
}
int main(int argc , char *argv[])
{
	int index ;
	for(index = 1 ; index < argc ; ++index)
	{
		up_to_low(argv[index]);
		write(1 , argv[index] , strlen(argv[index]));
		printf("\n");
	}
	return 0 ;
}
