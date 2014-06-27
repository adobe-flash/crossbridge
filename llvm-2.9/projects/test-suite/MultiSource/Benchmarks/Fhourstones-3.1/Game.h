// Fhourstones 3.0 Board Logic
// (http://www.cwi.nl/~tromp/c4/fhour.html)
//
// implementation of the well-known game
// usually played on a vertical board of 7 columns by 6 rows,
// where 2 players take turns in dropping counters in a column.
// the first player to get four of his counters
// in a horizontal, vertical or diagonal row, wins the game.
// if neither player has won after 42 moves, then the game is drawn.
//
// This software is copyright (c) 1996-2005 by
//      John Tromp
//      Insulindeweg 908
//      1095 DX Amsterdam
//      Netherlands
// E-mail: tromp@cwi.nl
//
// This notice must not be removed.
// This software must not be sold for profit.
// You may redistribute if your distributees have the
// same rights and restrictions.

#include <stdio.h>
#include <stdlib.h>
#define WIDTH 7
#define HEIGHT 6
// bitmask corresponds to board as follows in 7x6 case:
//  .  .  .  .  .  .  .  TOP
//  5 12 19 26 33 40 47
//  4 11 18 25 32 39 46
//  3 10 17 24 31 38 45
//  2  9 16 23 30 37 44
//  1  8 15 22 29 36 43
//  0  7 14 21 28 35 42  BOTTOM
#define H1 (HEIGHT+1)
#define H2 (HEIGHT+2)
#define SIZE (HEIGHT*WIDTH)
#define SIZE1 (H1*WIDTH)
#define ALL1 (((uint64)1<<SIZE1)-(uint64)1) // assumes SIZE1 < 63
#define COL1 (((uint64)1<<H1)-(uint64)1)
#define BOTTOM (ALL1 / COL1) // has bits i*H1 set
#define TOP (BOTTOM << HEIGHT)

#include <sys/types.h>
#include <stdint.h>
typedef uint64_t uint64;
typedef int64_t int64;

uint64 color[2];  // black and white bitboard
int moves[SIZE],nplies;
char height[WIDTH]; // holds bit index of lowest free square
  
void reset()
{
  int i;
  nplies = 0;
  color[0] = color[1] = 0;
  for (i=0; i<WIDTH; i++)
    height[i] = (char)(H1*i);
}

uint64 positioncode()
{
  return color[nplies&1] + color[0] + color[1] + BOTTOM;
// color[0] + color[1] + BOTTOM forms bitmap of heights
// so that positioncode() is a complete board encoding
}

void printMoves()
{
  int i;
 
  for (i=0; i<nplies; i++)
    printf("%d", 1+moves[i]);
}

// return whether newboard lacks overflowing column
int islegal(uint64 newboard)
{
  return (newboard & TOP) == 0;
}

// return whether columns col has room
int isplayable(int col)
{
  return islegal(color[nplies&1] | ((uint64)1 << height[col]));
}

// return whether newboard includes a win
int haswon(uint64 newboard)
{
  uint64 y = newboard & (newboard>>HEIGHT);
  if ((y & (y >> 2*HEIGHT)) != 0) // check \ diagonal
    return 1;
  y = newboard & (newboard>>H1);
  if ((y & (y >> 2*H1)) != 0) // check horizontal -
    return 1;
  y = newboard & (newboard>>H2); // check / diagonal
  if ((y & (y >> 2*H2)) != 0)
    return 1;
  y = newboard & (newboard>>1); // check vertical |
  return (y & (y >> 2)) != 0;
}

// return whether newboard is legal and includes a win
int islegalhaswon(uint64 newboard)
{
  return islegal(newboard) && haswon(newboard);
}

void backmove()
{
  int n;

  n = moves[--nplies];
  color[nplies&1] ^= (uint64)1<<--height[n];
}

void makemove(int n) 
{
  color[nplies&1] ^= (uint64)1<<height[n]++;
  moves[nplies++] = n;
}
