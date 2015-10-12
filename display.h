#ifndef DISPLAY_H
#define DISPLAY_H
#include "datatype.h"
#include "cmd.h"

#define MIN_ROW 25
#define MIN_COL 105

#define PITCH_HEIGHT 14
#define PITCH_WIDTH 15
#define PITCH_Y 0
#define PITCH_X 0

#define ROLL_HEIGHT 7
#define ROLL_WIDTH 45
#define ROLL_Y 0
#define ROLL_X 16

#define YAW_HEIGHT 13
#define YAW_WIDTH 19
#define YAW_Y  0
#define YAW_X  62

#define HEAD_HEIGHT 13
#define HEAD_WIDTH 19
#define HEAD_Y  0
#define HEAD_X  84

#define OTHER_HEIGHT 7
#define OTHER_WIDTH 45
#define OTHER_Y  7
#define OTHER_X  16

#define COCKPIT_HEIGHT 13
#define CONSOLE_HEIGHT 11


WINDOW *create_newwin(int height, int width, int starty, int startx);
void show_pitch(WINDOW *win,float degree);
void show_roll(WINDOW *win,float degree);
void show_yaw(WINDOW *win,float degree);
void show_heading(WINDOW *win,float degree);
void show_other(WINDOW *win,float degree);
void cockpit_display(flying_status_s *flying_status);
void cockpit_init();
WINDOW *roll;
WINDOW *pitch;
WINDOW *yaw;
WINDOW *heading;
WINDOW *other;
WINDOW *console;

#endif
