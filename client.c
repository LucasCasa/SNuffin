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

#define BORRA_BUFFER while(getchar() != '\n');

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

void getInformation();
char * getPass();

int main(int argc, char *argv[]){
	// int sockfd, portno, n;
 //   	struct sockaddr_in serv_addr;
 //   	struct hostent *server;

 //   	if (argc < 3) {
 //      fprintf(stderr,"usage %s hostname port\n", argv[0]);
 //      exit(0);
 //   }

 //   portno = atoi(argv[2]);
   
 //   /* Create a socket point */
 //   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
 //   if (sockfd < 0) {
 //      perror("ERROR opening socket");
 //      exit(1);
 //   }
	
 //   server = gethostbyname(argv[1]);
   
 //   if (server == NULL) {
 //      fprintf(stderr,"ERROR, no such host\n");
 //      exit(0);
 //   }

 //   bzero((char *) &serv_addr, sizeof(serv_addr));
 //   serv_addr.sin_family = AF_INET;
 //   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
 //   serv_addr.sin_port = htons(portno);
   
 //   /* Now connect to the server */
 //   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
 //      perror("ERROR connecting");
 //      exit(1);
 //   }

 //   /* Now ask for a message from the user, this message
 //      * will be read by server
 //   */

   	getInformation();
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
			}else if(pressed == 127){
				printf("BACKKKKK\n");
			}
		}
	}
	changemode(0);
	return 0;
}

void getInformation(){
	char * nombre;
   	char * pass = malloc(20 *sizeof(char));
   	int belongs=0;
	printf("Por favor escriba su nombre\n");
	scanf("%s",nombre);

	if(!belongs){
		printf("Su nombre no esta registrado. Ingrese una contraseña para registrasrse\n");
	}else{
		printf("Ingrese su contraseña para ingresar\n");
	}
	pass = getPass();
	//manda el string a la base de datos y devuelve si lo encontro o no
	if(!belongs){
		//le mando la contraseña para que la guarde en el usuario
	}else{
		int w_pass=1;
		//pregunto si lo que escribio esta bien. sino que ingrese su contraseña de vuelta. 
		do{
			printf("Contraseña incorrecta, ingrese de nuevo.\n");
			pass = getPass();
			//lo manda de vuelta
		}while(!w_pass);
	}
	free(pass);
	return;
}

char * getPass(){
	int i=0,c;
	char * pass = calloc(20,sizeof(char));
	while(i<20){
		printf("ENTRA ACA DESPUES WHILE\n");
		printf("%d\n", i);
		while(!kbhit()){
			c = getchar();
			if(c == 127 && i !=0){
				i--;
				printf("%c", 127);
				//back
			}else if (c == '\n'){
				return pass;
			}else{
				printf("*");
				i++;
				pass[i]=c;
			}	
		}
	}
	printf("%s\n",pass );
	printf("ENTRA ACA SALE WHILE\n");
	return pass;
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



