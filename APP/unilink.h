#ifndef __UNILINK_H
#define __UNILINK_H

#include <string.h>

#define UNILINK_HEAD_1			1
#define UNILINK_HEAD_2			2
#define UNILINK_LENGTH			3
#define UNILINK_COM_ID			4
#define UNILINK_DATA 			5
#define UNILINK_CHECK_SUM		6
#define UNILINK_END				7

typedef struct
{
	unsigned char unilink_id;
	unsigned char unilink_len;
	unsigned char unilink_sum;
	unsigned char unilink_data_pos;
	unsigned char unilink_data[256];
} __attribute__((packed)) unilink_t;

int sum_calculate(unilink_t* frame);
int parse_char_unilink(unsigned char Char, unsigned char *step, unilink_t *frame);
void unilink_pack(unsigned char id,unsigned char len,unsigned char* databuf,unilink_t* frame);
int unilink_to_send_buffer(unsigned char* buf,unilink_t* frame);

#endif
