#ifndef _CLIENT_H
#define _CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>

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

#define BUFFER 256
#define ARCHIVO "./config"

#define MAX_WORD 20

#define PLAYER1_COLOR   "\x1b[31;41m"
#define PLAYER2_COLOR   "\x1b[32;42m"
#define PLAYER3_COLOR   "\x1b[34;44m"
#define PLAYER4_COLOR   "\x1b[35;45m"
#define COLOR_RESET     "\x1b[0m"



void changeMode(int dir);
int kbhit (void);

void freeBoard(Board * b);
void initializeBoard(Board * b);

void printBoard(Board *b);
void printPlayerColor(int pNum);

void game(int slot);
void startGame();

void connHandler(int sig);

void prepareLobby();

void getInformation();
void getPass(char * pass);
void getName(char * name);

#endif
