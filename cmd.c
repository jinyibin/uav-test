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

  void send_way_point()
 {
 	uint16 crc_value;
 	uint8  buf[4096];
 	char file_head[128];
 	int i=0;
    FILE *fp;
    uint16 id;
    float  v;
    double Long;
    double lat;
    float  h;
    unsigned char   task;
    uint8 data[4096];
    uint32 frame_size;

    fp=fopen("wp_init.csv","r");
    if(fp==NULL){
    	printf("----|can not open file:wp_init.csv\n");
    	printf("----|sending waypoint failed\n");
    }
    //read out file information
    fscanf(fp,"%s,%s,%s,%s,%s,%s",file_head,file_head+10,file_head+20,file_head+30,file_head+40,file_head+50);
    //read data
    while(fscanf(fp,"%d,%f,%lf,%lf,%f,%x",&id,&v,&Long,&lat,&h,&task)==6){
    	*(uint16*)(data+i*27) = id;
    	*(float*)(data+2+i*27)  = v;
    	*(double*)(data+6+i*27)  = Long;
    	*(double*)(data+14+i*27)  = lat;
    	*(float*)(data+22+i*27)  = h;
    	*(uint8*)(data+26+i*27)  = task;
    	i++;
    	printf("----|%2d,%8f,%lf,%lf,%8f,%2x\n",id,v,Long,lat,h,task);

    }
    frame_size = i+14;
 	buf[0] = CTRL_FRAME_START1;
 	buf[1] = CTRL_FRAME_START2;
 	buf[2] = plane_id&0xFF;
 	buf[3] = plane_id>>8;
 	*(uint32*)(buf+4) = frame_size;
	buf[8] = 1;
	buf[9] = 1;
	buf[10] = CTRL_FRAME_TYPE_WAYPOINT_INIT;
    memcpy(buf+11,data,i*27);
 	crc_value = crc_checksum16(buf, frame_size-3);
 	buf[frame_size-3] = crc_value&0xFF;
 	buf[frame_size-2] = crc_value>>8;
 	buf[frame_size-1] = CTRL_FRAME_END;
 	memcpy(frame_wait_answer,buf,frame_size);
 	/*
 	printf("----|");
 	for( i=0;i<frame_size;i++)
 		printf("%2x ",frame_wait_answer[i]);
 	printf("\n");
 	*/
 	control_cmd_send(frame_wait_answer, frame_size);
 	printf("----|waiting for answer.....................\n");
 }

  void way_point_modify()
  {
  	uint16 crc_value;
  	uint8  buf[128];
  	char file_head[128];

     FILE *fp;
     uint8 m_type;
     uint16 id;
     float  v;
     double Long;
     double lat;
     float  h;
     unsigned char   task;
     uint8 data[32];
     uint32 frame_size;

     fp=fopen("wp_modify.csv","r");
     if(fp==NULL){
     	printf("----|can not open file:wp_modify.csv\n");
     	printf("----|sending waypoint modify command failed\n");
     }
     //read out file information
     fscanf(fp,"%s,%s,%s,%s,%s,%s,%s",file_head,file_head+10,file_head+20,file_head+30,file_head+40,file_head+50,file_head+60);
     // read data
     while(fscanf(fp,"%x,%d,%f,%lf,%lf,%f,%x",&m_type,&id,&v,&Long,&lat,&h,&task)==7){
    	 *(uint8*)(data) = m_type;
     	*(uint16*)(data+1) = id;
     	*(float*)(data+3)  = v;
     	*(double*)(data+7)  = Long;
     	*(double*)(data+15)  = lat;
     	*(float*)(data+23)  = h;
     	*(uint8*)(data+27)  = task;
     	*(uint32*)(data+28)  = 0;
     	printf("----|%2x,%2d,%8f,%lf,%lf,%8f,%2x\n",m_type,id,v,Long,lat,h,task);

     }
     frame_size = 46;
  	buf[0] = CTRL_FRAME_START1;
  	buf[1] = CTRL_FRAME_START2;
  	buf[2] = plane_id&0xFF;
  	buf[3] = plane_id>>8;
  	*(uint32*)(buf+4) = frame_size;
 	buf[8] = 1;
 	buf[9] = 1;
 	buf[10] = CTRL_FRAME_TYPE_WAYPOINT_INIT;
     memcpy(buf+11,data,32);
  	crc_value = crc_checksum16(buf, frame_size-3);
  	buf[frame_size-3] = crc_value&0xFF;
  	buf[frame_size-2] = crc_value>>8;
  	buf[frame_size-1] = CTRL_FRAME_END;
  	memcpy(frame_wait_answer,buf,frame_size);
  	/*
  	printf("----|");
  	for( i=0;i<frame_size;i++)
  		printf("%2x ",frame_wait_answer[i]);
  	printf("\n");
  	*/
  	control_cmd_send(frame_wait_answer, frame_size);
  	printf("----|waiting for answer.....................\n");
  }

  void send_joystick_data()
  {
	 	uint16 crc_value;
	 	uint8  buf[30];


	 	buf[0] = CTRL_FRAME_START1;
	 	buf[1] = CTRL_FRAME_START2;
	 	buf[2] = plane_id&0xFF;
	 	buf[3] = plane_id>>8;
	 	*(uint32*)(buf+4) = 0x1e;
		buf[8] = 1;
		buf[9] = 1;
	 	buf[10] = CTRL_FRAME_TYPE_STICK_DATA ;
	 	*(uint16*)(buf+11) = 0;
	 	*(uint16*)(buf+13) = 1;
	 	*(uint16*)(buf+15) = 2;
	 	*(uint16*)(buf+17) = 3;
	 	*(uint16*)(buf+19) = 4;
	 	*(uint16*)(buf+21) = 5;
	 	*(uint16*)(buf+23) = 6;
	 	*(uint16*)(buf+25) = 7;
	 	crc_value = crc_checksum16(buf, 27);
	 	buf[27] = crc_value&0xFF;
	 	buf[28] = crc_value>>8;
	 	buf[29] = CTRL_FRAME_END;

	 	control_cmd_send(buf, 30);
  }








