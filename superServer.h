#ifndef _SRV_H
#define _SRV_H


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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
#include "log.h"
#include "marshalling.h"
#include "childServer.h"
#include "serverStructs.h"
#include "dbconst.h"
#include <sys/select.h>
#include <sys/wait.h>

#define MAX_LOBBY    10





extern const int TRUE;
extern const int FALSE;



extern char* shmPointer;


void setDB();
void * waitForChild(void* pid);


#endif
