#include "client.h"

void startGame();
void game();

void changeMode(int dir);
int kbhit (void);

void printPlayerColor(int pNum);
void printBoard(Board *b);

void getInformation();

void getPass(char * pass);
void getName(char * name);

int checkStringGame(String * s);
int checkStringInfo(String * s);

int sendPoint(Point * p);
int sendString(String * p);


Connection * c;
char * address;
int f2;

StreamData sd,buffer;
StreamData sd2;


void game(){
/*TODO OJO MAGGIE QUE AHORA TENES QUE LEER LAS PRIMERAS 2 , la primera linea es un char* y la segunda es un int
	que tenes que pasarle al connect.*/
	char * aux;
	FILE * f;
	f = fopen(ARCHIVO, "r");
	if(f == NULL){
		perror("config");
	}
	address = malloc(BUFFER * sizeof(char));
	aux = fgets(address,BUFFER,f);
	if(aux == NULL){
		//error no se pudo leer
	}else{
		address = aux;
	}
	fscanf(f,"%d",&f2);
	c = connectToPeer(address,f2);
	getInformation();
	startGame();
	free(address);
}

void startGame(){
	changeMode(1);
	int game = 1;
	int pressed;
	Point * p = malloc(sizeof(Point));
	while(game){
		while(!kbhit() && game){
			pressed=getchar();
			if(pressed == DOWN_ARROW ){
				p->x = 0;
				p->y = 1;
				sendPoint(p);
			}else if(pressed == UP_ARROW){
				p->x = 0;
				p->y = -1;
				sendPoint(p);
			}else if(pressed == LEFT_ARROW){
				p->x = -1;
				p->y = 0;
				sendPoint(p);
			}else if(pressed == RIGHT_ARROW){
				p->x = 1;
				p->y = 0;
				sendPoint(p);
			}
			int type;
			receiveData(c,&sd2);
			void * info = unmarshalling(sd2.data,&type);
			if(type == BOARD_N){
				printBoard((Board * )info);
			}else if(type == STRING_N){
				String * s = (String *)info;
				game = checkStringGame(s);
			}
		}
	}
	free(p);
	changeMode(0);
}

int checkStringGame(String * s){
	if(strcmp(s-> string,"Terminado")){
		return 0;
	}
	return 1;
}

int checkStringInfo(String * s){
	if(strcmp(s-> string,"Pertenece")){
		return 1;
	}
	return 0;
}

int sendPoint(Point * p){
	sd.data = marshalling(p,POINT,&(sd.size));
	return sendData(c,&sd);
}

int sendString(String * string){
	sd.data = marshalling(string,STRING, &(sd.size));
	return sendData(c,&sd);
}

void getInformation(){
	String * password = malloc(sizeof(String));
	String * name = malloc(sizeof(String));
	char * nombre = malloc (20 *sizeof(char));
   	char * pass = malloc(20 *sizeof(char));
   	int belongs=0,rta,type;

   	getName(nombre);
	name->size= strlen(nombre);
	name->string = nombre;

	rta = sendString(name);
	if(rta == 0)
		printf("Error conectandose con el servidor\n");

	receiveData(c,&buffer);
	void * recieved = unmarshalling(buffer.data,&type);

	if(type == STRING_N){
		String * s = (String *) recieved;
		belongs = checkStringInfo(s);
	}

	if(belongs){
		printf("Ingrese su contraseña para poder ingresar\n");
	}else{
		printf("Su nombre no esta registrado. Ingrese una contraseña para poder registrasrse\n");
	}

	getPass(pass);
	password->size = strlen(pass);
	password->string = pass;

	rta = sendString(password);
	if(rta == 0)
		printf("Error conectandose con el servidor\n");

	receiveData(c,&buffer);
	void * passRec = unmarshalling(buffer.data,&type);
	if(type == STRING_N){
		String * s = (String *) passRec;
		belongs = checkStringInfo(s);
	}

	if(belongs){
		printf("Su nombre ha sido registrado correctamente\n");
	}else{
		int w_pass=1;
		do{
			printf("Contraseña incorrecta, ingresela de nuevo.\n");
			getPass(pass);
			password->size = strlen(pass);
			password->string = pass;

			rta = sendString(password);
			if(rta == 0)
				printf("Error conectandose con el servidor\n");
			receiveData(c,&buffer);
			void * passRec = unmarshalling(buffer.data,&type);
			if(type == STRING_N){
				String * s = (String *) passRec;
				w_pass = checkStringInfo(s);
			}
		}while(!w_pass);
		printf("Se ha podido registrar correctamente\n");
	}
	free(pass);
	free(nombre);
	free(password);
	free(name);
	return;
}



void getPass(char * pass){
	int i=0,c;
	while(i<20){
		changeMode(1);
		while(!kbhit() && i<20){
			c = getchar();
			if(c == BACKSPACE && i !=0){
				i--;
			}else if (c == '\n' && i != 0){
				pass[i]='\0';
				i=20;
			}else{
				pass[i]=c;
				i++;
			}
		}
	}
	changeMode(0);
	pass[i-1]='\0';
}

void getName(char * name){
	printf("Por favor escriba su nombre:\t");
	scanf("%20s",name);
}

void changeMode(int dir){
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

void printBoard(Board *b){
  for(int i = 0;i<b->rows;i++){
    for(int j = 0; j<b->columns;j++){
      printPlayerColor(b->board[i][j]);
    }
    printf("\n");
  }
}

void printPlayerColor(int pNum){
   	switch (pNum) {
	    case 1:
       		printf(PLAYER1_COLOR "aa" COLOR_RESET);
    		break;
     	case 2:
       		printf(PLAYER2_COLOR "vv" COLOR_RESET);
     		break;
     	case 3:
       		printf(PLAYER3_COLOR "cc" COLOR_RESET);
     		break;
     	case 4:
       		printf(PLAYER4_COLOR "dd" COLOR_RESET);
     		break;
     	default:
     		printf("  ");
     		break;
    }
}
