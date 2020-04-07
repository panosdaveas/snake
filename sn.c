#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <dos.h>

#define width 20
#define height 10
#define initial_speed 400
#define default_score 100

struct coordinate {
    int x;
    int y;  
    char ch;  
};
typedef struct coordinate coordinate;

struct snake {
    int length;    
    struct coordinate head;
    struct coordinate pre_head;
    struct coordinate tail;
    struct coordinate* body;
    struct coordinate new_head;
    struct coordinate food;
	struct coordinate bord[2*(width+height)-4];
    int score;
    int mode;
    int gameover;
    int speed;
    int speed_counter;
    int level;
    int eat_signal;
    int booster;
};
typedef struct snake snake;

struct cross{    
    coordinate left;
    coordinate right;
    coordinate down;
    coordinate up;    
};
typedef struct cross cross;

void delay(int number_of_seconds)
{     
    int milli_seconds =   number_of_seconds;     
    clock_t start_time = clock();     
    while (!kbhit() && clock() < start_time + milli_seconds);          
}

void Gotoxy() {
  COORD coord; coord.X = 0; coord.Y = 0;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);}

int is_obstacle(int x, int y){
    int sx= (width-2)/4; //steps
    int sy=sx/2;
	if((x>sy && x<2*sx && y==sy+1)
	|| (x<=(width-sx) && x>=(width-2*sx) && y==height-sy-2)
	|| (x==sx+1 && y<=(height-sy-1) && y>=(height-2*sy-1))
	|| (x==(width-sx-sy) && y>=sy && (y<height-2*sy-1))){
		return 1;
	}
	return 0;
}

int is_border(int x, int y){
    return (x == 0 || y== 0 || x == width-1 || y == height-1);
}

int is_o(int x,int y){      //for gameover graphics
    return(x==width/2 -1 && y==height/2+1);}

int is_snake(int x, int y, snake snake){
	int i=0;
    for ( i=0; i< snake.length; i++){
        if (x == snake.body[i].x && y == snake.body[i].y){
        	snake.body[i].ch='x';
			return 1;
        }
    }
    return 0;}

int is_food(int x,int y,snake snake){
    if(x==snake.food.x && y==snake.food.y){
        return 1;
    }
    return 0;
}

snake border_grid(snake snake){
	int i=0;
	int y=0;
	for(y=0; y<height; y++){
		int x=0;
		for (x=0; x<width; x++){
			if(x == 0 || y== 0 || x == width-1 || y == height-1){
				snake.bord[i].x=x;
				snake.bord[i].y=y;
				i++;
			}
		}
	}
	return snake;
}

int speed_up(snake snake){
    snake.speed = initial_speed - snake.level * 10;
    return snake.speed;
}

snake init_snake_position(snake snake,int length){
    snake.body=(coordinate*) malloc(sizeof(coordinate)*length);
    snake.new_head.x=0;
    snake.new_head.y=0;
    snake.gameover=0;
    snake.food.x=0;
    snake.food.y=0;   
    snake.score=0; 
    snake.speed=initial_speed;
    snake.speed_counter=0;
    snake.level = 0;
    snake.booster=0;
	snake=border_grid(snake);
    int middle_row=height/2;//even width & height
    int middle_col=width/2;   
	int i=1;
    snake.length=length;    
    coordinate center;
    center.x = middle_col;
    center.y = middle_row;
    snake.tail.x= center.x - (snake.length/2);      
    snake.tail.y= center.y;    
    snake.body[0].x=snake.tail.x;
    snake.body[0].y=snake.tail.y;    
    for (i=1; i<snake.length; i++){
        snake.body[i].x = snake.body[i-1].x +1;
        snake.body[i].y = snake.body[i-1].y;    
        if (i == snake.length-1){
            snake.head.x=snake.body[i].x;
            snake.head.y=snake.body[i].y;
        }
        if (i == snake.length-2){
            snake.pre_head.x=snake.body[i].x;
            snake.pre_head.y=snake.body[i].y;
        }
    }      
    return snake;
}

cross critical_cross(snake snake){    
    cross cross;
    cross.left.x=snake.head.x -1;
    cross.left.y=snake.head.y;
    cross.right.x=snake.head.x +1;
    cross.right.y=snake.head.y;
    cross.down.x=snake.head.x;
    cross.down.y=snake.head.y -1;
    cross.up.x=snake.head.x;
    cross.up.y=snake.head.y+1;
    return cross;
}

char direction(snake snake, cross cross){  

    char direction ;  
    char direction_reverse ; 
    if (snake.head.x - cross.left.x == 1){
        direction = 'r';
        direction_reverse = 'l';
        return direction;       
    }
    else if(snake.head.x-cross.right.x == -1){
        direction = 'l';
        direction_reverse = 'r';
        return direction;        
    }
    else if(snake.head.y-cross.up.y == -1){
        direction = 'd';
        direction_reverse = 'u';
        return direction;        
    }
    else if(snake.head.y - cross.down.y == 1){
        direction = 'u';
        direction_reverse= 'd';
        return direction;        
    }    
}

char reverse_direction(char direction){
    char direction_reverse ; 
    if (direction == 'r'){
        direction_reverse = 'l';
    }
    else if (direction == 'l'){
        direction_reverse = 'r';
    }
    else if (direction == 'u'){
        direction_reverse = 'd';
    }
    else if (direction == 'd'){
        direction_reverse = 'u';
    }
    return direction_reverse;
}

char compare_key_with_previous_direction(char key, char direction){
    char reverse_dir= reverse_direction(direction);
    if (key == reverse_dir){
        return direction;
    }
    else 
        return key;
}

coordinate default_movement(coordinate next_head,char dir){
    if (dir == 'r'){
        next_head.x ++ ;
    }else if(dir == 'l'){
        next_head.x -- ;
    }else if(dir == 'd'){
        next_head.y ++ ;
    }else if(dir == 'u'){
        next_head.y --;
    }
    return next_head;
}

int border_game_over(snake snake){
    int x=snake.new_head.x;
    int y=snake.new_head.y;    
    if (is_border(x,y)){       
        return 1;
    }
    return 0;
}

int snake_over_border(snake snake){
    int x=snake.new_head.x;
    int y=snake.new_head.y;    
    if (is_border(x,y)){       
        return 1;
    }
    return 0;
}

int obstacle_game_over(snake snake){
    int x=snake.new_head.x;
    int y=snake.new_head.y;    
    if (is_obstacle(x,y)){       
        return 1;
    }
    return 0;
}

int snake_game_over(snake snake){
    int x=snake.head.x;
    int y=snake.head.y;   
    int i=0;
    for (i=0; i<snake.length-1; i++){
        if ((x == snake.body[i].x)  &&  (y == snake.body[i].y)){            
            return 1;
        }
    }
    return 0;    
}

snake head_movement(snake snake, char dir,int x){    
    coordinate temp_head = snake.head;
    temp_head = default_movement(temp_head,dir);     
    snake.new_head=temp_head;
    switch(x){
        case 2:
        if (border_game_over(snake)){
            snake.gameover = 1 ;
        }   
        break;
        case 1:
        if (snake_over_border(snake)){
            if (snake.new_head.x== width-1){
                snake.new_head.x = 1;
            }
            else if(snake.new_head.x ==0 ){
                snake.new_head.x = width-2;
            }
            else if(snake.new_head.y == height-1){
                snake.new_head.y = 1;
            }
            else if(snake.new_head.y == 0){
                snake.new_head.y = height -2;
            }  
        }
        break;
        case 3:
        if (border_game_over(snake)){
            snake.gameover = 1 ;
        }
        else if (obstacle_game_over(snake)){
            snake.gameover =1;
        }
    }
    return snake;       
}

coordinate create_food(snake snake){
	int size=(((width-2)*(height-2))-snake.length) ;
    coordinate empty[size];
	int i=0;
	int y=0;
	int x=0;
	for ( y=0; y<height; y++){
        for (x=0; x<width; x++){
            if (!is_border(x,y) && !is_snake(x,y, snake) && !is_food(x,y,snake) && !is_obstacle(x,y)){
                 empty[i].x =x;
				 empty[i].y =y;
				 i++;
            }
            
        }
	}
	i=rand() % i ;
	snake.food.x=empty[i].x;
	snake.food.y=empty[i].y;
    return snake.food;
}

int score_boost(snake snake){
	int i=0;
	snake.booster=0;
	int x=snake.food.x;
	int y=snake.food.y;
	int mode=snake.mode;
/*	 if (snake.mode==1){
    		mode=1;
    	}
    	else{
    		mode=2;
    	}*/
	for(i=0; i<snake.length-1; i++){
    	int dx=snake.food.x - snake.body[i].x;
    	int dy=snake.food.y - snake.body[i].y;
    	switch(mode){
    		case 1:
			if ((dx==0 && dy==1) || (dx==0 && dy==-1) || (dy==0 && dx==1) || (dy==0 && dx==-1)){
			   snake.booster=1;
         	}
         	case 2:
         	if ((dx==0 && dy==1) || (dx==0 && dy==-1) || (dy==0 && dx==1) || (dy==0 && dx==-1) || x==1 || y==1 || x==width-2 || y==height-2){
			   snake.booster=1;
         	}
         	if ((x==1 || y==1 || x==width-2 || y==height-2) && ((dx==0 && dy==1) || (dx==0 && dy==-1) || (dy==0 && dx==1) || (dy==0 && dx==-1))){
         		snake.booster=3;
         	}
         	case 3:
         	if ((dx==0 && dy==1) || (dx==0 && dy==-1) || (dy==0 && dx==1) || (dy==0 && dx==-1) || x==1 || y==1 || x==width-2 || y==height-2 || is_obstacle(x,y+1)||is_obstacle(x,y-1)||is_obstacle(x+1,y)||is_obstacle(x-1,y)){
			   snake.booster=1;
 		     }
 		     if((is_obstacle(x,y+1)||is_obstacle(x,y-1)||is_obstacle(x+1,y)||is_obstacle(x-1,y)) && ((dx==0 && dy==1) || (dx==0 && dy==-1) || (dy==0 && dx==1) || (dy==0 && dx==-1))){
         		snake.booster=5;
           	  }
    	}
    }
    return snake.booster;
}

int score(snake snake){
	int x = snake.food.x;
	int y =snake.food.y;
    int step_x= (width/2-1)/4;
    int step_y=step_x/2;
    int score = 0;
    int boost=default_score/2;
    if(  ( x <= step_x || x >= (width-1)-step_x )  && ( y <= step_x || y >= (height-1)-step_x ) ){
    	score += (3*default_score);
    }
    else if(   x <= 2*step_x || x >= (width-1)- 2*step_x   ||  y <= 2*step_y || y >= (height-1)- 2*step_y  ){
    	score += (2*default_score);
    }
    else{
    	score += default_score;
    }
    score = snake.booster*boost;
    snake.score = score;
    return snake.score;
}

snake eat(snake snake){
     if (snake.head.x==snake.food.x && snake.head.y==snake.food.y){
        snake.length++;
        snake.score = score(snake); // correct is +=
        snake.speed_counter ++;
        if(snake.speed_counter % 4 == 0){
        	snake.level++;
        	snake.speed = speed_up(snake);
		}
		snake.food = create_food(snake);
		snake.booster=score_boost(snake);
     }
     else if(snake.food.x== 0 && snake.food.y== 0){
       snake.food = create_food(snake);
     }
     return snake;}

snake update_snake(snake snake){   
    int length=snake.length;            
    snake.head=snake.new_head;          
    snake=eat(snake);
    if(snake.length>length){
        coordinate * body = (coordinate*) malloc(sizeof(coordinate)*snake.length);  
		int j=0;     
        for (j=0; j<snake.length-1; j++){
        body[j]=snake.body[j];
    }   
    snake.body=(coordinate*) malloc(sizeof(coordinate)*snake.length);    
    int i=0;
    for (i=0; i<snake.length-1; i++){
        snake.body[i]=body[i];             
    }
    snake.body[snake.length -1]=snake.head;
    if (snake_game_over(snake)){
        snake.gameover = 1 ;
    }
    free (body);
    return snake;                            
    }  
    else{
        int i=0;                                                
        for (i=0; i<snake.length-1; i++){                       
            snake.body[i]=snake.body[i+1];                
        }                                                 
        snake.body[snake.length -1]=snake.head;             
        if (snake_game_over(snake)){                     
            snake.gameover = 1 ;                            
        }
    return snake;                                               
    }   
}

char keyboard(){
    char key;    
    key=getch();          
    return key;    
}

char keyboard_translate(char key){
    if ((key == 'a') || (key == 'A')){
        key = 'l';
    }else if((key == 'd') || (key == 'D')){
        key = 'r';
    }else if((key == 's') || (key == 'S')){
        key ='d';
    }else if((key == 'w') || (key == 'W')){
        key = 'u';
    }    
    return key;
}

int game_over(snake snake){
    if (snake.gameover == 1){
        system("cls");
        int y=0;
		for ( y=0; y<height; y++){
            int x=0;
			for ( x=0; x<width; x++){           
                if (is_border(x,y)){
                  printf("x");
                }
                else if(y==(height/2)-1 && x==(width/2 -5) ){    
                     int i=x;
                     int j=0;                   
                     char over[]="Game Over!\0";
                     for (j=0; over[j] !='\0'; j++){                         
                         printf("%c",over[j]);                         
                         i++;
                     }  
                     x=i-1;                                                                                                          
                }
                else if(is_o(x,y)){
                    printf("X");
                }
                else{
                    printf(" ");
                }
            }
            printf("\n");
        }      
    return 1;
    }
}

int play_again(){
    delay(initial_speed);
    printf("\nPlay again?(y/n):");
    char x = '\0';
    do{
	    x = getchar();
	    if ((x =='y') || (x == 'Y')){
	        return 1;
	    }else if((x == 'n') || (x == 'N')){
	        return 0;
	    }
	}while((x!='y') && (x!='n') && (x!='Y') && (x!='N'));
}

void print_map(snake snake,int temp_mode){
    int mode=1;    
    if(temp_mode == 1 || temp_mode == 2){
        mode = 1;
    }  
    else if(temp_mode == 3){
        mode = 2;
    }
    int y=0;
    int x=0;
    switch (mode){
        case 1:
            for ( y=0; y<height; y++){
                char buffer[width+1];  
                int x=0;
                for (x=0; x<width; x++){                             
                    if (is_border(x,y)){
                         buffer[x]='x';
                    }
                    else if(is_snake(x,y, snake)){
                         if (x==snake.head.x && y ==snake.head.y){
                             buffer[x]='o';                            
                         }else{
                             buffer[x]='x';                             
                         }                          
                    }                   
                    else if(is_food(x,y,snake) ){
                    		 buffer[x]='@';
                    }
                    else {
                        buffer[x]=' ';
                    }
                }
                buffer[width]='\0';
                printf("%s\n",buffer);
            }
            break;        
        case 2:    
            for (y=0; y<height; y++){
                char buffer[width+1]; 
                for ( x=0; x<width; x++){           
                    if (is_border(x,y)){                        
                        buffer[x]='x';
                    }
                    else if(is_snake(x,y, snake)){
                         if (x==snake.head.x && y ==snake.head.y){
                             buffer[x]='o';                            
                         }else{                             
                             buffer[x]='x';
                         }                          
                    }                   
                    else if(is_obstacle(x,y)){
                        buffer[x]='x';
                    }
                    else if(is_food(x,y,snake)){
                         buffer[x]='@';                        
                    }
                    else {
                        buffer[x]=' ';                      
                    }            
                }
                buffer[width]='\0';
                printf("%s\n",buffer);
            }  
            break;  
    }
}

int key_check(char key){
    if((key != 'w') && (key != 'a') && (key != 's') && (key != 'd') && (key != 'W') && (key != 'A') && (key != 'S') && (key != 'D')){
        return 1;
    } 
}

int welcome_message(){
    int i=0;
    int mode;
    do {
    	system("cls");
    	//Gotoxy();
    	mode=0;
	    for (i=0; i< height/2-3 ; i++){
	        printf("\n");
	    }
	    printf("%*c",width/2 -3,' ');
	    printf("Welcome!\n");

	    printf("\n");
	    printf("%*c",width/2,' ');
	    printf("^");
	    printf("\n");
	    printf("%*c",width/2,' ');
	    printf("W\n");
	    printf("%*c",width/2 - 4,' ');
	    printf("< A s D >\n");
	    printf("%*c",width/2 - 2,' ');
	    printf("  v  \n");
		    /*version 2 of intro*/
		    /*printf("\n");
		    printf("%*c",width/2,' ');
		    printf("_");
		    printf("\n");
		    printf("%*c",width/2-2,' ');
		    printf("_|W|_\n");
		    printf("%*c",width/2 - 3,' ');
	        printf("|A|s|D|\n");
	        printf("%*c",width/2 - 2,' ');
	        printf("-----\n");*/
	    printf("\n");
	    printf("%*c",width/2 -8,' ');
	    printf("Mode 1 | 2 | 3 :");
	    scanf("%d",&mode);
	    getchar();
	}while((mode!=1) && (mode!=2) && (mode!=3));
    return mode;    
}

int play(){    
    time_t a;
 	a=time(0);
    srand(a);
    int play_again_flag=1;
    int mode=welcome_message();
    system("cls");
        snake snake=init_snake_position(snake,4);
		snake.mode=mode;
        cross cross=critical_cross(snake);  
        char dir=direction(snake,cross);
        print_map(snake,mode);
        printf("\n");
        char key= dir;
        while(TRUE){
            delay(snake.speed);            
            if (kbhit()){
                key=keyboard();     
                if(key_check(key)==1){
                    key = dir;
                }
                else{
                    key =keyboard_translate(key);  
                }                                  
                char temp_key=compare_key_with_previous_direction(key,dir);       
                dir = temp_key;                
            }                    
            snake=head_movement(snake,dir,mode);           
            if(game_over(snake) == 1){            
                play_again_flag=play_again();           
                break;            
            }                          
            snake=update_snake(snake);  
            if(game_over(snake) == 1){           
                play_again_flag=play_again();
                break;
            } 
            Gotoxy();
            print_map(snake,mode);
            printf("\n");
            printf("%*c",width/2 -5,' ');
            printf("SCORE:%d",snake.score); 
            printf("\n");
            printf("%*c",width/2 -5,' ');
			int count=0;
			int n=snake.score;
			while(n!=0){
				n/=10;
				++count;
			}
    		printf("LEVEL:%*d\b",count,snake.level);
    		printf("\n");
        }
    return play_again_flag;    
}

int main(){
/*	print_score();
	system ("pause");*/
   int play_again_flag;
    do{          
        play_again_flag=play(); 
        system("cls");        
    }while(play_again_flag==1);
    return 0;
}
