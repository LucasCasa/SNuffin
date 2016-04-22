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

	#include "db.h"

	
	#define TRUE 1
	#define FALSE 0
	#define MAX_CLIENTS 50
	#define BUFFER_LENGTH 256
	#define MAX_CONNECTION_QUEUE 50
	#define MY_SOCK_PORT 8080

	#define handle_error(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)

	void handle_request(int,char*);

#endif