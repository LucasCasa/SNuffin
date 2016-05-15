#include "client.h"
#include <stdio.h>


#define BOARD_WIDTH   20
#define BOARD_HEIGHT  20 //esto no va aca, supongo que estara despues en structs.h o algun otro lado para
							//que lo pueda usar el server tambien

Connection * c;
char * address;
int f2;

Player * players[4];

StreamData * buffer;

int main(int argc, char const *argv[]){
	const char * slot;
	if(argc > 1){
		slot = argv[1];
	}else{
		slot = "1"; //el default.
	}
	int value = atoi(slot);

	buffer = calloc(1,sizeof(StreamData));
	buffer->data = calloc(BUFFER,sizeof(char));

	game(value);
	free(buffer->data);
	free(buffer);
	return 0;
}

void game(int slot){
	char * aux;
	int s;
	FILE * f;
	int rta;
	f = fopen(ARCHIVO, "r");
	if(f == NULL){
		perror("config");
	}
	address = calloc(BUFFER,sizeof(char));
	aux = fgets(address,BUFFER,f);
	if(aux == NULL){
		perror("No se pudo leer el archivo");
	}else{
		address = aux;
	}
	fscanf(f,"%d",&f2);

	c = connectToPeer(address,f2);

	rta = sendData(c,marshalling(&slot,SERVER_ID));
	if(rta == 0){
		printf("Error conectandose con el servidor\n");
	}
	receiveData(c,buffer);
	unmarshServerId(buffer->data, &s);
	c->fd = -1;
	do{c = connectToPeer(address,s);}while(c->fd==-1);

	printf("Sali del while, fd=%d\n",c->fd);

	getInformation();
	prepareLobby();
	//startGame();

	fclose(f);
	free(address);
}

void startGame(){
	changeMode(1);
	int game = 1;
	int pressed,rta;
	Board * b = calloc(1,sizeof(Board));
	initializeBoard(b);
	Point * p = calloc(1,sizeof(Point));
	Point * aux = calloc(1,sizeof(Point));
	while(game){
		while(!kbhit() && game){
			pressed=getchar();
			if(pressed == DOWN_ARROW ){
				p->x = 0;
				p->y = 1;
			}else if(pressed == UP_ARROW){
				p->x = 0;
				p->y = -1;
			}else if(pressed == LEFT_ARROW){
				p->x = -1;
				p->y = 0;
			}else if(pressed == RIGHT_ARROW){
				p->x = 1;
				p->y = 0;
			}
			int type;
			if((aux->x == 0 && aux->y == 0) || ((aux->x != p->x) && (aux->y != p->y))){
				aux = p;
				//solo se manda el punto nuevo si se cambia de direccion
				sendData(c,marshalling(p,POINT));
			}
			receiveData(c,buffer);
			rta = unmarshBoard(buffer->data,b);
			if(rta == 1){
				printBoard(b);
			}
			//dejo abierto a que me pueda mandar otra cosa
		}
	}
	free(p);
	free(aux);
	freeBoard(b);
	changeMode(0);
}

void getInformation(){
	char * name = calloc (MAX_WORD,sizeof(char));
   	char * password = calloc(MAX_WORD,sizeof(char));
   	int belongs=0,rta,type;

   	getName(name);

	rta = sendData(c,marshalling(name,STRING));
	if(rta == 0)
		printf("Error conectandose con el servidor\n");
	printf("Se mando el string %s\n", name);

	receiveData(c,buffer);
	printf("Vuelvo del receive\n");
	rta = unmarshBoolean(buffer->data,&belongs);
	if(belongs){
		printf("Ingrese su contraseña para poder ingresar\n");
	}else{
		printf("Su nombre no esta registrado. Ingrese una contraseña para poder registrarse\n");
	}

	getPass(password);

	rta = sendData(c,marshalling(password,STRING));
	if(rta == 0)
		printf("Error conectandose con el servidor\n");

	receiveData(c,buffer);
	rta = unmarshBoolean(buffer->data,&belongs);

	if(belongs){
		printf("Su nombre ha sido registrado correctamente\n");
	}else{
		int w_pass=0;
		do{
			printf("Contraseña incorrecta, ingresela de nuevo.\n");
			getPass(password);

			rta = sendData(c,marshalling(password,STRING));
			if(rta == 0)
				printf("Error conectandose con el servidor\n");
			receiveData(c,buffer);
			rta = unmarshBoolean(buffer->data,&w_pass);
		}while(w_pass == 0);
		printf("Se ha podido registrar correctamente\n");
	}
	free(name);
	free(password);
	return;
}



void getPass(char * pass){
	int i=0,c;
	while(i<MAX_WORD){
		changeMode(1);
		while(!kbhit() && i<MAX_WORD){
			c = getchar();
			if(c == BACKSPACE && i !=0){
				i--;
			}else if (c == '\n'){
				if(i !=0){
					pass[i]='\0';
					i=MAX_WORD;
				}
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
	printf("Por favor escriba su nombre:");
	scanf("%20s",name);
}

void prepareLobby(){
	int i=0,rta = 1;
	while(i<4 && rta){
		receiveData(c,buffer);
		players[i] = calloc(1,sizeof(Player));
		(players[i])->name = calloc(MAX_WORD,sizeof(char));
		rta = unmarshPlayer(buffer->data,players[i]);
		i++;
	}
	//seteo los colores
	sendData(c,marshalling(&TRUE,BOOLEAN));
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
  int i;
  int j;
  for(i = 0;i<b->rows;i++){
    for(j = 0; j<b->columns;j++){
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

void initializeBoard(Board * b){
	int i;
	b ->board = calloc(BOARD_HEIGHT,sizeof(char *));
	for(i =0;i<BOARD_HEIGHT;i++){
		b->board[i]= calloc(BOARD_WIDTH,sizeof(char));
	}
	return;
}

void freeBoard(Board * b){
	int i;
	for(i =0;i<BOARD_HEIGHT;i++){
		free(b->board[i]);
	}
	free(b->board);
	return;
}
