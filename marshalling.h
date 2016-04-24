#ifndef MARSHALLING_H
#define MARSHALLING_H
#include <stdio.h>

#define PLAYER '2'
#define STRING '3'
#define POINT '1'
#define BOARD '5'
#define SEP '#' //el separador


typedef struct {
	int x;
	int y;
}Point;

typedef struct {
	char ** board;
	int filas;
	int columnas;
}Board;

typedef struct {
	char * string;
	int size;
}String;

typedef struct{
	int pNumber;
	Point * dir;
}PlayerPos;

void * unmarshalling(char * data, int * type); //devuelve por parametro de que tipo es lo que se mando

char * marshalling(void * struc, int type);

#endif