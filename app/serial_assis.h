/**
 * @file serial_assis.h
 * @brief 串口助手
 * @version 0.1
 * @date 2020-06-25
 * @author WatWu
 */

#ifndef SERIAL_ASSIS_H
#define SERIAL_ASSIS_H

#include <stdint.h>

/**
 * @brief 启动串口助手
 * @param dev   [串口设备名]
 * @param baud  [波特率]
 * @return int  [启动结果]
 */
int start_serial_assistant(char* dev, uint32_t baud);

#endif
