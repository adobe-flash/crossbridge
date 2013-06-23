#include "types.h"
#define TRUE 	1

int colthr[128];
boolean colwon[128];
int moves[44],plycnt;
int rows[8], dias[19];
int columns[128], height[128];

void reset()
{
  int i;

  plycnt = 0;
  for (i=0; i<19; i++)
    dias[i] = 0;
  for (i=0; i<8; i++) {
    columns[i] = 1;
    height[i] = 1;
    rows[i] = 0;
  }
}

void play_init()
{
  int i;

  for (i=8; i<128; i+=8) {
    colthr[i] = 1;
    colthr[i+7] = 2;
  }
  for (i=16; i<128; i+=16)
    colwon[i] = colwon[i+15] = TRUE;
  
  reset();
}

void printMoves()
{
  int i;

  for (i=1; i<=plycnt; i++)
    printf("%d", moves[i]);
}

/* return whether a move by player side in column n at height h would win */
/* non-vertically */
boolean wins(int n, int h, int sidemask)
{ 
  int x,y;

  sidemask <<= (2*n);
  x = rows[h] | sidemask;
  y = x & (x<<2);
  if ((y & (y<<4)) != 0)
    return TRUE;
  x = dias[5+n+h] | sidemask;
  y = x & (x<<2);
  if ((y & (y<<4)) != 0)
    return TRUE;
  x = dias[5+n-h] | sidemask;
  y = x & (x<<2);
  return (y & (y<<4)) != 0;
}

void backmove()
{
  int mask,d,h,n,side;

  side = plycnt&1;
  n = moves[plycnt--];
  h = --height[n];
  columns[n] >>= 1;
  mask = ~(1 << (2*n + side));
  rows[h] &= mask;
  dias[5+n+h] &= mask;
  dias[5+n-h] &= mask;
}

void makemove(int n) 
{
  int mask,d,h,side;

  moves[++plycnt] = n;
  side = plycnt&1;
  h = height[n]++;
  columns[n] = (columns[n] << 1) + side;
  mask = 1 << (2*n + side);
  rows[h] |= mask;
  dias[5+n+h] |= mask;
  dias[5+n-h] |= mask;
}
