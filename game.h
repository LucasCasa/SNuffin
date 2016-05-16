#ifndef GAME_H
#define GAME_H

#include "structs.h"
#define BOARD_WIDTH   20
#define BOARD_HEIGHT  20
#define TAIL_CHAR     '-'
#define UPDATE_MILLISECONDS  500

#define PLAYER1_COLOR   "\x1b[31;41m"
#define PLAYER2_COLOR   "\x1b[32;42m"
#define PLAYER3_COLOR   "\x1b[34;44m"
#define PLAYER4_COLOR   "\x1b[35;45m"
#define COLOR_RESET     "\x1b[0m"
/* I should mark the head somehow*/


typedef struct GameBoard{
char bB[BOARD_HEIGHT][BOARD_WIDTH];
char gB[BOARD_HEIGHT][BOARD_WIDTH];
char numPl;
char pDef[4];
Point *pDir[4];
Point *pPos[4];
}GameBoard;

extern void playerLost(int);
void initBoard(GameBoard* b);
void loadSnakeNumber(int nSnake,GameBoard* b);
/*char* getPlayerColor(int pNum);
void printBoard(GameBoard *b);*/
void updateBoard(GameBoard* b);
void loadSnake(Point start, Point* end, int pNum,GameBoard *b);
void updateMovementDirection(int pNum, Point p);
int outOfBoard(Point* pPos);
//void printPlayerColor(int pNum);

#endif
