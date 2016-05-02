#ifndef _MARSHALLING_H
#define _MARSHALLING_H
#include <stdio.h>
#include "structs.h"

#define PLAYER '2'
#define STRING '3'
#define POINT '1'
#define BOARD '5'
#define SEP '#' //el separador

void * unmarshalling(char * data, int * type); //devuelve por parametro de que tipo es lo que se mando y el void * con la estructura


// el marshalling recibe un puntero a void con la estructura, un type con que tipo de estructura es. Devuelve el char * y por
//parámetro el tamaño del char *.
char * marshalling(void * struc, int type, int * size);

#endif