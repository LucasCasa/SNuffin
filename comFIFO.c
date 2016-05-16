#include "com.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

//SIN PROBLEMAS DE MEMORIA?

#define BUFFER_SIZE 1024

char* ADDR = "/tmp/server";
char* addr1 = NULL;
char* addr2 = NULL;


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
   c->fd = open(aux,0666);
   printf("Setee fd %d\n",c->fd2);
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
      //printf("Cree el fifo read %d\n",a );
    }
    if(access(auxw,F_OK) != 0){
      mkfifo(auxw,0666); // VER PORQUE NO DEJA O_RDONLY
      //printf("Cree el fifo write %d\n",a );
    }
    int fdw = open(auxw,0666);
    int fdr = open(auxr,0666);
   Connection *c = malloc(sizeof(Connection));
   /*int size = strlen(addr) + 1;
   c->addr = malloc(size);
   memcpy(c->addr,addr,size);*/
   if(addr[strlen(addr) - 1] == '\n'){
     addr[strlen(addr) -1] = 0;
   }
   //printf("Server Address %s\n",addr);
   char* auxSRV = calloc(BUFFER_SIZE,sizeof(char));
   if(id == 0){
     sprintf(auxSRV, "%s",addr);
   }else{
     sprintf(auxSRV, "%s%d",addr,id);
   }
   //printf("Abriendo %s%d\n",addr,id );
   printf("%d\n",access(auxSRV,F_OK) );
   c->fd2 = open(auxSRV,0666);
   //printf("fd :%d \n",c->fd);
   if(c->fd2 == -1){
     perror("ERROR: ");
   }
   //printf("Abierto\n");
   StreamData *d = malloc(sizeof(StreamData));
   d->data = malloc(BUFFER_SIZE);
   sprintf(d->data,"@%d",getpid());
   d->size = strlen(d->data);
   //printf("Mando inicio de conexio a: %d, %s\n",c->fd,auxSRV);
   sendData(c,d);
   //printf("Mandando\n");
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
  //printf("Nueva conexion: %d\n", getpid());
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
  return newc; // por testeo
}
void closeConn(Connection *c){
   StreamData *d = malloc(sizeof(StreamData));
   d->data = calloc(1,1);
   d->size = 1;
   //printf("Cierro la conexion con fd=%d y fd2=%d\n",c->fd,c->fd2);
   sendData(c,d);
   if(c->fd != 0){
      close(c->fd);
   }
   if(c->fd2 != 0){
      close(c->fd2);
   }
   if(addr1 != NULL){
      if(unlink(addr1) == -1){
         //printf("ERROR UNLINKING\n");
      }else{
         //printf("NO ERROR UNLINKING\n");
      }
      free(addr1);
   }else{
      //printf("ADDR1 is NULL\n");
   }

   if(addr2 != NULL){
      if(unlink(addr2) == -1){
         //printf("ERROR UNLINKING\n");
      }else{
         //printf("NO ERROR UNLINKING\n");
      }
      free(addr2);
   }else{
      //printf("ADDR2 is NULL\n");
   }
   free(d->data);
   free(d);

}

int sendData(Connection* c,StreamData* d){
  printf("Sending data to: %d\n",c->fd2);
  printf("Data: %s\n",d->data);
  return write(c->fd2, d->data,d->size);
}

void receiveData(Connection* c, StreamData* b){
  int a = -1;
    a = read(c->fd, b->data,BUFFER_SIZE);
    if(a == 256){
      perror("ERROR REC");
    }
    //printf("Readed %d, String size %d\n",a,strlen(b->data));
   //  for(int i = 0; i<a-2;i++){
   //    if(b->data[i] == 0){
   //       b->data[i] = '/';
   // }
  //}
  //printf("Received\n");
  //printf("STRING: %s\n",b->data);
  //printf("SIZE: %d\n",a );
  b->size = a;
}
