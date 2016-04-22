#include "server.h"

int main(void){
	printf("Starting server...\n");

	// File descriptors for server connection socket and client communication sockets repectively
	int sfd, sd, i, max_sd, activity, flag, readValue, new_socket;
	int clients[MAX_CLIENTS];
	fd_set readfds;
	// Buffer where read data will be copied
	char buffer[BUFFER_LENGTH];

	struct sockaddr_in my_addr, peer_addr;
    socklen_t peer_addr_size;

    // Set clients on 0 (none exist yet)
    for(i=0; i<MAX_CLIENTS; i++)
    	clients[i] = 0;

	// The socket fd
	printf("Creating socket...\n");
	sfd = socket(AF_INET,SOCK_STREAM,0);

	if(sfd==-1)
		handle_error("socket");

	// Reserves memory for the structure
	memset(&my_addr,0,sizeof(struct sockaddr_in));

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
   	if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1)
       handle_error("bind");

   	// Marks sfd as a passive sockdet.  MAX_CONNECTION_QUEUE is the max n° of pending connections
   	printf("Listening on port %d...\n", MY_SOCK_PORT);
   	if (listen(sfd, MAX_CONNECTION_QUEUE) == -1)
       handle_error("listen");

   	// Now incoming connections can be accepted

    printf("Waiting for connections...\n");
   	peer_addr_size = sizeof(struct sockaddr_un);

   	// Main cycle
	while (TRUE) {

		//clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(sfd, &readfds);
        max_sd = sfd;

        //add child sockets to set
        for ( i = 0 ; i < MAX_CLIENTS ; i++)
        {
            //socket descriptor
            sd = clients[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //set the timeout (s,ms)
        struct timeval tv = {1, 0};

        //wait for an activity on one of the sockets
        activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);

        if (activity < 0)
        	handle_error("select");

        //If its sfd then its an INCOMING CONNECTION
        if (FD_ISSET(sfd, &readfds))
        {
            if ((new_socket = accept(sfd, (struct sockaddr *)&my_addr, (socklen_t*)&peer_addr_size))<0)
            {
                handle_error("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(my_addr.sin_addr) , ntohs(my_addr.sin_port));

            //add new socket to array of sockets
            flag = FALSE;
            for (i = 0; i < MAX_CLIENTS && !flag; i++)
            {
                //if position is empty
                if( clients[i] == 0 )
                {
                    clients[i] = new_socket;
                    flag = TRUE;
                }
            }
        }

        //else its some IO operation on some other socket :)
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = clients[i];

            if (FD_ISSET( sd , &readfds))
            {

				// Sets buffer values on 0
			   	bzero(buffer,BUFFER_LENGTH);

                //Check if it was for closing , and also read the incoming message
                if ((readValue = read( sd , buffer, BUFFER_LENGTH)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&my_addr , (socklen_t*)&peer_addr_size);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(my_addr.sin_addr) , ntohs(my_addr.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    clients[i] = 0;
                }

                //Echo back the message that came in
                else
                {
                    handle_request(sd, buffer);
                }
            }
        }

   	}

   return 0;
}

void handle_request(int cfd, char * buffer){



   	printf("Reading data...\n");
   	if(read(cfd, buffer, BUFFER_LENGTH-1)==-1)
   		handle_error("read");

   char response[] = "I got your message\n\0";

   	printf("Writing response...\n");
   	if(write(cfd,response,strlen(response)+1)==-1)
   		handle_error("write");
}
