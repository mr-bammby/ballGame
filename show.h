//header file for output managing

#ifndef SHOW_H_INCLUDED
#define SHOW_H_INCLUDED

//sides defines
#define LEFT 0
#define RIGHT 1
#define DOWN 0
#define UP 1


//setup of playing field
//start_side: 0-left  1-right
//returns coordinate of the ball in (X,Y) format
int* setup(int start_side);


//moves the ball to coordinate
//limits: x [0,background_width]* ---- y [1,background_height-1]**
//*0 and width is out, 1 and width-1 is pad line
//** 0 and height are borders 
void move_ball(int new_Xcoordinate, int new_Ycoordinate);


//moves a pad
//side 0-left  1-right
//direction 0-down  1-up
void move_pad(int side, int direction);

#endif
