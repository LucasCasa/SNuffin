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

typedef struct{
	char * name;
	int num;
	int score;
	int ready;
}Player;

#endif