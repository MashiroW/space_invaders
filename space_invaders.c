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
#define PLAYER_X_MAX 80.00
 
// X: 190 - Y - 52
 
//ENEMIES
#define MINION_NBR 6
#define MINION_MAX_PER_LINE 6
 
//RESSOURCES
#define LEVIATHAN "./models/leviathan/leviathan_default.txt"
#define MINION_DEFAULT "./models/minion/minion_default.txt"
#define MINION_LEFT "./models/minion/minion_left.txt"
#define MINION_RIGHT "./models/minion/minion_right.txt"
#define PLAYER "./models/player/player_default2.txt"
#define BACKGROUND01 "./models/background/stars01.txt"
#define BACKGROUND02 "./models/background/stars02.txt"

#define TIMECHECK \
    if ( (last_timestamp + 0.7) < timestamp()) \
    { \
        last_timestamp = timestamp(); \
        background_status = !background_status; \
        display_update(element, minions, captains, leviathan, background_status); \
    } \

#define clear() printf("\033[H\033[J")
 
typedef struct player player;
struct player
{
    int HP;
    int posX;
    int posY;
};
 
typedef struct minion minion;
struct minion
{
    int HP;
    int posX;
    int posY;
    int model; // D:1 - E:2 - W:3
    int speed;
};

typedef struct captain captain;
struct captain
{
    int HP;
    int posX;
    int posY;
    int model; // D:1 - E:2 - W:3
    int speed;
};

typedef struct leviathan leviathan;
struct leviathan
{
    int HP;
    int posX;
    int posY;
    int speed;
};
 
typedef struct bullet bullet;
struct bullet
{
    int posX;
    int posY;
    int direction; // UP:1 - DOWN:0
    int speed;
    float lastupdate;
};

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
 
void display_background(bool background_status)
{
    printf("\033[%d;%dH", 1,1);
 
    int c;
    FILE *file;

    if (background_status == false) file = fopen(BACKGROUND01, "r");
    else file = fopen(BACKGROUND02, "r");

    if (file) 
    {
        while ((c = getc(file)) != EOF)
        {
            printf("%c", c);
        }
 
        fclose(file);
    }
}
 
void minion_update(minion element)
{
    clear();
    //display_background();
    
    if (element.model == 1)
        display_element_XY(element.posX, element.posY, MINION_DEFAULT); 
    else if (element.model == 2)
        display_element_XY(element.posX+5, element.posY-1, MINION_LEFT);
    else if (element.model == 3)
        display_element_XY(element.posX+5, element.posY-1, MINION_RIGHT);
}
 
void display_minions(minion *minions, leviathan *leviathan)
{
    for(int a = 0; a < MINION_NBR; a++)
    {
        if ( minions[a].model == 1) display_element_XY(minions[a].posX, minions[a].posY, MINION_DEFAULT);
        else if ( minions[a].model == 2) display_element_XY(minions[a].posX, minions[a].posY, MINION_LEFT);
        else if ( minions[a].model == 3) display_element_XY(minions[a].posX, minions[a].posY, MINION_RIGHT);
    }

    display_element_XY(leviathan[0].posX, leviathan[0].posY, LEVIATHAN);
}

void display_update(player *element, minion *minions, captain *captains, leviathan *leviathan, bool background_status)
{
    clear();
    display_background(background_status);
    display_element_XY((*element).posX, (*element).posY, PLAYER);
    display_minions(minions, leviathan);
}
 
double timestamp()
{
    struct timeval ts;
    gettimeofday(&ts, NULL); // return value can be ignored

    double secs;
    double usecs; 
    double timestamp;
    secs = ts.tv_sec;
    usecs = ts.tv_usec;

    while (usecs > 1) usecs = usecs/10;

    timestamp = secs + usecs;
    //printf("secs = %f -- usecs = %f -- timestamp = %f --\n", secs, usecs, timestamp);

    return timestamp;
}

void player_listen(player *element, minion *minions, captain *captains, leviathan *leviathan)
{
    bool background_status = false;
    double last_timestamp = timestamp();

    display_update(element, minions, captains, leviathan, background_status);
    

    while(1)
    {
        TIMECHECK

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
                TIMECHECK
                display_update(element, minions, captains, leviathan, background_status);
            }
    
            else
            {
                (*element).posX+=PLAYER_SPEED;
                TIMECHECK
                display_update(element, minions, captains, leviathan, background_status);
            }
        }
    
        // Q INPUT
        else if (input == 'q' && (*element).posX > PLAYER_X_MIN)
        {
            if ( (*element).posX - PLAYER_SPEED < PLAYER_X_MIN )
            {
                (*element).posX-=1;
                TIMECHECK
                display_update(element, minions, captains, leviathan, background_status);
            }
    
            else
            {
                (*element).posX-=PLAYER_SPEED;
                TIMECHECK
                display_update(element, minions, captains, leviathan, background_status);
            }
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
        minions[a].posX = PLAYER_X_MAX - (PLAYER_X_MAX/100)*70 + (((PLAYER_X_MAX/100)*70)/MINION_NBR)*(a+1);
        minions[a].posY = 10;
        minions[a].model = 1;
        minions[a].speed = 1;
    }
 
    //Display content of my array or minions
    //for(a = 0; a < MINION_NBR; a++) printf("---- Minion #%d\nHP = %d\nPosX = %d\nPosY = %d\nmodel = %d\n------------\n\n", a+1, minions[a].HP, minions[a].posX, minions[a].posY, minions[a].model);
 
    return minions;    
}

captain * captain_init()
{
    int a;
    captain *captains = malloc(MINION_NBR*sizeof(captain)); 
 
    for(a = 0; a < MINION_NBR/2; a++)
    {
        captains[a].HP = 5;
        captains[a].posX = PLAYER_X_MAX/2;
        captains[a].posY = 10;
        captains[a].model = 1;
        captains[a].speed = 2;
    }
  
    return captains;    
}

leviathan * leviathan_init()
{
    leviathan *leviathan = malloc(sizeof(leviathan));
    (*leviathan).HP = 100;
    (*leviathan).posX = PLAYER_X_MAX/2;
    (*leviathan).posY = 3;
    (*leviathan).speed = 3;

    return leviathan;
}
 
bullet * bullet_init(int posX)
{
    int a;
    bullet *bullet = malloc(sizeof(bullet));
    bullet[a].posX = PLAYER_X_MAX;
    bullet[a].posY = 4;
    bullet[a].speed = 4;
 
    return bullet;    
}
 
player * player_init()
{
    player *player = malloc(sizeof(player));
    (*player).HP = 10;
    (*player).posX = 50;
    (*player).posY = 50;
 
    return player;
}
 
int main()
{   
    minion *minions = minion_init(); //Access element with "minions[0].posX)"
    captain *captains = captain_init(); 
    leviathan *leviathan = leviathan_init(); 

    player *player = player_init(); // Access element with "(*player).posX)"

    //GAME
    player_listen(player, minions, captains, leviathan);
 
    // FREE, CLEAR SCREEN, QUIT    
    free(minions);
    free(player);
    free(leviathan);
    clear();
    return 0;
}
 
 
 
 
 
 
 
 
 
 
 
/*
int main()
{
    minion m1 = {
        .HP = 10
        .posX = 5,
        .posY = 5,
        .model = 1
        .is_dead = false;
    };
 
    char input; 
    double secs = 0;
 
    clear();
    update_display(m1);
 
    while (1) //GAME
    {
        input = key_pressed();
 
        // Esc
        if (input == 27) //While Escape not pressed
        {
            clear();
            exit(0);
        }
 
        // Z INPUT
        else if (input == 'z' && m1.posY >= 1)
        {
            m1.posY-=1;
            update_display(m1);
        }
 
        // S INPUT
        else if (input == 's' && m1.posY >= 1)
        {
            m1.posY+=1;
            update_display(m1);
        }
 
        // D INPUT
        else if (input == 'd' && m1.posX <= LIMIT_X)
        {
            m1.posX+=PLAYER_SPEED;
            update_display(m1);
        }
 
        // Q INPUT
        else if (input == 'q' && m1.posX >= 0)
        {
            m1.posX-=PLAYER_SPEED;
            update_display(m1);
        }
        
        // J INPUT
        else if (input == 'j') 
        {
            m1.model = 2;
            update_display(m1);
        }
 
        // K INPUT
        else if (input == 'k') 
        {
            m1.model = 1;
            update_display(m1);
        }
    
        // L INPUT
        else if (input == 'l') 
        {
            m1.model = 3;
            update_display(m1);
        }
 
    }
        
    clear();
    return 0;
}
 
*/