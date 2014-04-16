#  Linux旅程之线程
[TOC]
##线程
###1.线程概述

> 进程是系统中程序执行和资源分配的基本单位。每个进程有自己的数据段、代码段和堆栈段。这就造成进程在进行切换等操作时都需要有比较负责的上下文切换等动作。为了进一步减少处理器的空转时间支持多处理器和减少上下文切换开销，也就出现了线程。
> 
> 线程通常叫做轻量级进程。线程是在共享内存空间中并发执行的多道执行路径，是一个更加接近于执行体的概念，拥有独立的执行序列，是进程的基本调度单元，每个进程至少都有一个main线程。它与同进程中的其他线程共享进程空间｛堆
> 代码 数据 文件描述符 信号等｝，只拥有自己的栈空间，大大减少了上下文切换的开销。
> 线程和进程在使用上各有优缺点：线程执行开销小，占用的CPU少，线程之间的切换快，但不利于资源的管理和保护；而进程正相反。从可移植性来讲，多进程的可移植性要好些。
> 
> 同进程一样，线程也将相关的变量值放在线程控制表内。一个进程可以有多个线程，也就是有多个线程控制表及堆栈寄存器，但却共享一个用户地址空间。要注意的是，由于线程共享了进程的资源和地址空间，因此，任何线程对系统资源的操作都会给其他线程带来影响。
###2.线程的创建和退出
| 类型        | 创建        | 退出   |  等待  |
| --------   | --------   | -----:  | :----:  |
| 多进程| fork()     | exit() |    wait()   |
| 多线程| pthread_create()        |   pthread_exit() |   pthread_join()   |

```
#include <pthread.h>
int pthread_create(pthread_t* thread, pthread_attr_t * attr, void *(*start_routine)(void *), void * arg);
void pthread_exit(void *retval);

pthread_t pthid;
pthread_create(&pthid,NULL,pthfunc,NULL);
或pthread_create(&pthid,NULL,pthfunc,(void*)3);
pthread_exit(NULL);或pthread_exit((void*)3);//3作为返回值被pthread_join函数捕获。
```
eg:
```
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
void *thread_function(void *arg);

char message[] = "Hello World";
int main(int argc , char *argv[])
{
	int res ;
	pthread_t a_thread ;
	void *thread_result ;

	res = pthread_create(&a_thread , NULL , thread_function , (void *)message);
	if(res != 0)
	{
		perror("Thread create failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Waiting for thread to finish ...\n");
	//pthread_join相当于wait等待线程的退出,线程退出时，会有个退出码保存在第二个参数里
	res = pthread_join(a_thread , &thread_result);
	if(res != 0)
	{
		perror("Thread join failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Thread joined , it returned %s\n",(char *)thread_result);
	printf("Message is now %s\n",message);
	exit(EXIT_SUCCESS);

}
void *thread_function(void *arg)
{
	printf("thread_function is running . Argument was %s\n",(char *)arg);
	sleep(3);
	strcpy(message , "Bye!");
	pthread_exit("Thank you for the CPU time");
}
```
###3.线程的同步与互斥
**线程的互斥**

> 在Posix Thread中定义了一套专门用于线程互斥的mutex函数。mutex是一种简单的加锁的方法来控制对共享资源的存取，这个互斥锁只有两种状态（上锁和解锁），可以把互斥锁看作某种意义上的全局变量。为什么需要加锁，就是因为多个线程共用进程的资源，要访问的是公共区间时（全局变量），当一个线程访问的时候，需要加上锁以防止另外的线程对它进行访问，实现资源的独占。
```
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```
锁操作
```
加锁		int pthread_mutex_lock(pthread_mutex_t *mutex)
解锁		int pthread_mutex_unlock(pthread_mutex_t *mutex)
测试加锁	int pthread_mutex_trylock(pthread_mutex_t *mutex)
```
**code**
> linux_pthread ------> pthread_mutex.c
task --> 火车卖票 ， 生产者与消费者问题 ， 挑战读者写者问题

总结：线程互斥mutex：加锁步骤如下：
1. 定义一个全局的pthread_mutex_t lock; 或者用
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  //则main函数中不用init
2. 在main中调用 pthread_mutex_init函数进行初始化
3. 在子线程函数中调用pthread_mutex_lock加锁
4. 在子线程函数中调用pthread_mutex_unlock解锁
5. 最后在main中调用 pthread_mutex_destroy函数进行销毁

**线程的同步**

> 条件变量是利用线程间共享的全局变量进行同步的一种机制，主要包括两个动作：一个线程等待条件变量的条件成立而挂起；另一个线程使条件成立（给出条件成立信号）。为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。
```
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
int pthread_cond_destroy(pthread_cond_t *cond);
```
尽管POSIX标准中为条件变量定义了属性，但在Linux Threads中没有实现，因此cond_attr值通常为NULL，且被忽略。
等待条件有两种方式：无条件等待pthread_cond_wait()和计时等待pthread_cond_timedwait():
```
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
```
pthread_cond_wait之前，往往要用pthread_mutex_lock进行加锁，而调用pthread_cond_wait函数会将锁解开，然后将线程挂起阻塞。直到条件被pthread_cond_signal激发，再将锁状态恢复为锁定状态，最后再用pthread_mutex_unlock进行解锁）。

激发条件有两种形式，
pthread_cond_signal()激活一个等待该条件的线程，存在多个等待线程时按入队顺序激活其中一个；
而pthread_cond_broadcast()则激活所有等待线程
```
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
pthread_mutex_t mutex ;
pthread_cond_t cond ;

void *child1(void *arg)
{
	while(1)
	{
		printf("thread 1 get running\n");
		printf("thread 1 pthread_mutex_lock returns %d\n",pthread_mutex_lock(&mutex));
		pthread_cond_wait(&cond , &mutex);
		printf("thread 1 condition applied\n");
		pthread_mutex_unlock(&mutex);
		sleep(5);
	}
	pthread_exit(NULL);
}
void *child2(void *arg)
{
	while(1)
	{
		sleep(3);
		printf("thread 2 get running\n");
		printf("thread 2 pthread_mutex_lock returns %d\n",pthread_mutex_lock(&mutex));
		pthread_cond_wait(&cond , &mutex);
		printf("thread 2 condition applied\n");
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}
int main(int argc , char *argv)
{
	pthread_t tid1 , tid2 ;
	printf("hello , condition variable test\n");
	pthread_mutex_init(&mutex , NULL);
	pthread_cond_init(&cond , NULL);
	pthread_create(&tid1 , NULL , child1 , NULL);
	pthread_create(&tid2 , NULL , child2 , NULL);
	while(1)
	{
		sleep(2);
		pthread_cancel(tid1);
		sleep(2);
		pthread_cond_signal(&cond);
	}
}
```

**code**

> linux_pthread ------> cond_ticket.c
