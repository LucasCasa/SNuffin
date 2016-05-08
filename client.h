#ifndef _CLIENT_H
#define _CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>

#include <string.h>

#include <netdb.h>
#include <netinet/in.h>

#include "structs.h"

#include "com.h"
#include "marshalling.h"


#define UP_ARROW (char)'A' 
#define LEFT_ARROW (char)'D'
#define RIGHT_ARROW (char)'C'
#define DOWN_ARROW (char) 'B'
#define BACKSPACE 127

#define PLAYER1_COLOR   "\x1b[31;41m"
#define PLAYER2_COLOR   "\x1b[32;42m"
#define PLAYER3_COLOR   "\x1b[34;44m"
#define PLAYER4_COLOR   "\x1b[35;45m"
#define COLOR_RESET     "\x1b[0m"

#endif
