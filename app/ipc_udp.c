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

#define TAG "ipc"
#include "hey_log.h"

#define TIME_OUT_VOLUE          3   //通信超时值，单位秒

static int ipc_udp_net_fd = -1;
static pthread_t ipc_pthread_fd;

/**
 * @brief udp进程间通信线程
 * @param argv      [线程函数参数]
 * @return void*    [无]
 */
static void* ipc_udp_pthread(void* argv)
{
    argv = argv;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        HEY_ERR("Init ipc_udp sockfd error\n");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret;
    ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0) {
        HEY_ERR("Bind ipc port error\n");
        close(fd);
        exit(1);
    }
    HEY_INF("Bind ipc address successful!\n");

    ipc_udp_net_fd = fd;        //记录该句柄

    fd_set read_fds;
    struct timeval timeout = {0, 0};

    unsigned char net_buf[1024];
    struct sockaddr_in ipc_udp_from;
    socklen_t len;
    len = sizeof(ipc_udp_from);

    while(1) {
        if(ipc_udp_net_fd != -1) {
            FD_ZERO(&read_fds);
            FD_SET(ipc_udp_net_fd, &read_fds);
            timeout.tv_sec = TIME_OUT_VOLUE;

            int ret = select(ipc_udp_net_fd + 1, &read_fds, NULL, NULL, &timeout);
            switch(ret) {
                case -1:
                    HEY_ERR("Network socket select error !\n");
                    break;
                case 0:
                    HEY_DBG("Network socket select timeout !\n");
                    break;
                default : {
                    if(FD_ISSET(ipc_udp_net_fd, &read_fds)) {
                        ret = recvfrom(ipc_udp_net_fd, net_buf, sizeof(net_buf) - 1, 0, (struct sockaddr*)&ipc_udp_from, &len);
                        if(ret > 0) {
                            int i;
                            for(i = 0; i < ret; i++) {
                                putchar(net_buf[i]);
                            }
                        }
                    }
                    break;
                }
            }
        } else {
            usleep(100 * 1000);
        }
    }
}

/**
 * @brief 初始化udp进程间通信线程
 */
void init_ipc_udp_pthread(void)
{
    int ret;
    ret = pthread_create(&ipc_pthread_fd, NULL, ipc_udp_pthread, NULL);
    if(ret != 0) {
        HEY_ERR("Create ipc_udp_pthread pthread error\n");
        exit(1);
    }
    HEY_INF("Init ipc_udp_pthread pthread ok !\n");
}
