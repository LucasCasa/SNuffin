#include "com.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 256


int fdr;
int fdw;
char* ADDR = "/tmp/server";
//char* srvFifo;
//char* cliFifo;
//int* ids;
/** Aca va la comunicacion, hay que usar de estructura el com.h
    hay que hacer el listen para el server escuche las nuevas conexiones (me parece)
    CREO QUE EL SERVER Y EL CLIENT TIENEN DIFERENTES .C PERO EL .H ES EL MISMO
**/
/* Puede ser que cuando envie datos, antes de enviar el dato en si mande quien es*/

Connection * listenConnection(int id){
  char* aux = malloc(strlen(ADDR) + 7);
  printf("Malloquee\n");
  if(id == 0){
    memcpy(aux,ADDR,strlen(ADDR));
  }else{
    sprintf(aux,"%s%d",ADDR,id);
  }
  printf("Setee ADDr\n");
   mkfifo(aux,0666);
   printf("Hice FIFO, %s\n",aux);
   Connection *c = malloc(sizeof(Connection));
   c->fd2 = open(aux,0666);
   printf("Setee fd %d\n",c->fd2);
   free(aux);
   printf("libero aux\n");
   return c;
}
Connection * connectToPeer(char * addr,int id){
  /*Cargo la configuracion de la conexion del txt ESTO SE HACE EN EL CLIENT SERVER DUH*/

   /*el server crea el fifo que se va a utilizar para la primera comunicacion*/
    char* aux = malloc(BUFFER_SIZE);
    char* auxr = malloc(BUFFER_SIZE);
    char* auxw = malloc(BUFFER_SIZE);
    sprintf(aux,"/tmp/client%d",getpid());
    sprintf(auxr,"%sr",aux);
    sprintf(auxw,"%sw",aux);
    if(access(auxr,F_OK) != 0){
      int a = mkfifo(auxr,0666);
      printf("Cree el fifo read %d\n",a );
    }
    if(access(auxw,F_OK) != 0){
      int a = mkfifo(auxw,0666); // VER PORQUE NO DEJA O_RDONLY
      printf("Cree el fifo write %d\n",a );
    }
    printf("Hola\n");
    fdw = open(auxw,0666);
    fdr = open(auxr,0666);
    printf("CHAU\n");
   Connection *c = malloc(sizeof(Connection));
   /*int size = strlen(addr) + 1;
   c->addr = malloc(size);
   memcpy(c->addr,addr,size);*/
   addr[strlen(addr) -1] = 0;
   printf("Abriendo %s\n",addr );
   printf("%d\n",access(addr,F_OK) );
   c->fd = open(addr,0666);
   if(c->fd == -1){
     perror("ERROR: ");
   }
   printf("Abierto\n");
   StreamData *d = malloc(sizeof(StreamData));
   d->data = malloc(BUFFER_SIZE);
   sprintf(d->data,"@%d",getpid());
   d->size = strlen(d->data);
   printf("Mando inicio de conexio a: %d, %s\n",c->fd,addr);
   sendData(c,d);
   printf("Mandando\n");
   free(d->data);
   free(d);
   free(auxr);
   free(auxw);
   free(aux);
   return c;

}

Connection * acceptConnection(Connection * c){
  /* Forkeo el server y le devuelvo la nueva conexion(?)
  Creo que lo que habria que hacer es llamar a la conexion y esperar que te
  devuelva una nueva conexion. Esta funcion solo se llamaria desde el cliente y
  esta pensada para que envie un mensaje predeterminado que el servidor va a enteder
  que es para pasarle el nuevo server.*/
  StreamData *d = malloc(sizeof(StreamData));
  d->data = malloc(BUFFER_SIZE);
  receiveData(c,d);
  Connection * newc = malloc(sizeof(Connection));
  int id  = 0;
  if(d->data[0] == '@'){
    id = atoi(d->data + 1);
  }
  char* aux = malloc(BUFFER_SIZE);
  char* auxr = malloc(BUFFER_SIZE);
  char* auxw = malloc(BUFFER_SIZE);
  sprintf(aux,"/tmp/client%d",id);
  sprintf(auxr,"%sr",aux);
  sprintf(auxw,"%sw",aux);
  int fdw = open(auxw,0666);
  int fdr = open(auxr,0666);
  newc->fd = fdr; // el fdr es donde el peer va a hablarle al otro peer
  newc->fd2 = fdw; // el fdw es donde el peer va a escuchar al peer
  /* tendria que tener 2 fd
  o 2 connections*/

  return newc; // por testeo
}
void closeComm(Connection *c){
  close(c->fd);

}

int sendData(Connection* c,StreamData* d){
  printf("Sending data to: %d\n",c->fd);
  printf("Data: %s\n",d->data);
  return write(c->fd, d->data,d->size);
}

void receiveData(Connection* c, StreamData* b){
  int a = 0;
  if(c->fd2 == 0){
    a = read(fdr, b->data,BUFFER_SIZE);
  }else{
    a = read(c->fd2, b->data,BUFFER_SIZE);
  }
  printf("STRING: %s\n",b->data);
  printf("SIZE: %d\n",a );
  b->size = strlen(b->data);
}
