#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include "com.h"

int sockfd;
struct sockaddr_in serv_addr;

void start_ipc(int info) {
   int port, n;
   struct hostent *server;
   
   FILE * f;

   f = fopen("./configSocket", "r");
   if(f == NULL)
      perror("config");

   fscanf(f,"%d", &port);
	
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname("localhost");
   
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
}

int sendData(Connection * connection, StreamData * req){
   /* Send message to the server */
   int n = send(sockfd, req->data, req->size,MSG_NOSIGNAL);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
}

void receiveData(Connection * connection, StreamData * buffer){
   int ans, step = 0;

   ans=recv(sockfd,buffer->data,256,0);
   if(ans==-1)
      perror("receive");
   printf("Recibí: %s\n",buffer->data);
   buffer->size = step;
}



void close_ipc(){
   close(sockfd);
}

int main(void){
   start_ipc(1);
   Connection c;
   StreamData sd;


   sd.data = "MagdaMuffin";
   sd.size = strlen(sd.data);
   
   while(1){
      StreamData sd2;

      sd2.data = malloc(256);
      sd2.size = 256;
      sendData(&c,&sd);
      receiveData(&c,&sd2);
   
   }

   close_ipc();
}