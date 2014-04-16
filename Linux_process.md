# Linux旅程之进程
[TOC]
##进程
###1.进程标识

> OS会为每个进程分配一个唯一的整型ID，做为进程的标识号(pid)。进程除了自身的ID外，还有父进程ID(ppid),所有进程的祖先进程是同一个进程，它叫做init进程，ID为1，init进程是内核自举后的一个启动的进程。init进程负责引导系统、启动守护（后台）进程并且运行必要的程序。
> 进程的pid和ppid可以分别通过函数getpid()和getppid()获得。
```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
   printf("pid:%d   ppid:%d\n",getpid(),getppid());
   return 0;
}
```
###2.进程结构
> Linux系统是一个多进程的系统，它的进程之间具有并行性、互不干扰等特点。也就是说，进程之间是分离的任务，拥有各自的权利和责任。其中，每个进程都运行在各自独立的虚拟地址空间，因此，即使一个进程发生了异常，它也不会影响到系统的其他进程。

Linux中的进程包含3个段，分别为“数据段”、“代码段”和“堆栈段”。

>- “数据段”放全局变量、常数以及动态数据分配的数据空间。数据段分成普通数据段（包括可读可写/只读数据段，存放静态初始化的全局变量或常量）、BSS数据段（存放未初始化的全局变量）以及堆（存放动态分配的数据）。
- “代码段”存放的是程序代码的数据。
- “堆栈段”存放的是子程序的返回地址、子程序的参数以及程序的局部变量等。
###3.进程的创建
> Linux下有四类创建子进程的函数：system(),fork(),exec*(),popen()

**system()函数**
```
#include <stdlib.h>
int system(const char *string);
```

> system函数通过调用shell程序/bin/sh –c来执行string所指定的命令，该函数在内部是通过调用execve(“/bin/sh”,..)函数来实现的。通过system创建子进程后，原进程和子进程各自运行，相互间关联较少。如果system调用成功，将返回0。

**code**

> linux_process  ------> system_main.c && system_up_to_low.c

system函数后面的参数还可以是一个可执行程序，例如：system(“/home/wangxiao/1”);如果想要执行system后面进程的时候，不至于对当前进程进行阻塞，可以利用&将/home/wangxiao/1调到后台运行。

**fork()函数**
```
#include <unistd.h>
pid_t fork(void);
```

> 在linux中fork函数时非常重要的函数，它从已存在进程中创建一个新进程。新进程为子进程，而原进程为父进程。它和其他函数的区别在于：它执行一次返回两个值。其中父进程的返回值是子进程的进程号，而子进程的返回值为0.若出错则返回-1.因此可以通过返回值来判断是父进程还是子进程。
> fork函数创建子进程的过程为：使用fork函数得到的子进程是父进程的一个复制品，它从父进程继承了进程的地址空间，包括进程上下文、进程堆栈、内存信息、打开的文件描述符、信号控制设定、进程优先级、进程组号、当前工作目录、根目录、资源限制、控制终端，而子进程所独有的只有它的进程号、资源使用和计时器等。通过这种复制方式创建出子进程后，原有进程和子进程都从函数fork返回，各自继续往下运行，但是原进程的fork返回值与子进程的fork返回值不同，在原进程中，fork返回子进程的pid,而在子进程中，fork返回0,如果fork返回负值，表示创建子进程失败。

**codo**

> linux_process ------>fork.c

**exec()函数**
```
int execl(const char *path, const char *arg, ...)
```

 - path是包括执行文件名的全路径名
 - arg是可执行文件的命令行参数，多个用，分割注意最后一个参数必须为NULL。

> exec函数族的工作过程与fork完全不同，fork是在复制一份原进程，而exec函数是用exec的第一个参数指定的程序覆盖现有进程空间（也就是说执行exec族函数之后，它后面的所有代码不在执行）。

**code**

> linux_process ------>execl.c

**popen()函数**
```
#include <stdio.h>
FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);
```

 - command为可执行文件的全路径和执行参数；
 - type可选参数为”r”或”w”,如果为”w”,则popen返回的文件流做为新进程的标准输入流，即stdin，如果为”r”,则popen返回的文件流做为新进程的标准输出流。

> 如果type是“r”，（即command命令执行的输出结果作为当前进程的输入结果）。被调用程序的输出就可以被调用程序使用，调用程序利用popen函数返回的FILE*文件流指针，就可以通过常用的stdio库函数（如fread）来读取被调用程序的输出；如果tpye是“w”，（即当前进程的输出结果作为command命令的输入结果）。调用程序就可以用fwrite向被调用程序发送数据，而被调用程序可以在自己的标准输入上读取这些数据。

函数popen()：允许一个程序将另一个程序作为新进程来启动，并可以传递数据给它或者通过它接收数据。command字符串是要运行的程序名。open_mode必须是“r”或“w”。如果open_mode是“r”，被调用程序的输出就可以被调用程序使用，调用程序利用popen函数返回的FILE*文件流指针，就可以通过常用的stdio库函数（如fread）来读取被调用程序的输出；如果open_mode是“w”，调用程序就可以用fwrite向被调用程序发送数据，而被调用程序可以在自己的标准输入上读取这些数据。

> Example1：从标准管道流中读取  打印/etc/profile的内容

```
#include <stdio.h>
int main()
{
	FILE* fp = popen("cat /etc/profile", "r");
    char buf[512] = {0};
    while(fgets(buf, sizeof(buf), fp))
    {
   			puts(buf);
    }
    pclose(fp);
    return 0;
}
```

> Example2：写到标准管道流   统计buf单词数
```
#include<stdio.h>
int main()
{
	char buf[]={"aaa  bbb  ccc  ddd  eee  fff  ggg  hhh"};
	FILE *fp = popen("wc -w", "w");
	fwrite(buf, sizeof(buf), 1, fp);
	pclose(fp);
	return 0;
}
```
**code**
> linux_process ------> popen.c && popen_count_word.c

###4.进程控制与终止
> 用fork函数启动一个子进程时，子进程就有了它自己的生命并将独立运行。
> 如果父进程先于子进程退出，则子进程成为孤儿进程，此时将自动被PID为1的进程（即init）接管。孤儿进程退出后，它的清理工作有祖先进程init自动处理。但在init进程清理子进程之前，它一直消耗系统的资源，所以要尽量避免。

> 如果子进程先退出，系统不会自动清理掉子进程的环境，而必须由父进程调用wait或waitpid函数来完成清理工作，如果父进程不做清理工作，则已经退出的子进程将成为僵尸进程(defunct),在系统中如果存在的僵尸（zombie）进程过多，将会影响系统的性能，所以必须对僵尸进程进行处理。
```
#include <sys/types.h>
#include <sys/wait.h>
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```
eg:
```
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int main(int argc , char *argv[])
{
	int fd_read ;
	int fd_write ;
	fd_read = open(argv[1],O_RDONLY);
	fd_write = open(argv[2],O_WRONLY);
	if(fd_read == -1||fd_write == -1)
	{
		perror("open:");
		exit(-1);
	}
	pid_t pid = fork();
	if(pid > 0)
	{
		while(1)
		{
			char buf_read[128];
			char cmd[128];
			char buf[128];
			bzero(buf_read , 128);
			bzero(cmd , 128);
			bzero(buf , 128);
			int ired ;
			ired = read(fd_read ,buf_read , 128);
			if(strncmp(buf_read , "bye" , 3) == 0)
			{
				sprintf(cmd , "kill -9 %d",getpid());
				system(cmd);
			}
			buf_read[ired] = '\0';
			sprintf(buf , "from client1:%s",buf_read);
			write(1 , buf , strlen(buf));
		}
		pid_t pid_wait ;
		int status ;
		pid_wait = wait(&status);
	}
	else if(pid == 0)
	{
		while(1)
		{
			char buf_write[128];
			char cmd[128];
			char buf[128];
			bzero(buf , 128);
			bzero(buf_write , 128);
			bzero(cmd , 128);
			int ired ;
			ired = read(0 , buf_write , 128);
			if(ired <= 0)
			{
				char *p = "bye";
				write(fd_write , p , strlen(p));
				sprintf(cmd , "kill -9 %d",getppid());
				system(cmd);
			}
			buf_write[ired] = '\0';
			sprintf(buf , "from client3:%s",buf_write);
			write(fd_write , buf , strlen(buf));

		}
	}

}
```
###5.管道
**无名管道(pipe)**
无名管道的特点：

 - 只能在亲缘关系进程间通信（父子或兄弟） 
 - 半双工（固定的读端和固定的写端） 
 - 他是特殊的文件，可以用read、write等，只能在内存中
```
#include <unistd.h> 
int pipe(int fds[2]);
```
> 管道在程序中用一对文件描述符表示，其中一个文件描述符有可读属性，一个有可写的属性。fds[0]是读，fds[1]是写。

**code**

> linux_process ------> pipe.c

管道两端的关闭是有先后顺序的，如果先关闭写端则从另一端读数据时，read函数将返回0，表示管道已经关闭；但是如果先关闭读端，则从另一端写数据时，将会使写数据的进程接收到SIGPIPE信号，如果写进程不对该信号进行处理，将导致写进程终止，如果写进程处理了该信号，则写数据的write函数返回一个负值，表示管道已经关闭。

**命名管道(FIFO)**

> 无名管道只能在亲缘关系的进程间通信大大限制了管道的使用，有名管道突破了这个限制，通过指定路径名的范式实现不相关进程间的通信。

**code**

> linux_process ------> mult_chat
