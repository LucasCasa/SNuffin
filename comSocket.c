#include "com.h"
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


#define MAX_CONNECTIONS 4
#define MAX_CONNECTION_QUEUE 4

void removeEOL(char * str);

struct sockaddr_in my_addr;
//int peerLimit;
//int * peers;
socklen_t addr_size;


void closePeer(int);

Connection * connectToPeer(char* addr, int port){
	
	struct hostent *server;

	Connection * c = malloc(sizeof(Connection));

	/* Create a socket point */
	c->fd = socket(AF_INET, SOCK_STREAM, 0);

	if (c->fd < 0) {
	  perror("ERROR opening socket");
	  exit(1);
	}

	removeEOL(addr);
	server = gethostbyname(addr);

	if (server == NULL) {
	  perror("No such host");
	  exit(1);
	}

	bzero((char *) &my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&my_addr.sin_addr.s_addr, server->h_length);
	my_addr.sin_port = htons(port);

	/* Now connect to the server */
	if (connect(c->fd, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
	  perror("ERROR connecting");
	  exit(1);
	}
	return c;
}


Connection * listenConnection(int port){
	
	printf("Starting server...\n");

    Connection * c = malloc(sizeof(Connection));

	// The socket fd
	c->fd = socket(AF_INET,SOCK_STREAM,0);

	if(c->fd==-1){
		perror("socket");
		exit(1);
	}

	// Reserves memory for the structurelisten
	memset(&my_addr,0,sizeof(struct sockaddr_in));

	// Declare the type of connnection
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(port);

	if (bind(c->fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1){
		perror("bind");
		exit(1);
	}

	// Marks sfd as a passive sockdet.  MAX_CONNECTION_QUEUE is the max n° of pending connections
	printf("Listening on port %d...\n", port);
	if (listen(c->fd, MAX_CONNECTION_QUEUE) == -1){
		perror("listen");
		exit(1);
	}

	printf("Waiting for connections...\n");
  	addr_size = sizeof(struct sockaddr_in);
	return c;
}

Connection * acceptConnection(Connection * c){

	Connection * newConn = malloc(sizeof(Connection));

	if ((newConn->fd = accept(c->fd, (struct sockaddr *)&my_addr, (socklen_t*) &addr_size))<0){
 		perror("accept");
 		exit(1);
	}

	return newConn;
}

void closeConn(Connection * c){
	close(c->fd);
}

int sendData(Connection * connection, StreamData * req){
   /* Send message to the server */
   int n = send(connection->fd, req->data, req->size,MSG_NOSIGNAL);

   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   return 1;
}

void receiveData(Connection * connection, StreamData * buffer){
	int ans;

   	ans=recv(connection->fd,buffer->data,BUFFER_SIZE,0);
   	if(ans==-1){
  	    perror("receive");
   	}

   	buffer->size = ans;
}

void removeEOL(char * str){
	for(int i=0; str[i]!=0; i++){
		if(str[i]=='\n')
			str[i]=0;
	}
}
