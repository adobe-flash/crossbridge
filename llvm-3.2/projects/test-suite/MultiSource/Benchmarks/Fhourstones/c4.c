/* Fhourstones 2.0 connect-4 solver
// (http://www.cwi.nl/~tromp/c4/fhour.html)
//
// implementation of the well-known game
// played on a vertical board of 7 columns by 6 rows,
// where 2 players take turns in dropping counters in a column.
// the first player to get four of his counters
// in a horizontal, vertical or diagonal row, wins the game.
// if neither player has won after 42 moves, then the game is drawn.
//
// This software is copyright (c) 1996 by
//      John Tromp
//      Lindenlaan 33
//      1701 GT Heerhugowaard
//      Netherlands
// E-mail: tromp@cwi.nl
//
// This notice must not be removed.
// This software must not be sold for profit.
// You may redistribute if your distributees have the
// same rights and restrictions.
*/

#include "types.h"
#include "c4.h"

#define HISTINIT	{-1,-1,-1,-1,-1,-1,-1,-1,\
			-1, 0, 1, 2, 4, 2, 1, 0,\
			-1, 1, 3, 5, 7, 5, 3, 1,\
			-1, 2, 5, 8,10, 8, 5, 2,\
			-1, 2, 5, 8,10, 8, 5, 2,\
			-1, 1, 3, 5, 7, 5, 3, 1,\
			-1, 0, 1, 2, 4, 2, 1, 0}

int history[2][56] = {HISTINIT, HISTINIT};
int64 nodes, msecs;

extern int colthr[];
extern boolean colwon[];
extern int plycnt;
extern int columns[], height[];

extern int64 posed;

void c4_init()
{
  trans_init();
}

int ab(int alpha, int beta)
{
  int besti,i,j,h,k,l,val,score;
  int x,v,work;
  int nav, av[8];
  int64 poscnt;
  int side, otherside;

  nodes++;
  if (plycnt == 41)
    return DRAW;
  side = (otherside = plycnt & 1) ^ 1;
  for (i = nav = 0; ++i <= 7; ) {
    if ((h = height[i]) <= 6) {
      if (wins(i,h,3) || colthr[columns[i]] != 0) {
        if (h+1 <= 6 && wins(i,h+1,1<<otherside))
          return LOSE;
        av[0] = i;		/* forget other moves */
        while (++i <= 7)
          if ((h = height[i]) <= 6 &&
              (wins(i,h,3) || colthr[columns[i]] != 0))
            return LOSE;
        nav = 1;
        break;
      }
      if (!(h+1 <= 6 && wins(i,h+1,1<<otherside)))
        av[nav++] = i;
    }
  }
  if (nav == 0)
    return LOSE;
  if (nav == 1) {
    makemove(av[0]);
    score = -ab(-beta,-alpha);
    backmove();
    return score;
  }
  if ((x = transpose()) != ABSENT) {
    score = x >> 5;
    if (score == DRAWLOSE) {
      if ((beta = DRAW) <= alpha)
        return score;
    } else if (score == DRAWWIN) {
      if ((alpha = DRAW) >= beta)
        return score;
    } else return score; /* exact score */
  }
  poscnt = posed;
  l = besti = 0;
  score = -999999;	/* try to get the best bound if score > beta */
  for (i = 0; i < nav; i++) {
    for (j = i, val = -999999; j < nav; j++) {
      k = av[j];
      v = history[side][height[k]<<3|k];
      if (v > val) {
        val = v; l = j;
      }
    }
    j = av[l];
    if (i != l) {
      av[l] = av[i]; av[i] = j;
    }
    makemove(j);
    val = -ab(-beta,-alpha);
    backmove();
    if (val > score) {
      besti = i;
      if ((score = val) > alpha && (alpha = val) >= beta) {
        if (score == DRAW && i < nav-1)
          score = DRAWWIN;
        break;
      }
    }
  }
  if (besti > 0) {
    for (i = 0; i < besti; i++) {
      history[side][height[av[i]]<<3|av[i]]--;	/* punish bad historiess */
    }
    history[side][height[av[besti]]<<3|av[besti]] += besti;
  }
  poscnt = posed - poscnt;
  for (work=1; (poscnt>>=1) != 0; work++) ;	/* work=log #positions stored */
  if (x != ABSENT) {
    if (score == -(x>>5))	/* combine < and > */
      score = DRAW;
    transrestore(score, work);
  } else transtore(score, work);
  if (plycnt == REPORTPLY) {
    printMoves();
    printf("%c%d\n", "##-<=>+#"[4+score], work);
  }
  return score;
}

int solve()
{
  int i,side;
  int x,work,score;
  int64 poscnt;
  extern int64 millisecs();

  nodes = 0;
  msecs = 1;
  side = (plycnt+1) & 1;
  for (i = 0; ++i <= 7 ;)
    if (height[i] <= 6) {
      if (wins(i, height[i], 1<<side) || colthr[columns[i]] == (1<<side))
        return (side!=0 ? WIN : LOSE) << 5;	/* all score & no work:) */
    }
  if ((x = transpose()) != ABSENT) {
    if ((x & 32) == 0)   /* exact score */
      return x;
  }
  msecs = millisecs() - 1L;
  score = ab(LOSE,WIN);
  poscnt = posed;
  for (work=1; (poscnt>>=1) != 0; work++) ; /*work = log of #positions stored*/
  msecs = millisecs() - msecs;
  return score << 5 | work;
}

int main()
{
  int c, i, result;

  if (sizeof(int64) != 8) {
    printf("sizeof(int64) == %d; please redefine.\n", sizeof(int64));
    exit(0);
  }
  c4_init();
  puts("Fhourstones 2.0");
  printf("Using %d transposition table entries with %d probes.\n",
          TRANSIZE, PROBES);
  for (;;) {
    reset();
    while ((c = getchar()) != -1) {
      if (c >= '1' && c <= '7')
        makemove(c - '0');
      else if (c >= 'A' && c <= 'G')
        makemove(c - 'A' + 1);
      else if (c >= 'a' && c <= 'g')
        makemove(c - 'a' + 1);
      else if (c == '\n')
        break;
    }
    if (c == -1)
      break;
    printf("Solving %d-ply position after ", plycnt);
    printMoves();
    puts(" . . .");

    emptyTT();
    result = solve();		/* expect score << 5 | work */
    printf("score = %d (%c)  work = %d\n",
      (result>>5), "##-<=>+#"[4+(result>>5)], result&31);
    printf("%lu pos / %lu msec = %.1f Kpos/sec\n",
      (long)nodes, (long)msecs, (double)nodes/msecs);
    htstat();
  }
  return 0;
}
