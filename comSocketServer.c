#include "server.h"
#include "com.h"

void listen_requests();

/* GLOBAL VARS */
socklen_t addr_size;
int server_sfd, new_socket;
struct sockaddr_in my_addr;
int * clients;
int clientLimit;

void start_ipc(int isSuperServer){
  int i;

	printf("Starting server...\n");

	// The socket fd
 printf("Creating socket...\n");
 server_sfd = socket(AF_INET,SOCK_STREAM,0);

 if(server_sfd==-1)
  handle_error("socket");

	// Reserves memory for the structure
  memset(&my_addr,0,sizeof(struct sockaddr_in));

  // Declare the type of connnection
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  if(isSuperServer)
    my_addr.sin_port = htons(MY_SOCK_PORT);
  else
    my_addr.sin_port = htons(8081); // TODO

  printf("Binding socket to specified port...\n");
  if (bind(server_sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1)
   handle_error("bind");

  // Marks sfd as a passive sockdet.  MAX_CONNECTION_QUEUE is the max n° of pending connections
  printf("Listening on port %d...\n", MY_SOCK_PORT);
  if (listen(server_sfd, MAX_CONNECTION_QUEUE) == -1)
    handle_error("listen");

  printf("Waiting for connections...\n");
  addr_size = sizeof(struct sockaddr_in);

  if(isSuperServer){
    clients = malloc(MAX_CLIENTS*sizeof(int));
    clientLimit = MAX_CLIENTS;
  }else{
    clients = malloc(MAX_PLAYERS*sizeof(int));
    clientLimit = MAX_PLAYERS;
  }
  // Set clients on 0
  for(i=0; i<clientLimit; i++)
    clients[i] = 0;
}

void listen_requests(){
  int max_sd, space_available, i, sd, ans, ans_select;
  fd_set readfds;

  FD_ZERO(&readfds);
  //add master socket to set
  FD_SET(server_sfd, &readfds);
  max_sd = server_sfd;
  //add all other
  for(i=0;i<clientLimit;i++){
    if(clients[i]!=0){
      FD_SET(clients[i], &readfds);
      if(clients[i]>max_sd)
        max_sd = clients[i];
    }
  }
  struct timeval tv = {1, 0}; //the timeout (s,ms)
  //wait for an activity on one of the sockets
  ans_select = select( max_sd + 1 , &readfds , NULL , NULL , &tv);
  if (ans_select < 0)
    handle_error("select");
  //If its server_sfd then its an INCOMING CONNECTION
  if (FD_ISSET(server_sfd, &readfds)){
    if ((new_socket = accept(server_sfd, (struct sockaddr *)&my_addr, (socklen_t*) &addr_size))<0)
      handle_error("accept");
    printf("New connection\n");
    //add new socket to array of sockets
    space_available = FALSE;
    for (i = 0; i < MAX_CLIENTS && !space_available; i++){
      //if position is empty use it
      if( clients[i] == 0 ){
        clients[i] = new_socket;
        space_available = TRUE;
      }
    }
    if(!space_available)
      printf("Server is full.\n");
  }else{
    //else its an operation for an existing socket
    for (i = 0; i < MAX_CLIENTS; i++){
      sd = clients[i];

      if (sd!=0 && FD_ISSET( sd , &readfds)){
        char buffer[256];
        int j;
        for(j=0; j<256; j++)
          buffer[j] = 0;
        ans = recv(sd,buffer,256,0);
        printf("Receiving data: %s\n", buffer);
        Connection c;
        StreamData stream;
        c.id = sd;
        stream.data = buffer;
        stream.size = 256;
        if(ans>0)
          sendData(&c,&stream);
        //int ans = receiveData(sd, ); // TODO
        // if 0 then disconnect
        printf("recv: %d\n",ans);
        if(ans==0)
          clients[i] = 0;
      }
    }
  }
return;
}

void receiveData(Connection * connection, StreamData * buffer){

}

int  sendData(Connection * connection, StreamData * req){
   /* Send message to the server */
   int n = send(connection->id, req->data, req->size,MSG_NOSIGNAL);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
}


// If disconnect request return 0 else return 1
int handle_request(int cfd, char * buffer){

  int amount_read;
  bzero(buffer,BUFFER_LENGTH);

  printf("Reading data...\n");
  if((amount_read = recv(cfd, buffer, BUFFER_LENGTH-1,0))<0)
   handle_error("read");

  if(amount_read==0){
    printf("Disconnecting...\n");
    //Close the socket
    close( cfd );
    return 0;
  }

  printf("Data read: %s\n",buffer);

  char response[] = "Message received\n\0";

  printf("Writing response...\n");
  if(write(cfd,response,strlen(response)+1)==-1)
    handle_error("write");
  printf("Response sent\n");

  return 1;
}

void close_ipc(){
  close(server_sfd);
}

// Creo que no la voy a usar
/*
int calculateMax(int arr[], int size){
  int max = 0, i;
  for(i=0; i<size; i++){
    if(arr[i]>max)
      max = arr[i];
  }
  return max;
}*/

int main(void){
  start_ipc(1);
  while(1){
    listen_requests();
  }
  close_ipc();
}
