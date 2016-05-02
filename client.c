#include "client.h"


void start_game();
void game();

void changemode(int dir);
int kbhit (void);

void printPlayerColor(int pNum);
void printBoard(Board *b);

void getInformation();
void getPass(char * pass);
void getName(char * name);

Connection * c;
char * address; //address leo la primera linea del archivo de configuracion y mando la primera línea.
StreamData sd,buffer;

int main(){
	return 0;
}

void start_game(){
	c = connect(address);
	getInformation();
	game();
}

void game(){
	changemode(1);
	int pressed;
	int rta;
	Point p = {0,0};
	while(/*deberia ser mientras no haya perdido */ 1){
		while(!kbhit()){ /*If a key has been pressed */
			pressed=getchar();
			if(pressed == DOWN_ARROW ){
				p.x = 0;
				p.y = 1;
			}else if(pressed == UP_ARROW){
				p.x = 0;
				p.y = -1;
			}else if(pressed == LEFT_ARROW){
				p.x = -1;
				p.y = 0;
			}else if(pressed == RIGHT_ARROW){
				p.x = 1;
				p.y = 0;
			}
		}
		printf("%d %d\n",p.x,p.y);
		marshalling(p,POINT,&(sd.size));
		rta = sendData(p,sd);
	}
	changemode(0);
}

void getInformation(){
	String password,name;
	char * nombre = malloc (20 *sizeof(char));
   	char * pass = malloc(20 *sizeof(char));
   	int belongs,rta,type;

   	getName(nombre);
	name.size= strlen(nombre);
	name.string = nombre;

	sd.data = marshalling(name,STRING,&(sd.size));

	rta = sendData(c,sd);
	if(rta == 0)
		printf("Error conectandose con el servidor\n");
	
	recieveData(c,buffer);
	void * recieved = unmarshalling(buffer.data,&type);

	if(belongs){
		printf("Ingrese su contraseña para poder ingresar\n");
	}else{
		printf("Su nombre no esta registrado. Ingrese una contraseña para poder registrasrse\n");
	}

	getPass(pass);
	password.size = strlen(pass);
	password.string = pass;

	sd.data = marshalling(password,STRING,&(sd.size));

	int rta = sendData(c,sd);
	if(rta == 0)
		printf("Error conectandose con el servidor\n");
	recieveData();
	if(belongs){

	}else if{
		int w_pass=1;
		//pregunto si lo que escribio esta bien. sino que ingrese su contraseña de vuelta. 
		do{
			printf("Contraseña incorrecta, ingresela de nuevo.\n");
			getPass(pass);
			password.size = strlen(pass);
			password.string = pass;
			sd.data = marshalling(password,STRING,&(sd.size));
			int rta = sendData(c,sd);
			if(rta == 0)
				printf("Error conectandose con el servidor\n");
			recieveData(c,buffer);
			//lo manda de vuelta y cambio w_pass 
		}while(!w_pass);
	}
	free(pass);
	free(nombre);
	return;
}



void getPass(char * pass){
	int i=0,c;
	while(i<20){
		changemode(1);
		while(!kbhit()){
			c = getchar();
			if(c == BACKSPACE && i !=0){
				i--;
			}else if (c == '\n' && i != 0){
				pass[i]='\0';
				break;
			}else{
				pass[i]=c;
				i++;
			}	
		}
	}
	changemode(0);
	pass[i]='\0';
}

void getName(char * name){
	printf("Por favor escriba su nombre:\t");
	scanf("%20s",name);
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

void printBoard(Board *b){
  //system("clear");
  printf("Start\n");
  for(int i = 0;i<b->rows;i++){
    for(int j = 0; j<b->columns;j++){
      printPlayerColor(b->board[i][j]);
      //b->graphicBoard[i][j]
    }
    printf("\n");
  }
}

// void printPlayerColor(int pNum){
//   switch (pNum) {
//     case 1:
//       printf(PLAYER1_COLOR "aa" COLOR_RESET);
//     break;
//     case 2:
//       printf(PLAYER2_COLOR "vv" COLOR_RESET);
//     break;
//     case 3:
//       printf(PLAYER3_COLOR "cc" COLOR_RESET);
//     break;
//     case 4:
//       printf(PLAYER4_COLOR "dd" COLOR_RESET);
//     break;
//     default:
//     printf("  ");
//   }
// }



