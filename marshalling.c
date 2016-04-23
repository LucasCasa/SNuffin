#include "marshalling.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void itoa(int n, char s[]);
void reverse(char s[]);

void * unmarshalling(char * data, int * type);

void unmarshString(char * data, String * s );
void unmarshPoint(char * data, Point * p);
void unmarshBoard(char * data, Board * b);

char * marshPoint(Point * s);

char * marshBoard(Board* s);
char * marshString(String * s);
char * marshalling(void * struc, int type);

int main(){
	return 0;
}

void * unmarshalling(char * data, int * type){
	if(data == NULL){
		return NULL;
	}
	if(data[0] == STRING){
		String * s = malloc(sizeof(String));
		*type = STRING - '0';
		unmarshString(data,s);
		return s;

	}else if(data[0] == POINT){
		Point * p = malloc(sizeof(Point));
		*type = POINT - '0';
		unmarshPoint(data,p);
		return p;

	}else if (data[0] == BOARD){
		Board * t = malloc(sizeof(Board));
		*type = BOARD - '0';
		unmarshBoard(data,t);
		return t;
	}else{
		type = NULL;
		return NULL;
	}
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

void unmarshString(char * data, String * s){
	//data tiene la forma 1A#B donde A es el tamaño del string y B es el STRING
	int i,j=0,k;
	char num[3];
	char * endptr;
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
}

void unmarshPoint(char * data, Point * p){
	char num[3];
	char * endptr;
	int i,j=0;
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

}
void unmarshBoard(char * data, Board * b){
	char num[3],num2[3];
	char ** board;
	int f,c,cont;
	int i,j,k=-1;
	char * endptr;
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
	b->filas = f;
	b->columnas = c;
	for(i = i+1,j=0,cont=0;data[i]!='\0';i++,cont++){
		if((cont)%c== 0){
			k++;
			j=0;
		}
		board[k][j]=data[i];
		j++;
	}
	b->board = board;
}

char * marshPoint(Point * s){
	char * d;
	int i=0;
	d = malloc (1 + 1 + 1 + sizeof(s->x) + sizeof(s->y));
	d[i] = POINT;
	itoa(s->x,d+1);
	return d;
}
char * marshBoard(Board* s){
	return NULL;
}
char * marshString(String * s){
	return NULL;
}
 
 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

  void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }



