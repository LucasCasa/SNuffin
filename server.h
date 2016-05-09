#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "com.h"
#include "db.h"

extern char* shmPointer;

int split (const char *str, char c, char ***arr);
void setDB();
