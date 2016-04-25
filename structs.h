#ifndef _STRUCTS_H
#define _STRUCTS_H

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

#endif