#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "crc.h"
#include "cmd.h"
#include "process.h"

extern WINDOW *console;



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
    flying_status->vn =  *(int*)(data +40);
    flying_status->ve =  *(int*)(data +44);
    flying_status->vd =  *(int*)(data +48);
    flying_status->heading =  *(int*)(data +52);
    flying_status->b_h =  *(int*)(data +56);
    flying_status->lat =  *(double*)(data +60);
    flying_status->Long =  *(double*)(data +68);
    flying_status->g_h =  *(double*)(data +76);
    flying_status->vx =  *(float*)(data +84);
    flying_status->vy =  *(float*)(data +88);
    flying_status->vz =  *(float*)(data +92);
    flying_status->sonar_h =  *(uint32*)(data +96);
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
	 	*(uint16*)(buf+4) = 0x10;
		buf[6] = 1;
		buf[7] = 0;
		buf[8] = 1;
		buf[9] = 0;
	 	buf[10] = CTRL_FRAME_TYPE_CMD_CONFIRM ;
	 	buf[11] = data;
	 	buf[12] = 0;
	 	crc_value = crc_checksum16(buf, 13);
	 	buf[13] = crc_value&0xFF;
	 	buf[14] = crc_value>>8;
	 	buf[15] = CTRL_FRAME_END;
	 	control_cmd_send(buf, 16);
	 	printx(console,"----|confirm cmd sent,type=%x ,waiting for execution answer.....................\n",data);
	 	wrefresh(console);

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
 	*(uint16*)(buf+4) = 0x0F;
	buf[6] = 1;
	buf[7] = 0;
	buf[8] = 1;
	buf[9] = 0;
 	buf[10] = data ;
 	buf[11] = 0;
 	crc_value = crc_checksum16(buf, 12);
 	buf[12] = crc_value&0xFF;
 	buf[13] = crc_value>>8;
 	buf[14] = CTRL_FRAME_END;
 	memcpy(frame_wait_answer,buf,15);
 	printx(console,"----|");
 	for( i=0;i<15;i++)
 		printx(console,"%2x ",frame_wait_answer[i]);
 	printx(console,"\n");
 	control_cmd_send(frame_wait_answer, 15);
 	printx(console,"----|waiting for answer.....................\n");
 	wrefresh(console);
 }

  void send_way_point()
 {
 	uint16 crc_value;
 	uint8  buf[4096];
 	char file_head[128];
 	int i=0;
    FILE *fp;
    uint32 id;
    uint32   task;
    uint32   task_para;
    float  v;
    double Long;
    double lat;
    float  h;

    uint8 data[4096];
    uint32 frame_size;
    uint32 total_wp_num;
    uint32  wp_num_of_this_frame;

    fp=fopen("wp_init.csv","r");
    if(fp==NULL){
    	printx(console,"----|can not open file:wp_init.csv\n");
    	printx(console,"----|sending waypoint failed\n");
    }
    //read out file information
    fscanf(fp,"%s,%s",file_head+60,file_head+70);
    fscanf(fp,"%d,%d",&total_wp_num,&wp_num_of_this_frame);
    fscanf(fp,"%s,%s,%s,%s,%s,%s,%s",file_head,file_head+10,file_head+20,file_head+30,file_head+40,file_head+50,file_head+60);
    //read data
    while(fscanf(fp,"%d,%x,%x,%f,%lf,%lf,%f",&id,&task,&task_para,&v,&Long,&lat,&h)==7){
    	*(uint16*)(data+i*28) = (uint16)id;
    	*(uint8*)(data+2+i*28)  = (uint8)task;
    	*(uint8*)(data+3+i*28)  = (uint8)task_para;
    	*(float*)(data+4+i*28)  = v;
    	*(double*)(data+8+i*28)  = Long;
    	*(double*)(data+16+i*28)  = lat;
    	*(float*)(data+24+i*28)  = h;

    	i++;
    	printx(console,"----|%2d,%2x,%2x,%8f,%lf,%lf,%8f\n",id,task,task_para,v,Long,lat,h);

    }
    frame_size = i*28+18;
 	buf[0] = CTRL_FRAME_START1;
 	buf[1] = CTRL_FRAME_START2;
 	buf[2] = plane_id&0xFF;
 	buf[3] = plane_id>>8;
 	*(uint16*)(buf+4) = frame_size;
	buf[6] = 1;
	buf[7] = 0;
	buf[8] = 1;
	buf[9] = 0;
	buf[10] = CTRL_FRAME_TYPE_WAYPOINT_INIT;
	*(uint16*)(buf+11) = (uint16)total_wp_num;
	buf[13] = (uint8)wp_num_of_this_frame;
	buf[14] = 0x0;
    memcpy(buf+15,data,i*28);
 	crc_value = crc_checksum16(buf, frame_size-3);
 	buf[frame_size-3] = crc_value&0xFF;
 	buf[frame_size-2] = crc_value>>8;
 	buf[frame_size-1] = CTRL_FRAME_END;
 	memcpy(frame_wait_answer,buf,frame_size);

 	printx(console,"----|");
 	for( i=0;i<frame_size;i++)
 		printx(console,"%2x ",frame_wait_answer[i]);
 	printx(console,"\n");

 	control_cmd_send(frame_wait_answer, frame_size);
 	printx(console,"----|waiting for answer.....................\n");
 	wrefresh(console);
 }

  void way_point_modify()
  {
  	uint16 crc_value;
  	uint8  buf[128];
  	char file_head[128];

     FILE *fp;
     uint32 m_type;
     uint32 id;
     uint32   task;
     uint32   task_para;
     float  v;
     double Long;
     double lat;
     float  h;

     uint8 data[36];
     uint32 frame_size;
     int i=0;

     fp=fopen("wp_modify.csv","r");
     if(fp==NULL){
     	printx(console,"----|can not open file:wp_modify.csv\n");
     	printx(console,"----|sending waypoint modify command failed\n");
     }
     //read out file information
     fscanf(fp,"%s,%s,%s,%s,%s,%s,%s,%s",file_head,file_head+10,file_head+20,file_head+30,file_head+40,file_head+50,file_head+60,file_head+70);
     // read data
     while(fscanf(fp,"%d,%d,%d,%d,%f,%lf,%lf,%f",&m_type,&id,&task,&task_para,&v,&Long,&lat,&h)==8){
    	 *(uint32*)(data) = (uint32)m_type;
     	*(uint16*)(data+4) = id;
     	*(uint8*)(data+6)  = (uint8)task;
     	*(uint8*)(data+7)  = (uint8)task_para;
     	*(float*)(data+8)  = v;
     	*(double*)(data+12)  = Long;
     	*(double*)(data+20)  = lat;
     	*(float*)(data+28)  = h;

     	*(uint32*)(data+32)  = 0;
     	printx(console,"----|%2x,%2d,%2x,%2x,%8f,%lf,%lf,%8f\n",m_type,id,task,task_para,v,Long,lat,h);

     }
     frame_size = 50;
  	buf[0] = CTRL_FRAME_START1;
  	buf[1] = CTRL_FRAME_START2;
  	buf[2] = plane_id&0xFF;
  	buf[3] = plane_id>>8;
  	*(uint16*)(buf+4) = frame_size;
	buf[6] = 1;
	buf[7] = 0;
	buf[8] = 1;
	buf[9] = 0;
 	buf[10] = CTRL_FRAME_TYPE_WAYPOINT_MODIFY;
     memcpy(buf+11,data,36);
  	crc_value = crc_checksum16(buf, frame_size-3);
  	buf[frame_size-3] = crc_value&0xFF;
  	buf[frame_size-2] = crc_value>>8;
  	buf[frame_size-1] = CTRL_FRAME_END;
  	memcpy(frame_wait_answer,buf,frame_size);

  	printx(console,"----|");
  	for( i=0;i<frame_size;i++)
  		printx(console,"%2x ",frame_wait_answer[i]);
  	printx(console,"\n");

  	control_cmd_send(frame_wait_answer, frame_size);
  	printx(console,"----|waiting for answer.....................\n");
  	wrefresh(console);
  }

  void send_joystick_data()
  {
	 	uint16 crc_value;
	 	uint8  buf[30];


	 	buf[0] = CTRL_FRAME_START1;
	 	buf[1] = CTRL_FRAME_START2;
	 	buf[2] = plane_id&0xFF;
	 	buf[3] = plane_id>>8;
	 	*(uint16*)(buf+4) = 0x1e;
		buf[6] = 1;
		buf[7] = 0;
		buf[8] = 1;
		buf[9] = 0;
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
  void link_test()
   {
   	uint16 crc_value;
   	uint8  buf[18];
   	int i=0;

   	buf[0] = CTRL_FRAME_START1;
   	buf[1] = CTRL_FRAME_START2;
   	buf[2] = plane_id&0xFF;
   	buf[3] = plane_id>>8;
   	*(uint16*)(buf+4) = 0x12;
	buf[6] = 1;
	buf[7] = 0;
	buf[8] = 1;
	buf[9] = 0;
   	buf[10] = CTRL_FRAME_TYPE_LINK_TEST ;
   	*(uint32*)(buf+11) = 1;
   	crc_value = crc_checksum16(buf, 15);
   	buf[15] = crc_value&0xFF;
   	buf[16] = crc_value>>8;
   	buf[17] = CTRL_FRAME_END;

   	printx(console,"----|");
   	for( i=0;i<15;i++)
   		printx(console,"%2x ",buf[i]);
   	printx(console,"\n");
   	wrefresh(console);
   	control_cmd_send(buf, 18);

   }
  void heli_config()
    {
    	uint16 crc_value;
    	uint8  buf[25];
    	char file_head[254];

       FILE *fp;
       uint32 heli_type;
       uint32 fuel_vol;
       uint32 fuel_consum;
       uint32 swash_typ;
       uint32 servo_freq;
       uint32 tail_gyro;
       uint32 max_speed;
       uint32 gps_typ;
       uint32 radar;
       uint32 rsv1;
       uint32 rsv2;

       uint8 data[11];
       uint32 frame_size;
       int i=0;

       fp=fopen("heli_config.csv","r");
       if(fp==NULL){
       	printx(console,"----|can not open file:heli_config.csv\n");
       	printx(console,"----|sending heli config command failed\n");
       }
       //read out file information
       fscanf(fp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",file_head,file_head+10,
    		   file_head+20,file_head+30,file_head+40,file_head+50,
    		   file_head+60,file_head+70,file_head+80,file_head+90,file_head+100);
       // read data
       while(fscanf(fp,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &heli_type,&fuel_vol,
    		   &fuel_consum,&swash_typ,&servo_freq,&tail_gyro,&max_speed,&gps_typ,&radar,&rsv1,&rsv2)==11){
            data[0]=(uint8)heli_type;
            data[1]=(uint8)fuel_vol;
            data[2]=(uint8)fuel_consum;
            data[3]=(uint8)swash_typ;
            data[4]=(uint8)servo_freq;
            data[5]=(uint8)tail_gyro;
            data[6]=(uint8)max_speed;
            data[7]=(uint8)gps_typ;
            data[8]=(uint8)radar;
            data[9]=(uint8)rsv1;
            data[10]=(uint8)rsv2;

         	printx(console,"----|%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x\n",heli_type,fuel_vol,
     		   fuel_consum,swash_typ,servo_freq,tail_gyro,max_speed,gps_typ,radar,rsv1,rsv2);

       }
       frame_size = 25;
    	buf[0] = CTRL_FRAME_START1;
    	buf[1] = CTRL_FRAME_START2;
    	buf[2] = plane_id&0xFF;
    	buf[3] = plane_id>>8;
    	*(uint16*)(buf+4) = frame_size;
    	buf[6] = 1;
    	buf[7] = 0;
    	buf[8] = 1;
    	buf[9] = 0;
    	buf[10] = CTRL_FRAME_TYPE_HELI_CONFIG;
        memcpy(buf+11,data,frame_size-14);
    	crc_value = crc_checksum16(buf, frame_size-3);
    	buf[frame_size-3] = crc_value&0xFF;
    	buf[frame_size-2] = crc_value>>8;
    	buf[frame_size-1] = CTRL_FRAME_END;
    	memcpy(frame_wait_answer,buf,frame_size);

    	printx(console,"----|");
    	for( i=0;i<frame_size;i++)
    		printx(console,"%2x ",frame_wait_answer[i]);
    	printx(console,"\n");

    	control_cmd_send(frame_wait_answer, frame_size);
    	printx(console,"----|waiting for answer.....................\n");
    	wrefresh(console);
    }
  void send_fly_para1()
      {
      	uint16 crc_value;
      	uint8  buf[46];

         FILE *fp;
         uint32 data[16];
         uint32 frame_size;
         int i=0;

         fp=fopen("fly_para1.csv","r");
         if(fp==NULL){
        	 wattron(console,COLOR_PAIR(2));
         	printx(console,"----|can not open file:fly_para1.csv\n");
         	printx(console,"----|sending fly_para1 command failed\n");
         	wattroff(console,COLOR_PAIR(2));
         }

         // read data
         while(fscanf(fp,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", data,data+1,data+2,data+3,data+4,data+5,
      		   data+6,data+7,data+8,data+9,data+10,data+11,data+12,data+13,data+14,data+15)==16){


           	printx(console,"----|%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",data[0],data[1],
           			data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],
           			data[11],data[12],data[13],data[14],data[15]);

         }
         frame_size = 46;
      	buf[0] = CTRL_FRAME_START1;
      	buf[1] = CTRL_FRAME_START2;
      	buf[2] = plane_id&0xFF;
      	buf[3] = plane_id>>8;
      	*(uint16*)(buf+4) = frame_size;
    	buf[6] = 1;
    	buf[7] = 0;
    	buf[8] = 1;
    	buf[9] = 0;
      	buf[10] = CTRL_FRAME_TYPE_FLY_PARA1;
        *(uint16*)(buf+11) = (uint16)data[0];
        *(uint16*)(buf+13) = (uint16)data[1];
        *(uint16*)(buf+15) = (uint16)data[2];
        *(uint16*)(buf+17) = (uint16)data[3];
        *(uint16*)(buf+19) = (uint16)data[4];
        *(uint16*)(buf+21) = (uint16)data[5];
        *(uint16*)(buf+23) = (uint16)data[6];
        *(uint16*)(buf+25) = (uint16)data[7];
        *(uint16*)(buf+27) = (uint16)data[8];
        *(uint16*)(buf+29) = (uint16)data[9];
        *(uint16*)(buf+31) = (uint16)data[10];
        *(uint16*)(buf+33) = (uint16)data[11];
        *(uint16*)(buf+35) = (uint16)data[12];
        *(uint16*)(buf+37) = (uint16)data[13];
        *(uint16*)(buf+39) = (uint16)data[14];
        *(uint16*)(buf+41) = (uint16)data[15];
      	crc_value = crc_checksum16(buf, frame_size-3);
      	buf[frame_size-3] = crc_value&0xFF;
      	buf[frame_size-2] = crc_value>>8;
      	buf[frame_size-1] = CTRL_FRAME_END;
      	memcpy(frame_wait_answer,buf,frame_size);

      	printx(console,"----|");
      	for( i=0;i<frame_size;i++)
      		printx(console,"%2x ",frame_wait_answer[i]);
      	printx(console,"\n");

      	control_cmd_send(frame_wait_answer, frame_size);
      	printx(console,"----|waiting for answer.....................\n");
      	wrefresh(console);
      }

  void send_fly_para2()
       {
       	uint16 crc_value;
       	uint8  buf[46];

          FILE *fp;
          uint32 data[16];
          uint32 frame_size;
          int i=0;

          fp=fopen("fly_para2.csv","r");
          if(fp==NULL){
        	  wattron(console,COLOR_PAIR(2));
          	printx(console,"----|can not open file:fly_para2.csv\n");
          	printx(console,"----|sending fly_para2 command failed\n");
          	wattroff(console,COLOR_PAIR(2));
          }

          // read data
          while(fscanf(fp,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", data,data+1,data+2,data+3,data+4,data+5,
       		   data+6,data+7,data+8,data+9,data+10,data+11,data+12,data+13,data+14,data+15)==16){


            	printx(console,"----|%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",data[0],data[1],
            			data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],
            			data[11],data[12],data[13],data[14],data[15]);

          }
          frame_size = 46;
       	buf[0] = CTRL_FRAME_START1;
       	buf[1] = CTRL_FRAME_START2;
       	buf[2] = plane_id&0xFF;
       	buf[3] = plane_id>>8;
       	*(uint16*)(buf+4) = frame_size;
    	buf[6] = 1;
    	buf[7] = 0;
    	buf[8] = 1;
    	buf[9] = 0;
       	buf[10] = CTRL_FRAME_TYPE_FLY_PARA2;
         *(uint16*)(buf+11) = (uint16)data[0];
         *(uint16*)(buf+13) = (uint16)data[1];
         *(uint16*)(buf+15) = (uint16)data[2];
         *(uint16*)(buf+17) = (uint16)data[3];
         *(uint16*)(buf+19) = (uint16)data[4];
         *(uint16*)(buf+21) = (uint16)data[5];
         *(uint16*)(buf+23) = (uint16)data[6];
         *(uint16*)(buf+25) = (uint16)data[7];
         *(uint16*)(buf+27) = (uint16)data[8];
         *(uint16*)(buf+29) = (uint16)data[9];
         *(uint16*)(buf+31) = (uint16)data[10];
         *(uint16*)(buf+33) = (uint16)data[11];
         *(uint16*)(buf+35) = (uint16)data[12];
         *(uint16*)(buf+37) = (uint16)data[13];
         *(uint16*)(buf+39) = (uint16)data[14];
         *(uint16*)(buf+41) = (uint16)data[15];
       	crc_value = crc_checksum16(buf, frame_size-3);
       	buf[frame_size-3] = crc_value&0xFF;
       	buf[frame_size-2] = crc_value>>8;
       	buf[frame_size-1] = CTRL_FRAME_END;
       	memcpy(frame_wait_answer,buf,frame_size);

       	printx(console,"----|");
       	for( i=0;i<frame_size;i++)
       		printx(console,"%2x ",frame_wait_answer[i]);
       	printx(console,"\n");

       	control_cmd_send(frame_wait_answer, frame_size);
       	printx(console,"----|waiting for answer.....................\n");
       	wrefresh(console);
       }






