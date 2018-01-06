#ifndef __MAIN_H
#define __MAIN_H

#include <time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

//连接状态定义
#define LINK_NONE 				0			//未连接
#define LINK_ONLINE				1			//连接在线
#define LINK_OFFLINE 			2			//连接断开

#define TIME_OUT_VOLUE 			3 			//通信超时值，单位秒

extern pthread_t ipc_pthread;
extern struct sockaddr_in ipc_udp_from;
extern int ipc_udp_net_fd;					//进程间通信的socket

#endif
