#include "com.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#define BUFFER_SIZE 1024

char* ADDR = "/tmp/server";
char* addr1 = NULL;
char* addr2 = NULL;


Connection * listenConnection(int id){
  char* aux = malloc(strlen(ADDR) + 7);
  if(id == 0){
    memcpy(aux,ADDR,strlen(ADDR));
  }else{
    sprintf(aux,"%s%d",ADDR,id);
  }
   mkfifo(aux,0666);
   Connection *c = malloc(sizeof(Connection));
   c->fd = open(aux,0666);
   addr1 = aux;
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
      mkfifo(auxr,0666);
    }
    if(access(auxw,F_OK) != 0){
      mkfifo(auxw,0666); // VER PORQUE NO DEJA O_RDONLY
    }
    int fdw = open(auxw,0666);
    int fdr = open(auxr,0666);
   Connection *c = malloc(sizeof(Connection));

   if(addr[strlen(addr) - 1] == '\n'){
     addr[strlen(addr) -1] = 0;
   }

   char* auxSRV = calloc(BUFFER_SIZE,sizeof(char));
   if(id == 0){
     sprintf(auxSRV, "%s",addr);
   }else{
     sprintf(auxSRV, "%s%d",addr,id);
   }
   c->fd2 = open(auxSRV,0666);

   if(c->fd2 == -1){
     perror("ERROR: ");
     return NULL;
   }

   StreamData *d = malloc(sizeof(StreamData));
   d->data = malloc(BUFFER_SIZE);
   sprintf(d->data,"@%d",getpid());
   d->size = strlen(d->data);
   sendData(c,d);
   c->fd2 = fdw;
   c->fd = fdr;
   free(d->data);
   free(d);
   addr1 = auxr;
   addr2 = auxw;
   free(aux);
   free(auxSRV);
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
  newc->fd2 = fdr; // el fdr es donde el peer va a hablarle al otro peer
  newc->fd = fdw; // el fdw es donde el peer va a escuchar al peer
  /* tendria que tener 2 fd
  o 2 connections*/
  free(d->data);
  free(d);
  free(aux);
  free(auxr);
  free(auxw);
  return newc;
}
void closeConn(Connection *c){
   StreamData *d = malloc(sizeof(StreamData));
   d->data = calloc(1,1);
   d->size = 1;
   sendData(c,d);
   if(c->fd != 0){
      close(c->fd);
   }
   if(c->fd2 != 0){
      close(c->fd2);
   }
   if(addr1 != NULL){
      if(unlink(addr1) == -1){
         perror("unlinking");
      }
      free(addr1);
   }

   if(addr2 != NULL){
      if(unlink(addr2) == -1){
        perror("unlinking");
      }
      free(addr2);
   }
   free(d->data);
   free(d);

}

int sendData(Connection* c,StreamData* d){
  return write(c->fd2, d->data,d->size);
}

void receiveData(Connection* c, StreamData* b){
  int a = -1;
    a = read(c->fd, b->data,BUFFER_SIZE);
    if(a == 256){
      perror("ERROR REC");
    }

  b->size = a;
}
