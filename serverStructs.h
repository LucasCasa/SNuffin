#ifndef _SERVERSTRUCS_H
#define _SERVERSTRUCS_H

typedef struct Client{
    char* name;
    int state;
    int expecting;
    int score;
    Connection *con;
}Client;

typedef struct Server{
   int pid;
   int number;
}Server;
/*
typedef struct{
	char * name;
	int num;
	int score;
}Player;
*/
#endif
