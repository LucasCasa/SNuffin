#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "game.h"

/*
typedef struct Point{
  int x;
  int y;
}Point;

typedef struct Board{
char bB[BOARD_HEIGHT][BOARD_WIDTH];
char gB[BOARD_HEIGHT][BOARD_WIDTH];
char numPl;
char pDef[4];
(Point* pDir)[4];
(Point* pPos)[4];
}Board;

*/
int main(){
 Board b;
 Board* board = &b;
 initGraphicBoard(board);
 loadGame(4,board);
 game(board);
 return 0;
}
int game(Board *b){
  struct timespec timer;
  timer.tv_sec = 0;
  timer.tv_nsec = 1000000 * UPDATE_MILLISECONDS;

  while(!gameOver(b)){
    nanosleep(&timer,NULL);
    updateBoard(b);
    printBoard(b); /* for testing*/
  }
  return 0;
  /*Show the winner, update db scores */
}
void initGraphicBoard(Board* b){
  for(int i = 0;i<BOARD_WIDTH;i++){
    for(int j = 0; j<BOARD_HEIGHT;j++){
      b->gB[i][j] = 0;
      b->bB[i][j] = 0;
      //b->graphicBoard[i][j]
    }
  }
}
/*this function is called by the server when it receives a change in direction by the client
TBD how the direction is(?) passed by the client*/
void chageDirection(int pNum, char direction){

}

/*Starts a new game,Probably the board should be created here, TBD*/
int loadGame(int numPl,Board *b){
  Point p4s = {BOARD_WIDTH / 2, BOARD_HEIGHT - 1};
  Point* p4f = malloc(sizeof(Point));
  p4f->x = BOARD_WIDTH / 2;
  p4f->y = BOARD_HEIGHT - 5;
  Point p3s = {BOARD_WIDTH / 2, 1};
  Point* p3f = malloc(sizeof(Point));
  p3f->x = BOARD_WIDTH / 2;
  p3f->y = 1;
  Point p2s = {1,BOARD_HEIGHT / 2};
  Point* p2f = malloc(sizeof(Point));
  p2f->x = 5;
  p2f->y = BOARD_HEIGHT / 2;
  Point p1s = {BOARD_WIDTH - 1,BOARD_HEIGHT / 2};
  Point* p1f = malloc(sizeof(Point));
  p1f->x = BOARD_WIDTH - 5;
  p1f->y = BOARD_HEIGHT / 2;
  switch (numPl) {
    case 4:
      loadSnake(p4s,p4f,4,b); /* Abajo */
    case 3:
      loadSnake(p3s,p3f,3,b); /* Arriba */
    case 2:
      loadSnake(p2s,p2f,2,b); /* Esquina Derecha*/
      loadSnake(p1s,p1f,1,b); /* Esquina izquierda*/
    break;
    default:
      return 1;
    break;

  }
  b->numPl = numPl;
  return 0;
}
/*Loads snake at the start of the game, the end determines the head of the snake*/
void loadSnake(Point start, Point *end, int pNum,Board *b){
  int px = 0;
  int py = 0;
  Point *pDir = malloc(sizeof(Point));
  pDir->x = 0;
  pDir->y = 0;

  if(start.x == end->x){
    if(start.y < end->y){
      pDir->y = 1;
    }else{
      pDir->y = -1;
    }
  }else if(start.y == end->y){
    if(start.x < end->x){
      pDir->x = 1;
    }else{
      pDir->x = -1;
    }
  }else{
    printf("Me mande una cagada\n");
  }

  while(start.x + px != end->x || start.y + py != end->y){
    b->bB[b->pPos[pNum-1]->x + b->pDir[pNum-1]->x][b->pPos[pNum-1]->y + b->pDir[pNum-1]->y] = TAIL_CHAR;
    b->gB[b->pPos[pNum-1]->x + b->pDir[pNum-1]->x][b->pPos[pNum-1]->y + b->pDir[pNum-1]->y] = pNum;
    px += pDir->x;
    py += pDir->y;
  }
  b->pPos[pNum-1] = end;
  b->pDir[pNum-1] = pDir;
}
/* Timed  function that updates the board every cicle */
void updateBoard(Board* b){
  for(int i = 1; i<b->numPl+1;i++){
    if(b->pDef[i] == 0){
      if(b->bB[b->pPos[i]->x + b->pDir[i]->x][b->pPos[i]->y + b->pDir[i]->y] == TAIL_CHAR){
        b->pDef[i] = 1;
      }else{
        b->bB[b->pPos[i]->x + b->pDir[i]->x][b->pPos[i]->y + b->pDir[i]->y] = TAIL_CHAR;
        b->gB[b->pPos[i]->x + b->pDir[i]->x][b->pPos[i]->y + b->pDir[i]->y] = i;
      }
    }
  }
}
void printBoard(Board *b){
  printf("Start\n");
  for(int i = 0;i<BOARD_WIDTH;i++){
    for(int j = 0; j<BOARD_HEIGHT;j++){
      printPlayerColor(b->gB[i][j]);
      //b->graphicBoard[i][j]
    }
    printf("\n");
  }
}
char* getPlayerColor(int pNum){
  switch (pNum) {
    case 1:
    return PLAYER1_COLOR;
    case 2:
    return PLAYER2_COLOR;
    case 3:
    return PLAYER3_COLOR;
    case 4:
    return PLAYER4_COLOR;
    default:
    return COLOR_RESET;

  }
}
void printPlayerColor(int pNum){
  switch (pNum) {
    case 1:
      printf(PLAYER1_COLOR "a" COLOR_RESET);
    break;
    case 2:
      printf(PLAYER2_COLOR "v" COLOR_RESET);
    break;
    case 3:
      printf(PLAYER3_COLOR "c" COLOR_RESET);
    break;
    case 4:
      printf(PLAYER4_COLOR "d" COLOR_RESET);
    break;
    default:
    printf("0");
  }
}
int gameOver(){
  return 0;
}
