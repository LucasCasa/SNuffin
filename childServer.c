#include "childServer.h"

Client **clients;

int startGame = 0;

void initServer(int serverNumber){
   clients = malloc(sizeof(Client*) * 4);
   clients[0] = NULL;
   clients[1] = NULL;
   clients[2] = NULL;
   clients[3] = NULL;

  connectLogServer();
  logMsg("Initializing server");
  selfc = listenConnection(serverNumber);
  pthread_t listenThread;
  pthread_create(&listenThread, NULL,listenNewClients, selfc);

}

void lobby(){
   while(!startGame){;
      listenAndResolve(NULL);
   }
   initGame();
}

void listenAndResolve(){
   int cliNum = listenToClients();
   if(cliNum != -1){
      resolveRequest(cliNum);
   }else{
      logError("Error in Select");
   }
}
void initGame(){
  logMsg("Game starting");
   int finished = 0;
   pthread_t playerMovement;
   pthread_create(&playerMovement,NULL,listenToMovement,&finished);
   int nPlayers = 0;
   struct timespec timer;
   timer.tv_sec = 0;
   timer.tv_nsec = 1000000 * UPDATE_MILLISECONDS;

   //start board
   GameBoard b;
   GameBoard* board = &b;
   initBoard(board);
   for(int i = 0; i<MAX_PLAYERS; i++){
      if(clients[i] != NULL){
         nPlayers++;
         loadSnakeNumber(i+1,board);
      }
   }
   board->numPl = nPlayers;
   while(!finished){
     nanosleep(&timer,NULL);
     updateBoard(board);
     sendBoard(board);
     finished = gameOver();
   }
   for(int i = 0; i< MAX_PLAYERS;i++){
      if(clients[i] != NULL){
         if(clients[i]->state != LOOSE){
            setHighscoreDB(clients[i]->name, clients[i]->score + 10);
         }else{
            setHighscoreDB(clients[i]->name, clients[i]->score - 10);
         }
      }
   }
}
int gameOver(){
  StreamData * sd;
  logMsg("Game over");
   int total = 0;
   int lost = 0;
   for(int i = 0; i<MAX_PLAYERS;i++){
      if(clients[i] != NULL){
         total++;
         if(clients[i]->state == LOOSE){
            sd = marshalling((void*)&FALSE,BOOLEAN);
            lost++;
         }else{
            sd = marshalling((void*)&TRUE,BOOLEAN);
         }
         sendData(clients[i]->con,sd);
      }
   }
   return (lost >= total-1);
}
void playerLost(int nPlayer){
   clients[nPlayer]->state = LOOSE;
}
void sendBoard(GameBoard * gb){
   Board* bd = malloc(sizeof(Board));
   bd->board = malloc(sizeof(char*)*20);
   for (int i = 0; i<20 ; i++){
      bd->board[i] = gb->gB[i];
   }
   //printBoard(bd->board);
   bd->rows = 20;
   bd->columns = 20;
   StreamData* s = marshalling(bd,BOARD);
   for(int i = 0; i<MAX_PLAYERS;i++){
      if(clients[i] != NULL){
         sendData(clients[i]->con, s);
      }
   }
   free(bd->board);
   free(bd);
}
void *listenToMovement(void * condition){
   int cond = *(int*)condition;
   while(!cond){
      listenAndResolve();
   }
   return NULL;
}

void resolveRequest(int nClient){
   Connection *c = clients[nClient]->con;
   StreamData *d = malloc(sizeof(StreamData));
   d->data = malloc(BUFFER_SIZE);
   int res;
   res = readData(c,d);
   int expecting = clients[nClient]->expecting;
   if(!res){
    disconnect(nClient);
    return;
  }
   if(expecting == USER){
      res = validateUser(d,nClient);
   }else if(expecting == PASSWORD){
      res = validatePassword(d,nClient);
      if(res == 1){
         notifyNewPlayer(nClient);
         notifyExistingPlayers(nClient);
      }
   }else if(expecting == NEWPASSWORD){
      char* s = malloc(sizeof(d->size));
      res = unmarshString(d->data,s);
      if(res){
         res = createUserDB(clients[nClient]->name,s );
         StreamData *d2;
         if(!res){
            d2 = marshalling((void*)&TRUE,BOOLEAN);
            clients[nClient]->expecting = READY_TO_PLAY;
            clients[nClient]->state = WAITING;
         }else{
            d2 = marshalling((void*)&FALSE,BOOLEAN);
         }
         sendData(clients[nClient]->con, d2);
         notifyNewPlayer(nClient);
         notifyExistingPlayers(nClient);
         free(d2->data);
         free(d2);
      }else{
         res = -1;
      }
      free(s);
   }else if(expecting == READY_TO_PLAY){
      int value = 0;
      res = unmarshBoolean(d->data,&value);
      if(res){
         if(value){
            clients[nClient]->state = READY_PLAY;
            notifyNewPlayer(nClient);
            checkGameStart();
         }else{
            clients[nClient]->state = WAITING;
            notifyNewPlayer(nClient);
         }
      }
   }else if(expecting == MOVEMENT){
      Point p;
      res = unmarshPoint(d->data,&p);
      if(res){
         updateMovementDirection(nClient,p);
      }
   }else{
      logError("Invalid expecting state");
   }
   free(d->data);
   free(d);
}
void checkGameStart(){
   int nPlayers = 0;
   int start = 1;
   for(int i = 0; i<MAX_PLAYERS;i++){
      if(clients[i] != NULL){
         nPlayers++;
         start = start && (clients[i]->state == READY_PLAY);
      }
   }
   if(start && nPlayers > 1){
      startGame = 1;
      sendGameStart();
   }
}
void sendGameStart(){
   logMsg("Notifying players about game start");
   for(int i = 0; i<MAX_PLAYERS;i++){
      if(clients[i] != NULL){
         clients[i]->state = PLAYING;
         clients[i]->expecting = MOVEMENT;
         StreamData *d = marshalling((void*)&TRUE,BOOLEAN);
         sendData(clients[i]->con,d);
      }
   }
}
int validatePassword(StreamData * d,int nClient){
   char *s = malloc(d->size);
   unmarshString(d->data,s);
   int res;
   if(s != NULL){
      StreamData *d2;
      if(validPasswordDB(clients[nClient]->name,s)){
         d2 = marshalling((void*)&TRUE,BOOLEAN);
         clients[nClient]->expecting = READY_TO_PLAY;
         clients[nClient]->state = WAITING;
         res = 1;
      }else{
         d2 = marshalling((void*)&FALSE,BOOLEAN);
         res = 0;
      }
      sendData(clients[nClient]->con, d2);
      free(s);
      free(d2->data);
      free(d2);
      return res;
   }
   return -1;
}
int validateUser(StreamData * d, int nClient){
   char *s = malloc(d->size);
   int r = unmarshString(d->data,s);
   if(r == 0){
      logWarning("Invalid String received");
      return -1;
   }

   if(s != NULL){
      StreamData *d2;
      int e = ExistUserDB(s);
      if(e){
         d2 = marshalling((void*) &TRUE,BOOLEAN);
         clients[nClient]->expecting = PASSWORD;
         clients[nClient]->score = getHighScoreDB(s);
      }else{
         d2 = marshalling((void*)&FALSE,BOOLEAN);
         clients[nClient]->expecting = NEWPASSWORD;
         clients[nClient]->score = 0;
      }
      sendData(clients[nClient]->con, d2);
      clients[nClient]->name = malloc(strlen(s) + 1);
      memcpy(clients[nClient]->name, s, strlen(s) +1);
      free(s);
      return 0;
   }else{
      return -1;
      free(s);
   }
}



int listenToClients(){
  int res = 0;
  fd_set cli;
  int maxFD = 0;
  while(res != -1){
     for(int i = 0; i<MAX_PLAYERS;i++){
        if(clients[i] != NULL){
           int clifd = clients[i]->con->fd;
           if(clifd>maxFD)
            maxFD = clifd;
           FD_SET(clients[i]->con->fd,&cli);
        }
     }

     struct timeval tv = {1, 0}; //the timeout (s,ms)
     res = select(maxFD+1,&cli,NULL,NULL,&tv);
     for(int i = 0; i<MAX_PLAYERS;i++){
        if(clients[i] != NULL && FD_ISSET(clients[i]->con->fd,&cli)){
           return i;
        }
     }
  }
  return -1;
}
void*  listenNewClients(void* connection){
   int nPlayers = 0;
   int nextEmpty = 0;
   logMsg("Listening for new Players");
   while(1){

      if(nPlayers < MAX_PLAYERS){
         Connection *new = acceptConnection(connection);
         logMsg("New incoming connection"); //alto spanglish
         Client *c = malloc(sizeof(Client));
         c->con = new;
         c->state = LOGGING;
         c->expecting = USER;
         clients[nextEmpty] = c;
      }
      nPlayers = 0;
      for(int i = 0; i<MAX_PLAYERS;i++){
         if(clients[i] != NULL){
            nPlayers++;
            if(nextEmpty == i){
               nextEmpty++;
            }
         }else if(nextEmpty > i){
            nextEmpty = i;
         }
      }
   }
   pthread_exit(NULL);
}
/** Notifica a los jugadores que estan en el lobby que ingreso un nuevo jugador
Adicionalmente se utiliza para avisar que se actualizo el valor de ready de
un jugador*/
void notifyNewPlayer(int nPlayer){
  Client *c = clients[nPlayer];
  Player* p = CreatePlayerStruct(c,nPlayer);
  StreamData *d = marshalling(p,PLAYER);
  for(int i = 0; i< MAX_PLAYERS;i++){
    if(clients[i] != NULL && clients[i]->state != LOGGING && clients[i] != c){
      sendData(clients[i]->con,d);
    }
  }
  free(p);
  free(d->data);
  free(d);
}
/** Notifica al nuevo jugador de los jugadores que ya se encuentran en el lobby*/
void notifyExistingPlayers(int nPlayer){
  char * aux = calloc(128,1);
  Client *c = clients[nPlayer];
  for(int i = 0; i< MAX_PLAYERS;i++){
    if(clients[i] != NULL){
      sprintf(aux,"Notifying %s about %s", clients[nPlayer]->name,clients[i]->name);
      logMsg(aux);
      Player* p = CreatePlayerStruct(clients[i],i);
      StreamData *d = marshalling(p,PLAYER);
      sendData(c->con,d);
      free(p);
      free(d->data);
      free(d);
    }
  }
}

Player* CreatePlayerStruct(Client *c, int number){
  Player *p = malloc(sizeof(Player));
  p->name = c->name;
  p->score = c->score;
  p->num = number;
  p->ready = (c->state == READY_PLAY);
  return p;
}

void disconnect(int nClient){
  logMsg("Player disconnecting");
  for(int i=0; i<MAX_PLAYERS; i++){
    if(i!=nClient && clients[i]!=NULL){
      char * aux = calloc(10,1);
      sprintf(aux,"%d",nClient);
      sendData(clients[i]->con,marshalling(aux,STRING));
    }
  }
  free(clients[nClient]->name);
  free(clients[nClient]->con);
  free(clients[nClient]);
  clients[nClient] = NULL;
}
