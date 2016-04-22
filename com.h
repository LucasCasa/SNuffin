#ifndef COM_H
#define COM_H

typedef struct {
	char* address;
	int port;
} Connection;

typedef struct {
char* data;
int size;
} StreamData;

typedef StreamData (*RequestHandler)(StreamData* req);

StreamData * Request(Connection * connection, StreamData * req);

//int Response(RequestData * req, ResponseData * resp); CREO QUE ES AL DOPE

StreamData * ListenToRequest(Connection * connection, RequestHandler requestHandler);


#endif
