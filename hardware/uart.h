/**
 * @file uart.h
 * @brief 串口配置
 * @author WatWu
 * @date 2018-09-01
 */
#ifndef UART_H
#define UART_H

/**
 * @brief 设置串口DTR位
 * @param fd [已初始化的串口fd]
 */
void uart_set_dtr(int fd);

/**
 * @brief 清除串口DTR位
 * @param fd [已初始化的串口fd]
 */
void uart_clear_dtr(int fd);

/**
 * @brief 初始化串口
 * @param device_name   [串口设备名]
 * @param baudrate      [波特率]
 * @return int          [初始化结果，成功则返回初始化后的串口描述符]
 */
int init_uart(char* device_name, int baudrate);

#endif
