/***************************************
 * This program is modified for use in * 
 * benchmarking puposes. The complete  *
 * program is used in computer systems *
 * research at Chalmers University of  *
 * Technology, Sweden. Chalmers has    *
 * nothing to do with this program.    *
 *                                     *
 * Plese feel free to distribute this  *
 * program as you like.                *
 *                                     *
 * Peter Rundberg, biff@ce.chalmers.se *
 ***************************************/

/******************************************
 * A simple program that plays 4 in a row.
 * Peter Rundberg, biff@ce.chalmers.se
 *
 * TODO: * Smarter value function that can
 *         see blocked paths.
 *       * Parallel MiniMax algorithm.
 *       * OpenGL interface.
 ******************************************/
/******************************************
 * BUGS:
 *       * Alpha-Beta method 1 is not 
 *         working, why???
 ******************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/* 64 bit type needed */
typedef unsigned long long uint64;

/* The size of the playing field     */
/* It might not work if they are not */
/* set to ROWS 6 and COLS 7          */
#define ROWS 6
#define COLS 7

/* Bit patterns for finding sequences */
/* These are calculated in init_patterns() */
/* Patterns for finding 4 in a row */
uint64 C4VERT;
uint64 C4HORIZ;
uint64 C4UP_R; 
uint64 C4UP_L;
/* Patterns for finding 3 in a row */
uint64 C3VERT;
uint64 C3HORIZ;
uint64 C3UP_R;
uint64 C3UP_L;
/* Patterns for finding 2 in a row */
uint64 C2VERT;
uint64 C2HORIZ;
uint64 C2UP_R;
uint64 C2UP_L;

/* Rewards for positions */
#define C4REWARD 1000
#define C3REWARD 20
#define C2REWARD 5
#define C1REWARD 1
#define HUGE 100000

/* Recursion depth in MiniMax algorithm */
int DEPTH=3;

/* How offensive should the computer play.    */
/* Setting between -5 and 5 are valid and     */
/* a higher number means more aggresive play. */
int off=0;

/* Prototypes */
int minimax_comp_ab(int depth, uint64 b1, uint64 b2, int *col, int alpha, int beta);
int minimax_player_ab(int depth, uint64 b1, uint64 b2, int *col, int alpha, int beta);
int minimax_comp_ab2(int depth, uint64 b1, uint64 b2, int *col, int beta);
int minimax_player_ab2(int depth, uint64 b1, uint64 b2, int *col, int alpha);
int minimax_comp(int depth, uint64 b1, uint64 b2, int *col);
int minimax_player(int depth, uint64 b1, uint64 b2, int *col);
int bit_place_piece(int col, int player, uint64 *b1, uint64 *b2);

/* void init_patterns()                   */
/* Inits mask patterns with proper bits.  */
void init_patterns()
{
  int i;

  for (i=0;i<3;i++) {
    C4VERT = C4VERT | 1;
    C4VERT = C4VERT << (COLS);
  }
  C4VERT = C4VERT | 1;
  C3VERT = C4VERT >> COLS;
  C2VERT = C3VERT >> COLS;
  /*printf("Bit patterns: 0x%llx 0x%llx 0x%llx\n",C4VERT,C3VERT,C2VERT); */

  C4HORIZ = 0xf;
  C3HORIZ = C4HORIZ >> 1;
  C2HORIZ = C3HORIZ >> 1;
  /*printf("Bit patterns: 0x%llx 0x%llx 0x%llx\n",C4HORIZ,C3HORIZ,C2HORIZ); */
  
  for (i=0;i<3;i++) {
    C4UP_R = C4UP_R | 1;
    C4UP_R = C4UP_R << (COLS+1);
  }
  C4UP_R = C4UP_R | 1;
  C3UP_R = C4UP_R >> (COLS+1);
  C2UP_R = C3UP_R >> (COLS+1);
  /*printf("Bit patterns: 0x%llx 0x%llx 0x%llx\n",C4UP_R,C3UP_R,C2UP_R); */

  for (i=0;i<3;i++) {
    C4UP_L = C4UP_L | 8;
    C4UP_L = C4UP_L << (COLS-1);
  }
  C4UP_L = C4UP_L | 8;
  C3UP_L = C4UP_L >> (COLS-1);
  C2UP_L = C3UP_L >> (COLS-1);
  /*printf("Bit patterns: 0x%llx 0x%llx 0x%llx\n",C4UP_L,C3UP_L,C2UP_L); */
  /*exit(1); */
}


/* void init_board()                      */
/* Inits board with dots and height info  */
/* with zeros.                            */
void init_board(char b[COLS][ROWS+1])
{
  int i,j;

  for (i=0;i<COLS;i++)
    for (j=0;j<ROWS;j++)
      b[i][j]='.';
  
  for (i=0;i<COLS;i++)
    b[i][ROWS]=0;
}

/* void print_board()                     */
/* Prints board to terminal window.       */
/* No return values.                      */
void print_board(char b[COLS][ROWS+1])
{
  int i,j;

  putchar(' ');
  for (i=0;i<COLS;i++) /* Print height info */
    printf(" %d",b[i][ROWS]);
  putchar('\n');
  
  for (j=ROWS-1;j>=0;j--) { 
    printf("%d ",j);
    for (i=0;i<COLS;i++)  /* Print board */
      printf("%c ",b[i][j]);
    putchar('\n');
  }
  putchar(' ');
  for (i=0;i<COLS;i++)
    printf(" %d",i);
  putchar('\n');
  printf("----------------\n");
}

/* int place_piece(int col, int player)     */
/* Places piece on board...                 */
/* col: should be the column number from    */
/*      0 to COLS-1.                        */
/* player: should be 1 (o) or 2 (x)         */
/* Returns 0 on succes otherwise 1          */
int place_piece(int col, int player, char b[COLS][ROWS+1])
{
  if (col<0 || col>COLS-1) {
    printf("ERROR: Faulty column: %d.\n",col);
    return 1;
  }

  if (b[col][ROWS]>=ROWS) {
    /* printf("ERROR: Column %d is full.\n",col); */
    return 1;
  }
  
  if (player==1)
    b[col][(int) b[col][ROWS]]='o';
  else if (player==2)
    b[col][(int) b[col][ROWS]]='x';
  else {
    printf("ERROR: Unknown player.\n");
    return 1;
  }
  
  b[col][ROWS]++; 
     
  return 0;
}

/* Check if board is full */
int board_full(char b[COLS][ROWS+1])
{
  int i,temp=0;

  for (i=0;i<COLS;i++) /* Print height info */
   temp+=b[i][ROWS];
  if (temp == COLS*ROWS)
    return 1;
  return 0;
}

/* int find_winner()                     */
/* Finds winner on the board...          */
/* Returns 0 if no winner is persent,    */
/* otherwise the player number (1 or 2). */
/* Returns 3 if board is full.           */
int find_winner_p(char b[COLS][ROWS+1]) 
{
  unsigned int i,j;
  uint64 temp_board=0;  /* 64 bit integer, place ones in place of piece */
  uint64 one=1;
  char player;
  
  if (board_full(b))
    return 2;

  player='o';   /* Start with player 1... */
  
  for (j=0;j<ROWS;j++)
    for (i=0;i<COLS;i++)
      if (b[i][j]==player) {
	temp_board = temp_board | one << (i+j*COLS);
      }

  /*printf("player pieces: 0x%llx.\n",temp_board); */
 
  for (i=0;i<(ROWS-3)*COLS;i++) {  /* Finds vertical wins */
    if ((temp_board&C4VERT<<i)==C4VERT<<i)
      return 1;
  }
  for (i=0;i<ROWS;i++) {   /* Finds horizontal wins */
    for (j=0;j<COLS-3;j++) {
      if ((temp_board&C4HORIZ<<(j+i*COLS))==C4HORIZ<<(j+i*COLS))
	return 1;
    }
  }
  for (i=0;i<ROWS-3;i++) {   /* Finds up_right wins */
    for (j=0;j<COLS-3;j++) {
      if ((temp_board&C4UP_R<<(j+i*COLS))==C4UP_R<<(j+i*COLS))
	return 1;
    }
  }
  for (i=0;i<ROWS-3;i++) {   /* Finds up_left wins */
    for (j=0;j<COLS-3;j++) {
      if ((temp_board&C4UP_L<<(j+i*COLS))==C4UP_L<<(j+i*COLS))
	return 1;
    }
  } 
  return 0;
}

int find_winner_c(char b[COLS][ROWS+1]) 
{
  unsigned int i,j;
  uint64 temp_board=0;  /* 64 bit integer, place ones in place of piece */
  uint64 one=1;
  char player;
  
  if (board_full(b))
    return 2;
 
  player='x';   /* Then player 2... */
  
  for (j=0;j<ROWS;j++)
    for (i=0;i<COLS;i++)
      if (b[i][j]==player) {
	temp_board = temp_board | one << (i+j*COLS);
      }

  /* printf("comp pieces: 0x%llx.\n",temp_board); */

  for (i=0;i<(ROWS-3)*COLS;i++) {  /* Finds vertical wins */
    if ((temp_board&C4VERT<<i)==C4VERT<<i) {
      /* printf("Computer wins on vertical.\n"); */
      return 1;
    }
  }
  for (i=0;i<ROWS;i++) {   /* Finds horizontal wins */
    for (j=0;j<COLS-3;j++) {
      if ((temp_board&C4HORIZ<<(j+i*COLS))==C4HORIZ<<(j+i*COLS)) {
	/*printf("Computer wins on horizontal, i:%d, j:%d -- 0x%llx & 0x%llx.\n",i,j,temp_board,C4HORIZ<<(j+i*COLS)); */
	return 1;
      }
    }
  }
  for (i=0;i<ROWS-3;i++) {   /* Finds up_right wins */
    for (j=0;j<COLS-3;j++) {
      if ((temp_board&C4UP_R<<(j+i*COLS))==C4UP_R<<(j+i*COLS)) {
	/*printf("Computer wins on up_right.\n"); */
	return 1;
      }
    }
  }
  for (i=0;i<ROWS-3;i++) {   /* Finds up_left wins */
    for (j=0;j<COLS-3;j++) {
      if ((temp_board&C4UP_L<<(j+i*COLS))==C4UP_L<<(j+i*COLS)) {
	/*printf("Computer wins on up_left.\n"); */
	return 1;
      }
    }
  }

  return 0;
} 

/* int value()                           */
/* Values positions on the board...      */
/* Returns negative if human advantage,  */
/* positive on computer advantage.       */
/* See rewards at top of file.           */
int value(uint64 b1, uint64 b2) 
{
  int i,j,k;
  uint64 b,bo;
  float mod;
  int value=0;
  

  for (k=0;k<2;k++) {
    if (k==0) {
      b=b1;
      bo=b2;
      mod=-1+(float)off/10;
    } else {
      b=b2;
      bo=b1;
      mod=1+(float)off/10;
    }
    
    for (i=0;i<(ROWS-3)*COLS;i++) {  /* Finds vertical wins */
      if ((b&C4VERT<<i)==C4VERT<<i)
	value+=(int)C4REWARD*mod;
    }
    for (i=0;i<ROWS;i++) {   /* Finds horizontal wins */
      for (j=0;j<COLS-3;j++) {
	if ((b&C4HORIZ<<(j+i*6))==C4HORIZ<<(j+i*6))
	  value+=(int)C4REWARD*mod;
      }
    }
    for (i=0;i<ROWS-3;i++) {   /* Finds up_right wins */
      for (j=0;j<COLS-3;j++) {
	if ((b&C4UP_R<<(j+i*6))==C4UP_R<<(j+i*6))
	  value+=(int)C4REWARD*mod;
      }
    }
    for (i=0;i<ROWS-3;i++) {   /* Finds up_left wins */
      for (j=0;j<COLS-3;j++) {
	if ((b&C4UP_L<<(j+i*6))==C4UP_L<<(j+i*6))
	  value+=(int)C4REWARD*mod;
      }
    }
    /***/
    for (i=0;i<(ROWS-3)*COLS;i++) {  /* Finds vertical 3 in a row, that can win */
      if (((b&C3VERT<<i)==C3VERT<<i) && !(bo&C4VERT<<i)) 
	value+=(int)C3REWARD*mod;
    }
    for (i=0;i<ROWS;i++) {   /* Finds horizontal 3 in a row, that can win */
      for (j=0;j<COLS-2;j++) {
	if ((b&C3HORIZ<<(j+i*6))==C3HORIZ<<(j+i*6))
	  value+=(int)C3REWARD*mod;
      }
    }
    for (i=0;i<ROWS-3;i++) {   /* Finds up_right 3 in a row, that can win */
      for (j=0;j<COLS-3;j++) {
	if ((b&C3UP_R<<(j+i*6))==C3UP_R<<(j+i*6))
	  value+=(int)C3REWARD*mod;
      }
    }
    for (i=0;i<ROWS-3;i++) {   /* Finds up_left 3 in a row, that can win */
      for (j=0;j<COLS-3;j++) {
	if ((b&C3UP_L<<(j+i*6))==C3UP_L<<(j+i*6))
	  value+=(int)C3REWARD*mod;
      }
    }
    /***/
    for (i=0;i<(ROWS-3)*COLS;i++) {  /* Finds vertical 2 in a row, that can win */
      if (((b&C2VERT<<i)==C2VERT<<i) && !(bo&C4VERT<<i))
	value+=(int)C2REWARD*mod;
    }
    for (i=0;i<ROWS;i++) {   /* Finds horizontal 2 in a row, that can win */
      for (j=0;j<COLS-1;j++) {
	if ((b&C2HORIZ<<(j+i*6))==C2HORIZ<<(j+i*6))
	  value+=(int)C2REWARD*mod;
      }
    }
    for (i=0;i<ROWS-3;i++) {   /* Finds up_right 2 in a row, that can win */
      for (j=0;j<COLS-3;j++) {
	if ((b&C2UP_R<<(j+i*6))==C2UP_R<<(j+i*6))
	  value+=(int)C2REWARD*mod;
      }
    }
    for (i=0;i<ROWS-3;i++) {   /* Finds up_left 2 in a row, that can win */
      for (j=0;j<COLS-3;j++) {
	if ((b&C2UP_L<<(j+i*6))==C2UP_L<<(j+i*6))
	  value+=(int)C2REWARD*mod;
      }
    }
  } 
 
  return value;
} 

/* int think(char b[COLS][ROWS+1]) */
/* Initializer and caller for      */
/* computer brain.                 */
/* Returns which column computer   */
/* wants to place its piece in.    */
int think(char b[COLS][ROWS+1], int who, int ab)
{
  uint64 b1=0,b2=0;
  uint64 one=1;
  int i,j;
  char player;
  int col;

  player='o';   /* Start with player 1... (Human) */
  for (j=0;j<ROWS;j++)
    for (i=0;i<COLS;i++)
      if (b[i][j]==player) {
	b1 = b1 | one << (i+j*COLS);
      }
  player='x';   /* Then player 2... (Computer) */
  for (j=0;j<ROWS;j++)
    for (i=0;i<COLS;i++)
      if (b[i][j]==player) {
	b2 = b2 | one << (i+j*COLS);
      }

  if (ab==1) {
    if (who==2)
      minimax_comp_ab(1,b1,b2,&col,-HUGE,HUGE);
    if (who==1)
      minimax_player_ab(1,b1,b2,&col,-HUGE,HUGE);
  } else if (ab==2) {
    if (who==2)
      minimax_comp_ab2(1,b1,b2,&col,HUGE);
    if (who==1)
      minimax_player_ab2(1,b1,b2,&col,-HUGE);
  } else {
    if (who==2)
      minimax_comp(1,b1,b2,&col);
    if (who==1)
      minimax_player(1,b1,b2,&col);
  }
  
  bit_place_piece(col, 2, &b1, &b2);
  
  return col;
}

/******************************/
/* The "thinking" part of the */
/* computer player. AI??      */
/* Uses a MiniMax algorithm.  */
/* Recursive...               */
int bit_place_piece(int col, int player, uint64 *b1, uint64 *b2)
{
  uint64 board=*b1|*b2;
  uint64 one=1;
  int i;
  
  for (i=0;i<ROWS;i++) {
    if (!(board & one<<(i*COLS+col))) {
      if (player == 1)
	*b1=*b1 | one<<(i*COLS+col);
      else
	*b2=*b2 | one<<(i*COLS+col);
      /*printf("player: %d col: %d i: %d -- board: 0x%llx, b1: 0x%llx, b2: 0x%llx\n",player,col,i,board,*b1,*b2); */
      return 0;
    }
  }
  return 1;
}

int minimax_comp_ab(int depth, uint64 b1, uint64 b2, int *col, int alpha, int beta)
{
  int i,max=alpha,max_col=0,tmp;
  uint64 tmp_b;

  if (depth>=DEPTH) {
    return value(b1, b2);
  }

  for (i=0;i<COLS && max<beta;i++) { 
    tmp_b=b2;
    if (bit_place_piece(i,2,&b1,&tmp_b))
      continue;
    tmp=minimax_player_ab(depth+1, b1, tmp_b, col, max, beta);
    if (tmp>max) {
      max=tmp;
      max_col=i;
    }
  }
  
  *col=max_col;
  return max;
}

int minimax_player_ab(int depth, uint64 b1, uint64 b2, int *col, int alpha, int beta)
{
  int i,min=beta,min_col=0,tmp;
  uint64 tmp_b;
 
  if (depth>=DEPTH) {
    return value(b1, b2);
  }

  for (i=0;i<COLS && min>alpha;i++) { 
    tmp_b=b1;
    if (bit_place_piece(i,1,&tmp_b,&b2))
      continue;
    tmp=minimax_comp_ab(depth+1, tmp_b, b2, col, alpha ,min);
    if (tmp<=min) {  
      min=tmp;
      min_col=i;
    }
  }
  
  *col=min_col;
  return min;
}

int minimax_comp_ab2(int depth, uint64 b1, uint64 b2, int *col, int beta)
{
  int i,max=-HUGE,max_col=0,tmp;
  uint64 tmp_b;

  if (depth>=DEPTH) {
    return value(b1, b2);
  }

  for (i=0;i<COLS;i++) { 
    tmp_b=b2;
    if (bit_place_piece(i,2,&b1,&tmp_b))
      continue;
    tmp=minimax_player_ab2(depth+1, b1, tmp_b, col, max);
    if (tmp>max) {
      max=tmp;
      max_col=i;
    }
    if (max>beta)
      return max;
  }
  
  *col=max_col;
  return max;
}

int minimax_player_ab2(int depth, uint64 b1, uint64 b2, int *col, int alpha)
{
  int i,min=HUGE,min_col=0,tmp;
  uint64 tmp_b;
 
  if (depth>=DEPTH) {
    return value(b1, b2);
  }

  for (i=0;i<COLS;i++) { 
    tmp_b=b1;
    if (bit_place_piece(i,1,&tmp_b,&b2))
      continue;
    tmp=minimax_comp_ab2(depth+1, tmp_b, b2, col,min);
    if (tmp<=min) {  
      min=tmp;
      min_col=i;
    }
    if (min<alpha)
      return min;
  }
  
  *col=min_col;
  return min;
}

int minimax_comp(int depth, uint64 b1, uint64 b2, int *col)
{
  int i,max=-HUGE,max_col=0,tmp;
  uint64 tmp_b;

  if (depth>=DEPTH) {
    return value(b1, b2);
  }
  
  for (i=0;i<COLS;i++) { 
    tmp_b=b2;
    if (bit_place_piece(i,2,&b1,&tmp_b))
      continue;
    tmp=minimax_player(depth+1, b1, tmp_b, col);
    if (tmp>max) {
      max=tmp;
      max_col=i;
    }
  }
  
  *col=max_col;
  return max;
}

int minimax_player(int depth, uint64 b1, uint64 b2, int *col)
{
  int i,min=HUGE,min_col=0,tmp;
  uint64 tmp_b;
 
  if (depth>=DEPTH) {
    return value(b1, b2);
  }

  for (i=0;i<COLS;i++) { 
    tmp_b=b1;
    if (bit_place_piece(i,1,&tmp_b,&b2))
      continue;
    tmp=minimax_comp(depth+1, tmp_b, b2, col);
    if (tmp<=min) {  
      min=tmp;
      min_col=i;
    }
  }
  
  *col=min_col;
  return min;
}
/******************************/
/* End of "thinking" part...  */
/******************************/

/*************************/
/* Main routine          */
/* Starts up a game      */
/* Kinda boring, hu?     */
/*************************/

int main (int c, char *v[])
{
  /* Playing field is COLSxROWS and the extra row is for storing */
  /* height information for that column...                       */
  /* 0,0 is in the lower left corner and COLS-1,ROWS-1 the upper right.    */
  char b[COLS][ROWS+1];  
  int ab=0;
  int cvsc=1;
  int in;

  FILE *in_fp;
  
  fprintf(stderr,"Compile date: %s\n", COMPDATE);
  fprintf(stderr,"Compiler switches: %s\n", CFLAGS);
  
  in_fp=fopen(v[1],"r");
  if (!in_fp) {
    in_fp=fopen("test.in","r");
    if (!in_fp) {
      printf("ERROR: Could not open indata file\n");
      exit(1);
    }
  }

  fscanf(in_fp,"%d",&DEPTH);
  fscanf(in_fp,"%d",&ab);
  fclose(in_fp);
 
  printf("Recursion depth: %d\n", DEPTH);
  printf("Alpha-Beta pruning: %s\n", ab ? "on":"off");
  if (ab == 1) 
    printf("Using pruning method 1\n");
  if (ab == 2) 
    printf("Using pruning method 2\n");
    

  init_patterns();
  init_board(b);
  print_board(b);

  while (!(find_winner_p(b) || find_winner_c(b))) {
    if (cvsc)
      place_piece(think(b,1,ab),1,b);
    else {
      scanf("%d",&in);
      if (place_piece(in,1,b))
	continue;
    }
    place_piece(think(b,2,ab),2,b); 
    print_board(b);
  }

  if (find_winner_p(b)==1 && !find_winner_c(b))
    printf("The player is the winner.\n");
  if (find_winner_c(b)==1 && !find_winner_p(b))
    printf("The computer is the winner.\n");
  if ((find_winner_p(b)==2) || (find_winner_c(b)==1 && find_winner_p(b)))
    printf("It's a tie.\n");

  return 0;
}
