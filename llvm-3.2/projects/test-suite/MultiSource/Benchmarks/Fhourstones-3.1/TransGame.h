// Java Fhourstones 3.1 Transposition table logic
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

#include "Game.h"

#define LOCKSIZE 26
#define TRANSIZE 8306069
  // should be a prime no less than about 2^{SIZE1-LOCKSIZE}
#define SYMMREC 10 // symmetry normalize first SYMMREC moves
#define UNKNOWN 0
#define LOSS 1
#define DRAWLOSS 2
#define DRAW 3
#define DRAWWIN 4
#define WIN 5
#define LOSSWIN 6

typedef struct {
  unsigned biglock:LOCKSIZE;
  unsigned bigwork:6;
  unsigned newlock:LOCKSIZE;
  unsigned newscore:3;
  unsigned bigscore:3;
} hashentry;

unsigned int htindex, lock;
hashentry *ht;
  
uint64 posed; // counts transtore calls
  
void trans_init()
{
  ht = (hashentry *)calloc(TRANSIZE, sizeof(hashentry));
}
  
void emptyTT()
{
  int i;

  for (i=0; i<TRANSIZE; i++) {
    ht[i] = (hashentry){0,0,0,0,0};
  }
  posed = 0;
}
  
void hash()
{
  uint64 htmp, htemp = positioncode();
  if (nplies < SYMMREC) { // try symmetry recognition by reversing columns
    uint64 htemp2 = 0;
    for (htmp=htemp; htmp!=0; htmp>>=H1)
      htemp2 = htemp2<<H1 | (htmp & COL1);
    if (htemp2 < htemp)
      htemp = htemp2;
  }
  lock = (unsigned int)(htemp >> (SIZE1-LOCKSIZE));
  htindex = (unsigned int)(htemp % TRANSIZE);
}
  
int transpose()
{
  hashentry he;
  int biglock,newlock;

  hash();
  he = ht[htindex];
  if (he.biglock == lock)
    return he.bigscore;
  if (he.newlock == lock)
    return he.newscore;
  return UNKNOWN;
}
  
void transtore(int x, unsigned int lock, int score, int work)
{
  hashentry he;

  posed++;
  he = ht[x];
  if (he.biglock == lock || work >= he.bigwork) {
    he.biglock = lock;
    he.bigscore = score;
    he.bigwork = work;
  } else {
    he.newlock = lock;
    he.newscore = score;
  }
  ht[x] = he;
}

void htstat()      /* some statistics on hash table performance */
{
  int total, i;
  int typecnt[8];                /* bound type stats */
  hashentry he;
 
  for (i=0; i<8; i++)
    typecnt[i] = 0;
  for (i=0; i<TRANSIZE; i++) {
    he = ht[i];
    if (he.biglock != 0)
      typecnt[he.bigscore]++;
    if (he.newlock != 0)
      typecnt[he.newscore]++;
  }
  for (total=0,i=LOSS; i<=WIN; i++)
    total += typecnt[i];
  if (total > 0) {
    printf("- %5.3f  < %5.3f  = %5.3f  > %5.3f  + %5.3f\n",
      typecnt[LOSS]/(double)total, typecnt[DRAWLOSS]/(double)total,
      typecnt[DRAW]/(double)total, typecnt[DRAWWIN]/(double)total,
      typecnt[WIN]/(double)total);
  }
}
