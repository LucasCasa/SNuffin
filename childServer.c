#include "childServer.h"

Client **clients;

void initServer(int serverNumber){
   clients = malloc(sizeof(Client*) * 4);
   clients[0] = NULL;
   clients[1] = NULL;
   clients[2] = NULL;
   clients[3] = NULL; //MAL
   
  connectLogServer();
  Connection *s = listenConnection(serverNumber); //cambiar cuando este sockets
  pthread_t listenThread;
  logMsg("Se crea thread startListening");
  int i = pthread_create(&listenThread, NULL,listenNewClients, s);

}

void lobby(){
  //threads LALALALA

  while(1){
    logMsg("Arranco el select");
    int cliNum = listenToClients();
    if(cliNum != -1){
      resolveRequest(cliNum);
    }else{
      printf("ERROR ON SELECT\n");
    }
}
  //select LALALALA
  //Habria que crear una struct para saber cada cliente en que estado esta??? SIII
  //NO se cuando empezar el juego LALALALA

}
void resolveRequest(int nClient){
   Connection *c = clients[nClient]->con;
   StreamData *d = malloc(sizeof(StreamData));
   d->data = malloc(BUFFER_SIZE);
   receiveData(c,d);
   int* type = malloc(sizeof(int));
   int expecting = clients[nClient]->expecting;
   if(expecting == USER){
      validateUser(d,nClient);
   }else if(expecting == PASSWORD){
      if(validatePassword(d,nClient)){
        notifyNewPlayer(nClient);
      }
   }else if(expecting == READY_TO_PLAY){

   }else if(expecting == MOVEMENT){
      //llamo al que setea los movements de los jugadores


   }else{
      fprintf(stderr, "ERROR: expecting not valid\n"); // aca tamnbien iria un messague queue de error
   }


}
int validatePassword(StreamData * d,int nClient){
   char *s = malloc(d->size);
   unmarshString(d->data,s);
   int res;
   if(s != NULL){
      StreamData *d;
      if(validPasswordDB(clients[nClient]->name,s)){
         d = marshalling(&TRUE,BOOLEAN);
         clients[nClient]->expecting = READY_TO_PLAY;
         clients[nClient]->state = WAITING;
         res = 1;
      }else{
         d = marshalling(&FALSE,BOOLEAN);
         res = 0;
      }
      sendData(clients[nClient]->con, d);
      return res;
   }
   return 0;
}
void validateUser(StreamData * d, int nClient){
   char *s = malloc(d->size);
   unmarshString(d->data,s);
   printf("Check if user exists %s\n",d->data );
   if(s != NULL){
      StreamData *d;
      int e = ExistUserDB(s);
      printf("Exists Equals %d\n",e );
      if(e){
         printf("User exists\n");
         d = marshalling(&TRUE,BOOLEAN);
         clients[nClient]->score = getHighScoreDB(s);
      }else{
         d = marshalling(&FALSE,BOOLEAN);
         clients[nClient]->score = 0;
      }
      sendData(clients[nClient]->con, d);
      clients[nClient]->name = s;
      clients[nClient]->expecting = PASSWORD;
   }else{
      //NO SE
   }
}



int listenToClients(){
  char * aux = calloc(128,1);
  int res = 0;
  fd_set cli;
  int maxFD = 0;
  while(res != -1){
     for(int i = 0; i<MAX_PLAYERS;i++){
        if(clients[i] != NULL){
           int clifd = clients[i]->con->fd2;
           if(clifd>maxFD)
           maxFD = clifd;
           sprintf(aux,"I am Listening to client: %d on FD: %d\n",i,clients[i]->con->fd2);
           logMsg(aux);
           FD_SET(clients[i]->con->fd2,&cli);
        }else{
           printf("Client %d is NULL\n",i );
        }
     }

     struct timeval tv = {1, 0}; //the timeout (s,ms)
     res = select(maxFD+1,&cli,NULL,NULL,&tv);
     printf("I received a request %d\n",res );
     for(int i = 0; i<MAX_PLAYERS;i++){
        if(clients[i] != NULL && FD_ISSET(clients[i]->con->fd2,&cli)){
           return i; // o leo los datos??
        }
     }
  }
  return -1;
}
void*  listenNewClients(void* connection){
  int nPlayers = 0;
  logMsg("Escuchando nuevos players");
  while(nPlayers < MAX_PLAYERS){
    Connection *new = acceptConnection(connection);
    logMsg("NUEVO PLAYER WOOOO");
    Client *c = malloc(sizeof(Client));
    c->con = new;
    c->state = LOGGING; // define
    c->expecting = USER;
    clients[nPlayers] = c;
    nPlayers++;
    //tengo que mandar un mensaje a los demas jugadores
  }
  pthread_exit(NULL);
}
void notifyNewPlayer(int nPlayer){
  Client *c = clients[nPlayer];
  for(int i = 0; i< MAX_PLAYERS;i++){
    if(clients[i] != NULL && clients[i] != c){
      StreamData *d = marshalling(CreatePlayerStruct(c,nPlayer),PLAYER);
      sendData(clients[i]->con,d);
    }
  }
}


Player* CreatePlayerStruct(Client *c, int number){
  Player *p = malloc(sizeof(Player));
  p->name = c->name;
  p->score = c->score;
  p->num = number;
  return p;
}
