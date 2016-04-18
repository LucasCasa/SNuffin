#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>

#define BUFFER_LENGTH 256
#define MAX_CONNECTION_QUEUE 50
#define MY_SOCK_PORT 8080

#define handle_error(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)

void handle_request(int);

int main(void){

	// File descriptors for server connection socket and client communication sockets repectively
	int sfd, cfd;

	printf("Starting server...\n");

	struct sockaddr_in my_addr, peer_addr;
    socklen_t peer_addr_size;


	// The socket fd
	printf("Creating socket...\n");
	sfd = socket(AF_INET,SOCK_STREAM,0);

	if(sfd==-1)
		handle_error("socket");

	// Reserves memory for the structure
	memset(&my_addr,0,sizeof(struct sockaddr_un));

	// Declare the type of connnection
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	// Host to network swaps the endianess of the port number
   	my_addr.sin_port = htons(MY_SOCK_PORT);

   	/* ESTO ERA PARA CONEXION LOCAL*/
	// Set the path to which packets are being sent
	// From now on any packet with this destination gets fowarded to my sfd so it can be read
	//strncpy(my_addr.sun_path, MY_SOCK_PORT,sizeof(my_addr.sun_path) - 1);

	printf("Binding socket to specified port...\n");
   	if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1)
       handle_error("bind");

   	// Marks sfd as a passive sockdet.  MAX_CONNECTION_QUEUE is the max n° of pending connections
   	printf("Listening...\n");
   	if (listen(sfd, MAX_CONNECTION_QUEUE) == -1)
       handle_error("listen");

   	// Now incoming connections can be accepted

   	peer_addr_size = sizeof(struct sockaddr_un);

	while (1) {
		cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);

		if(cfd<0)
			handle_error("accept");

		// Create child process
		int pid = fork();

		if (pid < 0) {
			perror("ERROR on fork");
			exit(1);
		}

		if (pid == 0) {
			// This is the child process
			close(sfd);
			handle_request(cfd);
			exit(0);
		}
		else {
			// This is the parent process
			close(cfd);
		}
		
   	}

   return 0;
}

void handle_request(int cfd){

	// Buffer where read data will be copied
	char buffer[BUFFER_LENGTH]; 

	// Sets buffer values on 0
   	bzero(buffer,BUFFER_LENGTH);

   	printf("Reading data...\n");
   	if(read(cfd, buffer, BUFFER_LENGTH-1)==-1)
   		handle_error("read");

   char response[] = "I got your message\n\0";

   	printf("Writing response...\n");
   	if(write(cfd,response,strlen(response)+1)==-1)
   		handle_error("write");
}