#include "server.h"

char* shmPointer;
int main(int argc, const char* argv[]){

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
   (ExistUserDB("kuyum") )?printf("SI existe Kuyum\n"):printf("NO kuyum\n");
   (ExistUserDB("LALALAAAAA") )?printf("SI existe ALALA\n"):printf("NO LALA\n");
   (ExistUserDB("lucas") )?printf("SI existe lucas\n"):printf("NO lucas\n");
   (validPasswordDB("lucas","lucas") )?printf("FUNCIONAA\n"):printf("NO lucas\n");

   printf("Score de Lucas: %d\n",getHighScoreDB("lucas"));
   int aux = getHighScoreDB("lucas") + 1;
   setHighscoreDB("lucas",aux);
   printf("Nuevo Score de Lucas: %d\n",getHighScoreDB("lucas"));

   Connection * selfc = listenConnection(0);
   printf("Estoy esuchando\n");
   while(1){
      StreamData * buffer = malloc(sizeof(StreamData));
      buffer->data = malloc(2048);
      // receiveData(selfc,buffer);
      printf("Accept en %d\n",selfc->fd2);
      Connection *new = acceptConnection(selfc);
      printf("Nueva conexion\n");
      /*receiveData(new,buffer);
      printf("Recibi: %s\n",buffer->data);*/
      /* ACA tendria que haber un marshalling*/
      int joinServer = atoi(buffer->data);
      StreamData *d = malloc(sizeof(StreamData));
      int child = fork();
      if(child == 0){
         d->data = "Soy el hijo y te mande un mensaje";
         printf("%s\n",buffer->data);
         d->size = 34;
         sendData(new, d);
      }else{
         d->data = "Soy el padre y te mande un mensaje";
         d->size = 35;
         sendData(new, d);
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

void setDB(){
  int db = fork();
  if(db == 0){
     printf("Hola soy el hijo\n");
    manageDataBase();
  }
  printf("Soy el padre\n");
}
char ExistUserDB(char * user){
  shmPointer[TYPEPOS] = ISUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user));
  shmPointer[strlen(user) + 1 ] = 0;
  while(shmPointer[TYPEPOS] != READY);
  char c = shmPointer[RETURNPOS];
  return c;
}
int validPasswordDB(char* user, char* password){
 shmPointer[TYPEPOS] = ISPASSWORD;
 memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user)+1);
 memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
 while(shmPointer[TYPEPOS] != READY);
 char c = shmPointer[RETURNPOS];
 return c;
}
int getHighScoreDB(char * user){
  shmPointer[TYPEPOS] = GETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user) + 1);
  while(shmPointer[TYPEPOS] != READY);
  int i = atoi(shmPointer+ RETURNPOS);
  return i;
}
int setHighscoreDB(char* user, int value){
  shmPointer[TYPEPOS] = SETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT, user, strlen(user) + 1);
  loadInt(shmPointer + SECONDARGUMENT, value);
  while(shmPointer[TYPEPOS] != READY);
  return shmPointer[RETURNPOS];
}
int createUserDB(char* user, char* password){
  shmPointer[TYPEPOS] = CREATEUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user, strlen(user) + 1);
  memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
  while(shmPointer[TYPEPOS] != READY);
  return shmPointer[RETURNPOS];
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
