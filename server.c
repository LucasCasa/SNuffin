#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "com.h"
#include "db.h"

#define BUFFER_SIZE 1024

int main(int argc, const char* argv[]){

    int db = fork();
    if(db == 0){
      manageDataBase();
    }
    FILE * fp;
    char * srvAddr = malloc(BUFFER_SIZE);

    fp = fopen("./configFIFO", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   fgets(srvAddr,BUFFER_SIZE,fp);


    Connection * selfc = start_ipc(srvAddr);
    StreamData * buffer = malloc(sizeof(buffer));
    buffer->data = malloc(2048);
    receiveData(selfc,buffer);
    printf("%s\n",buffer->data);
    close_ipc(selfc);
    printf("Me fui\n");
}
