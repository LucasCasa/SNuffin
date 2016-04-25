#ifndef _MARSHALLING_H
#define _MARSHALLING_H
#include <stdio.h>
#include "structs.h"

#define PLAYER '2'
#define STRING '3'
#define POINT '1'
#define BOARD '5'
#define SEP '#' //el separador

void * unmarshalling(char * data, int * type); //devuelve por parametro de que tipo es lo que se mando

char * marshalling(void * struc, int type);

#endif