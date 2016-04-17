#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#define UP_ARROW (char)'A' 
#define LEFT_ARROW (char)'D'
#define RIGHT_ARROW (char)'C'
#define DOWN_ARROW (char)'B'

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
  if(ch != EOF){
	ungetc(ch, stdin);
	return 1;
  }
  return 0;
}

//http://www.experts-exchange.com/Programming/Languages/C/Q_10119844.html - posted by jos
char getch(){
	char c;
	system("stty raw");
	c= getchar();
	system("stty sane");
	//printf("%c",c);
	return(c);
}

int main(void){
	//deberia iniciar la coneccion con el servidor.
	printf("Por favor escriba su nombre\n");
	char string[10];
	scanf("%s",string);
	//manda el string a la base de datos y devuelve si lo encontro o no
	int belongs=0;
	if(!belongs){
		printf("Su nombre no esta registrado. Ingrese una contraseña para registrasrse\n");
	}else{
		printf("Ingrese su contraseña para ingresar\n");
	}
	char pass[10];
	scanf("%s",pass);
	if(!belongs){
		//le mando la contraseña para que la guarde en el usuario
	}else{
		//pregunto si lo que escribio esta bien. sino que ingrese su contraseña de vuelta. 
		//con un do while
	}
	int pressed;
	while(1){
		if(kbhit()) /*If a key has been pressed */{
			pressed=getch();
			if(pressed == DOWN_ARROW ){
				printf("ABAJO\n");
			}else if(pressed == UP_ARROW){
				printf("ARRIBA\n");
			}else if(pressed == LEFT_ARROW){
				printf("IZQUIERDA\n");
			}else if(pressed == RIGHT_ARROW){
				printf("DERECHA\n");
			}
		}
	}
	return 0;
}



