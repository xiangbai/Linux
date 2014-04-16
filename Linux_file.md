# Linux旅程之文件
[TOC]
##Linux文件操作
###1.基于文件描述符
**文件描述符**
> 内核为每个进程维护一个已打开文件的记录表，文件描述符是一个较小的正整数（0—1023），它代表记录表的一项，通过文件描述符和一组基于文件描述符的文件操作函数，就可以实现对文件的读、写、创建、删除等操作。

**文件函数**

> open（打开）、creat（创建）、close（关闭）、read（读取）、write（写入）、ftruncate（改变文件大小）、lseek（定位）、fsync（同步）、fstat（获取文件状态）、fchmod（权限）、flock（加锁）、fcntl（控制文件属性）、dup（复制）、dup2、select(文件监听) 和 ioctl

###2.打开、创建和关闭文件
```
#include <sys/types.h>   //头文件
#include <sys/stat.h>
#include <fcntl.h>
//文件名  打开方式
int open(const char *pathname, int flags)；
//文件名  打开方式  权限
int open(const char *pathname, int flags, mode_t mode);
//文件名  权限   //现在已经不常用了
int creat(const char *pathname, mode_t mode);	
//关闭文件描述符，是先前由open或creat创建文件时的返回值
int close(int fd);
```

> 文件使用完毕后，应该调用close关闭它，一旦调用close，则该进程对文件所加的锁全都被释放，并且使文件的打开引用计数减1，只有文件的打开引用计数变为0以后，文件才会被真正的关闭。

creat函数等价于：
`open(pathname,O_CREAT|O_TRUNC|O_WRONLY,mode);`
flags的可选项：
> O_RDONLY   以只读的方式打开
> O_WRONLY  以只写的方式打开
> O_RDWR      以读写的方式打开
> O_CREAT      如果文件不存在，则创建文件
> O_TRUNC     如果文件存在，将文件的长度截至0
> O_APPEND    已追加的方式打开文件，每次调用write，文件指针自动先移动到尾，用于多进程写同一个文件的情况。

**code**

> linux_file ------>open_file.c

###3.读写文件
```
#include <unistd.h>
//文件描述符  缓冲区  长度
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

> 对于read和write函数，出错返回-1，读取完了之后，返回0， 其他情况返回读写的个数。

**code**

> linux_file ------>rw_file.c
###4.改变文件大小
```
#include <unistd.h>
int ftruncate(int fd, off_t length);
```
> 函数ftruncate会将参数fd指定的文件大小改为参数length指定的大小。参数fd为已打开的文件描述词，而且必须是以写入模式打开的文件。如果原来的文件大小比参数length大，则超过的部分会被删去。
> 返回值 执行成功则返回0，失败返回-1。
```
int main()
{
    int fd = open("a.txt", O_WRONLY);
    ftruncate(fd, 1000);
    close(fd);
    return 0;
}
```
###5.文件定位
函数lseek将文件指针设定到相对于whence，偏移值为offset的位置
```
#include <sys/types.h>
#include <unistd.h>
//fd文件描述词
off_t lseek(int fd, off_t offset, int whence);
```
whence 可以是下面三个常量的一个
> SEEK_SET 从文件头开始计算 
SEEK_CUR 从当前指针开始计算 
SEEK_END 从文件尾开始计算

利用该函数可以实现文件空洞（对一个新建的空文件，可以定位到偏移文件开头1024个字节的地方，在写入一个字符，则相当于给该文件分配了1025个字节的空间，形成文件空洞）通常用于多进程间通信的时候的共享内存。
```
int main()
{
    int fd = open("c.txt", O_WRONLY | O_CREAT);
    lseek(fd, 1024, SEEK_SET);
    write(fd, "a", 1);
    close(fd);
    return 0;
}
```
###6.获取文件信息
可以通过fstat和stat函数获取文件信息，调用完毕后，文件信息被填充到结构体struct stat变量中，函数原型为：
```
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//文件名  stat结构体指针
int stat(const char *file_name, struct stat *buf);  
//文件描述词   stat结构体指针
int fstat(int fd, struct stat *buf);  			
```
struct stat结构体，可以man 2 stat查询
**codo**

> linux_file ------>stat_file.c

```
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
int main()
{
    int fd = open("/home/wangxiao/0926/a.txt", O_RDONLY);
    if(fd == -1)
    {
        perror("open error");
        exit(-1);
    }
    struct stat buf;
    int iRet = fstat(fd, &buf);
    if(iRet == -1)
    {
        perror("fstat error");
        exit(-1);
    }
    if(S_ISREG(buf.st_mode))
    {
        printf("regular file!\n");
    }
    if(S_ISDIR(buf.st_mode))
    {
        printf("directory!\n");
    }
    if(S_ISLNK(buf.st_mode))
    {
        printf("link file!\n");
}
printf("the size of file is : %d\n", buf.st_size);
time_t tt = buf.st_atime;
struct tm *pT = gmtime(&tt);
printf("%4d-%02d-%02d   %02d:%02d:%02d\n", (1900+pT->tm_year), (1+pT->tm_mon), pT->tm_mday, (8+pT->tm_hour), pT->tm_min, pT->tm_sec);
//  printf("the last access time is : %d\n", buf.st_atime);
close(fd);
return 0;
}
```
###7.目录文件
```
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
```
**code**

> linux_file ------>dir_file.c
###8.文件描述符的复制
> 系统调用函数dup和dup2可以实现文件描述符的复制，经常用来重定向进程的stdin(0),stdout(1),stderr(2)。
> dup返回新的文件描述符（没有使用的文件描述符的最小编号）。这个新的描述符是旧文件描述符的拷贝。这意味着两个描述符共享同一个数据结构。
> dup2允许调用者用一个有效描述符(oldfd)和目标描述符(newfd)，函数成功返回时，目标描述符将变成旧描述符的复制品，此时两个文件描述符现在都指向同一个文件，并且是函数第一个参数（也就是oldfd）指向的文件。
```
#include <unistd.h>   //头文件包含
int dup(int oldfd);
int dup2(int oldfd, int newfd);
```
文件描述符的复制是指用另外一个文件描述符指向同一个打开的文件，它完全不同于直接给文件描述符变量赋值。
```
char szBuf[32];
int fd=open(“./a.txt”,O_RDONLY);
int fd2=fd;  //类似于C语言的指针赋值，当释放掉一个得时候，另一个已经不能操作了 
close(fd);	//导致文件立即关闭		
//读取失败	
printf(“read:%d\n”,read(fd2),szBuf,sizeof(szBuf)-1);
close(fd2);  //无意义
```
在此情况下，两个文件描述符变量的值相同，指向同一个打开的文件，但是内核的文件打开引用计数还是为1，所以close(fd)或者close(fd2)都会导致文件立即关闭掉。
```
char szBuf[32];
int fd=open(“./a.txt”,O_RDONLY);
int fd2=dup(fd);	//内核的文件打开引用计算+1，变成2了
close(fd);	//当前还不会导致文件被关闭，此时通过fd2照样可以访问文件
printf(“read:%d\n”,read(fd2),szBuf,sizeof(szBuf)-1);
close(fd2);	//内核的引用计数变为0，文件正式关闭
```
**code**
> linux_file ------>dup1_file.c , dup2_file.c
```
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc,char *argv[])
{
    	char szBuf[32]={0};
    	int fda=open("./a.txt",O_RDWR); //假设a.txt的内容为：hello world
   	int fdaa=dup(fda);
    	read(fda,szBuf,4);
    	puts(szBuf);		//关闭之前先输入原来的内容
    	close(fda);	
      //lseek(fdaa, 0, SEEK_SET);
    	read(fdaa,szBuf,sizeof(szBuf));
    	puts(szBuf);		//输出现在的内容
    	close(fdaa);
}
```
> 解析：假设a.txt中的内容为：hello world.上面的例子会发现第一次输出的结果是hell。关闭close(fda)的时候，文件实际上还没有真正的关闭，此时文件指针已经向后移动了。执行第二次read命令将o world读出来，最后关闭fdaa。
```
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
int main()
{
        int fd = open("a.txt", O_WRONLY | O_CREAT);
        if(fd == -1)
        {
                perror("open error");
                exit(-1);
        }
	    printf("\n");	//必不可少
        close(1);
        int fd2 = dup(fd);
        close(fd);
```
> 该程序首先打开了一个文件，返回一个文件描述符，因为默认的就打开了0,1,2表示标准输入，标准输出，标准错误输出。而用close(1);则表示关闭标准输出，此时这个文件描述符就空着了。后面又用dup，此时dup(fd);则会复制一个文件描述符到当前未打开的最小描述符，此时这个描述符为1.后面关闭fd自身，然后在用标准输出的时候，发现标准输出重定向到你指定的文件了。那么printf所输出的内容也就直接输出到文件了。
###9.I/O多路转接模型
> I/O多路转接模型：在这种模型下，如果请求的I/O操作阻塞，且它不是真正阻塞I/O，而是让其中的一个函数等待，在这期间，I/O还能进行其他操作。如本节要介绍的select()函数，就是属于这种模型。
```
#include <sys/select.h>
#include <sys/time.h>
int select(int maxfd, fd_set *readset,fd_set *writeset, fd_set *exceptionset, const struct timeval * timeout);
```
> maxfd：最大的文件描述符(其值应该为最大的文件描述符字 + 1) readset：	内核读操作的描述符字集合
> writeset：内核写操作的描述符字集合 
exceptionset：内核异常操作的描述符字集合
> timeout：等待描述符就绪需要多少时间。NULL代表永远等下去，一个固定值代表等待固定时间，0代表根本不等待，检查描述字之后立即返回。

fd_set集合：

> void FD_ZERO(fd_set *fdset);  /* 将所有fd清零 */ 
void FD_SET(int fd,fd_set *fdset); 	/* 增加一个fd */
 void FD_CLR(int fd, fd_set *fdset); 	/*删除一个fd */
 int FD_ISSET(int fd, fd_set *fdset); 	/* 判断一个fd是否有设置 */

**code**
> linux_file ------>select_file.c
