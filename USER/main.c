/**
 * @file main.c
 * @brief linux应用开发模板
 * @author WatWu
 * @date 2018-08-31
 */

#include "main.h"
#include "ipc_udp.h"
#include "uart.h"

/**
 * @brief 程序入口，负责传入的参数判断，网络连接监听，线程初始化等
 * @param argc [传入的参数数量]
 * @param argv [传入的参数列表]
 * @return int [无]
 */
int main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);				//屏蔽掉SIGPIPE信号，防止网络断开时退出进程

	if(argc < 2)
	{
		printf("Usage:%s [multicast type : 0 or 1]\n",argv[0]);
		exit(1);
	}

	// init_ipc_udp_pthread();
	init_udp_multicast_pthread(atoi(argv[1]));

	while(1)
	{
		sleep(10);
	}
}
