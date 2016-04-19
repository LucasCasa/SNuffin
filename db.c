
#include "db.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
sqlite3 *db;
int init(){
     char *zErrMsg = 0;
     int rc;

     rc = sqlite3_open("players.db", &db);

     if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        //exit(0);
     }else{
        fprintf(stderr, "Opened database successfully\n");
     }
}

int main(){
  init();
  isUser("lucas");
  isPassword("lucas","hoyquiero");
  createUser("kuyum","kuyum");
  getHighscore("maggie");
  setHighscore("maggie",0);
  getHighscore("maggie");
  close2();
}

int isUser(char* user){

  char* sql = malloc(400);
  sqlite3_stmt* stmt;
  int c = 0;
  sprintf(sql,"SELECT Count(*) FROM player WHERE name = '%s'",user);
  printf("SQL: %s\n",sql );
  printError(sqlite3_prepare_v2(db,sql,-1,&stmt,NULL));
  printError(sqlite3_step(stmt));
  (sqlite3_column_int(stmt, 0) == 1)?printf("Exists\n"):printf("Don't Exist\n");
  printError(sqlite3_finalize(stmt));
  return 0;
}
int isPassword(char* user, char * pass){
  char* sql = malloc(400);
  sqlite3_stmt* stmt;
  char* result;
  int c = 0;
  sprintf(sql,"SELECT * FROM player WHERE name = '%s'",user);
  printf("SQL: %s\n",sql );
  printError(sqlite3_prepare_v2(db,sql,-1,&stmt,NULL) );
  printError(sqlite3_step(stmt) );
  result = sqlite3_column_text(stmt, 1);

  if(result != NULL && strcmp(result,pass) == 0){
    printf("Hay Match\n");
  }else{
    printf("No hay Match\n");
  }
  printError(sqlite3_finalize(stmt));
  return 0;
}

int createUser(char * user, char * pass){
char * sql = malloc(500);
char *zErrMsg = 0;
sprintf(sql,"INSERT INTO player VALUES ('%s','%s',%d)",user,pass,0);
printf("SQL: %s\n",sql);
int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Records created successfully\n");
   }
}
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
int getHighscore(char * player){
  char* sql = malloc(400);
  sqlite3_stmt* stmt;
  int c = 0;
  sprintf(sql,"SELECT * FROM player WHERE name = '%s'",player);
  printf("SQL: %s\n",sql );
  printError(sqlite3_prepare_v2(db,sql,-1,&stmt,NULL));
  printError(sqlite3_step(stmt));
  int result = sqlite3_column_int(stmt, 2);
  printf("Score: %d\n",result );
  printError(sqlite3_finalize(stmt));
  return 0;
}

int setHighscore(char * player,int highscore){
  char* sql = malloc(400);
  char* zErrMsg = 0;
  sqlite3_stmt* stmt;
  int c = 0;
  int rc = 0;
  sprintf(sql,"UPDATE player set score = %d where name = '%s' ",highscore,player);
  printf("SQL: %s\n",sql );
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Operation done successfully\n");
   }
}
int close2(){
printf("entre\n");
int rs = sqlite3_close_v2(db);

  if( rs ){
    fprintf(stderr, "Can't close database: %s\n", sqlite3_errmsg(db));
      //exit(0);
  }else{
    fprintf(stderr, "Closed database successfully\n");
  }
}

int printError(int rv){
  if(rv != 100 && rv != 0){
    fprintf(stderr, "%s\n", sqlite3_errmsg(db));
  }
  return rv;
}
