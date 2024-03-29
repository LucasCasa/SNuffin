#ifndef _DB_H
#define _DB_H
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "dbconst.h"

extern char* shmPointer;

int isUser(char* user);
int isPassword(char* user, char * pass);
int createUser(char * user, char * pass);
int getHighscore(char * player);
int setHighscore(char * player,int highscore);
int init();
int manageDataBase();
int close2();
int cmp(char* a,char*b);
//static int callback(void *NotUsed, int argc, char **argv, char **azColName);
int printError(int rv);
void loadInt(char* pos, int value);
#endif
