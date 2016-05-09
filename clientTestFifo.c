#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "com.h"


int main(int argc, const char* argv[]){

    FILE * fp;
    char * srvAddr = malloc(BUFFER_SIZE);
    char * cliAddr = malloc(BUFFER_SIZE);
    fp = fopen("./configFIFO", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   fgets(srvAddr,BUFFER_SIZE,fp);
   fgets(cliAddr,BUFFER_SIZE,fp);
   printf("%s\n",srvAddr);
   Connection *c = connectToPeer(srvAddr,0);
   int i = 0;
   printf("Abri la conexion,Esperando Datos\n");
   /*StreamData *d  = malloc(sizeof(StreamData));
   d->data = "1";
   d->size = 2;
   sendData(c,d);*/
  while(i < 2){
     StreamData *b = malloc(sizeof(StreamData));
    b->data = malloc(256);
    printf("EN el WHILE\n");
    receiveData(c,b);
    i++;
    printf("%s\n",b->data);
  }
    printf("Me fui\n");
}
