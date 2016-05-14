
#include "db.h"


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

int manageDataBase(){
   printf("Inicio DB\n");
  init();

  // createUser("maggie","maggie");
  // isUser("lucas");
  // setHighscore("kuyum",420);
  // getHighscore("lucas");
  // getHighscore("maggie");
  // getHighscore("kuyum");
  printf("Entro al While\n");
  while(1){
    if(shmPointer[TYPEPOS] != READY){
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

   }
    //printf("Base de Datos sigue funcionado\n");
  }
  /* Abro el pipe y me pongo a esuchar...
  */
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
  c = sqlite3_column_int(stmt, 0);
  (c == 1)?printf("Exists\n"):printf("Don't Exist\n");
  printError(sqlite3_finalize(stmt));
  return c;
}
int isPassword(char* user, char * pass){
  char* sql = malloc(400);
  sqlite3_stmt* stmt;
  char* result;
  int c = 0;
  sprintf(sql,"SELECT * FROM player WHERE name = '%s'",user);
  //printf("SQL: %s\n",sql );
  printError(sqlite3_prepare_v2(db,sql,-1,&stmt,NULL) );
  printError(sqlite3_step(stmt) );
  result = sqlite3_column_text(stmt, 1);
  printError(sqlite3_finalize(stmt));
  if(result != NULL && strcmp(result,pass) == 0){
    return 1;
  }else{
    return 0;
  }
}

int createUser(char * user, char * pass){
char * sql = malloc(500);
char *zErrMsg = 0;
sprintf(sql,"INSERT INTO player VALUES ('%s','%s',%d)",user,pass,0);
//printf("SQL: %s\n",sql);
int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return 1;
   }else{
      fprintf(stdout, "Records created successfully\n");
      return 0;
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
  return result;
}

int setHighscore(char * player,int highscore){
  char* sql = malloc(400);
  char* zErrMsg = 0;
  sqlite3_stmt* stmt;
  int c = 0;
  int rc = 0;
  sprintf(sql,"UPDATE player set score = %d where name = '%s' ",highscore,player);
  //printf("SQL: %s\n",sql );
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
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
      //exit(0);
  }else{
    fprintf(stderr, "Closed database successfully\n");
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
    //printf("%d\n", digits);
    aux = aux / 10;
  }
  pos[digits] = 0;
  while(value > 0){
    //printf("%d\n", digits);
    pos[--digits] = value % 10 + '0';
    value = value / 10;
  }
  //printf("%s\n",pos );
}
