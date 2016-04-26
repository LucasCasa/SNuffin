#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "com.h"

#define BUFFER_SIZE 1024


int main(int argc, const char* argv[]){

    FILE * fp;
    char * srvAddr = malloc(BUFFER_SIZE);
    char * cliAddr = malloc(BUFFER_SIZE);
    fp = fopen("./configFIFO", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   fgets(srvAddr,BUFFER_SIZE,fp);
   fgets(cliAddr,BUFFER_SIZE,fp);
   fprintf(cliAddr,"%s%d",cliAddr,getpid());

    Connection * selfc = start_ipc(cliAddr);
    Connection * server = malloc(sizeof(Connection));
    server->addr = srvAddr;
    StreamData * buffer = malloc(sizeof(buffer));
    buffer->data = "Probando";
    buffer->size = 9;
    sendData(server,buffer);
    Connection * fserver = accept(server);
    StreamData * buffer2 = malloc(sizeof(buffer));
    buffer->data = "Probando con el hijo";
    buffer->size = 21;
    sendData(fserver,buffer2);
    close_ipc(selfc);
    printf("Me fui\n");
}
