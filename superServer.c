#include "superServer.h"

//SIN PROBLEMAS DE MEMORIA?

char* shmPointer;
//sem_t semDB;
Connection* selfc;
int servers[MAX_LOBBY];
int listenLocation;

int main(int argc, const char* argv[]){
  printf("Inicializando servidor...\n");

  if(access("./config",F_OK)==-1){  // confing no existe
    printf("No existe archivo config.\n");
    exit(EXIT_FAILURE);
   }

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

   selfc = listenConnection(n); // PROXIMAMENTE LEO CONFIG PASO NUMERO
   printf("Escuchando conexiones...\n");
   StreamData * buffer = malloc(sizeof(StreamData));
   buffer->data = malloc(BUFFER_SIZE);
   while(1){
      Connection *new = acceptConnection(selfc);
      printf("Nueva conexión\n");
      readData(new,buffer);
      unmarshServerId(buffer->data,&joinServer);
      printf("Un cliente se quiere unir al server %d\n", joinServer);
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
            sendData(new,d);
            free(d->data);
            free(d);
            lobby();
            return 0;
        }else{
         servers[joinServer] = child;
         printf("Server nuevo = %d\n",servers[joinServer]);
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
    printf("%s\n",strerror(status));
  }
  printf("Servidor %d fue finalizado",s);

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
    manageDataBase();
  }
}

void connHandler(int sig){
	printf("Cerrando servidor\n");
	closeConn(selfc);
	exit(0);
}
