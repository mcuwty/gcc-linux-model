/**
 * @file ipc_udp.h
 * @brief UDP进程间通信
 * @author WatWu
 * @date 2018-08-31
 */

#ifndef IPC_UDP_H
#define IPC_UDP_H

/**
 * @brief 组播类型定义，服务器或客户端
 */
typedef enum
{
	MULTICAST_SERVER = 0,
	MULTICAST_CLIENT = 1
}multicast_type_e;

/**
 * @brief 初始化udp进程间通信线程
 */
void init_ipc_udp_pthread(void);

/**
 * @brief 初始化UDP组播线程
 * @param multicast_type [组播类型，服务器或客户端]
 */
void init_udp_multicast_pthread(int multicast_type);

#endif
