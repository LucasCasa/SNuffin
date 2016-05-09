#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "com.h"
#include "db.h"
#include "dbconst.h"

#define MAX_LOBBY 24

extern char* shmPointer;

int split (const char *str, char c, char ***arr);


void setDB();
char ExistUserDB(char * user);
int validPasswordDB(char* user, char* password);
int getHighScoreDB(char * user);
int setHighscoreDB(char* user, int value);
int createUserDB(char* user, char* password);
