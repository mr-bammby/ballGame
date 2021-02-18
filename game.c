//main game functionallity

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h> //for chatching ctrl c
#include <termios.h> //for enabling and disabling terminal echo
#include <pthread.h> //for threading
#include <unistd.h>

#include "show.h" //for desplaying

//ball
#define speed_increment 200 //in ms of ball move

//slope limits
#define number_of_possible_start_slopes 21
#define highest_start_slope 2.0
#define max_slope 4.0

//game states
#define START 0
#define GAME 1
#define END 2

//controling keys
#define leftUP_key  'w'
#define leftDOWN_key 's'
#define rightUP_key 'o'
#define rightDOWN_key 'l'
#define PLAY_key 'p' 


//ball
float ball_XcoordinateG;
float ball_YcoordinateG;
int ball_speed=1;
int ball_direction;
float slope;

//pad
int pad_position[2]; //(LEFT, RIGHT)
int pad_size=5;
float slope_change_factors[]={-0.25, -0.125, 0.0, 0.125, 0.25};

//background
int background_lenght = 150;
int background_heigth = 33;

//game
int playing_side = RIGHT;
int game_state=END;

//program
int program_running = 1;
// for ECHO enable and disable
struct termios term;
struct termios new_term;
pthread_t move_ball_thread_ID;


//ball slope manipulation
void border_rebound(void);
void pad_rebound(int place_on_pad);
//moveing pad
void pad_move_game(int side, int direction);
//thread function for moving ball
void* ball_move_thread(void* arg);
//game administration
void start_game(void);
void end_game(void);
void restart_game(void);
//ctrl c interrupt
void interruptHandler(int dummy);

void interruptHandler(int dummy){
	//enables ECHO
	tcsetattr(fileno(stdin), TCSANOW, &term);
	program_running=0;
}

void restart_game(void){
	game_state=START;
	if (playing_side == RIGHT){
		playing_side=LEFT;
		ball_direction=RIGHT;
	}
	else{
		playing_side=RIGHT;
		ball_direction=LEFT;
	}
	int* setup_data;
	setup_data=setup(playing_side, pad_size, background_lenght, background_heigth);
	ball_XcoordinateG=(float)*(setup_data+0);
	ball_YcoordinateG=(float)*(setup_data+1);
	pad_position[0]=*(setup_data+2);
	pad_position[1]=*(setup_data+3);
}

void start_game(void){
	game_state=GAME;
	//intializes random generator
	time_t seed;
	srand((unsigned)time(&seed));
	slope=highest_start_slope-((2*highest_start_slope)/(number_of_possible_start_slopes-1))*(rand()%number_of_possible_start_slopes);//random slope in chosen limits
	//start move ball thread
	pthread_create(&move_ball_thread_ID, NULL, &ball_move_thread, NULL);
}

void end_game(void){
	game_state=END;
	ball_speed=1;
}

void border_rebound(void){
	slope=-1*slope;
}

void pad_rebound(int place){
	slope += slope_change_factors[place];
	if (ball_direction==LEFT){
		ball_direction=RIGHT;
	}
	else{
		ball_direction=LEFT;
	}
	printf("pad");
	
}

void pad_move_game(int side, int direction){
	int moved = move_pad(side, direction);
	if (moved==1){
		if (direction==UP){
			pad_position[side]--;
		}	
		else{
			pad_position[side]++;
		}
		if(game_state==START && side==playing_side){
			if(direction==UP){		
				ball_YcoordinateG--;
			}	
			else{
				ball_YcoordinateG++;
			}
			move_ball((int)ball_XcoordinateG, (int)ball_YcoordinateG);
		}
	}
}

void* ball_move_thread(void* arg){
	int temp;
	//pthread_detach(pthread_self());
	while(program_running==1 && game_state==GAME){
		ball_YcoordinateG += slope;
		if ((int)ball_YcoordinateG <= 0){
			ball_YcoordinateG = 1.0;
			border_rebound();
		}
		else if((int)ball_YcoordinateG >= background_heigth){
			ball_YcoordinateG=(float)background_heigth-1;
			border_rebound();
		}
		if (ball_direction==RIGHT){
			ball_XcoordinateG++;
			if(ball_XcoordinateG == background_lenght-1){
				temp = (int)ball_YcoordinateG-pad_position[1];
				printf("temp: %d", temp);
				if(temp<(pad_size) && temp > 0){
					ball_XcoordinateG--;
					pad_rebound(temp);
				}
			}
			else if((int)ball_XcoordinateG==background_lenght){
				end_game();
			}
		}
		else{
			ball_XcoordinateG--;
			if((int)ball_XcoordinateG == 1){
				printf("here");
				temp = (int)ball_YcoordinateG-pad_position[0];
				printf("temp: %d", temp);
				if(temp<(pad_size) && temp > 0){
					ball_XcoordinateG++;
					pad_rebound(temp);
				}
			}
			else if((int)ball_XcoordinateG==0){
				end_game();
			}
		}
		move_ball((int)ball_XcoordinateG, (int)ball_YcoordinateG);
		usleep(100000);
	}
	pthread_exit(NULL);
}
int main(void){
	signal(SIGINT, interruptHandler);
	tcgetattr(fileno(stdin), &term);
	new_term=term;
	new_term.c_lflag &= ~ECHO;
	new_term.c_lflag &= ~ICANON;
	tcsetattr(fileno(stdin), TCSANOW, &new_term);
	char new_char;
	while(program_running==1){
		if(game_state==END){
			restart_game();
		}
		new_char=getchar();
		if (new_char==PLAY_key && game_state==START){
			start_game();
		}
		else if(new_char==leftUP_key){
			pad_move_game(LEFT, UP);
		}
		else if(new_char==leftDOWN_key){
			pad_move_game(LEFT, DOWN);
		}
		else if(new_char==rightUP_key){
			pad_move_game(RIGHT, UP);
		}
		else if(new_char==rightDOWN_key){
			pad_move_game(RIGHT, DOWN);
		}	
	}
	pthread_join(move_ball_thread_ID, NULL);	

}
