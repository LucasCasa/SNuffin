#include "superServer.h"

char* shmPointer;
//sem_t semDB;
int servers[MAX_LOBBY];
int listenLocation;

int main(int argc, const char* argv[]){
    /*set de db semaphore*/
    printf("INIT\n");
    servers[0] = 55;
    servers[1] = 0;
    servers[2] = 0;
    servers[3] = 0; //MAS
    int joinServer = 0;

    //sem_init(&semDB,1,1);
   /* map into memory */
   shmPointer = mmap(NULL,SH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
   for(int i = 0; i<SH_SIZE;i++){
      shmPointer[i] = 0;
   }
   pthread_mutexattr_t attr;
   pthread_condattr_t attrc;
   pthread_condattr_init(&attrc);
   pthread_mutexattr_init (&attr);
   pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
   pthread_condattr_setpshared(&attrc,PTHREAD_PROCESS_SHARED);
   pthread_mutex_init((pthread_mutex_t*)(shmPointer + MUTEX_OFFSET), &attr);
   pthread_cond_init((pthread_cond_t*)(shmPointer + COND_OFFSET), &attrc);
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
   Connection * selfc = listenConnection(n); // PROXIMAMENTE LEO CONFIG PASO NUMERO
   while(1){
      for(int i = 0; i<MAX_LOBBY;i++){
         printf("Soy %d,  Server %d es %d\n",getpid(),i,servers[i]);
      }
      StreamData * buffer = malloc(sizeof(StreamData));
      buffer->data = malloc(2048);
      // receiveData(selfc,buffer);
      printf("Accept en %d\n",selfc->fd2);
      Connection *new = acceptConnection(selfc);
      printf("Nueva conexion\n");
      receiveData(new,buffer);
      printf("Recibi: %s\n",buffer->data);

      unmarshServerId(buffer->data,&joinServer);

      printf("Client wants to join server %d\n", joinServer);
      for(int i = 0; i<MAX_LOBBY;i++){
         printf("Soy %d,  Server %d es %d\n",getpid(),i,servers[i]);
      }
      if(servers[joinServer] != 0){
         printf("Server ya existe\n");
         int newDirection = n + joinServer;
        sendData(new,marshalling(&newDirection, SERVER_ID));
      }else{
        int child = fork();
        int pipes[2];
        pipe(pipes);
        printf("\t\tMe Forkee\n");
        if(child == 0){
            initServer(n + joinServer);
            int newDirection = n + joinServer;
            StreamData *d = marshalling(&newDirection,SERVER_ID);
            printf("Envio data: %s \n", d->data);
            sendData(new,d); //HORRIBLE
            lobby();
            //startGame();
            closeLogServer();
            logMsg("vuelvo de init");
            return 0;
        }else{
         servers[joinServer] = child;
         printf("\t\t\tServer nuevo = %d\n",servers[joinServer]);
         pthread_t waitChild;
         pthread_create(&waitChild,NULL,waitForChild,&child);
      }
    }
      // if(buffer->data[0] == '@'){
      //   printf("Lei un accept\n");
      //   int child = fork();
      //   if(child == 0){){
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

void * waitForChild(void* pid){
  int s = *(int*) pid;
  int status = 0;
  waitpid(s,&status,0);
  printf("%s",strerror(status));
  printf("Servidor %d fue finalizado\n",s);

  for(int i = 0 ; i<MAX_LOBBY;i++){
    if(servers[i] == s){
      servers[i] = 0;
    }
  }
}

void setDB(){
  int db = fork();
  if(db == 0){
     printf("Hola soy el hijoDB\n");
    manageDataBase();
  }
  printf("Soy el padre\n");
}


/*
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
*/
