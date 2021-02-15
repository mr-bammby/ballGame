//showing game functions
#include "show.h"

#include <stdio.h>
#include <stdlib.h>

//background
int background_height=33;
int background_width=150;

//pad
int pad_size=5;
int pad_position_left;
int pad_position_right;

//ball
char ball_shape='O';
int ball_Xcoordinate;
int ball_Ycoordinate;

//moves a pad
//side 0-left  1-right
//direction 0-down  1-up
void move_pad(int side, int direction){
	int pad_position;
	int colum;
	//chooses moving side
	if (side==LEFT){
		pad_position=pad_position_left;
		colum=2;
	}
	else{
		pad_position=pad_position_right;
		colum=background_width;
	}
	//moves up if requested and possible
	if (direction==UP && pad_position>2){
		printf("\e[%d;%dH ", pad_position+pad_size, colum);
		pad_position--;
		printf("\e[%d;%dH|", pad_position, colum);
	}
	//moves down if requested and possible
	else if(direction==DOWN && (pad_position+pad_size)<(background_height)){
		printf("\e[%d;%dH ", pad_position, colum);
		pad_position++;
		printf("\e[%d;%dH|", pad_position+pad_size, colum);
	}
	//saves new position of a choosen pad
	if (side==LEFT){
		pad_position_left=pad_position;
	}
	else{
		pad_position_right=pad_position;
	}
	fflush(stdout);
}

//moves the ball to coordinate
//limits: x [0,background_width]* ---- y [1,background_height-1]**
//*0 and width is out, 1 and width-1 is pad line
//** 0 and height are borders 
void move_ball(int new_Xcoordinate, int new_Ycoordinate){
	printf("\e[%d;%dH ", ball_Ycoordinate+1, ball_Xcoordinate+1);//delets the ball 
	printf("\e[%d;%dH%c", new_Ycoordinate+1, new_Xcoordinate+1, ball_shape); //draws the ball
	fflush(stdout);
	//savesa new coordinates
	ball_Ycoordinate=new_Ycoordinate;
	ball_Xcoordinate=new_Xcoordinate;	
}

//setup of playing field
//start_side: 0-left  1-right
//returns coordinate of the ball in (X,Y) format
int* setup(int start_side){
	//start position
	//pads in the middle of 
	//ball in the middle of choosen pad
	pad_position_right=background_height/2 - pad_size/2 + 1;
	pad_position_left=background_height/2 - pad_size/2 + 1;
	if (start_side==LEFT){
		ball_Xcoordinate=2;
		ball_Ycoordinate=pad_position_left+pad_size/2;
	}
	else{
		ball_Xcoordinate=background_width-2;
		ball_Ycoordinate=pad_position_right+pad_size/2;
	}
	//clears terminal
	system("clear");
	//draws playing field
	for(int height=0; height<=background_height; height++){
		for(int width=0; width<=background_width;width++){
			if(height==0 || height==background_height){
				putchar('-');	//bottom and top border
			}	
			else if(width==0 || width==background_width){
				putchar(':'); //left and right out
			}
			else if(width==1 && height>=pad_position_left && height<(pad_position_left+pad_size)){
				putchar('|');	//left pad
			}
			else if(width==(background_width-1) && height>=pad_position_right && height<(pad_position_right+pad_size)){
				putchar('|');	//right pad
			}
			else if(width==ball_Xcoordinate && height==ball_Ycoordinate){
				putchar(ball_shape);	//ball
			}
			else{
				putchar(' '); //empty space
			}
		}
		putchar('\n');
	}
	fflush(stdout); //post changes
	//return
	static int return_coordinates[2];
	return_coordinates[0]=ball_Xcoordinate;
	return_coordinates[1]=ball_Ycoordinate;
	return return_coordinates;
}
