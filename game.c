#define BOARD_WIDTH   80
#define BOARD_HEIGHT  50
#define TAIL_CHAR     '·'

#define PLAYER1_COLOR /* Color1*/ 1
#define PLAYER2_COLOR /* Color2*/ 2
#define PLAYER3_COLOR /* Color3*/ 3
#define PLAYER4_COLOR /* Color4*/ 4

typedef struct Point{
  int x;
  int y;
}Point;

typedef struct Board{
char[BOARD_HEIGHT][BOARD_WIDTH]  backBoard;
char[BOARD_HEIGHT][BOARD_WIDTH]  graphicBoard;
char numberOfPlayers;
Point[4] playerDirection;
Point[4] playerPosition;
}Board;


int main(){
 Board b;

}

int chageDirection(int playerNumber, char direction){

}

int loadGame(int numberOfPlayers){
  switch (numberOfPlayers) {
    case 4:
      loadSnake(/* Abajo */);
    case 3:
      loadSnake(/* Arriba */);
    case 2:
      loadSnake(/* Esquina Derecha*/);
      loadSnake(/* Esquina izquierda*/);
    break;
    default:
      return 0;
    break;

  }

}

int loadSnake(point start, point end){


}

int updateBoard(Board b){
  for(int i = 0; i<b.numberOfPlayers;i++){
    if(b.backBoard[b.playerPosition[i].x + b.playerDirection[i].x][b.playerPosition[i].y + b.playerDirection[i].y] == TAIL_CHAR){
      playerDefeated(i);
    }else{
      b.backBoard[b.playerPosition[i].x + b.playerDirection[i].x][b.playerPosition[i].y + b.playerDirection[i].y] = TAIL_CHAR;
      b.graphicBoard[b.playerPosition[i].x + b.playerDirection[i].x][b.playerPosition[i].y + b.playerDirection[i].y] = getPlayerColor(i);
    }
  }
}

int playerDefeated(int playerNumber){

}
