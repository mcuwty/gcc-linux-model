/**
 * @file unilink.h
 * @brief 通用双机通信协议
 * @author WatWu
 * @date 2018-09-01
 */

#ifndef UNILINK_H
#define UNILINK_H

#include <string.h>

#define UNILINK_HEAD_1          1
#define UNILINK_HEAD_2          2
#define UNILINK_LENGTH          3
#define UNILINK_COM_ID          4
#define UNILINK_DATA            5
#define UNILINK_CHECK_SUM       6
#define UNILINK_END             7

typedef struct {
    unsigned char unilink_id;
    unsigned char unilink_len;
    unsigned char unilink_sum;
    unsigned char unilink_data_pos;
    unsigned char unilink_data[256];
} __attribute__((packed)) unilink_t;

/**
 * @brief 以unilink的格式对数据进行逐个字节解析
 * @param char_data [传入逐个字节数据]
 * @param step      [当前解析到的步骤]]
 * @param frame     [解析成功的unilink数据缓存区]
 * @return int      [解析结果，1 - 解析到unilink格式的数据；-1 - 未解析到unilink格式的数据]
 */
int unilink_parse_char(unsigned char char_data, unsigned char* step, unilink_t* frame);

/**
 * @brief 打包封装unilink格式的数据帧
 * @param id        [unilink的id]
 * @param len       [unilink的有效数据部分的长度，不含帧头帧尾等固定部分]
 * @param databuf   [unilink的有效数据]
 * @param frame     [打包封装好的unilink数据]
 */
void unilink_pack(unsigned char id, unsigned char len, unsigned char* databuf, unilink_t* frame);

/**
 * @brief unilink数据帧转换成字节数组
 * @param buf       [数组缓存区]
 * @param frame     [unilink格式的数据帧]
 * @return int      [转换后的字节数组长度]
 */
int unilink_send_to_buffer(unsigned char* buf, unilink_t* frame);

#endif
