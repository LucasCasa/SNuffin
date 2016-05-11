#ifndef _SRV_H
#define _SRV_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include "com.h"
#include "db.h"
#include "dbconst.h"
#include "marshalling.h"
#include <sys/select.h>

#define MAX_PLAYERS  4
#define TRUE         1
#define FALSE        0

/*Defino lo que puedo llegar a esperar del cliente*/

#define USER            1
#define PASSWORD        2
#define MOVEMENT        3
#define READY_TO_PLAY   4

/*End Define*/

/*Defino los estados del cliente*/

#define LOGGING         0
#define WAITNG          1
#define READY_PLAY      2
#define PLAYING         3
#define LOOSE           4

/* End Define */


typedef struct Client{
    char* name;
    int state;
    int expecting;
    Connection *con;
}Client;

extern char* shmPointer;

int split (const char *str, char c, char ***arr);


void setDB();
char ExistUserDB(char * user);
int validPasswordDB(char* user, char* password);
int getHighScoreDB(char * user);
int setHighscoreDB(char* user, int value);
int createUserDB(char* user, char* password);
void* startListening(void*);
void lobby();
int listenToClients();
void resolveRequest(int nClient);
void validateUser(StreamData * d);
void validatePassword(StreamData * d);


#endif
