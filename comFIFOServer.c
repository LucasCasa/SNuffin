#include "com.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BUFFER_SIZE 256
char* srvFifo;
char* cliFifo;
int* ids;
/** Aca va la comunicacion, hay que usar de estructura el com.h
    hay que hacer el listen para el server escuche las nuevas conexiones (me parece)
    CREO QUE EL SERVER Y EL CLIENT TIENEN DIFERENTES .C PERO EL .H ES EL MISMO
**/

void start_ipc(int isSuperServer){
  /*Cargo la configuracion de la conexion del txt*/
    FILE * fp;
    srvFifo = malloc(BUFFER_SIZE);
    cliFifo = malloc(BUFFER_SIZE);
    char* cliaux;
    size_t len = 0;
    ssize_t read;

    fp = fopen("./config", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   fgets(srvFifo,BUFFER_SIZE,fp);
   fgets(cliFifo,BUFFER_SIZE,fp);
   /*el server crea el fifo que se va a utilizar para la primera comunicacion*/
   if(!isSuperServer){
     sprintf(srvFifo, "%s%d",srvFifo, getpid());
   }
     mkfifo(srvFifo,0666);


   if(!access(srvFifo,F_OK)){
     printf("Se pudo acceder al FIFO del server\n");
   } else{
     printf("NO Se pudo acceder al FIFO del server\n");
   }


}


void close_ipc(){
  unlink(cliFifo);
  unlink(srvFifo);
}

int sendData(Connection* c,StreamData* d){
  int fd = 0;
    char* cliAux = malloc(sizeof(cliFifo) + 6);
    sprintf(cliAux,"%s%d",cliFifo,c->id);
    fd = open(cliAux, O_WRONLY);
    write(fd, d->data,d->size);
    close(fd);
    free(cliAux);
}

void receiveData(Connection* c, StreamData* b){
  int fd = 0;
  fd = open(srvFifo, O_RDONLY);
  int a = 0, step = 0;
  while((a = read(fd, b->data + step,BUFFER_SIZE)) != 0){
    step+=a;
  }
  b->size = strlen(b->data);
  close(fd);
}
