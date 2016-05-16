#include "client.h"
#include <stdio.h>


#define BOARD_WIDTH   20
#define BOARD_HEIGHT  20 //esto no va aca, supongo que estara despues en structs.h o algun otro lado para
							//que lo pueda usar el server tambien

Connection * c;
char * address;
int f2;


Player * players[MAX_PLAYERS];
int value;
int nPlayers=0;
int ready;
int gameStart;
int playingGame;
char * myName;

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
	for(int i  =0 ; i<MAX_PLAYERS;i++){
		players[i] = NULL;
	}
	game();
	free(myName);
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
	startGame();

	fclose(f);
	free(address);
}

void startGame(){
	pthread_t pressedPlayer;
	changeMode(1);
	playingGame =1;
	int * finished;
	pthread_create(&pressedPlayer,NULL,listenToKeys,&finished);
	int rta;
	Board * b = calloc(1,sizeof(Board));
	//initializeBoard(b);
	while(playingGame){
		readData(c,buffer);
		rta = unmarshBoard(buffer->data,b);
		if(rta == 1){
			printBoard(b);
		}
		//dejo abierto a que me pueda mandar otra cosa
	}
	pthread_join(pressedPlayer,NULL);
	freeBoard(b);
	changeMode(0);
}

void * listenToKeys(void * value){
	int pressed;
	Point * p = calloc(1,sizeof(Point));
	Point * aux = calloc(1,sizeof(Point));
	while(playingGame) /*el juego no arranca */{
		pressed=getchar();
		changeMode(1);
		while(!kbhit() && playingGame){
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
				if(aux->x != 0 || aux->y != 0){
					sendData(c,marshalling(p,POINT));
				}
			}
		}
	}
	free(p);
	free(aux);
	changeMode(0);
	return NULL;
}

void getInformation(){
	char * name = calloc (MAX_WORD,sizeof(char));
   	char * password = calloc(MAX_WORD,sizeof(char));
 	myName = calloc(MAX_WORD,sizeof(char));
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

	memcpy(myName,name,strlen(name)+1); //me guardo mi nombre

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
	pthread_t pressedPlayer;
	gameStart = FALSE;
	int * finished;
	pthread_create(&pressedPlayer,NULL,listenToPress,&finished);
	ready = FALSE;
	int type;
	Player * player = calloc(1,sizeof(Player));
	player-> name = calloc(MAX_WORD,sizeof(char));
	printf("El partido comenzará cuando se llene el lobby.\n");
	while(!gameStart){
			readData(c,buffer);
			printf("Lei data\n");
			printf("unmarshaleo '%s'\n",buffer->data );
			void* struc = unmarshalling(buffer,&type);
			printf("Unmarshalee data\n");
			if(type == PLAYER -'0'){
				printf("Encontre un player\n");
				player = (Player*) struc;
				printf("Player name '%s'\n",player->name );
				int aux = belongs(player);
				if(aux == -1){
					printf("El player es distinto\n");
					int pos = player->num;
					players[pos] = calloc(1,sizeof(Player));
					(players[pos])->name = calloc(MAX_WORD,sizeof(char));
					memcpy(players[pos]->name,player->name,strlen(player->name)+1);
					players[pos]->score= player->score;
					players[pos]->num = player->num;
					players[pos]->ready = player->ready;
				}else{
					players[aux]->ready = player->ready;
				}
			}else if(type == STRING-'0'){
				int leaving;
				sscanf((char*)struc,"%d",&leaving);
				playerLeft(leaving);
			}else if(type == BOOLEAN - '0'){
				gameStart = TRUE;
			}
			printLobby();
	}
	pthread_join(pressedPlayer,NULL);
	printf("El juego comenzará en breve\n");
	free(player->name);
	free(player);
}

void * listenToPress(void * value){
	int pressed;
	while(!gameStart) /*el juego no arranca */{
		changeMode(1);
		while(!kbhit() && !gameStart){
			pressed = getchar();
			if(pressed == ENTER && !ready){
				sendData(c,marshalling((void *)&TRUE,BOOLEAN));
				changeReady(myName,ready);
				ready = TRUE;
			}else if((pressed == 'X' || pressed == 'x') && ready){
				sendData(c,marshalling((void *)&FALSE,BOOLEAN));
				changeReady(myName,ready);
				ready = FALSE;
			}
		}
	}
	changeMode(0);
	return NULL;
}

void connHandler(int sig){
	printf("Me fui\n");
	closeConn(c);
	changeMode(0);
	exit(0);
}

void changeReady(char * myName,int state){
	int i;
	for(i =0;i<MAX_PLAYERS;i++){
		if(players[i]!=NULL){
			if(!strcmp(players[i]->name,myName)){
				players[i]->ready = state;
				return;
			}
		}
	}
	return;
}

//la funcion belongs devuelve -1 si el player no pertenece a los que ya mandaron, sino devuelve el numero que es en el array
int belongs(Player * player){
	int i;
	for(i =0;i<MAX_PLAYERS;i++){
		if(players[i] != NULL){
			if(!strcmp(players[i]->name,player->name))
				return i;
		}
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

void printLobby(){
	int i;
	//system("clear");
	printf("LOBBY NUMERO %d\n\n",value);
	printf("\tJUGADOR \t PUNTAJE \t READY \t\n\n");
	for(i=0;i<MAX_PLAYERS;i++){
		if(players[i] != NULL){
		printf("\t");
		printPlayerColor(i+1 + '0');
		printf(" %s \t    %d", players[i]->name,players[i]->score);
		if(players[i]->ready){
			printf("\t\t   x");
		}else{
			printf("\t\t    ");
		}
		printf("\n\n");
	}else{
		printf("\t   ");
		printf("FREE\n\n");
	}
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


void playerLeft(int leaving){
	int index;
	for(int i=0; i<MAX_PLAYERS; i++){
		if(players[i]!=NULL && players[i]->num==leaving){
			index = i;
		}
	}
	free(players[index]->name);
	free(players[index]);
	players[index] = NULL;
	nPlayers--;
}
