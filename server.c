#include "server.h"

char* shmPointer;
sem_t semDB;
int servers[MAX_LOBBY];
Client **clients;

int main(int argc, const char* argv[]){
    /*set de db semaphore*/
    clients = malloc(sizeof(Client*) * 4);
    clients[0] = NULL;
    clients[1] = NULL;
    clients[2] = NULL;
    clients[3] = NULL; //MAL
    sem_init(&semDB,1,1);
   /* map into memory */
   shmPointer = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
   for(int i = 0; i<1024;i++){
      shmPointer[i] = 0;
   }
   setDB();
   /*FILE * fp;
   char * srvAddr = malloc(BUFFER_SIZE);

   fp = fopen("./configFIFO", "r");
   if (fp == NULL)
   exit(EXIT_FAILURE);

   fgets(srvAddr,BUFFER_SIZE,fp);

   EN UN FUTURO HAY QUE HACER ESTO BIEN, ESTO ES SOLO PARA TEST
   */
   //Esto es para probar que se cambia el valor en ambos procesos.
   /*(ExistUserDB("kuyum") )?printf("SI existe Kuyum\n"):printf("NO kuyum\n");
   (ExistUserDB("LALALAAAAA") )?printf("SI existe ALALA\n"):printf("NO LALA\n");
   (ExistUserDB("lucas") )?printf("SI existe lucas\n"):printf("NO lucas\n");
   (validPasswordDB("lucas","lucas") )?printf("FUNCIONAA\n"):printf("NO lucas\n");

   printf("Score de Lucas: %d\n",getHighScoreDB("lucas"));
   int aux = getHighScoreDB("lucas") + 1;
   setHighscoreDB("lucas",aux);
   printf("Nuevo Score de Lucas: %d\n",getHighScoreDB("lucas"));
   */
   Connection * selfc = listenConnection(0); // PROXIMAMENTE LEO CONFIG PASO NUMERO
   printf("Estoy esuchando\n");
   while(1){
      StreamData * buffer = malloc(sizeof(StreamData));
      buffer->data = malloc(2048);
      // receiveData(selfc,buffer);
      printf("Accept en %d\n",selfc->fd2);
      Connection *new = acceptConnection(selfc);
      printf("Nueva conexion\n");
      receiveData(new,buffer);
      printf("Recibi: %s\n",buffer->data);
      /* ACA tendria que haber un marshalling*/
      int joinServer = atoi(buffer->data);
      if(servers[joinServer] != 0){
        sendData(new,marshalling(servers + joinServer, SERVER_ID));
      }else{
        int child = fork();
        if(child == 0){
            lobby();
        }else{
         servers[joinServer] = child;
         pthread_t waitChild;
         pthread_create(&waitChild,NULL,waitForChild,&child);
         sendData(new,marshalling(servers + joinServer ,SERVER_ID));
      }
    }
      // if(buffer->data[0] == '@'){
      //   printf("Lei un accept\n");
      //   int child = fork();
      //   if(child == 0){
      //     printf("Me forkee soy hijo\n");
      //     char * aux = malloc(sizeof(srvAddr) + 6);
      //     sprintf(aux,"%s%d",srvAddr,getpid());
      //     selfc = start_ipc(aux);
      //
      //   }else{
      //     Connection *c = malloc(sizeof(Connection));
      //     c->addr = buffer->data + 1;
      //     StreamData *daux = malloc(sizeof(StreamData));
      //     daux->data = malloc(BUFFER_SIZE);
      //     sprintf(daux->data,"%s%d",srvAddr,getpid());
      //     daux->size = strlen(daux->data);
      //     sendData(c,daux);
      //   }
      //}else{
      //  printf("%s\n",buffer->data);
      //}
      //}
   }
   printf("Me fui\n");
}

void lobby(){
  //threads LALALALA
  pthread_t* listenThread = malloc(sizeof(pthread_t));
  pthread_create(listenThread, NULL,startListening, NULL);
  int cliNum = listenToClients();
  if(cliNum != -1){
     resolveRequest(cliNum);
   }else{
      printf("ERROR ON SELECT\n");
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
         d = marshalling(TRUE,BOOLEAN);
         clients[nClient]->expecting = READY_TO_PLAY;
         clients[nClient]->state = WAITING;
         res = 1;
      }else{
         d = marshalling(FALSE,BOOLEAN);
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
   if(s != NULL){
      StreamData *d;
      if(ExistUserDB(s)){
         d = marshalling(TRUE,BOOLEAN);
         clients[nClient]->score = getHighScoreDB(s);
      }else{
         d = marshalling(FALSE,BOOLEAN);
         clients[nClient]->score = 0;
      }
      sendData(clients[nClient]->con, d);
      clients[nClient]->name = s;
      clients[nClient]->expecting = PASSWORD;
   }else{
      //NO SE
   }
}
void * waitForChild(void* pid){
  int s = *(int*) pid;
  waitpid(pid);
  for(int i = 0 ; i<MAX_LOBBY;i++){
    if(servers[i] == pid){
      servers[i] = 0;
    }
  }
}
int listenToClients(){
  fd_set* cli;
  int nfds = 0;
  for(int i = 0; i<MAX_PLAYERS;i++){
    if(clients[i] != NULL){
      nfds++;
      FD_SET(clients[i]->con->fd,cli);
    }
  }
  select(nfds,cli,NULL,NULL,NULL);
  for(int i = 0; i<MAX_PLAYERS;i++){
    if(clients[i] != NULL && FD_ISSET(clients[i]->con->fd,cli)){
      return i; // o leo los datos??
    }
  }
  return -1;
}
void*  startListening(void* info){
  int nPlayers = 0;
  Connection *s = listenConnection(getpid()); //cambiar cuando este sockets

  while(nPlayers < MAX_PLAYERS){
    Connection *new = acceptConnection(s);
    Client *c = malloc(sizeof(Client));
    c->con = new;
    c->state = LOGGING; // define
    c->expecting = USER;
    clients[nPlayers] = c;

    //tengo que mandar un mensaje a los demas jugadores
  }
  pthread_exit(NULL);
}
void notifyNewPlayer(int nPlayer){
  Client *c = clients[nPlayer];
  for(int i = 0; i< MAX_PLAYERS;i++){
    if(clients[i] != NULL && clients[i] != c){
      StreamData *d = marshalling(CreatePlayerStruct(c,nPlayer),PLAYER);
      sendData(d,clients[i]->con);
    }
  }
}
void setDB(){
  int db = fork();
  if(db == 0){
     printf("Hola soy el hijo\n");
    manageDataBase();
  }
  printf("Soy el padre\n");
}
char ExistUserDB(char * user){
  sem_wait(&semDB);
  shmPointer[TYPEPOS] = ISUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user));
  shmPointer[strlen(user) + 1 ] = 0;
  while(shmPointer[TYPEPOS] != READY);
  char c = shmPointer[RETURNPOS];
  sem_post(&semDB);
  return c;
}
int validPasswordDB(char* user, char* password){
  sem_wait(&semDB);
 shmPointer[TYPEPOS] = ISPASSWORD;
 memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user)+1);
 memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
 while(shmPointer[TYPEPOS] != READY);
 char c = shmPointer[RETURNPOS];
 sem_post(&semDB);
 return c;
}
int getHighScoreDB(char * user){
  sem_wait(&semDB);
  shmPointer[TYPEPOS] = GETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user) + 1);
  while(shmPointer[TYPEPOS] != READY);
  int i = atoi(shmPointer+ RETURNPOS);
  sem_post(&semDB);
  return i;
}
int setHighscoreDB(char* user, int value){
  sem_wait(&semDB);
  shmPointer[TYPEPOS] = SETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT, user, strlen(user) + 1);
  loadInt(shmPointer + SECONDARGUMENT, value);
  while(shmPointer[TYPEPOS] != READY);
  sem_post(&semDB);
  return shmPointer[RETURNPOS];
}
int createUserDB(char* user, char* password){
  sem_wait(&semDB);
  shmPointer[TYPEPOS] = CREATEUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user, strlen(user) + 1);
  memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
  while(shmPointer[TYPEPOS] != READY);
  sem_post(&semDB);
  return shmPointer[RETURNPOS];
}
Player* CreatePlayerStruct(Client *c, int number){
  Player *p = malloc(sizeof(Player));
  p->name = c->name;
  p->score = c->score;
  p->num = number;
  return p;
}
int split (const char *str, char c, char ***arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}
