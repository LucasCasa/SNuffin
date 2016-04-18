
#include "db.h"
#include <sqlite3.h>
#include <stdlib.h>
/*sqlite3 *db;*/
int init(){
  /*   char *zErrMsg = 0;
     int rc;

     rc = sqlite3_open("players.db", &db);

     if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        //exit(0);
     }else{
        fprintf(stderr, "Opened database successfully\n");
     }*/
}
int main(){
/*init();*/
isUser("lucas");
close();
}
int isUser(char* user){
  sqlite3 *db;
  sqlite3_open("players.db", &db);
  char* sql = malloc(650);
  sqlite3_stmt* stmt;
  int c = 0;
  sprintf(sql,"SELECT Count(*) FROM player"/*WHERE player = %s,user*/);
  printf("%d\n",sqlite3_prepare_v2(db,sql,sizeof(sql),&stmt,NULL) );
  printf("%d\n",sqlite3_step(stmt) );
  printf("%d\n",sqlite3_column_int(stmt, 0) );
  sqlite3_finalize(stmt);
  return 0;
}
int isPassword(char* user, char * pass){

}

int createUser(char * user, char * pass){

}

int getHighscore(char * player){

}

int setHighscore(char * player,int highscore){

}
int close(){
//  sqlite3_close(db);
}
