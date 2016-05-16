#ifndef _COMWRAPPER_H
#define _COMWRAPPER_H

#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "com.h"

#define BLOCK 5

void readData(Connection * conn, StreamData * sd);

#endif