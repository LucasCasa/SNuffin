#ifndef _SERVER_H
#define _SERVER_H
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <string.h>
	#include <sys/un.h>
	#include <netdb.h>
	#include <netinet/in.h>
    #include <arpa/inet.h>

	//#include "db.h"
	
	#define TRUE 1
	#define FALSE 0
	#define MAX_CLIENTS 10
	#define BUFFER_LENGTH 256
	#define MAX_CONNECTION_QUEUE 50
	#define MY_SOCK_PORT 42096
	#define MAX_PLAYERS 4

	#define handle_error(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)

	int handle_request(int,char*);

#endif