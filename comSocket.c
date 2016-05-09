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

struct sockaddr_in my_addr;
//int peerLimit;
//int * peers;
socklen_t addr_size;


void closePeer(int);

Connection * connectToPeer(char* addr, int port){
	int n;
	struct hostent *server;

	Connection * c = malloc(sizeof(Connection));

	/* Create a socket point */
	c->fd = socket(AF_INET, SOCK_STREAM, 0);

	if (c->fd < 0) {
	  perror("ERROR opening socket");
	  exit(1);
	}

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
	int i;
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

  	/*peers = malloc(MAX_CONNECTIONS*sizeof(int));
    peerLimit = MAX_CONNECTIONS;

    // Set peers on 0
  	for(i=0; i<peerLimit; i++)
    	peers[i] = 0;*/
	return c;
}

Connection * acceptConnection(Connection * c){

	Connection * newConn = malloc(sizeof(Connection));

	if ((newConn->fd = accept(c->fd, (struct sockaddr *)&my_addr, (socklen_t*) &addr_size))<0){
 		perror("accept");
 		exit(1);
	}

	return newConn;

	/*int max_sd, space_available, i, sd, ans, ans_select;
	fd_set readfds;

	FD_ZERO(&readfds);
	//add master socket to set
	FD_SET(c->fd, &readfds);
	max_sd = c->fd;
	//add all other
	for(i=0;i<peerLimit;i++){
		if(peers[i]!=0){
			FD_SET(peers[i], &readfds);
			if(peers[i]>max_sd)
		  		max_sd = peers[i];
		}
	}
	struct timeval tv = {1, 0}; //the timeout (s,ms)
	//wait for an activity on one of the sockets
	ans_select = select( max_sd + 1 , &readfds , NULL , NULL , &tv);
	if (ans_select < 0){
		perror("select");
		exit(1);
	}
	//If its server_sfd then its an INCOMING CONNECTION
	if (FD_ISSET(c->fd, &readfds)){
		if ((sd = accept(c->fd, (struct sockaddr *)&my_addr, (socklen_t*) &addr_size))<0){
	 		perror("accept");
	 		exit(1);
		}
		printf("New connection\n");
		//add new socket to array of sockets
		space_available = FALSE;
		for (i = 0; i < peerLimit && !space_available; i++){
	  	//if position is empty use it
	  	if( peers[i] == 0 ){
	    	peers[i] = sd;
	    	space_available = TRUE;
	  	}
	}
	if(!space_available)
	  printf("Server is full.\n");
	}//else{
		//else its an operation for an existing socket
		for (i = 0; i < peerLimit; i++){
			sd = peers[i];
			printf("sd=%d\n",sd);

			if (sd!=0 && FD_ISSET(sd, &readfds)){
				printf("isset\n");
				Connection * conn = malloc(sizeof(Connection));
				conn->fd = sd;
				return conn; 
			}
		}
	//}
	return NULL;*/
}

void closeConn(Connection * c){
	close(c->fd);
	//free(peers);
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
   	// if 0 then peer closed connection
  	/*else if(ans==0){
  		closePeer(connection->fd);
  	}*/
   	buffer->size = ans;
}

/*void closePeer(int fd){
	int i;
	for(i=0; i<peerLimit; i++){
		if(peers[i]==fd){
			peers[i] = 0;
			return;
		}
	}
}*/

// Main for client

int main(){
	Connection * self = connectToPeer("localhost",8080);

	StreamData * sd2 = malloc(sizeof(StreamData));
	sd2->data = calloc(BUFFER_SIZE,1);

	StreamData * sd = malloc(sizeof(StreamData));
	sd->data = malloc(BUFFER_SIZE);
	
	sprintf(sd->data,"hello testing");
	printf("send data\n");
	sendData(self,sd);

	printf("receive data\n");
	receiveData(self,sd2);
	printf("after receive\n");

	printf("Received: %s\n", sd2->data);
}


// Main for server
/*
int main(){
	Connection * self = listenConnection(8080);
	Connection * connection;

	StreamData * sd = malloc(sizeof(StreamData));
	sd->data = malloc(BUFFER_SIZE);

	StreamData * sd2 = malloc(sizeof(StreamData));
	sd2->data = malloc(BUFFER_SIZE);

	while(1) {
		connection = acceptConnection(self);
		if(connection!=NULL){
			receiveData(connection, sd);
			printf("Received: %s\n", sd->data);
			printf("Size: %d\n", sd->size);
			receiveData(connection, sd2);
			printf("Received2: %s\n", sd2->data);
			printf("Size2: %d\n", sd2->size);
			if(sd->size>0)
				sendData(connection, sd);
		}
	}
}*/

/*
//returns length of ip address (bytes)
int processIP(char * addr, int * port){
	int i=0, length=0;
	*port = 0;
	char c;
	while((c=addr[i])!='\0'){
		if(c==':')
			length = i-1;
		else if(length!=0){
			(*port)*=10;
			(*port)+=c-'0';
		}
		i++;
	}
	return length+1;
}*/
