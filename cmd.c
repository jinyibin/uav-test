#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "crc.h"
#include "cmd.h"
#include "process.h"





void flying_status_parse(uint8 *data,flying_status_s *flying_status)
{

    flying_status->roll = *(float*)data;

    flying_status->pitch =  *(float*)(data +4);
    flying_status->yaw =  *(float*)(data +8);
    flying_status->gx =  *(float*)(data +12);
    flying_status->gy =  *(float*)(data +16);
    flying_status->gz =  *(float*)(data +20);
    flying_status->ax =  *(float*)(data +24);
    flying_status->ay =  *(float*)(data +28);
    flying_status->az =  *(float*)(data +32);
    flying_status->g_time =  *(uint32*)(data +36);
    flying_status->vn =  *(uint32*)(data +40);
    flying_status->ve =  *(uint32*)(data +44);
    flying_status->vd =  *(uint32*)(data +48);
    flying_status->h =  *(uint32*)(data +52);
    flying_status->b_h =  *(uint32*)(data +56);
    flying_status->lat =  *(double*)(data +60);
    flying_status->Long =  *(double*)(data +68);
    flying_status->g_h =  *(double*)(data +76);
    flying_status->vx =  *(float*)(data +84);
    flying_status->vy =  *(float*)(data +88);
    flying_status->vz =  *(float*)(data +92);
    flying_status->sonar_h =  *(float*)(data +96);
    flying_status->waypoint_dest =  *(uint32*)(data +100);
    flying_status->pwm[0] = *(uint16*)(data+102);
    flying_status->pwm[1] = *(uint16*)(data+104);
    flying_status->pwm[2] = *(uint16*)(data+106);
    flying_status->pwm[3] = *(uint16*)(data+108);
    flying_status->pwm[4] = *(uint16*)(data+110);
    flying_status->pwm[5] = *(uint16*)(data+112);
    flying_status->pwm[6] = *(uint16*)(data+114);
    flying_status->pwm[7] = *(uint16*)(data+116);
    flying_status->pwm[8] = *(uint16*)(data+118);
    flying_status->pwm[9] = *(uint16*)(data+120);
    flying_status->status =  *(uint16*)(data+122);
    flying_status->gps_status =  *(uint8*)(data+124);
    flying_status->imu_status =  data[125];
    flying_status->cpu_s =  data[126];
    flying_status->voltage =  data[127]*3.3*11/256;
    flying_status->engine =  data[128];
    flying_status->cpu_temp =  *(int8*)(data+129);
    flying_status->rsv =  data[130];

}








 void send_cmd_confirm(uint8 data)
 {
	 	uint16 crc_value;
	 	uint8  buf[16];

	 	buf[0] = CTRL_FRAME_START1;
	 	buf[1] = CTRL_FRAME_START2;
	 	buf[2] = plane_id&0xFF;
	 	buf[3] = plane_id>>8;
	 	*(uint32*)(buf+4) = 0x10;
		buf[8] = 1;
		buf[9] = 1;
	 	buf[10] = CTRL_FRAME_TYPE_CMD_CONFIRM ;
	 	buf[11] = data;
	 	buf[12] = 0;
	 	crc_value = crc_checksum16(buf, 13);
	 	buf[13] = crc_value&0xFF;
	 	buf[14] = crc_value>>8;
	 	buf[15] = CTRL_FRAME_END;
	 	control_cmd_send(buf, 16);
	 	printf("----|confirm cmd sent,type=%x ,waiting for execution answer.....................\n",data);

 }


  void send_control_cmd(uint8 data)
 {
 	uint16 crc_value;
 	uint8  buf[15];
 	int i=0;

 	buf[0] = CTRL_FRAME_START1;
 	buf[1] = CTRL_FRAME_START2;
 	buf[2] = plane_id&0xFF;
 	buf[3] = plane_id>>8;
 	*(uint32*)(buf+4) = 0x0F;
	buf[8] = 1;
	buf[9] = 1;
 	buf[10] = data ;
 	buf[11] = 0;
 	crc_value = crc_checksum16(buf, 12);
 	buf[12] = crc_value&0xFF;
 	buf[13] = crc_value>>8;
 	buf[14] = CTRL_FRAME_END;
 	memcpy(frame_wait_answer,buf,15);
 	printf("----|");
 	for( i=0;i<15;i++)
 		printf("%2x ",frame_wait_answer[i]);
 	printf("\n");
 	control_cmd_send(frame_wait_answer, 15);
 	printf("----|waiting for answer.....................\n");
 }








