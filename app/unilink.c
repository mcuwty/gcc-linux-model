/**
 * @file unilink.c
 * @brief 通用双机通信协议
 * @author WatWu
 * @date 2018-09-01
 */

#include "unilink.h"

/**
 * @brief 计算校验和
 * @param frame [unilink格式的数据帧]
 * @return int 	[校验和结果]
 */
static int unilink_calculate_sum(unilink_t* frame)
{
	unsigned char sum = 0;
	int i;

	sum += frame->unilink_len;
	sum += frame->unilink_id;

	for(i = 0; i < frame->unilink_len; i++)
		sum += frame->unilink_data[i];

	return sum;
}

/**
 * @brief 以unilink的格式对数据进行逐个字节解析
 * @param char_data [传入逐个字节数据]
 * @param step 		[当前解析到的步骤]]
 * @param frame 	[解析成功的unilink数据缓存区]
 * @return int 		[解析结果，1 - 解析到unilink格式的数据；-1 - 未解析到unilink格式的数据]
 */
int unilink_parse_char(unsigned char char_data, unsigned char *step, unilink_t *frame)
{
	switch(*step)
	{
		case UNILINK_HEAD_1:
			if(char_data == 0xA5)
				*step = UNILINK_HEAD_2;
			break;
		case UNILINK_HEAD_2:
			if(char_data == 0x5A)
				*step = UNILINK_LENGTH;
			else
				*step = UNILINK_HEAD_1;
			break;
		case UNILINK_LENGTH:
			frame->unilink_len = char_data;
			frame->unilink_data_pos = 0;
			*step = UNILINK_COM_ID;
			break;
		case UNILINK_COM_ID:
			frame->unilink_id = char_data;
			*step = UNILINK_DATA;
			break;
		case UNILINK_DATA:
			frame->unilink_data[frame->unilink_data_pos++] = char_data;
			if(frame->unilink_data_pos >= frame->unilink_len)
				*step = UNILINK_CHECK_SUM;
			break;
		case UNILINK_CHECK_SUM:
			frame->unilink_sum = char_data;
			*step = UNILINK_END;
			break;
		default:
			*step = UNILINK_HEAD_1;
			break;
	}

	if(*step == UNILINK_END)
	{
		unsigned char sum;
		*step = UNILINK_HEAD_1;

		sum = unilink_calculate_sum(frame);

		if(sum == frame->unilink_sum)
			return 1;
		else
			return -1;
	}
	else
		return -1;
}

/**
 * @brief 打包封装unilink格式的数据帧
 * @param id 		[unilink的id]
 * @param len 		[unilink的有效数据部分的长度，不含帧头帧尾等固定部分]
 * @param databuf 	[unilink的有效数据]
 * @param frame 	[打包封装好的unilink数据]
 */
void unilink_pack(unsigned char id, unsigned char len, unsigned char* databuf, unilink_t* frame)
{
	frame->unilink_id = id;
	frame->unilink_len = len;
	memcpy(frame->unilink_data, databuf, len);
	frame->unilink_sum = unilink_calculate_sum(frame);
}

/**
 * @brief unilink数据帧转换成字节数组
 * @param buf 	[数组缓存区]
 * @param frame [unilink格式的数据帧]
 * @return int 	[转换后的字节数组长度]
 */
int unilink_send_to_buffer(unsigned char* buf, unilink_t* frame)
{
	buf[0] = 0xA5;
	buf[1] = 0x5A;
	buf[2] = frame->unilink_len;
	buf[3] = frame->unilink_id;
	memcpy(&buf[4], frame->unilink_data, frame->unilink_len);
	buf[frame->unilink_len + 4] = frame->unilink_sum;

	return (frame->unilink_len + 5);
}
