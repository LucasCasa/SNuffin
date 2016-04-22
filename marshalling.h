#include <stdio.h>

typedef struct {
	int x;
	int y;
}Point;

typedef struct {
	char ** tablero;
	int filas;
	int columnas;
}Board;

typedef struct {
	char * string;
	int size;
}String;


#define STRING '3'
#define POINT '1'
#define BOARD '5'
#define SEP '#' //el separador

void * unmarshalling(char * data);

char * marshalling(void * struc, int type);