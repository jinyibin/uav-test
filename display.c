#include "display.h"

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);

	//box(local_win, 0 ,0);
	                 /* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	//wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}


void show_pitch(WINDOW *win,float degree)
{
	int scale[11]={5,4,3,2,1,0,-1,-2,-3,-4,-5};
    int i=1;
    int data;
    data=(int)degree;
    wclear(win);
    //calculate scale
    if((data>-5)&&(data<5)){
        for(i=0;i<11;i++){
     	   if(scale[i]==data)
     		   break;
        }
      	wattron(win,COLOR_PAIR(2)|A_BOLD);
       	mvwprintw(win,2+i, 8," <--");
       	mvwprintw(win,2+i, 2,"-->");
       	    //waddch(win,ACS_DEGREE);
       	wattroff(win,COLOR_PAIR(2)|A_BOLD);

    }else if(data<=-5){
        scale[10]=data;
        for(i=9;i>-1;i--){
     	   scale[i]=scale[i+1]+1;
        }

       	wattron(win,COLOR_PAIR(2)|A_BOLD);
        mvwprintw(win,12, 8," <--");
        mvwprintw(win,12, 2,"-->");
        	    //waddch(win,ACS_DEGREE);
        wattroff(win,COLOR_PAIR(2)|A_BOLD);


    }else{
        scale[0]=data;
        for(i=1;i<11;i++){
     	   scale[i]=scale[i-1]-1;
        }
       	wattron(win,COLOR_PAIR(2)|A_BOLD);
        mvwprintw(win,2, 8," <--");
        mvwprintw(win,2, 2,"-->");
        	    //waddch(win,ACS_DEGREE);
        wattroff(win,COLOR_PAIR(2)|A_BOLD);

    }
    //draw window outline
    box(win, 0 ,0);
    //show window title
    wattron(win,COLOR_PAIR(1)|A_BOLD);
    mvwprintw(win,1, 1,"PITCH");
    wattroff(win,COLOR_PAIR(1)|A_BOLD);
    //show degree value in red color
    wattron(win,COLOR_PAIR(2)|A_BOLD);
    mvwprintw(win,1, 10,"%3.1f",degree);
    //waddch(win,ACS_DEGREE);
    wattroff(win,COLOR_PAIR(2)|A_BOLD);

    //draw scale
    for(i=0;i<11;i++){
    	mvwprintw(win,i+2, 5,"%3d",scale[i]);
    }
    wrefresh(win);
}
void show_roll(WINDOW *win,float degree)
{
	int scale[11]={-5,-4,-3,-2,-1,0,1,2,3,4,5};
    int i=1;
    char s_string[4];
    int data;
    wclear(win);
    //wmove(other,0,0);
    data=(int)degree;
    //calculate scale
    //calculate scale
     if((data>-5)&&(data<5)){

        for(i=0;i<11;i++){
     	   if(scale[i]==data)
     		   break;
        }

        wattron(win,COLOR_PAIR(4)|A_BOLD);
        mvwprintw(win,2,2+i*4," ");
        mvwprintw(win,4,2+i*4," ");
        wattroff(win,COLOR_PAIR(4)|A_BOLD);

     }else if(data<=-5){

         scale[0]=data;
         for(i=1;i<11;i++){
      	   scale[i]=scale[i-1]+1;
         }
         wattron(win,COLOR_PAIR(4)|A_BOLD);
         mvwprintw(win,2,2," ");
         mvwprintw(win,4,2," ");
         wattroff(win,COLOR_PAIR(4)|A_BOLD);


     }else{
         scale[10]=data;
         for(i=9;i>-1;i--){
      	   scale[i]=scale[i+1]-1;
         }
         wattron(win,COLOR_PAIR(4)|A_BOLD);
         mvwprintw(win,2,42," ");
         mvwprintw(win,4,42," ");
         wattroff(win,COLOR_PAIR(4)|A_BOLD);

     }

    //draw window outline
    box(win, 0 ,0);
    //show window title
    wattron(win,COLOR_PAIR(1)|A_BOLD);
    mvwprintw(win,1, 1," ROLL");
    wattroff(win,COLOR_PAIR(1)|A_BOLD);

    wattron(win,COLOR_PAIR(3)|A_BOLD);
    mvwprintw(win,1,39,"%5.1f",degree);
    wattroff(win,COLOR_PAIR(3)|A_BOLD);

    for(i=0;i<11;i++){
    	sprintf(s_string,"%d",scale[i]);
        switch(strlen(s_string))
        {
        case 1:
            mvwprintw(win,3, 2+i*4,"%d",scale[i]);
            break;
        case 2:
        	mvwprintw(win,3, 2+i*4,"%d",scale[i]);
            break;
        case 3:
        	mvwprintw(win,3, 2+i*4-1,"%d",scale[i]);
            break;
        case 4:
        	wattron(win,COLOR_PAIR(2)|A_BOLD);
        	mvwprintw(win,4, 2,"      warning:rolling too much %s -",s_string);
        	wattroff(win,COLOR_PAIR(2)|A_BOLD);
            break;
        default:
        	mvwprintw(win,3, 2+i*4,"%d",scale[i]);
            break;
        }


    }
    wrefresh(win);
}

void show_yaw(WINDOW *win,float degree)
{
	int bias;
    int i=1;
    int zero_y=0; //coordinates of zero
    int zero_x=9;

    //draw window outline
    wborder(win, '.', '.', '.', '.', '.', '.', '.', '.');
    mvwaddch(win,0,9,ACS_UARROW);
    mvwprintw(win,1,9,"0");
    mvwprintw(win,6,0,"<-90");
    mvwprintw(win,6,16,"90>");
    mvwaddch(win,12,9,ACS_DARROW);
    wattron(win,COLOR_PAIR(2)|A_BOLD);
    mvwaddch(win,6,9,ACS_DIAMOND);
    wattroff(win,COLOR_PAIR(2)|A_BOLD);
    //show window title
    wattron(win,COLOR_PAIR(1)|A_BOLD);
    mvwprintw(win,4,8,"YAW");
    wattroff(win,COLOR_PAIR(1)|A_BOLD);

    wattron(win,COLOR_PAIR(3)|A_BOLD);
    mvwprintw(win,8,7,"%6.1f",degree);
    wattroff(win,COLOR_PAIR(3)|A_BOLD);

    //calculate coordinates
    bias=(int)(degree/6);
    //data is at top line,-60~60
    if((bias>=-9)&&(bias<=9)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y,zero_x+bias,ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at left line -126~-60
    if((bias>=-20)&&(bias<=-10)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y+(-bias-9),0,ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at right line 60~126
    if((bias>=10)&&(bias<=20)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y+bias-9,zero_x+9,ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at bottom line -132~-180
    if((bias>=-30)&&(bias<=-21)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y+12,0+(-bias-20),ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at bottom line 132~180
    if((bias>=21)&&(bias<=30)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y+12,18-(bias-20),ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }

    wrefresh(win);
}

void show_heading(WINDOW *win,float degree)
{
	int bias;
    int zero_y=0; //coordinates of zero
    int zero_x=9;

    //draw window outline
    wborder(win, '.', '.', '.', '.', '.', '.', '.', '.');
    mvwaddch(win,0,9,ACS_UARROW);
    mvwprintw(win,1,9,"0");
    mvwprintw(win,6,0,"<270");
    mvwprintw(win,6,16,"90>");
    mvwprintw(win,11,8,"180");
    mvwaddch(win,12,9,ACS_DARROW);
    wattron(win,COLOR_PAIR(2)|A_BOLD);
    mvwaddch(win,6,9,ACS_DIAMOND);
    wattroff(win,COLOR_PAIR(2)|A_BOLD);
    //show window title
    wattron(win,COLOR_PAIR(1)|A_BOLD);
    mvwprintw(win,4,6,"HEADING");
    wattroff(win,COLOR_PAIR(1)|A_BOLD);

    wattron(win,COLOR_PAIR(3)|A_BOLD);
    mvwprintw(win,8,7,"%6.1f",degree);
    wattroff(win,COLOR_PAIR(3)|A_BOLD);

    //calculate coordinates
    bias=(int)(degree/6);
    //data is at top line,0~60
    if((bias>=0)&&(bias<=9)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,0,9+bias,ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at right line 60~126
    if((bias>=10)&&(bias<=20)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y+bias-9,zero_x+9,ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at bottom line 132~180
    if((bias>=21)&&(bias<=30)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y+12,18-(bias-20),ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at bottom line 180~234
    if((bias>=31)&&(bias<=39)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,zero_y+12,9+(-bias+30),ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }
    //data is at left line 240~-300
    if((bias>=40)&&(bias<=50)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,12+(-bias+39),0,ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }

    //data is at left line 306~-360
    if((bias>=51)&&(bias<=60)){
    	wattron(win,COLOR_PAIR(2)|A_BOLD);
    	mvwaddch(win,0,bias-50,ACS_DIAMOND);
    	wattroff(win,COLOR_PAIR(2)|A_BOLD);
    }

    wrefresh(win);
}

void cockpit_init()
{
	initscr();			/* Start curses mode 		*/
	cbreak();

	if(has_colors() == FALSE)
		{	endwin();
			printf("Your terminal does not support color\n");
			exit(1);
		}
	start_color();			// Start color
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_WHITE);
	init_pair(4, COLOR_BLACK, COLOR_RED);

	//check if the screen is big enough
	if((LINES < MIN_ROW)| (COLS < MIN_COL)){
        endwin();
        printf("your screen is too small %d x %d ,please resize it !\n",LINES,COLS);
        exit(1);
    }

	//generate separating line
	attron(COLOR_PAIR(1));
	mvhline(COCKPIT_HEIGHT+1,0,'=',COLS);
	attroff(COLOR_PAIR(1));
	refresh();

	roll=newwin(ROLL_HEIGHT,ROLL_WIDTH, ROLL_Y,ROLL_X);
	show_roll(roll,0);
	pitch=newwin(PITCH_HEIGHT,PITCH_WIDTH, PITCH_Y,PITCH_X);
	show_pitch(pitch,0);
	yaw=newwin(YAW_HEIGHT,YAW_WIDTH, YAW_Y,YAW_X);
    show_yaw(yaw,0);
	heading=newwin(HEAD_HEIGHT,HEAD_WIDTH,HEAD_Y,HEAD_X);
    show_heading(heading,0);

    other=newwin(OTHER_HEIGHT,OTHER_WIDTH,OTHER_Y,OTHER_X);

	console=create_newwin(CONSOLE_HEIGHT,COLS, COCKPIT_HEIGHT+2,0);

    keypad(console,TRUE);
	scrollok(console,TRUE);
	printx(console,"please enter the COM name,enter 'y' to use default /dev/ttyUSB0:");
    wrefresh(console);
}
void cockpit_display(flying_status_s *flying_status)
{
    show_roll(roll,flying_status->roll*180/PI);
    show_pitch(pitch,flying_status->pitch*180/PI);
    show_yaw(yaw,flying_status->yaw*180/PI);
    show_heading(heading,flying_status->heading/100000);
    //wclear(other);
    //wmove(other,0,0);
    mvwprintw(other,0,0,"Height_Baro:%7.3fm   Height_GPS :%7.3fm\n",(float)(flying_status->b_h)/1000,flying_status->g_h);
    mvwprintw(other,1,0,"Sonar      :%7.3fcm  Next_Destin:%5d\n",(float)(flying_status->sonar_h)/58,flying_status->waypoint_dest);
    mvwprintw(other,2,0,"Volt_Batter:%7.3fV   Temp_CPU   :%4dC\n",flying_status->voltage,flying_status->cpu_temp);
    mvwprintw(other,3,0,"Longitude:%12.8f  Latitude:%12.8f\n",flying_status->Long,flying_status->lat);

    switch(flying_status->status)
    {
    case AIRCRAFT_PREPARING:
    	mvwprintw(other,4,0,"Plane_Stat :Preparing %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_READY:
    	mvwprintw(other,4,0,"Plane_Stat :Ready to go %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_TAKEOFF:
    	mvwprintw(other,4,0,"Plane_Stat :takeoff %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_LANDING:
    	mvwprintw(other,4,0,"Plane_Stat :landing %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_FLYING:
    	mvwprintw(other,4,0,"Plane_Stat :flying %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_TASK:
    	mvwprintw(other,4,0,"Plane_Stat :task %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_REMOTE1:
    	mvwprintw(other,4,0,"Plane_Stat :Remote1 %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_REMOTE2:
    	mvwprintw(other,4,0,"Plane_Stat :Remote2 %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_HOVERING:
    	mvwprintw(other,4,0,"Plane_Stat :%8x Hovering\n",flying_status->status);
    	break;
    case AIRCRAFT_RETURN:
    	mvwprintw(other,4,0,"Plane_Stat :Return %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_OBS_AVOIDING:
    	mvwprintw(other,4,0,"Plane_Stat :Obstacle avoidance %5x \n",flying_status->status);
    	break;
    case AIRCRAFT_MANUAL_MODE:
    	mvwprintw(other,4,0,"Plane_Stat :Manual %5x\n",flying_status->status);
    	break;
    case AIRCRAFT_FAULT:
    	mvwprintw(other,4,0,"Plane_Stat :Fault %5x\n",flying_status->status);
    	break;
    default:
    	break;
    }
    mvwprintw(other,5,0,"CPU_Status :%4d\n",flying_status->cpu_s);
    wrefresh(other);

}
