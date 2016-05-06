#include "com.h"

#define MAX_CONNECTIONS 4 
#define MAX_CONNECTION_QUEUE 4

struct sockaddr_in my_addr;
int sockfd, peerLimit;
int * peers;

Connection * connect(char* addr, int port){
	int port, n;
	struct hostent *server;

	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
	  perror("ERROR opening socket");
	  exit(1);
	}

	server = gethostbyname(addr);

	if (server == NULL) {
	  fprintf(stderr,"ERROR, no such host\n");
	  exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
	  perror("ERROR connecting");
	  exit(1);
	}

	Connection * c = malloc(sizeof(Connection));
	c->fd = sockfd;
	return c;
}


Connection * listen(int port){
	printf("Starting server...\n");

	// The socket fd
	sockfd = socket(AF_INET,SOCK_STREAM,0);

	if(sockfd==-1)
		handle_error("socket");

	// Reserves memory for the structurelisten
	memset(&my_addr,0,sizeof(struct sockaddr_in));

	// Declare the type of connnection
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1)
		handle_error("bind");

	// Marks sfd as a passive sockdet.  MAX_CONNECTION_QUEUE is the max n° of pending connections
	printf("Listening on port %d...\n", port);
	if (listen(sockfd, MAX_CONNECTION_QUEUE) == -1)
		handle_error("listen");

	printf("Waiting for connections...\n");
  	addr_size = sizeof(struct sockaddr_in);

  	peers = malloc(MAX_CONNECTIONS*sizeof(int));
    peerLimit = MAX_CONNECTIONS;

    // Set peers on 0
  	for(i=0; i<clientLimit; i++)
    	peers[i] = 0;

    Connection * c = malloc(sizeof(Connection));
	c->fd = sockfd;
	return c;
}

Connection* accept(Connection * c){
	int max_sd, space_available, i, sd, ans, ans_select;
	fd_set readfds;

	FD_ZERO(&readfds);
	//add master socket to set
	FD_SET(c->fd, &readfds);
	max_sd = server_sfd;
	//add all other
	for(i=0;i<clientLimit;i++){
		if(peers[i]!=0){
			FD_SET(peers[i], &readfds);
			if(peers[i]>max_sd)
		  		max_sd = peers[i];
		}
	}
	struct timeval tv = {1, 0}; //the timeout (s,ms)
	//wait for an activity on one of the sockets
	ans_select = select( max_sd + 1 , &readfds , NULL , NULL , &tv);
	if (ans_select < 0)
		handle_error("select");
	//If its server_sfd then its an INCOMING CONNECTION
	if (FD_ISSET(c->fd, &readfds)){
		if ((new_socket = accept(server_sfd, (struct sockaddr *)&my_addr, (socklen_t*) &addr_size))<0)
	 		handle_error("accept");
		printf("New connection\n");
		//add new socket to array of sockets
		space_available = FALSE;
		for (i = 0; i < MAX_peers && !space_available; i++){
	  	//if position is empty use it
	  	if( peers[i] == 0 ){
	    	peers[i] = new_socket;
	    	space_available = TRUE;
	  	}
	}
	if(!space_available)
	  printf("Server is full.\n");
	}else{
		//else its an operation for an existing socket
		for (i = 0; i < MAX_peers; i++){
		  sd = peers[i];

		  if (sd!=0 && FD_ISSET( sd , &readfds)){
		  	char buffer[256];
		    int j;
		    for(j=0; j<256; j++)
		    	buffer[j] = 0;
		    ans = recv(sd,buffer,256,0);
		    printf("Receiving data: %s\n", buffer);
		    Connection c;
		    StreamData stream;
		    c.fd = sd;
		    stream.data = buffer;
		    stream.size = 256;
		    if(ans>0)
		    	sendData(&c,&stream);
		    //int ans = receiveData(sd, ); // TODO
		    // if 0 then disconnect
		    printf("recv: %d\n",ans);
		    if(ans==0)
		    	peers[i] = 0;
		  }
		}
	}
	return;
}

vofd closeConn(Connection * c){
	close(c->fd);
	free(peers);
}

int  sendData(Connection * connection, StreamData * req);

vofd receiveData(Connection * connection, StreamData * buffer);


//returns length of ip address (bytes)
/*int processIP(char * addr, int * port){
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
