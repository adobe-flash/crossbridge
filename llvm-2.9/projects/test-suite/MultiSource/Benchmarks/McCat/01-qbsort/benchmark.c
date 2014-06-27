
/****
    Copyright (C) 1996 McGill University.
    Copyright (C) 1996 McCAT System Group.
    Copyright (C) 1996 ACAPS Benchmark Administrator
                       benadmin@acaps.cs.mcgill.ca

    This program is free software; you can redistribute it and/or modify
    it provided this copyright notice is maintained.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
****/

/*         benchmark.c          */
/*   quicksort and bubblesort   */
/*            made              */
/*          24/09/96            */
/*             by               */
/*    Jacob Grydholt Jensen     */
/* Email: grydholt@daimi.aau.dk */
/*             &&               */
/*       Claus Brabrand         */
/* Email: zracscan@daimi.aau.dk */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort.h"
#include "readlist.h"

#ifdef TIMEREPEAT
#include <sys/time.h>
#endif

BOOL LessThan(int x, int y);

BOOL LessThan(int x, int y)
{
  return (x < y);
}

int main(int argc, char **argv)
{
  int listno = 1;
  int err;
  LinkList *ll; /* ll = lINKED lIST */
  List *l;      /* l = lIST */
  /* Andrew Trick: added "repeat" to measure performance, not just correctness.
   * Also added FreeLinkList and removed leaks.
   * This required adding origList and origLinkList. */
  LinkList *origLinkList;
  int *origList;
  int repeat = 0;
  if (argc > 1) {
    repeat = strtol(argv[1], 0, 0);
  }
#ifdef TIMEREPEAT
  long long stime = 0;
  struct timeval t, tt;
#endif
  while ((err = ReadList(&ll, &l)) == 0) {
    origLinkList = ll;
    origList = l->l;
    l->l = (int*) malloc(sizeof(int)*l->n);
    memcpy(l->l, origList, sizeof(int)*l->n);
#ifdef TIMEREPEAT
    gettimeofday(&t,0);
#endif
    for (; repeat > 0; --repeat) {
      l = BubbleSort(l, LessThan);
      memcpy(l->l, origList, sizeof(int)*l->n);
      /* QuickSort returns a new list, and origLinkList is unmodified */
      ll = QuickSort(origLinkList, LessThan);
      FreeLinkList(ll);
    }
#ifdef TIMEREPEAT
    gettimeofday(&tt,0);
    stime += (tt.tv_sec-t.tv_sec)*1000000 + (tt.tv_usec-t.tv_usec);
#endif
    printf("\nList read (reverse order): ");fflush(stdout);
    PrintList(l);
    printf("\nBubbleSort: "); fflush(stdout);
    l = BubbleSort(l, LessThan);
    PrintList(l);
    printf("\nQuickSort:  "); fflush(stdout);
    ll = QuickSort(origLinkList, LessThan);
    PrintLinkList(ll);
    printf("\n");
    FreeLinkList(ll);
    FreeLinkList(origLinkList);
    free(l->l);
    free(l);
    free(origList);
    listno++;
  }

  switch(err) {
  case COMMA_EXPECTED:
    printf("Comma expected in list number %d\n", listno);
    exit(1);
    break;
  case READ_EOF:
    printf("Last list read\n");
    break;
  default:
    printf("Program Error: Unrecognized errorcode from ReadList\n");
    exit(1);
    break;
  }
#ifdef TIMEREPEAT
  printf("sort time = %lld usec\n", stime);
#endif
  exit(0);
}
