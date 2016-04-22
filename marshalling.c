#include "marshalling.h"
#include <stdio.h>
#include <stdlib.h>


void * unmarshalling(char * data){
	if(data == NULL){
		return NULL;
	}
	if(data[0] == STRING){
		String * s = malloc(sizeof(String));
		//HandleString(&s, data);
		return s;

	}else if(data[0] == POINT){
		Point * p = malloc(sizeof(Point));

	}else if (data[0] == BOARD){
		Board * t = malloc(sizeof(Board));
	}else{
		//hay error, me mando cualquier cosa
	}
}

char * marshPoint(String* s){
return NULL;
}
char * marshBoard(Board* s){
return NULL;
}
char * marshString(Point* s){
	char * d;
	int i=0;
	d = malloc (1 + 1 + 1 + sizeof(s->x) + sizeof(s->y));
	d[i] = POINT;
	itoa(s->x,d+1,10);

}

char * marshalling(void * struc, int type){
	if(type == STRING){
		return marshString((String * )struc);
	}else if(type == BOARD){
		return marshBoard((Board *)struc);
	}else if(type == POINT){
		return marshPoint((Point *)struc);
	}else {
		return NULL;
	}
}


int main(){
	
}
