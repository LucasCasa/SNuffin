TARGET = superServer.c childServer.c db.c log.c marshalling.c dbManager.c comWrapper.c
CLIENT = client.c marshalling.c comWrapper.c
FIFO = comFIFO.c
SOCKETS = comSocket.c


all: fifo clientfifo log

fifo: log
	gcc $(TARGET) $(FIFO) -o server.out -lrt -lpthread -l sqlite3 -Wall -std=gnu99

sockets: log
	gcc $(TARGET) $(SOCKETS) -o server.out -lrt -lpthread -l sqlite3 -std=gnu99

clientfifo: log
	gcc $(CLIENT) $(FIFO) -o client.out -Wall -lpthread -std=gnu99

clientsocket: log
	gcc $(CLIENT) $(SOCKETS) -o client.out -lpthread -Wall -std=gnu99

log:
	gcc mqserver.c -o log.out -lrt -Wall -std=gnu99

clean:
	rm *.out

.PHONY: fifo sockets clientfifo clientsocket
