#include "com.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BUFFER_SIZE 256
//char* srvFifo;
//char* cliFifo;
//int* ids;
/** Aca va la comunicacion, hay que usar de estructura el com.h
    hay que hacer el listen para el server escuche las nuevas conexiones (me parece)
    CREO QUE EL SERVER Y EL CLIENT TIENEN DIFERENTES .C PERO EL .H ES EL MISMO
**/

Connection * c start_ipc(char * addr){
  /*Cargo la configuracion de la conexion del txt ESTO SE HACE EN EL CLIENT SERVER DUH*/
    /*FILE * fp;
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
   */
   /*el server crea el fifo que se va a utilizar para la primera comunicacion*/
     mkfifo(addr,0666);


   if(!access(addr,F_OK)){
     printf("Se pudo crear el FIFO\n");
   } else{
     printf("No se pudo crear el FIFO\n");
   }


}

Connection * accept(Connection * c){
  /* Forkeo el server y le devuelvo la nueva conexion(?)*/
  return c; // por testeo
}
void close_ipc(Connection *c){
  unlink(c->addr);
}

int sendData(Connection* c,StreamData* d){
  int fd = 0;
    //char* cliAux = malloc(sizeof(cliFifo) + 6);
    fd = open(c->addr, O_WRONLY);
    write(fd, d->data,d->size);
    close(fd);
}

void receiveData(Connection* c, StreamData* b){
  int fd = 0;
  fd = open(c->addr, O_RDONLY);
  int a = 0, step = 0;
  while((a = read(fd, b->data + step,BUFFER_SIZE)) != 0){
    step+=a;
  }
  b->size = strlen(b->data);
  close(fd);
}
