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
#include "log.h"
#include "marshalling.h"
#include <sys/select.h>
#include <sys/wait.h>

#define MAX_PLAYERS  4
#define MAX_LOBBY    10


/*Defino lo que puedo llegar a esperar del cliente*/

#define USER            1
#define PASSWORD        2
#define MOVEMENT        3
#define READY_TO_PLAY   4

/*End Define*/
extern const int TRUE;
extern const int FALSE;
/*Defino los estados del cliente*/

#define LOGGING         0
#define WAITING         1
#define READY_PLAY      2
#define PLAYING         3
#define LOOSE           4

/* End Define */


typedef struct Client{
    char* name;
    int state;
    int expecting;
    int score;
    Connection *con;
}Client;

typedef struct Server{
   int pid;
   int number;
}Server;

extern char* shmPointer;

int split (const char *str, char c, char ***arr);


void setDB();
char ExistUserDB(char * user);
int validPasswordDB(char* user, char* password);
int getHighScoreDB(char * user);
int setHighscoreDB(char* user, int value);
int createUserDB(char* user, char* password);
void* listenNewClients(void*);
void initServer(int serverNumber);
void lobby();
int listenToClients();
void resolveRequest(int nClient);
void validateUser(StreamData * d,int nClient);
int validatePassword(StreamData * d,int nClient);
void notifyNewPlayer(int nPlayer);
Player* CreatePlayerStruct(Client *c, int number);
void * waitForChild(void* pid);


#endif
