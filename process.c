#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include "datatype.h"
#include "cmd.h"
#include "crc.h"
#include "serial.h"
#include "process.h"


static int control_fd = -1;

static uint32 frame_count=0;
static int frame_tail_err=0;
static int running =0;
 static void *sensor_data_collect();
 static pthread_t recv_pid;



int com_open(char *dev)
{
    control_fd = serial_open(dev, 115200, 0, 1);

	if (control_fd < 0) {

		printf("COM %s open failed,control_fd = %d\n",  dev ,control_fd);
		return SERIAL_CTRL_OPEN_FAILED;
	}
	//

	return 0;
}

void receive_enable()
{
    running = 1;
    pthread_create(&recv_pid, NULL, sensor_data_collect, NULL);
}

void com_close()
{
	void* result2 = NULL;
	running =0;
	pthread_join(recv_pid,&result2);
	serial_close(control_fd);
}

static void save_flying_status(flying_status_s *flying_status)
{
	frame_count++;
	fprintf(fp_fly_status,"%d,%d,",frame_count,flying_status->g_time);
	fprintf(fp_fly_status,"%f,%f,%f,",flying_status->roll,flying_status->pitch,flying_status->yaw);
	fprintf(fp_fly_status,"%f,%f,%f,",flying_status->gx,flying_status->gy,flying_status->gz);
	fprintf(fp_fly_status,"%f,%f,%f,",flying_status->ax,flying_status->ay,flying_status->az);
	fprintf(fp_fly_status,"%d,%d,%d,",flying_status->vn,flying_status->ve,flying_status->vd);
	fprintf(fp_fly_status,"%d,%d,",flying_status->heading,flying_status->b_h);
	fprintf(fp_fly_status,"%f,%f,%f,",flying_status->lat,flying_status->Long,flying_status->g_h);
	fprintf(fp_fly_status,"%f,%f,%f,",flying_status->vx,flying_status->vy,flying_status->vz);
	fprintf(fp_fly_status,"%d,%d,",flying_status->sonar_h,flying_status->waypoint_dest);
	fprintf(fp_fly_status,"%d,%d,%d,%d,%d,",flying_status->pwm[0],flying_status->pwm[1],flying_status->pwm[2],flying_status->pwm[3],flying_status->pwm[4]);
	fprintf(fp_fly_status,"%d,%d,%d,%d,%d,",flying_status->pwm[5],flying_status->pwm[6],flying_status->pwm[7],flying_status->pwm[8],flying_status->pwm[9]);
	fprintf(fp_fly_status,"%x,%x,%x,%x,",flying_status->status,flying_status->gps_status,flying_status->imu_status,flying_status->cpu_s);
	fprintf(fp_fly_status,"%f,%x,%d,%x\n",flying_status->voltage,flying_status->engine,flying_status->cpu_temp,flying_status->rsv);
}

static int frame_compare(uint16 frame_size,uint8 *cmd,uint8 *answer)
{
  int i=0;
  printf("----|receive answer from air:");
  if(frame_size<30){
      for(i=0;i<frame_size;i++){
         printf("%2x",answer[i]);
      }
      printf("\n");


      printf("----|the command  we sent is:");
      for(i=0;i<frame_size;i++){
          printf("%2x",cmd[i]);
      }
      printf("\n");
  }
  for(i=0;i<frame_size;i++){
     if(cmd[i]!=answer[i])
    	 return -1;

  }
  return 1;

}

static void version_extract(uint8 *data)
{
   uint8 board_id=data[0];
   uint32 kernel = *(uint32*)(data+1);
   uint32 app = *(uint32*)(data+5);
   uint32 logic = *(uint32*)(data+9);
   printf("----|board_id:%x \n",board_id);
   printf("----|kernel:%x \n",kernel);
   printf("----|software:%x \n",app);
   printf("----|FPGA:%d \n",logic);

}

static void error_parse(uint8 *data)
{
   int8 error_tag=data[0];
   switch(error_tag){
   case INVALID_CMD:
	   printf("----|invalid command\n");
		   break;
   case UNSUPPORTED_CMD:
	   printf("----|unsupported command\n");
		   break;
   case CMD_TYPE_MISMATCH:
	   printf("----|CMD_TYPE_MISMATCH\n");
	   break;
   case SERIAL_NO_DATA:
	   printf("----|SERIAL_NO_DATA\n");
	   break;
   case SPI_OPEN_FAILED:
	   printf("----|SPI_OPEN_FAILED\n");
	   break;
   case SPI_SETUP_FAILED:
	   printf("----|SPI_SETUP_FAILED\n");
	   break;
   case SPI_DUMP_FAILED:
	   printf("----|SPI_DUMP_FAILED\n");
	   break;
   case PWM_WRITE_FAILED:
	   printf("----|PWM_WRITE_FAILED\n");
	   break;
   case SPI_WRITE_FAILED:
	   printf("----|SPI_WRITE_FAILED\n");
	   break;
   case ADC_TEMP_OPEN_FAILED:
	   printf("----|ADC_TEMP_OPEN_FAILED\n");
	   break;
   case ADC_PS_OPEN_FAILED:
	   printf("----|ADC_PS_OPEN_FAILED\n");
	   break;
   case SERIAL_GPS_OPEN_FAILED:
	   printf("----|SERIAL_GPS_OPEN_FAILED\n");
	   break;
   case SERIAL_CTRL_OPEN_FAILED:
	   printf("----|SERIAL_CTRL_OPEN_FAILED\n");
	   break;
   case CTRL_FRAME_CRC_FAILED:
	   printf("----|CTRL_FRAME_CRC_FAILED\n");
	   break;
   case GPS_FRAME_CRC_FAILED:
	   printf("----|GPS_FRAME_CRC_FAILED\n");
	   break;
   }
}

int control_data_parse(unsigned char *buf, frame_info *frame_info,frame_wait_exe *frame_wait_exe)
{


	unsigned char frame_type = buf[10];
	flying_status_s flying_status;
	int ret=-1;
	int i;
	unsigned char data_buf[4096];


	    if(frame_type==CTRL_FRAME_TYPE_FLY_STATUS) {
	        //copy data field to data_buf to avoid alignment trap
	    	memcpy(data_buf,buf+CTRL_FRAME_MASK_DATA,frame_info->frame_size);
	    	// if the frame is flying status,extract the flying status

            flying_status_parse(data_buf,&flying_status);
            // save the raw frame data
	    	fwrite(buf,frame_info->frame_size,1,fp_fly_status_raw);
	    	// save flying status as CSV format,so that we can use EXCEL to open it
	    	save_flying_status(&flying_status);

		}else if(frame_type==CTRL_FRAME_TYPE_CMD_ACK){
            //if the frame is an answer from air ,check if the answer is ok
			ret=frame_compare(frame_info->frame_size-14,frame_wait_answer,buf+CTRL_FRAME_MASK_DATA);
			if(ret<0){
				printf("----|answer from air is wrong\n");
				return -1;
			}
			//the answer is ok
            frame_wait_exe->type = frame_wait_answer[CTRL_FRAME_MASK_FRAME_TYPE];
            frame_wait_exe->data_size = (frame_wait_answer[CTRL_FRAME_MASK_FRAME_SIZE+1]<<8 )| frame_wait_answer[CTRL_FRAME_MASK_FRAME_SIZE] ;
            frame_wait_exe->frame_num = (frame_wait_answer[CTRL_FRAME_MASK_FRAME_NUM-1]<<8) | frame_wait_answer[CTRL_FRAME_MASK_FRAME_NUM];
            frame_wait_exe->frame_id  = (frame_wait_answer[CTRL_FRAME_MASK_FRAME_ID-1]<<8) | frame_wait_answer[CTRL_FRAME_MASK_FRAME_ID];
			//send out confirm command to the air
			send_cmd_confirm(frame_wait_exe->type);
		}else if(frame_type==CTRL_FRAME_TYPE_CMD_EXE){
			printf("----|receive cmd execute answer:");
			for(i=0;i<frame_info->frame_size;i++)
			     printf("%2x ",buf[i]);
			printf("\n");
			if(buf[11]==frame_wait_exe->type)
				printf("----|command execute sucessfully\n");
			else
				printf("----|command confirm error,cmd type mismatch\n");



		}else if(frame_type==CTRL_FRAME_TYPE_VERSION){
			printf("----|receive version information:");
			for(i=0;i<frame_info->frame_size;i++)
			     printf("%2x ",buf[i]);
			printf("\n");
            version_extract(buf+CTRL_FRAME_MASK_DATA);


		}else if(frame_type==CTRL_FRAME_TYPE_ERROR){
			printf("----|warning :receive error information");
			for(i=0;i<frame_info->frame_size;i++)
			     printf("%2x ",buf[i]);
			printf("\n");
			error_parse(buf+CTRL_FRAME_MASK_DATA);

		}else{
			print_err("----|unsupported frame received\n");

		}


	return 0;
}




/* serial_data_recv_ctrl()
 * try to receive a frame from the serial port for control frame
 * frame_info: pointer to store frame information ,including frame size and valid number of bytes received
 * buf:        allocated buffer to hold raw received data
 * return      FRAME SIZE  if have received a valid frame
 *             0 if have not received a valid frame
 */
unsigned int serial_data_recv_ctrl(frame_info *frame_info ,unsigned char *buf)
{
	  unsigned int nread=0;
	  unsigned int i=0;
	  unsigned int frame_head_found=0;
	  unsigned int frame_crc;

    	// make sure buf never will be overflowed
      nread=read(control_fd,buf+frame_info->bytes_received,BUF_SIZE_CTRL-frame_info->bytes_received);
      fwrite(buf+frame_info->bytes_received,nread,1,fp_bytes_raw);
 	  frame_info->bytes_received += nread;

 	while(frame_info->bytes_received > 0){
 		//print_debug("ctrl bytes received %d\n",frame_info->bytes_received);

 	   // start searching frame head if at least 2 bytes has been received
 	   if(frame_info->bytes_received >= 2){
 	        for(i=0;i<frame_info->bytes_received-1;i++){
 	             if((buf[i]==CTRL_FRAME_START1)&&(buf[i+1]==CTRL_FRAME_START2)){
 	                //found the frame head ,remove useless received bytes before the beginning of the frame
 	                  if(i>0){
 	                       memmove(buf,buf+i,frame_info->bytes_received-i);
 	                       frame_info->bytes_received=frame_info->bytes_received-i;
 	                  }
 	                       frame_head_found = 1;
 	                       //print_debug("ctrl head found\n");
 	                       break;
 	              }
 	         }
 	    }else{
 	    	// not enough data
 	    	return 0;
 	    }

 	  if(frame_head_found){
            // frame head has been found ,start extract frame if we have received minimum bytes of a frame
 	        if(frame_info->bytes_received < CTRL_FRAME_MINIMUM_LEN){
 	        	//do not have enough data for a valid frame
 	        	//print_debug("ctrl :do not have enough data for a valid frame\n");
 	        	return 0;

 	        }
            // we have found the frame head ,extract the machine ID
            //frame_info->machine_id = (buf[3]<<8) |  buf[2];
             //extract the frame size
            frame_info->frame_size = (buf[CTRL_FRAME_MASK_FRAME_SIZE+1]<<8) | buf[CTRL_FRAME_MASK_FRAME_SIZE];

            //check if frame size is valid
            if((frame_info->frame_size >= CTRL_FRAME_MINIMUM_LEN)&&(frame_info->frame_size <= CTRL_FRAME_MAX_LEN)){
                 if(frame_info->frame_size > frame_info->bytes_received){
            	    // do not have received whole frame
                	 //print_debug("ctrl :do not have received whole frame\n");
            	     return 0;
                 }

                 // we have received the whole frame ,so check the frame tail
                 if(buf[frame_info->frame_size-1]==CTRL_FRAME_END){

            	     frame_crc = (buf[frame_info->frame_size-2]<<8) | buf[frame_info->frame_size-3];
            	     if(frame_crc==crc_checksum16(buf, frame_info->frame_size-3)){
            	     //if(1){
            		    // we have a valid CRC
                         if(frame_tail_err){
                        	 print_debug("frame time :%d\n",*(uint32*)(buf+47));
                        	 frame_tail_err=0;

                         }
            		     return frame_info->frame_size;
            	     }else{
                        // invalid CRC ,remove the whole frame from the buffer

                        print_debug("invalid crc frame,,frame time :%d\n",*(uint32*)(buf+47));

            		    memmove(buf,buf+frame_info->frame_size,frame_info->bytes_received-frame_info->frame_size);
            	    	frame_info->bytes_received=frame_info->bytes_received-frame_info->frame_size;
            	    	frame_info->frame_size = 0;
            		    frame_head_found = 0;


            	     }

                 }else{
            	     //frame tail not found ,so the frame is invalid,
            	     //we should have incorrectly detected a start of frame
                     //remove the 2 frame head bytes and start searching frame head again
                	 frame_tail_err=1;
                	 print_debug("ctrl :frame tail not found ,so the frame is invalid\n");
                	 for(i=0;i<frame_info->bytes_received;i++)
                		 printf("%2x ",buf[i]);
                	 printf("\n");
            	     memmove(buf,buf+2,frame_info->bytes_received-2);
            	     frame_info->bytes_received=frame_info->bytes_received-2;
            	     frame_info->frame_size = 0;
            	     frame_head_found = 0;
            	     continue;
                 }
            }else{
                // invalid frame_size ,which means wrong frame head is detected
                 // we need to remove the 2 wrong frame head bytes
            	 print_debug("ctrl :invalid frame_size \n");
            	 for(i=0;i<frame_info->bytes_received;i++)
            		 printf("%2x ",buf[i]);
            	 printf("\n");
                 memmove(buf,buf+2,frame_info->bytes_received-2);
                 frame_info->bytes_received=frame_info->bytes_received-2;
                 frame_info->frame_size = 0;
                 frame_head_found = 0;
            }
       }else{
            //unable to find a valid start of frame
            //so check the last byte is FRAME_START1 in order to keep it for next time
    	    print_debug("ctrl :invalid start of frame\n");
       	    for(i=0;i<frame_info->bytes_received;i++)
       		   printf("%2x ",buf[i]);
       	    printf("\n");
            if(buf[frame_info->bytes_received-1]==CTRL_FRAME_START1){
                buf[0] = CTRL_FRAME_START1;
                frame_info->bytes_received = 1;
            }else{
                //discard the whole buffer
                frame_info->bytes_received = 0;
            }
            return 0;
        }


 	}
 	return 0;
}


void *sensor_data_collect()
{


	unsigned int data_len = 0;

	fd_set rfds;

	unsigned char buf_ctrl[BUF_SIZE_CTRL];        // buffer to store raw control frame data




	 frame_info frame_info_ctrl={0,0};
	 frame_wait_exe frame_wait_exe = {frame_wait_answer+CTRL_FRAME_MASK_DATA,0,0,0,0};

	struct timeval tv;

	while (running) {
	         FD_ZERO(&rfds);
	         FD_SET(control_fd, &rfds);
	         tv.tv_sec=0;
             tv.tv_usec=80000; //80 ms
		     if (select(1 + control_fd, &rfds, NULL, NULL, &tv) > 0) {
                    if (FD_ISSET(control_fd, &rfds)) {

					data_len = serial_data_recv_ctrl(&frame_info_ctrl,buf_ctrl);

					if(data_len > 0){

					   control_data_parse(buf_ctrl,&frame_info_ctrl,&frame_wait_exe);
					   if(frame_info_ctrl.bytes_received > frame_info_ctrl.frame_size){
					        memmove(buf_ctrl,buf_ctrl+frame_info_ctrl.frame_size,frame_info_ctrl.bytes_received-frame_info_ctrl.frame_size);
                    	}
					   frame_info_ctrl.frame_size=0;
					   frame_info_ctrl.bytes_received -= data_len;
					}
				}
		    } else {

			     //print_err("Device read timeout\n");// serial port received timeout.

		    }
	}

}



int control_cmd_send(uint8 *buf,uint32 buf_size)
{
	return serial_write(control_fd, buf, buf_size);

}



