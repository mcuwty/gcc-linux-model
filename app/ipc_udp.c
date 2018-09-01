/**
 * @file ipc_udp.c
 * @brief UDP进程间通信
 * @author WatWu
 * @date 2018-08-31
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "ipc_udp.h"

#define TIME_OUT_VOLUE 			3 			//通信超时值，单位秒

#define MULTICAST_ADDR			"226.1.1.1"	//组播地址
#define MULTICAST_PORT			48000		//组播端口
#define LOCAL_ADDRESS			"127.0.0.1"	//本地地址

int ipc_udp_net_fd  = -1;					//进程间通信的socket

static pthread_t ipc_pthread_fd;
static pthread_t udp_multicast_pthread_fd;

/**
 * @brief udp进程间通信线程
 * @param pro 		[线程函数参数]
 * @return void* 	[无]
 */
static void *ipc_udp_pthread(void* pro)
{
	pro = pro;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("Init ipc_udp sockfd error");
		exit(1);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr(LOCAL_ADDRESS);

	int ret;
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret < 0)
	{
		perror("Bind ipc port error");
		close(fd);
		exit(1);
	}
	printf("Bind ipc address successful!\n");

	ipc_udp_net_fd = fd;		//记录该句柄

	fd_set read_fds;
	struct timeval timeout = {0, 0};

	unsigned char net_buf[1024];
	struct sockaddr_in ipc_udp_from;
	socklen_t len;
	len = sizeof(ipc_udp_from);

	while(1)
	{
		if(ipc_udp_net_fd != -1)
		{
			FD_ZERO(&read_fds);
			FD_SET(ipc_udp_net_fd, &read_fds);
			timeout.tv_sec = TIME_OUT_VOLUE;

			int ret = select(ipc_udp_net_fd + 1, &read_fds, NULL, NULL, &timeout);
			switch(ret)
			{
				case -1:						//错误
					printf("Network socket select error !\n");
					break;
				case 0:							//超时
					printf("Network socket select timeout !\n");
					break;
				default :						//有可读的数据
				{
					if(FD_ISSET(ipc_udp_net_fd, &read_fds))
					{
						ret = recvfrom(ipc_udp_net_fd, net_buf, sizeof(net_buf) - 1, 0, (struct sockaddr*)&ipc_udp_from, &len);
						if(ret > 0)
						{
							int i;
							for(i = 0; i < ret; i++)
							{
								putchar(net_buf[i]);
							}
						}
					}
					break;
				}
			}
		}
		else
			usleep(100 * 1000);					//非阻塞的线程必须要加休眠，否则会占满CPU
	}
}

/**
 * @brief 初始化udp进程间通信线程
 */
void init_ipc_udp_pthread(void)
{
	int ret;

	//创建udp进程间通信线程
	ret = pthread_create(&ipc_pthread_fd, NULL, ipc_udp_pthread, NULL);
	if(ret != 0)
	{
		perror("Create ipc_udp_pthread pthread error");
		exit(1);
	}
	printf("Init ipc_udp_pthread pthread ok !\n");
}

/**
 * @brief UDP组播线程
 * @param pro 		[线程函数参数列表]
 * @return void* 	[无]
 */
static void *udp_multicast_pthread(void *pro)
{
	int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_fd < 0)
	{
		perror("Init udp sockfd error");
		exit(1);
	}

	int reuse = 1;
	if(setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(udp_fd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR OK !\n");

	int multicast_type = *((int*)pro);

	if(MULTICAST_SERVER == multicast_type)			//作为udp组播服务器
	{
		struct sockaddr_in group_addr;
		group_addr.sin_family = AF_INET;
		group_addr.sin_port = htons(MULTICAST_PORT);
		group_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);

		struct in_addr local_addr;
		local_addr.s_addr = inet_addr(LOCAL_ADDRESS);
		if(setsockopt(udp_fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&local_addr, sizeof(local_addr)) < 0)
		{
			perror("Setting local address error");
			exit(1);
		}
		else
			printf("Setting the local address OK\n");

		while(1)
		{
			if(sendto(udp_fd, "Multicast !", strlen("Multicast !") + 1, 0, (struct sockaddr*)&group_addr, sizeof(group_addr)) < 0)
			{
				perror("Send multicast data error");
			}
			else
			{
				printf("Send multicast data success !\n");
			}

			sleep(3);
		}
	}
	else if(MULTICAST_CLIENT == multicast_type)		//作为udp组播客户端
	{
		struct sockaddr_in local_addr;
		local_addr.sin_family = AF_INET;
		local_addr.sin_port = htons(MULTICAST_PORT);
		local_addr.sin_addr.s_addr = INADDR_ANY;
		if(bind(udp_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)))
		{
			perror("Binding datagram socket error");
			close(udp_fd);
			exit(1);
		}
		else
			printf("Binding datagram socket OK.\n");

		struct ip_mreq group;
		group.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
		group.imr_interface.s_addr = inet_addr(LOCAL_ADDRESS);
		if(setsockopt(udp_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
		{
			perror("Adding multicast group error");
			close(udp_fd);
			exit(1);
		}
		else
			printf("Adding multicast group OK.\n");

		unsigned char net_buf[1024];
		struct sockaddr_in udp_ip_from;
		socklen_t len;
		len = sizeof(udp_ip_from);

		while(1)
		{
			if(recvfrom(udp_fd, net_buf, sizeof(net_buf) - 1, 0, (struct sockaddr*)&udp_ip_from, &len) < 0)
			{
				perror("Reading datagram message error");
			}
			else
			{
				printf("The message from multicast server is: %s\n", net_buf);
			}
		}
	}
	else
	{
		printf("Multicast type error : %d\n", multicast_type);
		exit(1);
	}
}

/**
 * @brief 初始化UDP组播线程
 * @param multicast_type [组播类型，服务器或客户端]
 */
void init_udp_multicast_pthread(int multicast_type)
{
	int ret;

	int type = multicast_type;
	//创建udp组播线程
	ret = pthread_create(&udp_multicast_pthread_fd, NULL, udp_multicast_pthread, &type);
	if(ret != 0)
	{
		perror("Create udp_multicast_pthread pthread error");
		exit(1);
	}
	printf("Init udp_multicast_pthread pthread ok !\n");
}
