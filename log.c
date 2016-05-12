/** COMPILAR CON -Wall -lrt */

#include "log.h"

mqd_t qDescriptor;


// Returns 1 if it can connect 0 if not (Server is not active)
int connectLogServer(){

	if((qDescriptor = mq_open(Q_SRV_NAME,O_CREAT | O_RDWR))==-1){
		return 0;
	}

	return 1;
}

void logMsg(char * msg, int level){

	if (mq_send (qDescriptor, msg, strlen(msg), level) == -1) {
        perror ("Unable to send message to log server");
    }
}

void closeLogServer(){
	mq_close(qDescriptor);
}
