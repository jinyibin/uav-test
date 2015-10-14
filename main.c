#include "datatype.h"
#include "cmd.h"
#include <pthread.h>
#include "display.h"
#include "process.h"
#include "serial.h"
#include <unistd.h>

pthread_t send_data;
int joystick_test=0;
static int running =0;

void file_fly_status_init();
void *send_data_periodly();
void how_to_use();



int main( int argc,char *argv[])
{
	char buf[128];
	char *dev=buf;
	char *command;
	int ret;

    cockpit_init();

    wgetstr(console,buf);
    //buf[strlen(buf)-1]='\0';
    if(strcmp(buf,"y")==0)
        dev="/dev/ttyUSB0";
    else
    	dev=buf;

    printx(console,"initializing %s ..........\n",dev);
    wrefresh(console);
    ret=com_open(dev);
    if(ret<0){
    	getch();
    	endwin();

    	return 0;
    }

    printx(console,"%s is ready to go\n",dev);
    printx(console,"please enter the plane ID,enter 'yes' to use default ID=1:\n");
    wrefresh(console);
    wgetstr(console,buf);
    //buf[strlen(buf)-1]='\0';
    if(strcmp(buf,"yes")==0)
        plane_id=1;
    else
    	plane_id=atoi(buf);

    how_to_use();
    command=buf;


    fp_fly_status_raw=fopen("fly_status.raw","w");
    if(fp_fly_status_raw==NULL){
    	endwin();
    	printf("can not open file:fly_status.raw\n");
    	exit(1);
    }
    fp_fly_status=fopen("fly_status.csv","w");
    if(fp_fly_status==NULL){
    	endwin();
    	printf("can not open file:fly_status.csv\n");

    	exit(1);
    }

    fp_bytes_raw=fopen("rawdata","w");
    if(fp_fly_status==NULL){
    	endwin();
    	printf("can not open file:rawdata\n");
    	exit(1);
    }
    file_fly_status_init();
    running =1;
   	pthread_create(&send_data, NULL, send_data_periodly, NULL);
   	receive_enable();
	while (1) {

		wgetstr(console,buf);
		//buf[strlen(buf)-1]='\0';
        if(strcmp(command,"help")==0){
        	how_to_use();
        	continue;
        }

        if(strcmp(command,"servo test")==0){
        	printx(console,"----|sending servo test command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_SERVO_TEST);
        	continue;
        }
        if(strcmp(command,"version")==0){
            printx(console,"----|sending read version command :\n");
            wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_VERSION_READ);
        	continue;
        }
        if(strcmp(command,"take off")==0){
        	printx(console,"----|sending take off command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_TAKEOFF);
        	continue;
        }

        if(strcmp(command,"export")==0){
                	printx(console,"----|sending export data command :\n");
                	wrefresh(console);
                	send_control_cmd(CTRL_FRAME_TYPE_EXPORT_DATA);
                	continue;
                }


        if(strcmp(command,"hover")==0){
        	printx(console,"----|sending hover command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_HOVER);
        	continue;
        }

        if(strcmp(command,"remote2")==0){
        	printx(console,"----|sending remote2 command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_REMOTE_CTRL2);
        	continue;
        }

        if(strcmp(command,"remote1")==0){
        	printx(console,"----|sending remote1 command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_REMOTE_CTRL1);
        	continue;
        }


        if(strcmp(command,"land")==0){
        	printx(console,"----|sending land command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_LAND);
        	continue;
        }

        if(strcmp(command,"return")==0){
        	printx(console,"----|sending return command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_RETURN);
        	continue;
        }
        if(strcmp(command,"manual")==0){
        	printx(console,"----|sending manual command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_MANUAL_MODE);
        	continue;
        }
        if(strcmp(command,"ground ok")==0){
        	printx(console,"----|sending ground ok command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_GROUND_OK);
        	continue;
        }

        if(strcmp(command,"fly")==0){
        	printx(console,"----|sending fly command :\n");
        	wrefresh(console);
        	send_control_cmd(CTRL_FRAME_TYPE_FLYING);
        	continue;
        }

        if(strcmp(command,"config")==0){
        	printx(console,"----|sending heli config command :\n");
        	wrefresh(console);
        	heli_config();
        	continue;
        }

        if(strcmp(command,"para set1")==0){
        	printx(console,"----|sending fly para1 command :\n");
        	wrefresh(console);
        	send_fly_para1();
        	continue;
        }

        if(strcmp(command,"para set2")==0){
        	printx(console,"----|sending fly para2 command :\n");
        	wrefresh(console);
        	send_fly_para2();
        	   	continue;
        }
        if(strcmp(command,"link test")==0){
           	printx(console,"----|sending link test command :\n");
           	wrefresh(console);
            link_test();
            continue;
        }
        if(strcmp(command,"joystick")==0){
           	printx(console,"----|sending joystick data,enter 'stop' to stop the sending.... :\n");
           	wrefresh(console);
           	joystick_test=1;

        	continue;
        }

        if(strcmp(command,"wp init")==0){
           	printx(console,"----|sending waypoint... :\n");
           	wrefresh(console);
           	send_way_point();

        	continue;
        }
        if(strcmp(command,"wp modify")==0){
           	printx(console,"----|sending waypoint modify command... :\n");
           	way_point_modify();

        	continue;
        }

        if(strcmp(command,"reset")==0){
           	printx(console,"----|sending reset command... :\n");
           	send_control_cmd(CTRL_FRAME_TYPE_RESET);

        	continue;
        }
        if(strcmp(command,"stop")==0){
            joystick_test=0;
        	continue;
        }

        if(strcmp(command,"exit")==0){

        	break;
        }
        if(strcmp(command,"save")==0){

        	fflush(fp_fly_status);
        	fflush(fp_fly_status_raw);
        	fflush(fp_bytes_raw);
            printx(console,"----|file saved... :\n");
            wrefresh(console);
        	continue;
        }

		usleep(20000);

	}
	running = 0;
	com_close();
	fclose(fp_fly_status);
	fclose(fp_fly_status_raw);
	fclose(fp_bytes_raw);
	void* result1 = NULL;
	pthread_join(send_data,&result1);


    endwin();
	exit(0);
	return 0;
}


void how_to_use()
{
  wclear(console);
  wmove(console,0,0);
  printx(console,"--------------------command list-----------------------\n");
  printx(console,"exit       : save the data and exit the program \n");
  printx(console,"save       : save flying status data\n");
  printx(console,"stop       : stop the sending data,but not exit  \n");
  printx(console,"help       : show you command list\n");
  printx(console,"servo test : send servo test command\n");
  printx(console,"link  test : send link test command\n");
  printx(console,"joystick   : send joystick data\n");
  printx(console,"version    : read version of app,kernel,board and FPGA etc\n");
  wattron(console,A_BLINK|A_BOLD);
  printx(console,"please press any key to see next page (1/3):\n");
  wgetch(console);
  wattroff(console,A_BLINK|A_BOLD);
  wrefresh(console);

  wclear(console);
  wmove(console,0,0);
  printx(console,"take off   : send plane take off command list\n");
  printx(console,"remote1    : set plane into remote mode1\n");
  printx(console,"remote2    : set plane into remote mode2\n");
  printx(console,"hover      : let the plane hover \n");
  printx(console,"fly        : let the plane fly according to the way point\n");
  printx(console,"return     : let the plane fly back\n");
  printx(console,"land       : let the plane land\n");
  printx(console,"config     : configure the auto pilot system,like heli type,oil volume,max speed etc\n");
  printx(console,"para set1  : set the parameter1 of auto pilot system\n");
  wattron(console,A_BLINK|A_BOLD);
  printx(console,"please press any key to see next page (2/3):\n");
  wgetch(console);
  wattroff(console,A_BLINK|A_BOLD);
  wrefresh(console);

  wclear(console);
  wmove(console,0,0);
  printx(console,"para set2  : set the parameter2 of auto pilot system\n");
  printx(console,"wp modify  : modify the way point,add data in the wp_modify.csv\n");
  printx(console,"             format:m_type,id,v,Long,lat,h,task\n");
  printx(console,"wp init    : initialize the way point,add data in the wp_ini.csv\n");
  printx(console,"firmware   : update the firmware\n");
  printx(console,"export     : export data\n");
  printx(console,"manual     : manual mode\n");
  printx(console,"ground ok  : ground ready to go\n");
  printx(console,"reset      : reset\n");
  printx(console,"----------------------command list over--------------------------\n");

  wrefresh(console);
}

void file_fly_status_init()
{
   fprintf(fp_fly_status,"frame,time(ms),");
   fprintf(fp_fly_status,"roll,pitch,yaw,");
   fprintf(fp_fly_status,"gx,gy,gz,");
   fprintf(fp_fly_status,"ax,ay,az,");
   fprintf(fp_fly_status,"vn,ve,vd,");
   fprintf(fp_fly_status,"heading,b_high,");
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


