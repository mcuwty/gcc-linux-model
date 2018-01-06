/****************************************************************************************
 * 文件名  ：unilink.c
 * 描述    ：双机通信协议
 * 开发平台：linux
 * 开发工具：gcc-4.8.4/arm-linux-gnueabihf-gcc-4.9.4
 ***************************************************************************************/
#include "unilink.h"

//计算检验和
int sum_calculate(unilink_t* frame)
{
	unsigned char sum = 0;
	int i;

	sum += frame->unilink_len;
	sum += frame->unilink_id;

	for(i=0;i<frame->unilink_len;i++)
		sum += frame->unilink_data[i];

	return sum;
}

//解读功能模块指令帧
//输入参数：Char - 数据内容，按照字节进行解读
//			*step - 当前解读到的步骤
// 			*frame - 功能模块的结构体指针
//返回值：0 - 非完整指令帧；1 - 解读到完整指令帧
int parse_char_unilink(unsigned char Char, unsigned char *step, unilink_t *frame)
{
	switch(*step)
	{
		case UNILINK_HEAD_1:
			if( Char == 0xA5 )
				*step = UNILINK_HEAD_2;
			break;
		case UNILINK_HEAD_2:
			if( Char == 0x5A )
				*step = UNILINK_LENGTH;
			else
				*step = UNILINK_HEAD_1;
			break;
		case UNILINK_LENGTH:
			frame->unilink_len = Char;
			frame->unilink_data_pos = 0;
			*step = UNILINK_COM_ID;
			break;
		case UNILINK_COM_ID:
			frame->unilink_id = Char;
			*step = UNILINK_DATA;
			break;
		case UNILINK_DATA:
			frame->unilink_data[frame->unilink_data_pos++] = Char;
			if(frame->unilink_data_pos >= frame->unilink_len)
				*step = UNILINK_CHECK_SUM;
			break;
		case UNILINK_CHECK_SUM:
			frame->unilink_sum = Char;
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

		sum = sum_calculate(frame);

		if( sum == frame->unilink_sum )
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

//封装功能模块指令帧
void unilink_pack(unsigned char id,unsigned char len,unsigned char* databuf,unilink_t* frame)
{
	frame->unilink_id = id;
	frame->unilink_len = len;
	memcpy(frame->unilink_data,databuf,len);
	frame->unilink_sum = sum_calculate(frame);
}

//功能模块指令帧发送到缓存数组
int unilink_to_send_buffer(unsigned char* buf,unilink_t* frame)
{
	buf[0] = 0xA5;
	buf[1] = 0x5A;
	buf[2] = frame->unilink_len;
	buf[3] = frame->unilink_id;
	memcpy(&buf[4],frame->unilink_data,frame->unilink_len);
	buf[frame->unilink_len+4] = frame->unilink_sum;

	return (frame->unilink_len+5);
}
