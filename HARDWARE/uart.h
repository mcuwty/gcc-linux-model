#ifndef __UART_H
#define __UART_H

#include "main.h"

int _setup_uart(int fd,int baud);
int Init_uart(char* port,int baudrate);

#endif
