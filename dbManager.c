#include "dbManager.h"

pthread_cond_t* cond;
pthread_cond_t* ready;
pthread_mutex_t* mutex;

char ExistUserDB(char * user){
  //sem_wait(&semDB);
  cond  = (pthread_cond_t*) (shmPointer + COND_OFFSET);
  ready = (pthread_cond_t*) (shmPointer + COND2_OFFSET);
  mutex = (pthread_mutex_t*)(shmPointer + MUTEX_OFFSET);

  pthread_mutex_lock(mutex);
  shmPointer[TYPEPOS] = ISUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user));
  shmPointer[strlen(user) + 1 ] = 0;
  pthread_cond_signal(cond);
  while(shmPointer[TYPEPOS] != READY){
     pthread_cond_wait(ready,mutex);
  }
  char c = shmPointer[RETURNPOS];
  printf("Recibi de la base %d\n",c );
  //sem_post(&semDB);
  pthread_mutex_unlock(mutex);
  return c;
}
int validPasswordDB(char* user, char* password){
 //sem_wait(&semDB);
 pthread_mutex_lock(mutex);
 shmPointer[TYPEPOS] = ISPASSWORD;
 memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user)+1);
 memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
 pthread_cond_signal(cond);
 while(shmPointer[TYPEPOS] != READY){
    pthread_cond_wait(ready,mutex);
 }
 char c = shmPointer[RETURNPOS];
 pthread_mutex_unlock(mutex);
 //sem_post(&semDB);
 return c;
}
int getHighScoreDB(char * user){
  //sem_wait(&semDB);
  pthread_mutex_lock(mutex);
  shmPointer[TYPEPOS] = GETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user) + 1);
  pthread_cond_signal(cond);
  while(shmPointer[TYPEPOS] != READY){
     pthread_cond_wait(ready,mutex);
  }
  int i = atoi(shmPointer+ RETURNPOS);
  pthread_mutex_unlock(mutex);
  //sem_post(&semDB);
  return i;
}
int setHighscoreDB(char* user, int value){
  //sem_wait(&semDB);
  pthread_mutex_lock(mutex);
  shmPointer[TYPEPOS] = SETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT, user, strlen(user) + 1);
  sprintf(shmPointer + SECONDARGUMENT,"%d", value);
  pthread_cond_signal(cond);
  while(shmPointer[TYPEPOS] != READY){
     pthread_cond_wait(ready,mutex);
  }
  int ret = shmPointer[RETURNPOS];
  pthread_mutex_unlock(mutex);
  return ret;
}
int createUserDB(char* user, char* password){
  //sem_wait(&semDB);
  printf("Creo user\n");
  pthread_mutex_lock(mutex);
  printf("estoy en el mutex\n");
  shmPointer[TYPEPOS] = CREATEUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user, strlen(user) + 1);
  memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
  printf("llegue a la condicion\n");
  pthread_cond_signal(cond);
  while(shmPointer[TYPEPOS] != READY){
     pthread_cond_wait(ready,mutex);
  }
  printf("pase la condicion\n");
  int ret = shmPointer[RETURNPOS];
  pthread_mutex_unlock(mutex);
  printf("Suelto el mutex\n");
  return ret;
}
