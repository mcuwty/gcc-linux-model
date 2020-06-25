/**
 * @file serial_assis.c
 * @brief
 * @version 0.1
 * @date 2020-06-25
 * @author WatWu
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/select.h>

#include "uart.h"

#define TAG "assis"
#include "hey_log.h"

static int uart_fd = -1;
static pthread_t serial_assis_fd;

/**
 * @brief 串口助手线程
 * @param argv      [线程函数参数]
 * @return void*    [无]
 */
static void* serial_assis_pthread(void* argv)
{
    uart_fd = *(int*)argv;

    uart_clear_dtr(uart_fd);
    usleep(2000);
    uart_set_dtr(uart_fd);

    fd_set read_fds;
    struct timeval timeout = {0, 0};
    unsigned char uart_buf[512];

    while(1) {
        if(uart_fd > 0) {
            FD_ZERO(&read_fds);
            FD_SET(uart_fd, &read_fds);
            timeout.tv_sec = 1;

            int ret = select(uart_fd + 1, &read_fds, NULL, NULL, &timeout);
            switch(ret) {
                case -1:
                    HEY_ERR("uart select error !\n");
                    break;
                case 0:
                    HEY_DBG("uart select timeout !\n");
                    break;
                default : {
                    if(FD_ISSET(uart_fd, &read_fds)) {
                        memset(uart_buf, 0, 512);
                        ret = read(uart_fd, uart_buf, sizeof(uart_buf) - 1);
                        if(ret > 0) {
                            printf("%s", uart_buf);
                            fflush(stdout);
                        }
                    }
                    break;
                }
            }
        } else {
            sleep(1);
        }
    }

    return 0;
}

/**
 * @brief 启动串口助手
 * @param dev   [串口设备名]
 * @param baud  [波特率]
 * @return int  [启动结果]
 */
int start_serial_assistant(char* dev, uint32_t baud)
{
    int ret;
    int fd;

    fd = init_uart(dev, baud);
    if(fd < 0) {
        HEY_ERR("Init uart %s error !", dev);
        return -1;
    }

    ret = pthread_create(&serial_assis_fd, NULL, serial_assis_pthread, &fd);
    if(ret != 0) {
        HEY_ERR("Create serial_assis_pthread pthread error\n");
        close(fd);
        return -1;
    }
    HEY_INF("Init serial_assis_pthread pthread ok !\n");

    return 0;
}
