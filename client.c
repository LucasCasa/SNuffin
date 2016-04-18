#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>

#include <netdb.h>
#include <netinet/in.h>

#define UP_ARROW (char)'A' 
#define LEFT_ARROW (char)'D'
#define RIGHT_ARROW (char)'C'
#define DOWN_ARROW (char) 'B'

typedef struct{
	int x;
	int y;
}Point;

typedef struct{
	char * address;
}Connection;

typedef struct {
	char * data;
	int size;
} RequestData;

typedef struct {
	char * data;
	int size;
} ResponseData;

void changemode(int dir);
int kbhit (void);

RequestData marshalling(int direction[]);
char * unmarshalling(ResponseData * data);

void getInformation();

int main(int argc, char *argv[]){
	int sockfd, portno, n;
   	struct sockaddr_in serv_addr;
   	struct hostent *server;
   	getInformation();
   	printf("(Llegue hasta aca\n");
	changemode(1);
	int pressed;
	Point p = {0,0};
	while(/*deberia ser mientras no haya perdido */ 1){
		while(!kbhit()){ /*If a key has been pressed */
			pressed=getchar();
			if(pressed == DOWN_ARROW ){
				printf("ABAJO\n");
				p.y = 1;
			}else if(pressed == UP_ARROW){
				printf("ARRIBA\n");
				p.y = -1;
			}else if(pressed == LEFT_ARROW){
				printf("IZQUIERDA\n");
				p.x = -1;
			}else if(pressed == RIGHT_ARROW){
				printf("DERECHA\n");
				p.x = 1;
			}
		}
	}
	changemode(0);
	return 0;
}

void getInformation(){
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
	int i=0;
	int pressed;
	char pass[10];
	changemode(1);
	while(i<10 && !kbhit()){
		pressed = getchar();
		if(pressed != '\n'){
			printf("*");
			pass[i]=pressed;
			i++;
		}
	}
	if(!belongs){
		//le mando la contraseña para que la guarde en el usuario
	}else{
		//pregunto si lo que escribio esta bien. sino que ingrese su contraseña de vuelta. 
		//con un do while
	}
	changemode(0);
	return;
}

void changemode(int dir){
  static struct termios oldt, newt;
 
  if ( dir == 1 ){
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
 
int kbhit (void){
  struct timeval tv;
  fd_set rdfs;
 
  tv.tv_sec = 0;
  tv.tv_usec = 0;
 
  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);
 
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
 
}

RequestData marshalling(int direction[]){
	RequestData * dataS = malloc(sizeof(RequestData));
	dataS->data = "IZQUIERDA";
	dataS->size = sizeof(dataS->data);
	return *dataS;
}

char * unmarshalling(ResponseData * data){
	char * dataR=malloc(2*sizeof(char));
	if(1){
		//se pregunta para ver que es lo que recibe y modificar dataR
	}
	return dataR;
}



