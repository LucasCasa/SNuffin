#include "client.h"

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
			}
		}
	}
	changemode(0);
	return 0;
}

void getInformation(){
	String password,name;
	char * nombre = malloc (20 *sizeof(char));
   	char * pass = malloc(20 *sizeof(char));
   	int belongs=0;
	printf("Por favor escriba su nombre\n");
	scanf("%20s",nombre);
	name.size= strlen(nombre);
	name.string = nombre;

	//manda el nombre

	if(!belongs){
		printf("Su nombre no esta registrado. Ingrese una contraseña para registrasrse\n");
	}else{
		printf("Ingrese su contraseña para ingresar\n");
	}
	pass = getPass();
	password.size = strlen(pass);
	password.string = pass;
	//manda el string a la base de datos y devuelve si lo encontro o no
	if(!belongs){
		//le mando la contraseña para que la guarde en el usuario
	}else{
		int w_pass=1;
		//pregunto si lo que escribio esta bien. sino que ingrese su contraseña de vuelta. 
		do{
			printf("Contraseña incorrecta, ingrese de nuevo.\n");
			pass = getPass();
			password.size = strlen(pass);
			password.string = pass;
			//lo manda de vuelta y cambio w_pass 
		}while(!w_pass);
	}
	free(pass);
	free(nombre);
	return;
}

char * getPass(){
	int i=0,c;
	char * pass = malloc(20*sizeof(char));
	
	while(i<20){
		changemode(1);
		while(!kbhit()){
			c = getchar();
			if(c == BACKSPACE && i !=0){
				i--;
			}else if (c == '\n' && i != 0){
				pass[i]='\0';
				return pass;
			}else{
				pass[i]=c;
				i++;
			}	
		}
	}
	changemode(0);
	pass[i]='\0';
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



