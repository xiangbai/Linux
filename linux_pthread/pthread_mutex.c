/*************************************************************************
	> File Name: thread_mutex.c
	> Author: wang
	> Mail:xiangbai@qq.com 
	> Created Time: Mon 31 Mar 2014 10:33:16 AM CST
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
int ticket = 10 ;
int count = 0 ;
pthread_mutex_t mutex ;
void *win1_handle(void *arg)
{
	while(pthread_mutex_lock(&mutex) , ticket > 0)
	{
		sleep(5);
		--ticket ;
		count++ ;
		printf("window1 sole 1 , left %d\n",ticket);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	//循环之外时需要在次解锁，以控制最后一次解锁
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
void *win2_handle(void *arg)
{
	while(pthread_mutex_lock(&mutex) , ticket > 0)
	{
		--ticket ;
		count++ ;
		printf("window2 sole 1 , left %d\n",ticket);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
int main(int argc , char *argv[])
{
	pthread_t thread1 , thread2 ;
	pthread_mutex_init(&mutex , NULL);
	pthread_create(&thread1 , NULL , win1_handle , NULL);
	pthread_create(&thread2 , NULL , win2_handle , NULL);
	
	pthread_join(thread1 , NULL);
	pthread_join(thread2 , NULL);
	printf("count : %d\n",count);
	return 0 ;
}
