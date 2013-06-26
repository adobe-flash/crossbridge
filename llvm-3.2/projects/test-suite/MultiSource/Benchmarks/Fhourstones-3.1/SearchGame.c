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

#include "TransGame.h"
#include <sys/time.h>
 
#define BOOKPLY 0  // full-width search up to this depth
#define REPORTPLY -1

uint64 millisecs()
{
  static int64 Time = 0;
  return Time++; // No time count for LLVM
}

int history[2][SIZE1];
uint64 nodes, msecs;

int min(int x, int y) { return x<y ? x : y; }
int max(int x, int y) { return x>y ? x : y; }

void inithistory()
{
  int side,i,h;
  for (side=0; side<2; side++)
    for (i=0; i<(WIDTH+1)/2; i++)
      for (h=0; h<H1/2; h++)
        history[side][H1*i+h] = history[side][H1*(WIDTH-1-i)+HEIGHT-1-h] =
        history[side][H1*i+HEIGHT-1-h] = history[side][H1*(WIDTH-1-i)+h] =
         4+min(3,i) + max(-1,min(3,h)-max(0,3-i)) + min(3,min(i,h)) + min(3,h);
}

int ab(int alpha, int beta)
{
  int besti,i,j,l,v,val,score,ttscore;
  int winontop,work;
  int nav,av[WIDTH];
  uint64 poscnt,newbrd,other;
  int side,otherside;
  unsigned int hashindx,hashlock;

  nodes++;
  if (nplies == SIZE-1) // one move left
    return DRAW; // by assumption, player to move can't win
  otherside = (side = nplies & 1) ^ 1;
  other = color[otherside];
  for (i = nav = 0; i < WIDTH; i++) {
    newbrd = other | ((uint64)1 << height[i]); // check opponent move
    if (!islegal(newbrd)) 
      continue;
    winontop = islegalhaswon(other | ((uint64)2 << height[i]));
    if (haswon(newbrd)) { // immediate threat
      if (winontop) // can't stop double threat
        return LOSS;
      nav = 0; // forced move
      av[nav++] = i;
      while (++i < WIDTH)
        if (islegalhaswon(other | ((uint64)1 << height[i])))
          return LOSS;
      break;
    }
    if (!winontop)
      av[nav++] = i;
  }
  if (nav == 0)
    return LOSS;
  if (nplies == SIZE-2) // two moves left
    return DRAW; // opponent has no win either
  if (nav == 1) {
    makemove(av[0]);
    score = LOSSWIN-ab(LOSSWIN-beta,LOSSWIN-alpha);
    backmove();
    return score;
  }
  ttscore = transpose();
  if (ttscore != UNKNOWN) {
    if (ttscore == DRAWLOSS) {
      if ((beta = DRAW) <= alpha)
        return ttscore;
    } else if (ttscore == DRAWWIN) {
      if ((alpha = DRAW) >= beta)
        return ttscore;
    } else return ttscore; // exact score
  }
  hashindx = htindex;
  hashlock = lock;
  poscnt = posed;
  besti=0;
  score = LOSS;
  for (i = 0; i < nav; i++) {
    val = history[side][(int)height[av[l = i]]];
    for (j = i+1; j < nav; j++) {
      v = history[side][(int)height[av[j]]];
      if (v > val) {
        val = v; l = j;
      }
    }
    for (j = av[l]; l>i; l--)
      av[l] = av[l-1];
    makemove(av[i] = j);
    val = LOSSWIN-ab(LOSSWIN-beta,LOSSWIN-alpha);
    backmove();
    if (val > score) {
      besti = i;
      if ((score=val) > alpha && nplies >= BOOKPLY && (alpha=val) >= beta) {
        if (score == DRAW && i < nav-1)
          score = DRAWWIN;
        if (besti > 0) {
          for (i = 0; i < besti; i++)
            history[side][(int)height[av[i]]]--; // punish bad histories
          history[side][(int)height[av[besti]]] += besti;
        }
        break;
      }
    }
  }
  if (score == LOSSWIN-ttscore) // combine < and >
    score = DRAW;
  poscnt = posed - poscnt;
  for (work=0; (poscnt>>=1) != 0; work++) ; // work=log #positions stored
  transtore(hashindx, hashlock, score, work);
  if (nplies <= REPORTPLY) {
    printMoves();
    printf("%c%d\n", "#-<=>+"[score], work);
  }
  return score;
}

int solve()
{
  int i, side = nplies & 1, otherside = side ^ 1, score;

  nodes = 0;
  msecs = 1L;
  if (haswon(color[otherside]))
      return LOSS;
  for (i = 0; i < WIDTH; i++)
    if (islegalhaswon(color[side] | ((uint64)1 << height[i])))
      return WIN;
  inithistory();
  msecs = millisecs();
  score = ab(LOSS, WIN);
  msecs = 1L + millisecs() - msecs; // prevent division by 0
  return score;
}

int main()
{
  int c, result, work;
  uint64 poscnt;
                                                                                
  if (sizeof(uint64) != 8) {
    printf("sizeof(uint64) == %d; please redefine.\n", sizeof(uint64));
    exit(0);
  }
  trans_init();
  puts("Fhourstones 3.1 (C)");
  printf("Boardsize = %dx%d\n",WIDTH,HEIGHT);
  printf("Using %d transposition table entries.\n", TRANSIZE);

  for (;;) {
    reset();
    while ((c = getchar()) != -1) {
      if (c >= '1' && c <= '0'+WIDTH)
        makemove(c - '1');
      else if (c == '\n')
        break;
    }
    if (c == -1)
      break;
    printf("\nSolving %d-ply position after ", nplies);
    printMoves();
    puts(" . . .");

    emptyTT();
    result = solve();   // expect score << 6 | work
    poscnt = posed;
    for (work=0; (poscnt>>=1) != 0; work++) ; //work = log of #positions stored 
    printf("score = %d (%c)  work = %d\n",
      result, "#-<=>+"[result], work);
    //printf("%llu pos / %llu msec = %.1f Kpos/sec\n",
    //  nodes, msecs, (double)nodes/msecs);
    htstat();
  }
  return 0;
}
