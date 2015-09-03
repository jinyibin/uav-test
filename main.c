#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "datatype.h"
#include "cmd.h"
#include <pthread.h>

#include "process.h"
#include "serial.h"

#include <unistd.h>

pthread_t send_data;
int joystick_test=0;
int running=0;

void how_to_use()
{
  printf("-------------------command list------------------\n");
  printf("exit       : save the data and exit the program \n");
  printf("stop       : stop the sending data,but not exit  \n");
  printf("help       : show you command list\n");
  printf("servo test : send servo test command\n");
  printf("link  test : send link test command\n");
  printf("joystick   : send joystick data\n");
  printf("version    : read version of app,kernel,board and FPGA etc\n");
  printf("take off   : send plane take off command list\n");
  printf("remote1    : set plane into remote mode1\n");
  printf("remote2    : set plane into remote mode2\n");
  printf("hover      : let the plane hover \n");
  printf("fly        : let the plane fly according to the way point\n");
  printf("return     : let the plane fly back\n");
  printf("land       : let the plane land\n");
  printf("config     : configure the auto pilot system,like heli type,oil volume,max speed etc\n");
  printf("para set1  : set the parameter1 of auto pilot system\n");
  printf("para set2  : set the parameter2 of auto pilot system\n");
  printf("wp modify  : modify the way point,add data in the wp_modify.csv\n");
  printf("             format:m_type,id,v,Long,lat,h,task\n");
  printf("wp init    : initialize the way point,add data in the wp_ini.csv\n");
  printf("             format:id,v,Long,lat,h,task\n");
  printf("firmware   : update the firmware\n");
  printf("export     : export data\n");
}

void file_fly_status_init()
{
   fprintf(fp_fly_status,"frame,time(ms),");
   fprintf(fp_fly_status,"roll,pitch,yaw,");
   fprintf(fp_fly_status,"gx,gy,gz,");
   fprintf(fp_fly_status,"ax,ay,az,");
   fprintf(fp_fly_status,"vn,ve,vd,");
   fprintf(fp_fly_status,"high,b_high,");
   fprintf(fp_fly_status,"lat,long,g_h,");
   fprintf(fp_fly_status,"vx,vy,vz,");
   fprintf(fp_fly_status,"sonar,wp_id,");
   fprintf(fp_fly_status,"pwm0,pwm1,pwm2,pwm3,pwm4,pwm5,pwm6,pwm7,pwm8,pwm9,");
   fprintf(fp_fly_status,"fly_status,gps_s,imu_s,cpu_s,voltage,engine,cpu_t,rsv\n");
}

void *send_data_periodly()
{
    while(running){
	    if(joystick_test==1){
            send_joystick_data();
        }
        usleep(20000);
    }
    return 0;

}




int main( int argc,char *argv[])
{
	char buf[128];
	char *dev=buf;
	char *command;
	int ret;


    printf("please enter the COM name,enter 'y' to use default /dev/ttyUSB0:");
    fgets(buf,40,stdin);
    buf[strlen(buf)-1]='\0';
    if(strcmp(buf,"y")==0)
        dev="/dev/ttyUSB0";
    else
    	dev=buf;

    printf("initializing %s ..........\n",dev);
    ret=com_open(dev);
    if(ret<0)
    	return 0;
    printf("%s is ready to go\n",dev);

    printf("please enter the plane ID,enter 'y' to use default ID=1:\n");
    fgets(buf,40,stdin);
    buf[strlen(buf)-1]='\0';
    if(strcmp(buf,"y")==0)
        plane_id=1;
    else
    	plane_id=atoi(buf);

    how_to_use();
    command=buf;


    fp_fly_status_raw=fopen("fly_status.raw","w");
    if(fp_fly_status_raw==NULL){
    	printf("can not open file:fly_status.raw\n");
    	return 0;
    }
    fp_fly_status=fopen("fly_status.csv","w");
    if(fp_fly_status==NULL){
    	printf("can not open file:fly_status.csv\n");
    	return 0;
    }
    file_fly_status_init();
    running =1;
   	pthread_create(&send_data, NULL, send_data_periodly, NULL);
	while (1) {

		fgets(buf,40,stdin);
		buf[strlen(buf)-1]='\0';
        if(strcmp(command,"help")==0){
        	how_to_use();
        	continue;
        }

        if(strcmp(command,"servo test")==0){
        	printf("----|sending servo test command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_SERVO_TEST);
        	continue;
        }
        if(strcmp(command,"version")==0){
            printf("----|sending read version command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_VERSION_READ);
        	continue;
        }
        if(strcmp(command,"take off")==0){
        	printf("----|sending take off command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_TAKEOFF);
        	continue;
        }

        if(strcmp(command,"export")==0){
                	printf("----|sending export data command :\n");
                	send_control_cmd(CTRL_FRAME_TYPE_EXPORT_DATA);
                	continue;
                }


        if(strcmp(command,"hover")==0){
        	printf("----|sending hover command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_HOVER);
        	continue;
        }

        if(strcmp(command,"remote2")==0){
        	printf("----|sending remote2 command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_REMOTE_CTRL2);
        	continue;
        }

        if(strcmp(command,"remote1")==0){
        	printf("----|sending remote1 command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_REMOTE_CTRL1);
        	continue;
        }


        if(strcmp(command,"land")==0){
        	printf("----|sending land command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_LAND);
        	continue;
        }

        if(strcmp(command,"return")==0){
        	printf("----|sending return command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_RETURN);
        	continue;
        }

        if(strcmp(command,"fly")==0){
        	printf("----|sending fly command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_FLYING);
        	continue;
        }

        if(strcmp(command,"config")==0){
        	printf("----|sending heli config command :\n");
        	send_control_cmd(CTRL_FRAME_TYPE_HELI_CONFIG);
        	continue;
        }
        if(strcmp(command,"link test")==0){
           	printf("----|sending link test command :\n");
            link_test();
        	continue;
        }
        if(strcmp(command,"joystick")==0){
           	printf("----|sending joystick data,enter 'stop' to stop the sending.... :\n");
           	joystick_test=1;

        	continue;
        }

        if(strcmp(command,"wp init")==0){
           	printf("----|sending waypoint... :\n");
           	send_way_point();
        	continue;
        }
        if(strcmp(command,"wp modify")==0){
           	printf("----|sending waypoint modify command... :\n");
           	way_point_modify();
        	continue;
        }
        if(strcmp(command,"stop")==0){
            joystick_test=0;
        	continue;
        }

        if(strcmp(command,"exit")==0){

        	break;
        }

		usleep(20000);

	}
	com_close();
	fclose(fp_fly_status);
	fclose(fp_fly_status_raw);
	void* result = NULL;
	running = 0;
	pthread_join(send_data,&result);

	exit(0);
	return 0;
}
