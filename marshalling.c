#include "marshalling.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

int numPlaces (int n) ;

void * unmarshalling(StreamData * data, int * type);

int unmarshString(char * data, String * s );
int unmarshPoint(char * data, Point * p);
int unmarshBoard(char * data, Board * b);
int unmarshServerId(char * data, Integer * p);
int unmarshPlayerPos(char * data, PlayerPos * p);

StreamData * marshalling(void * struc, int type);

char * marshPoint(Point * s, int * size);
char * marshPlayerPos(PlayerPos * p, int * size);
char * marshBoard(Board* s, int * size);
char * marshString(String * s, int * size);
char * marshInt(Integer * s, int * size);
char * marshServerId(Integer * p, int * size);


void * unmarshalling(StreamData * d, int * type){
	if(d == NULL || d->data == NULL){
		return NULL;
	}
	if(d->data[0] == STRING){
		String * s = malloc(sizeof(String));
		*type = STRING - '0';
		unmarshString(d->data,s);
		return s;
	}else if(d->data[0] == PLAYER){
		PlayerPos * p = malloc(sizeof(PlayerPos));
		*type = PLAYER - '0';
		unmarshPlayerPos(d->data,p);
		return p;
	}else if(d->data[0] == POINT){
		Point * p = malloc(sizeof(Point));
		*type = POINT - '0';
		unmarshPoint(d->data,p);
		return p;
	}else if (d->data[0] == BOARD){
		Board * t = malloc(sizeof(Board));
		*type = BOARD - '0';
		unmarshBoard(d->data,t);
		return t;
	}else if(d->data[0] == SERVER_ID){
		Integer * t = malloc(sizeof(Integer));
		*type = SERVER_ID - '0';
		unmarshServerId(d->data,t);
		return t;
	}else{
		type = NULL;
		return NULL;
	}
	return NULL;
}

StreamData * marshalling(void * struc, int type){
	StreamData * sd = malloc(sizeof(StreamData));
	char * aux;
	int size;
	type = type + '0';
	if(type == STRING){
		aux = marshString((String * )struc,&size);
	}else if(type == BOARD){
		aux = marshBoard((Board *)struc,&size);
	}else if(type == POINT){
		aux = marshPoint((Point *)struc,&size);
	}else if (type == PLAYER){
		aux = marshPlayerPos((PlayerPos *)struc,&size);
	}else if(type == SERVER_ID){
		aux = marshServerId((Integer *)struc,&size);
	}else{
		aux = NULL;
		size = 0;
	}
	sd -> data = aux;
	sd -> size = size;
	return sd;
}

int unmarshPlayerPos(char * data, PlayerPos * p){
	char * endptr;
	char num[5];
	if(data[0] != PLAYER){
		p = NULL;
		return 0;
	}
	p->pNumber = data[1] - '0';
	p->dir = malloc(sizeof(Point));
	int i = 2,j=0;
	for(i =2;data[i]!=SEP;i++,j++){
		num[j]=data[i];
	}
	long m= strtol(num, &endptr,10);
	p->dir->x = (int)m;
	for(i = i+1,j=0;data[i]!='\0';i++,j++){
		num[j]=data[i];
	}
	m=strtol(num,&endptr,10);
	p->dir->y =(int)m;
	return 1;
}

int unmarshServerId(char * data, Integer * p){
	char num[5];
	int i,j=0;
	char * endptr;
	if(data[0] != SERVER_ID){
		p = NULL;
		return 0;
	}
	for(i=1;data[i]!='\0';i++,j++){
		num[j]=data[i];
	}
	long m = strtol(num, &endptr,10);
	p->info = (int)m;
	return 1;
}

int unmarshPoint(char * data, Point * p){
	char num[5];
	char * endptr;
	int i=1,j=0;
	if(data[0] != POINT){
		p = NULL;
		return 0;
	}
	for(i =1;data[i]!=SEP;i++,j++){
		num[j]=data[i];
	}
	long m= strtol(num, &endptr,10);
	p->x = (int)m;
	for(i = i+1,j=0;data[i]!='\0';i++,j++){
		num[j]=data[i];
	}
	m=strtol(num,&endptr,10);
	p->y =(int)m;
	return 1;

}

int unmarshString(char * data, String * s){
	int i,j=0,k;
	char num[5];
	char * endptr;
	if(data[0] == STRING){
		s = NULL;
		return 0;
	}
	for(i=1;data[i]!=SEP;i++){
		num[i-1]=data[i];
	}
	long m = strtol(num, &endptr,10);
	s->size = (int)m;
	char * aux = malloc(m*sizeof(char));
	k=i+1;
	for(i= i+1;i<'\0';i++,j++){
		aux[j]=data[i];
	}
	s->string=aux;
	return 1;
}

int unmarshBoard(char * data, Board * b){
	char num[5],num2[5];
	char ** board;
	int f,c,cont;
	int i,j,k=-1;
	char * endptr;
	if(data[0] != BOARD){
		b = NULL;
		return 0;
	}
	for(i=1,j=0;data[i]!=SEP;i++,j++){
		num[j]=data[i];
	}
	for(i=i+1,j=0;data[i]!=SEP;i++,j++){
		num2[j]=data[i];
	}
	f = (int)strtol(num,&endptr,10);
	c = (int)strtol(num2,&endptr,10);
	board = malloc(f * sizeof(char));
	for(j=0;j<f;j++){
		board[j]= malloc(c * sizeof(char));
	}
	b->rows = f;
	b->columns = c;
	for(i = i+1,j=0,cont=0;data[i]!='\0';i++,cont++){
		if((cont)%c== 0){
			k++;
			j=0;
		}
		board[k][j]=data[i];
		j++;
	}
	b->board = board;
	return 1;
}

char * marshPoint(Point * s,int * size){
	char * d;
	int cant = 1 + 1 + 1 + numPlaces(s->x) + numPlaces(s->y);
	d = malloc (cant*sizeof(char));
	sprintf(d,"%d%d%c%d",POINT - '0',s->x,SEP,s->y);
	d[cant -1]='\0';
	*size = cant;
	return d;
}

char * marshBoard(Board* s,int * size){
	char * d1;
	char * d2;
	int i,j;
	int size1 = 1 + 1 + 1 + 1 + numPlaces(s -> rows) + numPlaces(s->columns);
	int size2 = (s->rows)*(s->columns);
	d1 = malloc ((size1 + size2)*sizeof(char));
	d2 = malloc (size2 *sizeof(char));
	sprintf(d1,"%d%d%c%d%c",BOARD - '0',s->rows, SEP, s->columns,SEP);
	for(i=0;i<s->rows;i++){
		printf("%d\n", i);
		for(j=0;j<s->columns;j++){
			d2[j + i*(s->rows)]= (s->board)[i][j];
		}
	}
	strcat(d1,d2);
	*size = size1 + size2;
	return d1;
}

char * marshString(String * s, int * size){
	char * d;
	int cant = 1 + 1 + 1 + s->size + numPlaces(s->size);
	d = malloc (cant*sizeof(char));
	sprintf(d,"%d%d%c%s",STRING - '0', s->size,SEP, s->string);
	d[cant -1] ='\0';
	*size = cant;
	return d;
}

char * marshPlayerPos(PlayerPos * p,int * size){
	char * d;
	int cant = 1 + 1 + 1 + numPlaces(p->dir->x) + numPlaces(p->dir->y) + 1;
	d = malloc(cant * sizeof(char));
	sprintf(d,"%d%d%d%c%d",PLAYER - '0',p->pNumber,p->dir->x,SEP,p->dir->y);
	*size = cant;
	return d;
}

char * marshServerId(Integer * p, int * size){
	char * d;
	int cant = 1 + numPlaces(p->info) + 1;
	d = malloc(cant * sizeof(char));
	*size = cant;
	sprintf(d,"%d%d",SERVER_ID -'0',p->info);
	return d;
}

 int numPlaces (int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX : -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}
