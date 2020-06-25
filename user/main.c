/**
 * @file main.c
 * @brief linux应用开发模板
 * @author WatWu
 * @date 2018-08-31
 */

#include <string.h>
#include <termios.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "ipc_udp.h"
#include "uart.h"
#include "serial_assis.h"

#define TAG "main"
#include "hey_log.h"

/**
 * @brief 程序入口，负责传入的参数判断，网络连接监听，线程初始化等
 * @param argc [传入的参数数量]
 * @param argv [传入的参数列表]
 * @return int [无]
 */
int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);                //屏蔽掉SIGPIPE信号，防止网络断开时退出进程

    if(argc < 3) {
        HEY_ERR("Usage:%s [uart dev] [baud]\n", argv[0]);
        exit(1);
    }

    // init_ipc_udp_pthread();
    int ret = start_serial_assistant(argv[1], atoi(argv[2]));
    if(ret < 0) {
        HEY_ERR("Start serial assistant failed, device : %s\n", argv[1]);
    }

    while(1) {
        sleep(1);
    }
}
