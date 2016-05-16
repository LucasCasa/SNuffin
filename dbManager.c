#include "dbManager.h"

pthread_cond_t* cond;
pthread_cond_t* ready;
pthread_mutex_t* mutex;

char ExistUserDB(char * user){
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
  pthread_mutex_unlock(mutex);
  return c;
}
int validPasswordDB(char* user, char* password){
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
 return c;
}
int getHighScoreDB(char * user){
  pthread_mutex_lock(mutex);
  shmPointer[TYPEPOS] = GETHIGHSCORE;
  memcpy(shmPointer + FIRSTARGUMENT,user,strlen(user) + 1);
  pthread_cond_signal(cond);
  while(shmPointer[TYPEPOS] != READY){
     pthread_cond_wait(ready,mutex);
  }
  int i = atoi(shmPointer+ RETURNPOS);
  pthread_mutex_unlock(mutex);
  return i;
}
int setHighscoreDB(char* user, int value){
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
  pthread_mutex_lock(mutex);
  shmPointer[TYPEPOS] = CREATEUSER;
  memcpy(shmPointer + FIRSTARGUMENT,user, strlen(user) + 1);
  memcpy(shmPointer + SECONDARGUMENT,password, strlen(password) + 1);
  pthread_cond_signal(cond);
  while(shmPointer[TYPEPOS] != READY){
     pthread_cond_wait(ready,mutex);
  }
  int ret = shmPointer[RETURNPOS];
  pthread_mutex_unlock(mutex);
  return ret;
}
