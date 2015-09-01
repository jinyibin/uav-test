#ifndef PROCESS_H
#define PROCESS_H
#include "datatype.h"


#define SERIAL_BUF_SIZE 1024


#define CONTROL_COM "/dev/ttyLP3"



#define GPS_SENSOR 0
#define CONTROL_DEVICE 1
#define HIGH_SENSOR 2

#define BUF_SIZE_GPS 512
#define BUF_SIZE_CTRL 4096
#define GPS_FRAME_LEN 104


#define GPS_FRAME_START1 0xFF
#define GPS_FRAME_START2 0x02
#define GPS_FRAME_START3 0x90
#define GPS_FRAME_END 0x03
#define GPS_FRAME_MINIMUM_LEN 104
#define GPS_FRAME_MAX_LEN     200

#define GPS_FRAME_LEN_NO_DATA 8       // total length of field other than data
#define GPS_FRAME_CRC_LEN    GPS_FRAME_LEN-5
#define GPS_FRAME_MASK_CMD   2        // cmd field position start from 0
#define GPS_FRAME_MASK_FRAME_SIZE 3   // frame size field position start from 0




typedef struct frame_info
{

	uint32 bytes_received;
	uint32 frame_size;

}frame_info;

typedef struct frame_wait_exe
{

	uint8 *data;        //pointer to the buffer storing the data waiting to be processed
	uint8 type;        //frame type waiting to be confirmed
	uint16 frame_num;   // total number of frames that a whole data packet is divided
	uint16 frame_id;    // which frame it is in a data packet
	uint32 data_size;

}frame_wait_exe;

uint16 plane_id;
unsigned char frame_wait_answer[BUF_SIZE_CTRL]; // buffer to store data field of control frame
FILE *fp_fly_status_raw;
FILE *fp_fly_status;



int com_open(char *dev);
void com_close();
int control_data_parse(unsigned char *buf, frame_info *frame_info,frame_wait_exe *frame_wait_exe);
int control_cmd_send(uint8 *buf,uint32 buf_size);

#endif
