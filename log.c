/** COMPILAR CON -Wall -lrt */

#include "log.h"

mqd_t qDescriptor;
int connected = 0;

// Returns 1 if it can connect 0 if not (Server is not active)
int connectLogServer(){

	if((qDescriptor = mq_open(Q_SRV_NAME,O_WRONLY | O_NONBLOCK))==-1){
		return 0;
	}
	connected = 1;
	return 1;
}

void sendLog(char * msg, int level){

	if(!connected)
		return;

	// add pid identifier as prefix
	char * aux = calloc(strlen(msg) + 10,1);
	sprintf(aux,"%d>%s\n",getpid(),msg);


	if (mq_send (qDescriptor, aux, strlen(aux), level) == -1) {
        perror ("Unable to send message to log server");
    }
}

void logMsg(char * msg){
	sendLog(msg,INFO);
}

void logWarning(char * msg){
	sendLog(msg,WARNING);
}

void logError(char * msg){
	sendLog(msg,ERROR);
}

void closeLogServer(){
	mq_close(qDescriptor);
}
