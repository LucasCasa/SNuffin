TARGET = superServer.c childServer.c db.c log.c marshalling.c dbManager.c
CLIENT = client.c marshalling.c
FIFO = comFIFO.c
SOCKETS = comSockets.c


all: fifo clientfifo log

fifo: log
	gcc $(TARGET) $(FIFO) -o server.out -lrt -lpthread -l sqlite3 -Wall

sockets: log
	gcc $(TARGET) $(SOCKETS) -o server.out -lrt -lpthread -l sqlite3

clientfifo: log
	gcc $(CLIENT) $(FIFO) -o client.out -Wall

clientsocket: log
	gcc $(CLIENT) $(FIFO) -o client.out -Wall

log:
	gcc mqserver.c -o log.out -lrt -Wall

clean:
	rm *.out

.PHONY: fifo sockets clientfifo clientsocket
