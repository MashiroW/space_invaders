#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define clear() printf("\033[H\033[J")

#define PLAYER_SPEED 2

#define LIMIT_X 207
//#define LIMIT_Y

#define MINION_DEFAULT "./models/minion/minion_default.txt"
#define MINION_LEFT "./models/minion/minion_left.txt"
#define MINION_RIGHT "./models/minion/minion_right.txt"


typedef struct minion minion;
struct minion
{
    int posX;
    int posY;
    int direction; // D:1 - E:2 - W:3
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
            if (c != '\n') 
                putchar(c);

            else
            {
                y_draw+=1;
                printf("\033[%d;%dH", y_draw, x_draw);
            }
        }

        fclose(file);
    }
}

int main()
{
    minion m1 = {
        .posX = 5,
        .posY = 5,
        .direction = 1
    };

    bool sprite = true;

    while (key_pressed() != 27) //While Escape not pressed
    {

        // Z INPUT
        while (key_pressed() == 'z' && m1.posY >= 1)
        {
            m1.posY-=PLAYER_SPEED;
        }

        // S INPUT
        while (key_pressed() == 's' && m1.posY >= 1)
        {
            m1.posY+=PLAYER_SPEED;
        }

        // D INPUT
        while (key_pressed() == 'd' && m1.posX <= 207)
        {
            m1.posX+=PLAYER_SPEED+4;
        }

        // Q INPUT
        while (key_pressed() == 'q' && m1.posX >= 0)
        {
            m1.posX-=PLAYER_SPEED+4;
        }
        
        // J INPUT
        while (key_pressed() == 'j') 
        {
            m1.direction = 3;
        }

        // K INPUT
        while (key_pressed() == 'k') 
        {
            m1.direction = 2;
        }

        clear();

        /*
        if (sprite == true)
            display_element_XY(m1.posX, m1.posY, MINION_DEFAULT); 
        else    
            display_element_XY(m1.posX+5, m1.posY-1, MINION_LEFT);

        printf("\033[%d;%dH", 0,0);
        printf("%d",m1.posX);
        */
    }

    clear();

	return 0;
}




/*

typedef struct vaisseau VAISSEAU;
struct vaisseau
{
    char Direction;           //N => Nord, S => Sud, E => EST, O => OUEST
    int PosX;                  //Position courante coin gauche X du vaisseau
    int PosY;                  //Position courante coin gauche Y du vaisseau
    int Blindage;              //Niveau de blindage en cours du vaisseau(0=>rien, 1=>blindé, 2=>ultra-blindé, etc.)
    int Touches;               //Nombre de fois que le vaisseau est touché parun missile
    char Carros_ennemi [3][16];//Carrosserie du vaisseau ennemi, servira pourl’affichage du vaisseau ennemi à tout moment
    char Carrosserie [6][40];  //Carrosserie du vaisseau du joueur, servira pourl’affichage du vaisseau du joueur à tout moment
    char Type;                 //’M’=> mon vaisseau, ’E’=> vaisseau ennemi
    char Couleur[30];          //Couleur du vaisseau
    int Etat;                  //État du vaisseau 1=> actif, 2=> en destruction,3 => inactif
    int Mise_a_jour;           //utile pour la suppression du vaisseau en tenantcompte d’un certain délai
    struct vaisseau*NXT;     //Pointeur vers un prochain vaisseau, servirapour la liste chaînée - Vous pouvez rajouter d’autres variables si nécessaire
};

*/