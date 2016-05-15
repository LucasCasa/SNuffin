#ifndef _DBMANAGER_H
#define _DBMANAGER_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include "dbconst.h"

extern char* shmPointer;
extern sem_t semDB;
char ExistUserDB(char * user);
int validPasswordDB(char* user, char* password);
int getHighScoreDB(char * user);
int setHighscoreDB(char* user, int value);
int createUserDB(char* user, char* password);

#endif
