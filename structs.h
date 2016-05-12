#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "com.h"

typedef struct {
	int x;
	int y;
}Point;

typedef struct {
	char ** board;
	int rows;
	int columns;
}Board;

typedef struct {
	char * string;
	int size;
}String;

typedef struct{
	int info;
}Integer;

typedef struct{
	char * name;
	int num;
	int score;
}Player;

#endif