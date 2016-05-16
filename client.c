#include "client.h"
#include <stdio.h>


#define BOARD_WIDTH   20
#define BOARD_HEIGHT  20 //esto no va aca, supongo que estara despues en structs.h o algun otro lado para
							//que lo pueda usar el server tambien

Connection * c;
char * address;
int f2;


Player * players[OTHER_PLAYERS];
int value;

int ready;

StreamData * buffer;

int main(int argc, char const *argv[]){
	const char * slot;
	if(argc > 1){
		slot = argv[1];
	}else{
		slot = "1"; //el default.
	}
	value = atoi(slot);

	buffer = calloc(1,sizeof(StreamData));
	buffer->data = calloc(BUFFER,sizeof(char));
	signal(SIGINT, connHandler);

	game();
	free(buffer->data);
	free(buffer);
	return 0;
}

void game(){
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

	rta = sendData(c,marshalling(&value,SERVER_ID));
	if(rta == 0){
		printf("Error conectandose con el servidor\n");
	}
	readData(c,buffer);
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
	//initializeBoard(b);
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
			if((aux->x == 0 && aux->y == 0) || ((aux->x != p->x) && (aux->y != p->y))){
				aux -> x = p -> x;
				aux -> y = p->y;
				//solo se manda el punto nuevo si se cambia de direccion
				sendData(c,marshalling(p,POINT));
			}
			readData(c,buffer);
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
   	int belongs=0,belongs2=0,rta;

   	getName(name);

	rta = sendData(c,marshalling(name,STRING));
	if(rta == 0)
		printf("Error conectandose con el servidor\n");

	readData(c,buffer);
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

	readData(c,buffer);
	rta = unmarshBoolean(buffer->data,&belongs2);

	if(belongs2 && !belongs){
		printf("Su nombre ha sido registrado correctamente\n");
	}else if(belongs2 && belongs){
		printf("Ha podido ingresar correctamente\n");
	}else{
		int w_pass=0;
		do{
			printf("Contraseña incorrecta, ingresela de nuevo.\n");
			getPass(password);

			rta = sendData(c,marshalling(password,STRING));
			if(rta == 0)
				printf("Error conectandose con el servidor\n");
			readData(c,buffer);
			unmarshBoolean(buffer->data,&w_pass);
		}while(w_pass == 0);
		printf("Ha podido ingrear correctamente\n");
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
	ready = FALSE;
	int i=0,rta = 1;
	Player * player = calloc(1,sizeof(Player));
	player-> name = calloc(MAX_WORD,sizeof(char));
	printf("El partido comenzará cuando se llene el lobby.\n");
	changeMode(1);
	while(i<OTHER_PLAYERS && rta){
		int pressed;
		readData(c,buffer);
		rta = unmarshPlayer(buffer->data,players[i]);
		int aux = belongs(players[i],i);
		if(aux == -1){ 
			players[i] = calloc(1,sizeof(Player));
			(players[i])->name = calloc(MAX_WORD,sizeof(char));
			memcpy(players[i]->name,player->name,strlen(player->name)+1);
			players[i]->score= player->score;
			players[i]->num = player->num;
			players[i]->ready = player->ready;
			i++;
		}else{
			players[aux]->ready = player->ready;
		}
		printLobby(i);
		pressed = getchar();
		if(pressed== 10){ //esto no tiene que ser asi tiene que ser como lo de las flechitas
			sendData(c,marshalling((void*)&TRUE,BOOLEAN));
			ready = TRUE;
		}
	}
	printf("JUEGO EMPEZADO\n");
	changeMode(0);
	free(player->name);
	free(player);
}

void connHandler(int sig){
	printf("Me fui\n");
	closeConn(c);
	exit(0);
}

//la funcion belongs devuelve -1 si el player no pertenece a los que ya mandaron, sino devuelve el numero que es en el array
int belongs(Player * player,int nPlayers){
	int i;
	for(i =0;i<nPlayers;i++){
		if(strcmp(players[i]->name,player->name))
			return i;
	}
	return -1;
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
  printf("SNUFFIN\n");
  for(i = 0;i<b->rows;i++){
    if(i==0){
      for(j=0;j<=b->columns;j++){
        printf("__");
      }
      printf("\n");
    }
    for(j = 0; j<b->columns;j++){
      if(j ==0){
        printf("|");
      }
      printPlayerColor(b->board[i][j]);
    }
    printf("|");
    printf("\n");
  }
  for(j=0;j<=b->rows;j++){
    printf("__");
  }
  printf("\n");
}

void printPlayerColor(int pNum){
   	switch (pNum) {
	    case '1':
       		printf(PLAYER1_COLOR "aa" COLOR_RESET);
    		break;
     	case '2':
       		printf(PLAYER2_COLOR "vv" COLOR_RESET);
     		break;
     	case '3':
       		printf(PLAYER3_COLOR "cc" COLOR_RESET);
     		break;
     	case '4':
       		printf(PLAYER4_COLOR "dd" COLOR_RESET);
     		break;
     	default:
     		printf("  ");
     		break;
    }
}

void printLobby(int nPlayers){
  int i,j;
  system("clear");
  printf("LOBBY NUMERO %d\n\n",value);
  printf("\tJUGADOR \t PUNTAJE \t READY \t\n\n");
  for(i=0;i<nPlayers;i++){
    printf("\t");
    printPlayerColor(i+1 + '0');
    printf(" %s \t    %d", players[i]->name,players[i]->score);
    if(players[i]->ready){
    	printf("\t\t   x");
    }else{
    	printf("\t\t    ");
    }
    printf("\n\n");
  }
  for(j=nPlayers;j<OTHER_PLAYERS;j++){
    printf("\t   ");
    printf("FREE\n\n");
  }

  if(!ready){
  	printf(" -PRESS <ENTER> WHEN READY\n\n");
  	printf(" -PRESS <X> TO CANCEL\n\n");
  }else{
  	printf("ESPERANDO A LOS OTROS JUGADORES\n\n");
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
