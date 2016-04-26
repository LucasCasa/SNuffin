#include "com.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

Connection * self;
#define BUFFER_SIZE 256
//char* srvFifo;
//char* cliFifo;
//int* ids;
/** Aca va la comunicacion, hay que usar de estructura el com.h
    hay que hacer el listen para el server escuche las nuevas conexiones (me parece)
    CREO QUE EL SERVER Y EL CLIENT TIENEN DIFERENTES .C PERO EL .H ES EL MISMO
**/
/* Puede ser que cuando envie datos, antes de enviar el dato en si mande quien es*/

Connection * start_ipc(char * addr){
  /*Cargo la configuracion de la conexion del txt ESTO SE HACE EN EL CLIENT SERVER DUH*/

   /*el server crea el fifo que se va a utilizar para la primera comunicacion*/
     mkfifo(addr,0666);


   if(!access(addr,F_OK)){
     printf("Se pudo crear el FIFO\n");
   } else{
     printf("No se pudo crear el FIFO\n");
   }

   Connection *c = malloc(sizeof(Connection));
   int size = strlen(addr) + 1;
   c->addr = malloc(size);
   memcpy(c->addr,addr,size);
   self = c;
   return c;
}

Connection * accept(Connection * c){
  /* Forkeo el server y le devuelvo la nueva conexion(?)
  Creo que lo que habria que hacer es llamar a la conexion y esperar que te
  devuelva una nueva conexion. Esta funcion solo se llamaria desde el cliente y
  esta pensada para que envie un mensaje predeterminado que el servidor va a enteder
  que es para pasarle el nuevo server.*/
  StreamData *d = malloc(sizeof(StreamData));
  d->data = malloc(BUFFER_SIZE);
  sprintf(d->data,"@%d",getpid());
  d->size = strlen(d->data);
  sendData(c,d);
  StreamData *d2 = malloc(sizeof(StreamData));
  d->data = malloc(BUFFER_SIZE);
  receiveData(self,d2);
  Connection *c2 = malloc(sizeof(Connection));
  c2->addr = d2->data;
  return c2; // por testeo
}
void close_ipc(Connection *c){
  unlink(c->addr);
}

int sendData(Connection* c,StreamData* d){
  int fd = 0;
  printf("Sending data to: %s\n",c->addr);
  printf("Data: %s\n",d->data);
    //char* cliAux = malloc(sizeof(cliFifo) + 6);
    fd = open(c->addr, O_WRONLY);
    write(fd, d->data,d->size);
    close(fd);
}

void receiveData(Connection* c, StreamData* b){
  int fd = 0;
  printf("receiving data in: %s \n",c->addr );
  if(c == NULL){ // NO ESTOY SEGURO DE ESTO
    fd = open(self, O_RDONLY);
  }else{
    fd = open(c->addr, O_RDONLY);
  }

  int a = 0, step = 0;
  /*while((*/a = read(fd, b->data + step,BUFFER_SIZE);/*) != 0){
    step+=a;
  }*/
  printf("STRING: %s\n",b->data);
  printf("SIZE: %d\n",a );
  b->size = strlen(b->data);
  close(fd);
}
