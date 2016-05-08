#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "com.h"
//include "db.h"


#define BUFFER_SIZE 1024
int split (const char *str, char c, char ***arr);

int main(int argc, const char* argv[]){

    int pipeDB[2] = setDB(); //Devuelvo 2 pipes, uno donde se escribe y otro donde se lee
                                        //Posicion 0 escribe, posiocion 1 lee
    /*FILE * fp;
    char * srvAddr = malloc(BUFFER_SIZE);

    fp = fopen("./configFIFO", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   fgets(srvAddr,BUFFER_SIZE,fp);

   EN UN FUTURO HAY QUE HACER ESTO BIEN, ESTO ES SOLO PARA TEST
    */
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

int setDB(){
  int pipe1[2];
  int pipe2[2];
  if (pipe(pipe1) || pipe(pipe2)){
    fprintf (stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }
  int db = fork();
  if(db == 0){
    close(pipe1[1]);
    close(pipe2[0]);
    manageDataBase(pipe1[0],pipe2[1]);
  }
  close(pipe1[0]);
  close(pipe2[1]);
  int p[2];
  p[0] = pipe1[1];
  p[1] = pipe2[0];
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
