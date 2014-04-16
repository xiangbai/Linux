# Linux旅程之网络编程
[TOC]
##1.协议概述
###1.OSI参考模型
协议protocol：通信双方必须遵循的规矩 由iso规定  rpc文档

 **osi参考模型：**
（应-表-会-传-网-数-物） 

应用层 表示层 会话层 传输层 网络层 数据链路层 物理层

**tcp/ip模型4层:**

应用层{http超文本传输协议   ftp文件传输协议  telnet远程登录   ssh安全外壳协议  stmp简单邮件发送   pop3收邮件}
传输层{tcp传输控制协议,udp用户数据包协议} 
网络层{ip网际互联协议 icmp网络控制消息协议 igmp网络组管理协议}
网络接口层{arp地址转换协议,rarp反向地址转换协议，mpls多协议标签交换}
TCP协议：传输控制协议 面向连接的协议 能保证传输安全可靠 速度慢（有3次握手）
UDP协议：用户数据包协议 非面向连接  速度快 不可靠
通常是ip地址后面跟上端口号：ip用来定位主机  port区别应用（进程）
http的端口号80  ssh-->22  telnet-->23  ftp-->21  用户自己定义的通常要大于1024

> TCP/IP协议族的每一层的作用：
>  
 - 网络接口层：负责将二进制流转换为数据帧，并进行数据帧的发送和接收。要注意的是数据帧是独立的网络信息传输单元。

>  - 网络层：负责将数据帧封装成IP数据报，并运行必要的路由算法。

>  - 传输层：负责端对端之间的通信会话连接和建立。传输协议的选择根据数据传输方式而定。

>  - 应用层：负责应用程序的网络访问，这里通过端口号来识别各个不同的进程。

（1）对数据可靠性的要求
	对数据要求高可靠性的应用需选择TCP协议，如验证、密码字段的传送都是不允许出错的，而对数据的可靠性要求不那么高的应用可选择UDP传送。
（2）应用的实时性
	TCP协议在传送过程中要使用三次握手、重传确认等手段来保证数据传输的可靠性。使用TCP协议会有较大的时延，因此不适合对实时性要求较高的应用，如VOIP、视频监控等。相反，UDP协议则在这些应用中能发挥很好的作用。
（3）网络的可靠性
	由于TCP协议的提出主要是解决网络的可靠性问题，它通过各种机制来减少错误发生的概率。因此，在网络状况不是很好的情况下需选用TCP协议（如在广域网等情况），但是若在网络状况很好的情况下（如局域网等）就不需要再采用TCP协议，而建议选择UDP协议来减少网络负荷。

##2.网络编程

> 使用端口号和网络地址的组合可以唯一的确定整个网络中的一个网络进程。

###1.套接口

> Linux中的网络编程是通过socket接口来进行的。socket是一种特殊的I/O接口，它也是一种文件描述符。它是一种常用的进程之间通信机制，通过它不仅能实现本地机器上的进程之间的通信，而且通过网络能够在不同机器上的进程之间进行通信。
> 
> 每一个socket都用一个半相关描述{协议、本地地址、本地端口}来表示；一个完整的套接字则用一个相关描述{协议、本地地址、本地端口、远程地址、远程端口}来表示。socket也有一个类似于打开文件的函数调用，该函数返回一个整型的socket描述符，随后的连接建立、数据传输等操作都是通过socket来实现的。

**socket类型**
（1）流式socket（SOCK_STREAM）	用于TCP通信
	流式套接字提供可靠的、面向连接的通信流；它使用TCP协议，从而保证了数据传输的正确性和顺序性。
（2）数据报socket（SOCK_DGRAM）	用于UDP通信
	数据报套接字定义了一种无连接的服务，数据通过相互独立的报文进行传输，是无序的，并且不保证是可靠、无差错的。它使用数据报协议UDP。
（3）原始socket （SOCK_RAW）		用于新的网络协议实现的测试等
	原始套接字允许对底层协议如IP或ICMP进行直接访问，它功能强大但使用较为不便，主要用于一些协议的开发。 
```
<netinet/in.h>
```
```
struct sockaddr 
{
    	unsigned short sa_family; 	/*地址族*/
    	char sa_data[14]; 	//14字节的协议地址，包含该socket的IP地址和端口号
};
struct sockaddr_in 
{
    	short int sa_family; 		/*地址族*/
    	unsigned short int sin_port; 	/*端口号*/
    	struct in_addr sin_addr; 		/*IP地址*/
    	unsigned char sin_zero[8]; 	/*填充0 以保持与struct sockaddr同样大小*/
};
struct in_addr
{
		unsigned long int  s_addr;	/* 32位IPv4地址，网络字节序 */
};
```

**数据存储优先顺序转换**

> 计算机数据存储有两种字节优先顺序：高位字节优先（称为大端模式）和低位字节优先（称为小端模式）。内存的低地址存储数据的低字节，高地址存储数据的高字节的方式叫小端模式。内存的高地址存储数据的低字节，低地址存储数据高字节的方式称为大端模式。

eg:对于内存中存放的数0x12345678来说

> 如果是采用大端模式存放的，则其真实的数是：0x12345678 
如果是采用小端模式存放的，则其真实的数是：0x78563412

区分两个概念：

 - 网络字节序：也称为大端模式(字数据的高字节存储在低地址中，而字数据的低字节则存放在高地址中)，高位字节优先。
 - 本地字节序：也称为小端模式(低地址中存放的是字数据的低字节，高地址存放的是字数据的高字节)，低位字节优先。

> eg : 0x1234在Little-endian模式CPU内存中的存放方式为：  
>          内存地址     0x4000      0x4001
>          存放内容       0x34          0x12 
而在Big-endian模式CPU内存中的存放方式为：
>           内存地址    0x4000      0x4001
>           存放内容       0x12         0x34

本地字节序可能是小端模式，也可能是大端模式。而端口号和IP地址都是以网络字节序(大端)存储的，因此需要将本地字节序和网络字节序相互对应起来。

> htons(),ntohs(),htonl()和ntohl().这四个地址分别实现网络字节序和主机字节序的转化，这里的h代表host,n代表network，s代表short，l代表long。通常16位的IP端口号用s代表，而IP地址用l来代表。

```
#include<stdio.h>
#include <string.h>
typedef union tag
{
	int a ;
	char buf[4];
}IP;
int reverse(int src) 
{
	int b ;
	b = ((src & 0xff) << 24)|((src & 0xff000000)>>24)|(( src & 0xff00) <<8)|((src & 0xff0000)>>8);
	return b ;
}

	//int a = 0x0fc8d93a ;
void  ntoa(int ip, char* ip_addr)
{
	memset(ip_addr, 0,strlen(ip_addr) );
	int a[4] , index;
	for(index = 0 ; index < 4 ; index ++)
	{
		a[index] = (int)(ip & 0xff);
		ip = ip >> 8 ;
	}
	sprintf(ip_addr, "%d.%d.%d.%d",a[0], a[1], a[2],a[3]);
}
int aton(char* ip_addr)
{
	int buf[4] ;
	memset(buf, 0, 16);
	sscanf(ip_addr,"%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]);
	int index = 0 ;
	int result = 0 ;
	for(;index <  4; index ++)
	{
		result |= (buf[index] & 0xff) ;
		if(index < 3){
		result = result << 8 ;}
	}
	return result ;

}
int main()
{
	int a = 0x0fc8d93a ;
	int b ;
	char buf[128]="";
	printf("before reverse: %08x\n", a);
	b = reverse(a);
	printf("after reverse: %08x\n", b);
	ntoa(a, buf);
	printf("ip_add: %s\n", buf);
	int c = aton(buf);
	printf("ip: %08x\n",c);
	return 0 ;

}
```
```
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//a = 0x 0f c8 d9 3a ---> b = 0x 3a d9 c8 0f 
int reverse_ip(int a)
{
	int b ;
	b = ((a & 0xff) << 24) | ((a & 0xff000000) >> 24) | ((a & 0x0000ff00) << 8) | ((a & 0x00ff0000)>>8);
	return b ;
}
// ip = 0x 0f c8 d9 3a ---> 58.217.200.15
void ntoa(int ip , char *ip_addr)
{
	memset(ip_addr , 0 , strlen(ip_addr));
	int a[4] , index ;
	for(index = 0 ; index < 4 ; ++index)
	{
		a[index] = (int)(ip & 0xff);
		ip = ip >> 8 ;
	}
	sprintf(ip_addr , "%d.%d.%d.%d",a[0] , a[1] , a[2] ,a[3]);
}
//58.217.200.15  --> 0x 0f c8 d9 3a
int aton(char *ip_addr)
{
	int buf[4];
	memset(buf , 0 , sizeof(buf));
	sscanf(ip_addr , "%d.%d.%d.%d" , &buf[0],&buf[1] , &buf[2] , &buf[3]);
	int index = 0 ;
	int result = 0 ;
	for(; index < 4 ; ++index)
	{
		result |= (buf[index] & 0xff);
		printf("before result = %x\n",result);
		if(index < 3)
		{
			result = result << 8;
		}
	}
	return result ;
}
int main(int argc , char *argv[])
{
	int a = 0x0fc8d93a ;
	int b = reverse_ip(a);
	printf("before reverse : %x\n",a);
	printf("after reverese : %x\n",b);
	char buf[128] = " ";
	ntoa(a , buf);
	printf("ip_addr : %s\n",buf);
	int c = aton(buf);
	printf("ip : %08x\n",c);
	return 0 ;
}
```
**地址格式转化**
        通常用户在表达地址时采用的是点分十进制表示的数值（或者是为冒号分开的十进制Ipv6地址），而在通常使用的socket编程中使用的则是32位的网络字节序的二进制值，这就需要将这两个数值进行转换。这里在Ipv4中用到的函数有inet_aton()、inet_addr()和inet_ntoa()，而IPV4和Ipv6兼容的函数有inet_pton()和inet_ntop()。
IPv4的函数原型：
```
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int inet_aton(const char *straddr, struct in_addr *addrptr);
char *inet_ntoa(struct in_addr inaddr);
in_addr_t inet_addr(const char *straddr);
```

 - *函数inet_aton()：*将点分十进制数的IP地址转换成为网络字节序的32位二进制数值。返回值：成功，则返回1，不成功返回0.
参数straddr：存放输入的点分十进制数IP地址字符串。
参数addrptr：传出参数，保存网络字节序的32位二进制数值。
 - *函数inet_ntoa()：*将网络字节序的32位二进制数值转换为点分十进制的IP地址。
 - *函数inet_addr()：*功能与inet_aton相同，但是结果传递的方式不同。inet_addr()若成功则返回32位二进制的网络字节序地址。
IPv4和IPv6的函数原型： 
```
    #include <arpa/inet.h>
    int inet_pton(int family, const char *src, void *dst);
    const char *inet_ntop(int family, const void *src, char *dst, socklen_t len);
```
函数inet_pton跟inet_aton实现的功能类似，只是多了family参数，该参数指定为AF_INET，表示是IPv4协议，如果是AF_INET6，表示IPv6协议。
```
    #include<stdio.h>
    #include<sys/socket.h>
    #include<netinet/in.h>
    #include<arpa/inet.h>
    int main()
    {
    	char ip[] = "192.168.0.101";
    	struct in_addr myaddr ;
    	/*inet_aton-->用于将ip地址转化为网络字节序*/
    	int iRet = inet_aton(ip , &myaddr);
    	printf("inet_aton : %x\n",myaddr.s_addr);
    
    	/*inet_addr，同上，只是参数不同*/
    	printf("inet_addr(ip) : %x\n",inet_addr(ip));
    
    	/*inet_pton -->将IP地址转换成网络字节序，也使用于IPV6*/
    	iRet = inet_pton(AF_INET , ip , &myaddr);
    	printf("inet_pton : %x\n",myaddr.s_addr);
    
    	myaddr.s_addr = 0xac100ac4 ;
    	/*inet_ntoa -->将网络字节序转换为IP地址*/
    	printf("inet_ntoa : %s\n",inet_ntoa(myaddr));
    
    	/*inet_ntop*/
    	inet_ntop(AF_INET , &myaddr , ip , 16);
    	puts(ip);
    	return 0 ;
    }
```

> 在linux中，有一些函数可以实现主机名和地址的转化，最常见的有gethostbyname()、gethostbyaddr()等，它们都可以实现IPv4和IPv6的地址和主机名之间的转化。其中gethostbyname()是将主机名转化为IP地址，gethostbyaddr()则是逆操作，是将IP地址转化为主机名。
```
#include <netdb.h>
struct hostent* gethostbyname(const char* hostname);
struct hostent* gethostbyaddr(const char* addr, size_t len, int family);
struct hostent
{
    	char *h_name;		/*正式主机名*/
    	char **h_aliases;	/*主机别名*/
    	int h_addrtype;		/*主机IP地址类型 IPv4为AF_INET*/
    	int h_length;	/*主机IP地址字节长度，对于IPv4是4字节，即32位*/
    	char **h_addr_list;	/*主机的IP地址列表*/
}
```
```
#include<stdio.h>
#include<netdb.h>
#include<sys/socket.h>
int main(int argc , char *argv[])
{
	char *ptr , **pptr ;
	struct hostent *hptr ;
	char str[32] = {'\0'};

	ptr = argv[1] ;
	//获取域名，返回的是一个结构体指针
	if((hptr = gethostbyname(ptr)) == NULL)
	{
		printf("gethostbyname error for host :%s\n",ptr);
		return 0 ;
	}
	printf("official hostname : %s\n",hptr->h_name);
	//网络字节序转换为IP地址
	for(pptr = hptr->h_aliases ; *pptr!= NULL ; pptr++ )
	{
		printf("h_aliases-->address : %s\n",inet_ntop(hptr->h_addrtype , *pptr ,str , sizeof(str)));
	}
	switch(hptr->h_addrtype)
	{
		case AF_INET:
		case AF_INET6:
			//IP地址列表，即输出其所有IP地址列表
			pptr = hptr->h_addr_list ;
			for(; *pptr != NULL ; pptr++)
				printf("address:%s\n",inet_ntop(hptr->h_addrtype , *pptr , str , sizeof(str)));
			printf("first address : %s\n",inet_ntop(hptr->h_addrtype , hptr->h_addr , str , sizeof(str)));
			break ;
		default:
			printf("unknown address type\n");
			break;
	}
	return 0 ;
}
```
##3.Socket编程
###1.TCP通信
TCP通信的基本步骤如下：

> 服务端：socket---bind---listen---while(1){---accept---recv---send---close---}---close
> 
> 客户端：socket----------------------------------connect---send---recv-----------------close

服务器端：

 - 头文件包含：
```
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
```
 - socket函数：生成一个套接口描述符

> 原型：int socket(int domain,int type,int protocol);

参数：

> domain->{ AF_INET：Ipv4网络协议   AF_INET6：IPv6网络协议} type->｛tcp：SOCK_STREAMudp：SOCK_DGRAM} 
protocol->指定socket所使用的传输协议编号。通常为0.

返回值：成功则返回套接口描述符，失败返回-1。

 - bind函数：用来绑定一个端口号和IP地址，使套接口与指定的端口号和IP地址相关联。

> 原型：int bind(int sockfd,struct sockaddr * my_addr,int addrlen);

参数：

> sockfd为前面socket的返回值。
 my_addr为结构体指针变量
```
struct sockaddr_in my_addr;  //定义结构体变量
memset(&my_addr, 0, sizeof(struct sockaddr)); //将结构体清空
//或bzero(&my_addr, sizeof(struct sockaddr));
 my_addr.sin_family = AF_INET;  //表示采用Ipv4网络协议
my_addr.sin_port = htons(8888); 
 //表示端口号为8888，通常是大于1024的一个值。
//htons()用来将参数指定的16位hostshort转换成网络字符顺序
my_addr.sin_addr.s_addr = inet_addr("192.168.0.101"); 
// inet_addr()用来将IP地址字符串转换成网络所使用的二进制数字，如果为INADDR_ANY，这表示服务器自动填充本机IP地址。
```
 

 - listen函数
使服务器的这个端口和IP处于监听状态，等待网络中某一客户机的连接请求。如果客户端有连接请求，端口就会接受这个连接。

> 原型：int listen(int sockfd, int backlog);

参数：

> sockfd为前面socket的返回值.
即sfd backlog指定同时能处理的最大连接要求，通常为10或者5。 最大值可设至128

 - accept函数：接受远程计算机的连接请求，建立起与客户机之间的通信连接。

> 原型：int accept(int s,struct sockaddr * addr,int * addrlen);

参数：

> s为前面socket的返回值.即sfd
> addr为结构体指针变量，和bind的结构体是同种类型的，系统会把远程主机的信息（远程主机的地址和端口号信息）保存到这个指针所指的结构体中。
> addrlen表示结构体的长度，为整型指针

返回值：成功则返回新的socket处理代码new_fd，失败返回-1

 - recv函数：用新的套接字来接收远端主机传来的数据，并把数据存到由参数buf 指向的内存空间

> 原型：int recv(int sockfd,void *buf,int len,unsigned int flags);

参数：

> sockfd为前面accept的返回值.即new_fd，也就是新的套接字。
 buf表示缓冲区
len表示缓冲区的长度
flags通常为0

返回值：成功则返回实际接收到的字符数，可能会少于你所指定的接收长度。失败返回-1

 - send函数：用新的套接字发送数据给指定的远端主机

> 原型：int send(int s,const void * msg,int len,unsigned int flags);

参数：

> s为前面accept的返回值.即new_fd 
msg一般为常量字符串
 len表示长度 
flags通常为0

返回值：成功则返回实际传送出去的字符数，可能会少于你所指定的发送长度。失败返回-1

 - close函数

当使用完文件后若已不再需要则可使用close()关闭该文件，并且close()会让数据写回磁盘，并释放该文件所占用的资源
	
> 原型：int close(int fd); 

参数：

> fd为前面的sfd,new_fd

返回值：若文件顺利关闭则返回0，发生错误时返回-1
常用实例：close(new_fd);  close(sfd);

客户端：

 - connect函数

用来请求连接远程服务器，将参数sockfd 的socket 连至参数serv_addr 指定的服务器IP和端口号上去。

> 原型：int connect (int sockfd,struct sockaddr * serv_addr,int addrlen);

参数：

> sockfd为前面socket的返回值，即sfd
> serv_addr为结构体指针变量，存储着远程服务器的IP与端口号信息。addrlen表示结构体变量的长度

返回值：成功则返回0，失败返回-1
常用实例：
```
struct sockaddr_in seraddr;//请求连接服务器
memset(&seraddr, 0, sizeof(struct sockaddr));
seraddr.sin_family = AF_INET;
 seraddr.sin_port = htons(8888);	//服务器的端口号
seraddr.sin_addr.s_addr = inet_addr("192.168.0.101");  //ip
```
###2.UDP通信
UDP通信流程图如下：

> 服务端：socket---bind---recvfrom---sendto---close
> 客户端：socket----------sendto---recvfrom---close

 - ·sendto()函数原型：
```
int sendto(int sockfd, const void *msg,int len,unsigned int flags,const struct sockaddr *to, int tolen);
```
该函数比send()函数多了两个参数，to表示目地机的IP地址和端口号信息，而tolen常常被赋值为sizeof (struct sockaddr)。sendto 函数也返回实际发送的数据字节长度或在出现发送错误时返回-1。

 - ·recvfrom()函数原型：
‵‵‵‵
int recvfrom(int sockfd,void *buf,int len,unsigned int flags,struct sockaddr *from,int *fromlen);
from是一个struct sockaddr类型的变量，该变量保存连接机的IP地址及端口号。fromlen常置为sizeof (struct sockaddr)。当recvfrom()返回时，fromlen包含实际存入from中的数据字节数。Recvfrom()函数返回接收到的字节数或 当出现错误时返回-1，并置相应的errno。
