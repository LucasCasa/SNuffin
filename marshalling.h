#ifndef _MARSHALLING_H
#define _MARSHALLING_H
#include <stdio.h>
#include "structs.h"

#define PLAYER '2'
#define STRING '3'
#define POINT '1'
#define BOARD '5'
#define SERVER_ID '8'

#define STRING_N STRING - '0'
#define POINT_N POINT - '0'
#define BOARD_N BOARD - '0'
#define PLAYER_N PLAYER - '0'

#define SEP '#' //el separador

void * unmarshalling(char * data, int * type); //devuelve por parametro de que tipo es lo que se mando y el void * con la estructura


// el marshalling recibe un puntero a void con la estructura, un type con que tipo de estructura es.Devuelve un streamData
StreamData * marshalling(void * struc, int type);

#endif