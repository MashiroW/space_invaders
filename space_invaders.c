#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
 
//PLAYER
#define PLAYER_HP 10
#define PLAYER_SPEED 4
#define PLAYER_X_MIN 5
#define PLAYER_X_MAX 82.00
 
//ENEMIES
#define MINION_NBR 5
 
//RESSOURCES
#define MINION_DEFAULT "./models/minion/minion_default.txt"
#define MINION_LEFT "./models/minion/minion_left.txt"
#define MINION_RIGHT "./models/minion/minion_right.txt"
#define PLAYER "./models/player/player_default.txt"
#define BACKGROUND "./models/background/stars02.txt"
 
#define clear() printf("\033[H\033[J")
 
typedef struct player player;
struct player
{
    int posX;
    int posY;
};
 
typedef struct minion minion;
struct minion
{
    int HP;
    int posX;
    int posY;
    int direction; // D:1 - E:2 - W:3
};
 
typedef struct bullet bullet;
struct bullet
{
    int posX;
    int posY;
    float lastupdate;
};
 
void get_time(double secs)
{
    struct timeval start, stop;
    gettimeofday(&start, NULL);
 
    // Do stuff  here
 
    gettimeofday(&stop, NULL);
    secs += (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("time taken %f\n",secs);
}
 
char key_pressed()
{
    struct termios oldterm, newterm;
    int oldfd; char c, result = 0;
    tcgetattr (STDIN_FILENO, &oldterm);
    newterm = oldterm; newterm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (STDIN_FILENO, TCSANOW, &newterm);
    oldfd = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl (STDIN_FILENO, F_SETFL, oldfd | O_NONBLOCK);
    c = getchar();
    tcsetattr (STDIN_FILENO, TCSANOW, &oldterm);
    fcntl (STDIN_FILENO, F_SETFL, oldfd);
    if (c != EOF) {ungetc(c, stdin); result = getchar();}
    return result;
}
 
void display_element_XY(int x, int y, char *file_path)
{
    int x_draw = x;
    int y_draw = y;
 
    printf("\033[%d;%dH", y_draw,x_draw);
 
    int c;
    FILE *file;
    file = fopen(file_path, "r");
    if (file) 
    {
        while ((c = getc(file)) != EOF)
        {
            if (c == '\n') 
            {
                y_draw+=1;
                printf("\033[%d;%dH", y_draw, x_draw);
            }
 
            else
                printf("%c", c);                
        }
 
        fclose(file);
    }
}
 
void display_background()
{
    printf("\033[%d;%dH", 1,1);
 
    int c;
    FILE *file;
    file = fopen(BACKGROUND, "r");
    if (file) 
    {
        while ((c = getc(file)) != EOF)
        {
            if ( c != ' ')
                printf("%c", c);                 
        }
 
        fclose(file);
    }
}
 
void minion_update(minion element)
{
    clear();
    //display_background();
    
    if (element.direction == 1)
        display_element_XY(element.posX, element.posY, MINION_DEFAULT); 
    else if (element.direction == 2)
        display_element_XY(element.posX+5, element.posY-1, MINION_LEFT);
    else if (element.direction == 3)
        display_element_XY(element.posX+5, element.posY-1, MINION_RIGHT);
}
 
void player_update(player element)
{
    clear();
    //display_background();
    display_element_XY(element.posX, element.posY, PLAYER); 
}
 
void player_listen(player *element)
{
    char input; 
    input = key_pressed();
 
    // Esc
    if (input == 27) //While Escape not pressed
    {
        clear();
        exit(0);
    }
 
    // D INPUT
    else if (input == 'd' && (*element).posX < PLAYER_X_MAX)
    {
        if ( (*element).posX + PLAYER_SPEED > PLAYER_X_MAX )
        {
            (*element).posX+=1;
            player_update(*element);
        }
 
        else
        {
            (*element).posX+=PLAYER_SPEED;
            player_update(*element);
        }
    }
 
    // Q INPUT
    else if (input == 'q' && (*element).posX > PLAYER_X_MIN)
    {
        if ( (*element).posX - PLAYER_SPEED < PLAYER_X_MIN )
        {
            (*element).posX-=1;
            player_update(*element);
        }
 
        else
        {
            (*element).posX-=PLAYER_SPEED;
            player_update(*element);
        }
    }   
}
 
minion * minion_init()
{
    int a;
    minion *minions = malloc(MINION_NBR*sizeof(minion)); 
 
    for(a = 0; a < MINION_NBR; a++)
    {
        minions[a].HP = 5;
        minions[a].posX = PLAYER_X_MAX - (PLAYER_X_MAX/100)*70 + (((PLAYER_X_MAX/100)*70)/MINION_NBR)*a;
        minions[a].posY = 4;
        minions[a].direction = 1;
    }
 
    //Display content of my array or minions
    //for(a = 0; a < MINION_NBR; a++) printf("---- Minion #%d\nHP = %d\nPosX = %d\nPosY = %d\nDirection = %d\n------------\n\n", a+1, minions[a].HP, minions[a].posX, minions[a].posY, minions[a].direction);
 
    return minions;    
}
 
player * player_init()
{
    player *player = malloc(sizeof(player));
    (*player).posX = 50;
    (*player).posY = 35;
 
    return player;
}
 
int main()
{   
    minion *minions = minion_init(); //Access element with "minions[0].posX)"
    player *player = player_init(); // Access element with "(*player).posX)"
 
    player_update((*player));
 
    while (1) //GAME
    {
        player_listen(player);
    }
 
    // FREE, CLEAR SCREEN, QUIT    
    free(minions);
    free(player);
    clear();
    return 0;
}
