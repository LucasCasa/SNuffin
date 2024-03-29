#ifndef _MARSHALLING_H
#define _MARSHALLING_H
#include <stdio.h>
#include "structs.h"

#define EXIT 0
#define PLAYER '2'
#define STRING '3'
#define POINT '1'
#define BOARD '5'
#define PERDIO '6'
#define TERMINO '7'
#define SERVER_ID '8'
#define BOOLEAN '4'

#define SEP '#' //el separador


extern const int TRUE;
extern const int FALSE;

int numPlaces (int n) ;

int getType(StreamData * data);

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
