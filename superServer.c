#include "superServer.h"

//SIN PROBLEMAS DE MEMORIA?

char* shmPointer;
//sem_t semDB;
Connection* selfc;
int servers[MAX_LOBBY];
int listenLocation;

int main(int argc, const char* argv[]){
    connectLogServer();
    logMsg("INIT SERVER");
    int joinServer = 0;
   /* map into memory */
   shmPointer = mmap(NULL,SH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
   for(int i = 0; i<SH_SIZE;i++){
      shmPointer[i] = 0;
   }
   signal(SIGINT, connHandler);
   pthread_mutexattr_t attr;
   pthread_condattr_t attrc;
   pthread_condattr_init(&attrc);
   pthread_mutexattr_init (&attr);
   pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
   pthread_condattr_setpshared(&attrc,PTHREAD_PROCESS_SHARED);
   pthread_mutex_init((pthread_mutex_t*)(shmPointer + MUTEX_OFFSET), &attr);
   pthread_cond_init((pthread_cond_t*)(shmPointer + COND_OFFSET), &attrc);
   pthread_cond_init((pthread_cond_t*)(shmPointer + COND2_OFFSET), &attrc);
   setDB();
   FILE * fp;
   char * srvAddr = malloc(BUFFER_SIZE);

   fp = fopen("./config", "r");
   if (fp == NULL)
      exit(EXIT_FAILURE);

   fgets(srvAddr,BUFFER_SIZE,fp);
   int n = 0;
   fscanf(fp,"%d",&n);
   listenLocation = n;

  /* EN UN FUTURO HAY QUE HACER ESTO BIEN, ESTO ES SOLO PARA TEST
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
   selfc = listenConnection(n); // PROXIMAMENTE LEO CONFIG PASO NUMERO
   StreamData * buffer = malloc(sizeof(StreamData));
   buffer->data = malloc(BUFFER_SIZE);
   while(1){
      Connection *new = acceptConnection(selfc);
      logMsg("Nueva conexion");
      readData(new,buffer);
      char* aux = malloc(BUFFER_SIZE);
      sprintf(aux,"Recibi: %s",buffer->data);
      logMsg(aux);

      unmarshServerId(buffer->data,&joinServer);

      sprintf(aux,"Client wants to join server %d", joinServer);
      logMsg(aux);
      free(aux);
      if(servers[joinServer] != 0){
         int newDirection = n + joinServer;
         StreamData * d = marshalling(&newDirection, SERVER_ID);
        sendData(new,d);
        free(d->data);
        free(d);
      }else{
        int child = fork();
        int pipes[2];
        pipe(pipes);
        if(child == 0){
            initServer(n + joinServer);
            int newDirection = n + joinServer;
            StreamData *d = marshalling(&newDirection,SERVER_ID);
            char* aux = malloc(BUFFER_SIZE);
            sprintf(aux,"Envio data: %s", d->data);
            logMsg(aux);
            free(aux);
            sendData(new,d); //HORRIBLE
            free(d->data);
            free(d);
            lobby();
            //startGame();
            logMsg("vuelvo de init");
            closeLogServer();
            return 0;
        }else{
         servers[joinServer] = child;
         char* aux = malloc(BUFFER_SIZE);
         sprintf(aux,"Server nuevo = %d",servers[joinServer]);
         logMsg(aux);
         free(aux);
         pthread_t waitChild;
         pthread_create(&waitChild,NULL,waitForChild,&child);
      }
    }
   }
   free(buffer->data);
   free(buffer);
   free(selfc);
   printf("Me fui\n");
   return 0;
}

void * waitForChild(void* pid){
  int s = *(int*) pid;
  int status = 0;
  waitpid(s,&status,0);
  if(status != 0){
  logError(strerror(status));
  }
  char* aux = malloc(BUFFER_SIZE);
  sprintf(aux,"Servidor %d fue finalizado",s);
  logWarning(aux);

  for(int i = 0 ; i<MAX_LOBBY;i++){
    if(servers[i] == s){
      servers[i] = 0;
    }
  }
  return NULL;
}

void setDB(){
  int db = fork();
  if(db == 0){
     printf("Hola soy el hijoDB\n");
    manageDataBase();
  }
  printf("Soy el padre\n");
}

void connHandler(int sig){
	printf("Me fui\n");
	closeConn(selfc);
	exit(0);
}
