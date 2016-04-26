#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "com.h"
#include "db.h"


#define BUFFER_SIZE 1024
int split (const char *str, char c, char ***arr);

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

    while(1){
    StreamData * buffer = malloc(sizeof(buffer));
    buffer->data = malloc(2048);
    receiveData(selfc,buffer);

    if(buffer->data[0] == '@'){
      printf("Lei un accept\n");
      int child = fork();
      if(child == 0){
        printf("Me forkee soy hijo\n");
        char * aux = malloc(sizeof(srvAddr) + 6);
        sprintf(aux,"%s%d",srvAddr,getpid());
        selfc = start_ipc(aux);

      }else{
        Connection *c = malloc(sizeof(Connection));
        c->addr = buffer->data + 1;
        StreamData *daux = malloc(sizeof(StreamData));
        daux->data = malloc(BUFFER_SIZE);
        sprintf(daux->data,"%s%d",srvAddr,getpid());
        daux->size = strlen(daux->data);
        sendData(c,daux);
      }
    }else{
      printf("%s\n",buffer->data);
    }
    }

    close_ipc(selfc);
    printf("Me fui\n");
}

int split (const char *str, char c, char ***arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}
