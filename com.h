#ifndef COM_H
#define COM_H

typedef struct {
	char* addr;
} Connection;

typedef struct {
char* data;
int size;
} StreamData;

//typedef StreamData (*RequestHandler)(StreamData* req);

Connection * connect(char* addr);
Connection * listen(char* addr);
void closeConn(Connection * c);
int  sendData(Connection * connection, StreamData * req);
Connection* accept(Connection *c);
//int Response(RequestData * req, ResponseData * resp); CREO QUE ES AL DOPE

void receiveData(Connection * connection, StreamData * buffer);

/* habria que hacer una con un select, pero no tengo ni idea de como hacerlo*/

#endif

connectionRequests = listen(addressObject);

while(1) {
	connection = accept(connectionRequests);
	read(connection, whatever);
	response = process(whatever);
	write(connection, response);
}
