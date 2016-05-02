#include "com.h"

struct sockaddr_in my_addr;

Connection * connect(char* addr, int port){
	int port, n, length;
	struct hostent *server;
	char * ip;

	length = processIP(addr,&port);
	ip = malloc(length+1);
	memcpy(ip,addr,length);
	ip[length+1] = '\0';

	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
	  perror("ERROR opening socket");
	  exit(1);
	}

	server = gethostbyname(ip);

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

	int length, sockfd;

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
}


void closeConn(Connection * c);
int  sendData(Connection * connection, StreamData * req);
Connection* accept(Connection *c);
void receiveData(Connection * connection, StreamData * buffer);


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
