#include "childServer.h"

Client **clients;
//SIN PROBLEMAS DE MEMORIA?

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
  pthread_create(&listenThread, NULL,listenNewClients, s);

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
   int res;
   readData(c,d);
   int expecting = clients[nClient]->expecting;
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
         createUserDB(clients[nClient]->name,s );
         StreamData *d2;
         if(!res){
            d2 = marshalling((void*)&TRUE,BOOLEAN);
            clients[nClient]->expecting = READY_TO_PLAY;
            clients[nClient]->state = WAITING;
         }else{
            d2 = marshalling((void*)&FALSE,BOOLEAN);
         }
         sendData(clients[nClient]->con, d2);
         free(d2->data);
         free(d2);
      }else{
         res = -1;
      }
      free(s);
   }else if(expecting == READY_TO_PLAY){

   }else if(expecting == MOVEMENT){
      //llamo al que setea los movements de los jugadores


   }else{
      fprintf(stderr, "ERROR: expecting not valid\n"); // aca tamnbien iria un messague queue de error
   }
   if(res == -1){
      if(d->data[0] == 0){
      printf("START FREE NAME\n");
      //free(clients[nClient]->name);
      printf("FREE NAME\n");
      free(clients[nClient]->con);
      printf("FREE CON\n");
      free(clients[nClient]);
      printf("FREE CLIENT\n");
      clients[nClient] = NULL;
      }
   }
   free(d->data);
   free(d);
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
   free(s);
   return -1;
}
int validateUser(StreamData * d, int nClient){
   char *s = malloc(d->size);
   int r = unmarshString(d->data,s);
   if(r == 0){

      return -1;
   }
   printf("Check if user exists %s\n",d->data );
   if(s != NULL){
      StreamData *d2;
      int e = ExistUserDB(s);
      printf("Exists Equals %d\n",e );
      if(e){
         printf("User exists\n");
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
   }else{
      return -1;
      free(s);
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
           //logMsg(aux);
           FD_SET(clients[i]->con->fd2,&cli);
        }else{
          // printf("Client %d is NULL\n",i );
        }
     }

     struct timeval tv = {1, 0}; //the timeout (s,ms)
     res = select(maxFD+1,&cli,NULL,NULL,&tv);
     printf("I received a request %d\n",res );
     for(int i = 0; i<MAX_PLAYERS;i++){
        if(clients[i] != NULL && FD_ISSET(clients[i]->con->fd2,&cli)){
           free(aux);
           return i; // o leo los datos??
        }
     }
  }
  free(aux);
  return -1;
}
void*  listenNewClients(void* connection){
   int nPlayers = 0;
   int nextEmpty = 0;
   logMsg("Escuchando nuevos players");
   while(1){ // game don*t start

      if(nPlayers < MAX_PLAYERS){
         Connection *new = acceptConnection(connection);
         logMsg("NUEVO PLAYER WOOOO"); //alto spanglish
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
      //no hago free porque lo sigo usando
   }
   pthread_exit(NULL);
}
void notifyNewPlayer(int nPlayer){
  Client *c = clients[nPlayer];
  for(int i = 0; i< MAX_PLAYERS;i++){
    if(clients[i] != NULL && clients[i] != c){
      Player* p = CreatePlayerStruct(c,nPlayer);
      StreamData *d = marshalling(p,PLAYER);
      sendData(clients[i]->con,d);
      free(p);
      free(d->data);
      free(d);
    }
  }
}

void notifyExistingPlayers(int nPlayer){
  char * aux = calloc(128,1);
  logMsg("Notyfifying about existing players\n");
  Client *c = clients[nPlayer];
  for(int i = 0; i< MAX_PLAYERS;i++){
    if(clients[i] != NULL && clients[i] != c){
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
  return p;
}
