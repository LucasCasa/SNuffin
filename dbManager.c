#include "dbManager.h"

pthread_cond_t* cond;
pthread_mutex_t* mutex;
char ExistUserDB(char * user){
  //sem_wait(&semDB);
  cond =(pthread_cond_t*) (shmPointer + COND_OFFSET) ;
  mutex = (pthread_mutex_t*)(shmPointer + MUTEX_OFFSET);
  printf("Intento lockear\n");
  int s = pthread_mutex_lock(mutex);
  printf("lockeo %d\n",s);
  shmPointer[TYPEPOS] = ISUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user));
  shmPointer[strlen(user) + 1 ] = 0;
  printf("SETEADO llamo a db\n");
  pthread_cond_signal(cond);
  printf("ME desbloquee\n");
  char c = shmPointer[RETURNPOS];
  //sem_post(&semDB);
  pthread_mutex_unlock(mutex);
  return c;
}
int validPasswordDB(char* user, char* password){
 //sem_wait(&semDB);
 int s = pthread_mutex_lock(mutex);
 shmPointer[TYPEPOS] = ISPASSWORD;
 memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user)+1);
 memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
 pthread_cond_signal(cond);
 char c = shmPointer[RETURNPOS];
 pthread_mutex_unlock(mutex);
 //sem_post(&semDB);
 return c;
}
int getHighScoreDB(char * user){
  //sem_wait(&semDB);
  shmPointer[TYPEPOS] = GETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user) + 1);
  while(shmPointer[TYPEPOS] != READY);
  int i = atoi(shmPointer+ RETURNPOS);
  //sem_post(&semDB);
  return i;
}
int setHighscoreDB(char* user, int value){
  //sem_wait(&semDB);
  shmPointer[TYPEPOS] = SETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT, user, strlen(user) + 1);
  sprintf(shmPointer + SECONDARGUMENT,"%d", value);
  while(shmPointer[TYPEPOS] != READY);
  //sem_post(&semDB);
  return shmPointer[RETURNPOS];
}
int createUserDB(char* user, char* password){
  //sem_wait(&semDB);
  shmPointer[TYPEPOS] = CREATEUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user, strlen(user) + 1);
  memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
  while(shmPointer[TYPEPOS] != READY);
  //sem_post(&semDB);
  return shmPointer[RETURNPOS];
}