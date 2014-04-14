/*************************************************************************
	> File Name: ticket.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Mon 31 Mar 2014 04:56:32 PM CST
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
int count = 0 , flag = 0 ;
pthread_mutex_t mutex ;
pthread_cond_t cond ;
void *window1_sell(void *arg)
{
	printf("thd1 :\n");
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(count > 0)
		{
			flag = 1 ;
			count-- ;
			printf("thd1 sold 1 , left :%d\n ",count);
			pthread_mutex_unlock(&mutex) ;
			sleep(1);
		}
		else if(flag == 0 && count == 0)
		{
			pthread_cond_wait(&cond , &mutex);
			pthread_mutex_unlock(&mutex);
			sleep(1);
			printf("thd1 begin :\n");

		}
		else if(flag ==1 && count == 0)
		{
			pthread_cond_wait(&cond , &mutex);
			pthread_mutex_unlock(&mutex);

		}
	}
	pthread_exit((void *)1);
}

void *window2_sell(void *arg)
{
	printf("thd2 :\n");
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(count > 0)
		{
			flag = 1 ;
			count-- ;
			printf("thd2 sold 1 , left :%d\n ",count);
			pthread_mutex_unlock(&mutex) ;
			sleep(1);
		}
		else if(flag == 0 && count == 0)
		{
			pthread_cond_wait(&cond , &mutex);
			pthread_mutex_unlock(&mutex);
			sleep(1);
			printf("thd2 begin :\n");

		}
		else if(flag ==1 && count == 0)
		{
			pthread_cond_wait(&cond , &mutex);
			pthread_mutex_unlock(&mutex);

		}
	}
	pthread_exit((void *)2);
}
int main(int argc , char *argv[])
{
	pthread_t thd1 , thd2 ;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond , NULL);
	//创建两个线程
	pthread_create(&thd1 , NULL , window1_sell , NULL);
	pthread_create(&thd2 , NULL , window2_sell , NULL);
	sleep(5);
    //为票数赋值
	while(1)
	{
		pthread_mutex_lock(&mutex);
		//首次放票的情况
		if(flag == 0 && count == 0)
		{
			printf("Begin :\n");
			count = 20 ;
		}
		//继续放票
		else if(flag == 1 && count == 0)
		{
			printf("Begin again :\n");
			count = 10 ;
		}
		pthread_mutex_unlock(&mutex);
		pthread_cond_broadcast(&cond);
		sleep(2);
	}
	int status1 , status2 ;
	pthread_join(thd1 , (void *)&status1);
	pthread_join(thd2 , (void *)&status2);
	printf("%d  %d\n",status1 , status2);
	return 0 ;
}
