#include "db.h"

//SIN PROBLEMAS DE MEMORIA?
sqlite3 *db;

int init(){
     //char *zErrMsg = 0;
     int rc;

     rc = sqlite3_open("players.db", &db);

     if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;//exit(0);
     }else{
        fprintf(stderr, "Opened database successfully\n");
        return 0;
     }
}

int manageDataBase(){
  init();
  pthread_cond_t* cond =(pthread_cond_t*)(shmPointer + COND_OFFSET);
  pthread_cond_t* ready =(pthread_cond_t*)(shmPointer + COND2_OFFSET);
  pthread_mutex_t* mutex = (pthread_mutex_t*)(shmPointer + MUTEX_OFFSET);
  while(1){
     pthread_mutex_lock(mutex);
     while(shmPointer[TYPEPOS] == READY){
        pthread_cond_wait(cond, mutex);
     }
      switch(shmPointer[TYPEPOS]){
        case ISUSER:
          shmPointer[RETURNPOS] = (char) isUser(shmPointer + FIRSTARGUMENT);
          shmPointer[TYPEPOS] = READY;
        break;
        case ISPASSWORD:
          shmPointer[RETURNPOS] = (char) isPassword(shmPointer + FIRSTARGUMENT, shmPointer + SECONDARGUMENT);
          shmPointer[TYPEPOS] = READY;
        break;
        case GETHIGHSCORE:
          loadInt(shmPointer + RETURNPOS,getHighscore(shmPointer + FIRSTARGUMENT));
          shmPointer[TYPEPOS] = READY;
        break;
        case SETHIGHSCORE:
          shmPointer[RETURNPOS] = setHighscore(shmPointer + FIRSTARGUMENT, atoi(shmPointer + SECONDARGUMENT));
          shmPointer[TYPEPOS] = READY;
        break;
        case CREATEUSER:
          shmPointer[RETURNPOS] = createUser(shmPointer + FIRSTARGUMENT, shmPointer + SECONDARGUMENT);
          shmPointer[TYPEPOS] = READY;
        break;
        default:
          printf("Comando enviado a la DB invalido\n");
        break;
      }
      pthread_cond_signal(ready);
      pthread_mutex_unlock(mutex);
  }

  close2();
}

int isUser(char* user){

  char* sql = malloc(400);
  sqlite3_stmt* stmt;
  int c = 0;
  sprintf(sql,"SELECT Count(*) FROM player WHERE name = '%s'",user);
  printError(sqlite3_prepare_v2(db,sql,-1,&stmt,NULL));
  printError(sqlite3_step(stmt));
  c = sqlite3_column_int(stmt, 0);
  printError(sqlite3_finalize(stmt));
  free(sql);
  return c;
}
int isPassword(char* user, char * pass){
  char* sql = malloc(400);
  sqlite3_stmt* stmt;
  const unsigned char* result;
  //int c = 0;
  sprintf(sql,"SELECT * FROM player WHERE name = '%s'",user);
  printError(sqlite3_prepare_v2(db,sql,-1,&stmt,NULL) );
  printError(sqlite3_step(stmt) );
  result = sqlite3_column_text(stmt, 1);
  printError(sqlite3_finalize(stmt));
  free(sql);
  if(result != NULL && strcmp((char*)result,pass) == 0){
    return 1;
  }else{
    return 0;
  }
}

int createUser(char * user, char * pass){
char * sql = malloc(500);
char *zErrMsg = 0;
sprintf(sql,"INSERT INTO player VALUES ('%s','%s',%d)",user,pass,0);
int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
free(sql);
if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return 1;
   }else{
      fprintf(stdout, "Records created successfully\n");
      return 0;
   }

}

int getHighscore(char * player){
  char* sql = malloc(400);
  sqlite3_stmt* stmt;
  //int c = 0;
  sprintf(sql,"SELECT * FROM player WHERE name = '%s'",player);
  printError(sqlite3_prepare_v2(db,sql,-1,&stmt,NULL));
  printError(sqlite3_step(stmt));
  int result = sqlite3_column_int(stmt, 2);
  free(sql);
  printError(sqlite3_finalize(stmt));
  return result;
}

int setHighscore(char * player,int highscore){
  char* sql = malloc(400);
  char* zErrMsg = 0;
  int rc = 0;
  sprintf(sql,"UPDATE player set score = %d where name = '%s' ",highscore,player);
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  free(sql);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return 1;
   }else{
      fprintf(stdout, "Operation done successfully\n");
      return 0;
   }
}
int close2(){
printf("entre\n");
int rs = sqlite3_close_v2(db);

  if( rs ){
    fprintf(stderr, "Can't close database: %s\n", sqlite3_errmsg(db));
      return 1;
  }else{
    fprintf(stderr, "Closed database successfully\n");
    return 0;
  }
}

int printError(int rv){
  if(rv != 100 && rv != 101 && rv != 0){
    fprintf(stderr, "%s\n", sqlite3_errmsg(db));
  }
  return rv;
}
void loadInt(char* pos, int value){
  int aux = value;
  int digits = 0;
  while(aux > 0){
    digits++;
    aux = aux / 10;
  }
  pos[digits] = 0;
  while(value > 0){
    pos[--digits] = value % 10 + '0';
    value = value / 10;
  }
}
