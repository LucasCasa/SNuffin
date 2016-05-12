/** COMPILAR CON -Wall -lrt */


#include "log.h"

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

int main (int argc, char **argv)
{
    mqd_t qd_server;   // queue descriptors
    unsigned int priority;

    printf ("Initializing logging server...\n");

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_server = mq_open (Q_SRV_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    char in_buffer [MSG_BUFFER_SIZE];

    printf ("Listening for messages\n\n");

    while (1) {
        char * prioMsg;

        // retrieve message
        if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, &priority) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }

        switch(priority){
            case INFO:  prioMsg = "INFO";
                        break;
            case WARNING:   prioMsg = "WARNING";
                            break;
            case ERROR: prioMsg = "ERROR";
                        break;            
        }

        printf ("%s: %s\n",prioMsg, in_buffer);
    }
}