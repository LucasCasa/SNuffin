#ifndef _CHILDSERVER_H
#define _CHILDSERVER_H
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
#include "log.h"
#include "marshalling.h"
#include <sys/wait.h>
#include "dbManager.h"
#include "serverStructs.h"
#include "comWrapper.h"
#include "game.h"

#define MAX_PLAYERS  4

extern Connection* selfc;
/*Defino lo que puedo llegar a esperar del cliente*/

#define USER            1
#define NEWPASSWORD     5
#define PASSWORD        2
#define MOVEMENT        3
#define READY_TO_PLAY   4

/*End Define*/


/*Defino los estados del cliente*/

#define LOGGING         0
#define WAITING         1
#define READY_PLAY      2
#define PLAYING         3
#define LOOSE           4

/* End Define */

void* listenNewClients(void*);
void initServer(int serverNumber);
void lobby();
void listenAndResolve();
void initGame();
int isgameOver();
void playerLost(int nPlayer);
void sendBoard(GameBoard * gb);
void *listenToMovement(void * condition);
void checkGameStart();
void sendGameStart();

int listenToClients();
void resolveRequest(int nClient);
int validateUser(StreamData * d,int nClient);
int validatePassword(StreamData * d,int nClient);
void notifyNewPlayer(int nPlayer);
void notifyExistingPlayers(int nPlayer);
Player* CreatePlayerStruct(Client *c, int number);
void disconnect(int nClient);


extern Board* b;
#endif
