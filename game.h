#ifndef GAME_H
#define GAME_H

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


typedef struct Point{
  int x;
  int y;
}Point;

typedef struct Board{
char bB[BOARD_HEIGHT][BOARD_WIDTH];
char gB[BOARD_HEIGHT][BOARD_WIDTH];
char numPl;
char pDef[4];
Point *pDir[4];
Point *pPos[4];
}Board;


int game(Board *b);
void initBoard(Board* b);
int gameOver();
char* getPlayerColor(int pNum);
void printBoard(Board *b);
void updateBoard(Board* b);
void loadSnake(Point start, Point* end, int pNum,Board *b);
int loadGame(int numPl,Board *b);
void chageDirection(int pNum, Point p);
void printPlayerColor(int pNum);

#endif
