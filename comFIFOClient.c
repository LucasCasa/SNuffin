#include "com.h"
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256
char* srvFifo;
char* cliFifo;
/** Aca va la comunicacion, hay que usar de estructura el com.h
    hay que hacer el listen para el server escuche las nuevas conexiones (me parece)
    CREO QUE EL SERVER Y EL CLIENT TIENEN DIFERENTES .C PERO EL .H ES EL MISMO
**/

void init_ipc(){
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
   mkfifo(srvFifo,0666);
   /*el cliente crea el fifo que se va a usar una vez que la comunicacion fue
   establecida*/
   char* cliaux = malloc(sizeof(cliFifo) + 6);
   sprintf(cliaux, "%s%d",cliFifo, getpid());
   mkfifo(cliaux, 0666);
   free(cliaux);
   if(!access(srvFifo,F_OK)){
     printf("Se pudo acceder al FIFO del server")
   } else{
     printf("NO Se pudo acceder al FIFO del server")
   }


}


void close_ipc(){
  unlink(cliFifo);
  unlink(srvFifo);
}

int sendData(Connection* c,StreamData* d){
    char* cliAux = malloc(sizeof(cliFifo) + 6);
    sprintf(cliAux,"%s%d",cliFifo,c->id);
    fd = open(cliAux, O_WRONLY);
    write(fd, d->data,d->size);
    close(fd);
    free(cliAux);
}

void receiveData(Connection* c, StreamData* b){
  char* cliAux = malloc(sizeof(cliFifo) + 6);
  sprintf(cliAux,"%s%d",cliFifo,c->id);
  fd = open(cliAux, O_WRONLY);
  int a = 0, step = 0;
  while((a = read(fd, b->data + step,b->size)) != 0){
    step+=a;
  }
  b->size = strlen(b->data);
  close(fd);
  free(cliAux);
}
