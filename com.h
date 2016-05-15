#ifndef COM_H
#define COM_H

#include <sys/select.h>

#define BUFFER_SIZE 256

typedef struct {
	int fd;
	int fd2;
} Connection;

typedef struct {
char* data;
int size;
} StreamData;


// int va a ser puerto para sockets y id para fifo
Connection * connectToPeer(char* addr,int);
Connection * listenConnection(int);
void closeConn(Connection * c);
int  sendData(Connection * connection, StreamData * req);
Connection * acceptConnection(Connection *c);
void receiveData(Connection * connection, StreamData * buffer);
void setFDs(fd_set * set, Connection ** connections);
Connection * getConnByFD(int fd, Connection ** conns, int size);

#endif

/*
connectionRequests = listen(addressObject);

while(1) {
	connection = accept(connectionRequests);
	read(connection, whatever);
	response = process(whatever);
	write(connection, response);
}
*/
