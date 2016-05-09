#ifndef _STRUCTS_H
#define _STRUCTS_H

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
	int pNumber;
	Point * dir;
}PlayerPos;

typedef struct{
	int info;
}Integer;

#endif