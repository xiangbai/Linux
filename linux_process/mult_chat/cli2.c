/*************************************************************************
  > File Name: cli2.c
  > Author: wang
  > Mail:xiangbai@qq.com 
  > Created Time: Sat 29 Mar 2014 04:56:56 PM CST
 ************************************************************************/

#include"chat.h"
int main(int argc , char *argv[])
{
	int fd_21 , fd_23 , fd_12 , fd_32 ;
	fd_12 = open("cl1_cl2.fifo" , O_RDONLY);
	fd_21 = open("cl2_cl1.fifo" , O_WRONLY);
	fd_23 = open("cl2_cl3.fifo",O_WRONLY);
	fd_32 = open("cl3_cl2.fifo",O_RDONLY);
	if(fd_12 == -1 || fd_21 ==-1 || fd_23 == -1 || fd_32 == -1)
	{
		perror("open:");
		exit(-1);
	}
	pid_t pid = fork();
	fd_set fd_rd , fd_rd_back ;
	FD_ZERO(&fd_rd);
	FD_SET(fd_12 , &fd_rd);
	FD_SET(fd_32 , &fd_rd);
	struct timeval tm ;
	tm.tv_sec = 1 ;
	tm.tv_usec = 0 ;
	char buf[128];
	char cmd[128];
	int ired ;
	if(pid > 0)
	{
		//cli1向cli1,cli3发送消息
		while(1)
		{
			bzero(buf , 0);
			bzero(cmd , 0);
			//从屏幕中写入内容
			ired = read(0 , buf , 128);
			//结束通话
			if(ired <= 0)
			{
				write(fd_21 , "bye" , 3);
				write(fd_23 , "bye" , 3);
				sprintf(cmd , "kill -9 %d" , pid);
				system(cmd);
				close(fd_12);
				close(fd_21);
				close(fd_23);
				close(fd_32);
				break;
			}
			buf[ired] = '\0';
			//将内容写入管道
			write(fd_21 , buf, strlen(buf));
			write(fd_23 , buf , strlen(buf));
		}
		pid_t pid_wait ;
		int status ;
		pid_wait = wait(&status);

		close(fd_12);
		close(fd_21);
		close(fd_23);
		close(fd_32);
	}
	else if(pid == 0)
	{
		while(1)
		{
			bzero(buf , 128);
			bzero(cmd , 128);
			fd_rd_back = fd_rd ;
			select(fd_12 > fd_32 ? fd_12+1 : fd_32+1 , &fd_rd_back , NULL ,NULL , &tm);
			if(FD_ISSET(fd_12 , &fd_rd_back))
			{
				//从cli2中读出内容
				ired = read(fd_12 , buf , 128);
				if(strncmp(buf , "bye" , 3) == 0)
				{
					sprintf(cmd , "guai says : %s" , buf);
					write(1 , cmd , strlen(cmd));
					bzero(cmd , 128);
					sprintf(cmd , "kill -9 %d" , getppid());
					system(cmd);
					close(fd_12);
					close(fd_21);
					close(fd_23);
					close(fd_32);
					break ;
				}
				buf[ired] = '\0';
				sprintf(cmd , "guai says : %s" ,buf);
				if(ired > 0)
				{
				write(1 , cmd , strlen(cmd));
				}

			}
			//从cli3中读出内容
			else if(FD_ISSET(fd_32 , &fd_rd_back))
			{
				ired = read(fd_32 , buf , 128);

				if(strncmp(buf , "bye" , 3) == 0)
				{
					sprintf(cmd , "lei says : %s" , buf);
					write(1 , cmd , strlen(cmd));
					bzero(cmd , 128);
					sprintf(cmd , "kill -9 %d" , getppid());
					system(cmd);
					close(fd_12);
					close(fd_21);
					close(fd_23);
					close(fd_32);
					break ;
				}

				buf[ired] = '\0';
				sprintf(cmd , "lei says : %s" ,buf);
				if(ired > 0)
				{
				write(1 , cmd , strlen(cmd));
				}
			}
		}
	}
	return 0 ;
}

