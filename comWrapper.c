#include "com.h"
#include "comWrapper.h"

char ** split(char * source, int*amount, int size);
void clearArr(char * arr,int size);

char ** readBuffer = NULL;
int rBuffSize=0;
int currentPos=0;

int readData(Connection * conn, StreamData * sd){
    if(readBuffer==NULL){
        receiveData(conn,sd);
        readBuffer = split(sd->data,&rBuffSize, sd->size); // el char* data lo divide por los '\0' y los almacena en el readBuffer
        currentPos = 0;
    }
    sd->size = strlen(readBuffer[currentPos]);
    memcpy(sd->data,readBuffer[currentPos],sd->size+1);

    currentPos++;

    if(currentPos == rBuffSize){
        for(int i=0; i<rBuffSize; i++){
            free(readBuffer[i]);
        }
        free(readBuffer);

        readBuffer = NULL;
        currentPos = 0;
    }

    if(sd->size<=1)
        return 0;
    return 1;
}

/** Divides the source by '\0' and returns a pointer to the array
    of char* where they are stored, setting the amount*/
char ** split(char * source, int*amount, int size){
    int auxCounter=0, resSize=BLOCK;
    int totalStrings=0;
    char * aux = calloc(size,1);
    char ** res = malloc(BLOCK*sizeof(char*));
    for(int i=0; i<size; i++){
        if(source[i]!=0){
            aux[auxCounter++] = source[i];
        }else{
            if(totalStrings==resSize){
                res = realloc(res,(sizeof(char*))*(totalStrings+BLOCK));
                resSize = totalStrings+BLOCK;
            }
            res[totalStrings] = calloc(auxCounter+2,1);
            memcpy(res[totalStrings],aux,auxCounter+1);
            clearArr(aux,size);
            totalStrings++;
            auxCounter = 0;
        }
    }
    int s= sizeof(char*) * totalStrings;
    res = realloc(res,s);
    *amount = totalStrings;
    return res;
}

void clearArr(char * arr,int size){
    int i=0;
    if(arr == NULL)
        return;
    while(i<size){
        arr[i++] = 0;
    }
}
