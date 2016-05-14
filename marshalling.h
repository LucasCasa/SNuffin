#ifndef _MARSHALLING_H
#define _MARSHALLING_H
#include <stdio.h>
#include "structs.h"

#define PLAYER '2'
#define STRING '3'
#define POINT '1'
#define BOARD '5'
#define SERVER_ID '8'
#define BOOLEAN '4'

#define TRUE 1
#define FALSE 0

#define SEP '#' //el separador

int numPlaces (int n) ;

void * unmarshalling(StreamData * data, int * type);

int unmarshString(char * data, char * s );
int unmarshPoint(char * data, Point * p);
int unmarshBoard(char * data, Board * b);
int unmarshServerId(char * data, int * p);
int unmarshPlayer(char * data, Player * p);
int unmarshBoolean(char * data, int * value);

StreamData * marshalling(void * struc, int type);

char * marshPoint(Point * s, int * size);
char * marshPlayer(Player * p, int * size);
char * marshBoard(Board* s, int * size);
char * marshString(char * s, int * size);
char * marshServerId(int * p, int * size);
char * marshBoolean(int value,int * size);

#endif
