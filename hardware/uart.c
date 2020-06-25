/**
 * @file uart.c
 * @brief 串口初始化
 * @version 0.1
 * @date 2020-06-25
 * @author WatWu
 */

#include <sys/file.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

/**
 * @brief 使用特定波特率设置串口描述符
 * @param fd    [打开的串口文件描述符]
 * @param baud  [波特率]
 * @return int  [设置结果，0为成功]
 */
static int setup_uart_fd(int fd, int baud)
{
    if(!isatty(fd)) {
        fprintf(stderr, "\nERROR: file descriptor %d is NOT a serial port\n", fd);
        return -1;
    }

    struct termios config;
    if(tcgetattr(fd, &config) < 0) {
        fprintf(stderr, "\nERROR: could not read configuration of fd %d\n", fd);
        return -1;
    }

    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
                        INLCR | PARMRK | INPCK | ISTRIP | IXON);

    config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
                        ONOCR | OFILL | OPOST);

    #ifdef OLCUC
    config.c_oflag &= ~OLCUC;
    #endif

    #ifdef ONOEOT
    config.c_oflag &= ~ONOEOT;
    #endif

    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8;

    config.c_cc[VMIN]  = 1;
    config.c_cc[VTIME] = 10;

    switch(baud) {
        case 1200:
            if(cfsetispeed(&config, B1200) < 0 || cfsetospeed(&config, B1200) < 0) {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 1800:
            cfsetispeed(&config, B1800);
            cfsetospeed(&config, B1800);
            break;
        case 9600:
            cfsetispeed(&config, B9600);
            cfsetospeed(&config, B9600);
            break;
        case 19200:
            cfsetispeed(&config, B19200);
            cfsetospeed(&config, B19200);
            break;
        case 38400:
            if(cfsetispeed(&config, B38400) < 0 || cfsetospeed(&config, B38400) < 0) {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 57600:
            if(cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0) {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 115200:
            if(cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0) {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 230400:
            if(cfsetispeed(&config, B230400) < 0 || cfsetospeed(&config, B230400) < 0) {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 460800:
            if(cfsetispeed(&config, B460800) < 0 || cfsetospeed(&config, B460800) < 0) {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 921600:
            if(cfsetispeed(&config, B921600) < 0 || cfsetospeed(&config, B921600) < 0) {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        default:
            fprintf(stderr, "ERROR: Desired baud rate %d could not be set, aborting.\n", baud);
            return -1;
            break;
    }

    if(tcsetattr(fd, TCSAFLUSH, &config) < 0) {
        fprintf(stderr, "\nERROR: could not set configuration of fd %d\n", fd);
        return -1;
    }

    return 0;
}

/**
 * @brief 设置串口DTR位
 * @param fd [已初始化的串口fd]
 */
void uart_set_dtr(int fd)
{
    int status;
    ioctl(fd, TIOCMGET, &status);   /* GET the State of MODEM bits in Status */
    status |= TIOCM_DTR;            // Set the RTS pin
    ioctl(fd, TIOCMSET, &status);
}

/**
 * @brief 清除串口DTR位
 * @param fd [已初始化的串口fd]
 */
void uart_clear_dtr(int fd)
{
    int status;
    ioctl(fd, TIOCMGET, &status);   /* GET the State of MODEM bits in Status */
    status &= ~TIOCM_DTR;           // clear the RTS pin
    ioctl(fd, TIOCMSET, &status);
}

/**
 * @brief 初始化串口
 * @param device_name   [串口设备名]
 * @param baudrate      [波特率]
 * @return int          [初始化结果，成功则返回初始化后的串口描述符]
 */
int init_uart(char* device_name, int baudrate)
{
    int fd = open(device_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if(fd == -1) {
        return -1;
    } else {
        fcntl(fd, F_SETFL, 0);    //配置为阻塞式
    }

    if(setup_uart_fd(fd, baudrate) < 0) {
        return -1;
    }

    return fd;
}
