#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define Q_SRV_NAME "/mq-server"

#define INFO 1
#define WARNING 2
#define ERROR 3

int connectLogServer();
void logMsg(char * msg, int level);
void closeLogServer(); 