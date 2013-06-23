/*
 * Fsanity.c
 *
 * The author of this software is Alain K\"{a}gi.
 *
 * Copyright (c) 1993 by Alain K\"{a}gi and the University of Wisconsin
 * Board of Trustees.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR THE UNIVERSITY OF
 * WISCONSIN MAKE ANY REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING
 * THE MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 *
 * ------------------------------------------------------------------------
 *
 * This a collection of functions which check certain invariants.
 *   
 * ------------------------------------------------------------------------
 *
 * $Id: Fsanity.c 5682 2003-03-03 16:11:58Z lattner $
 *
 */

#include <stdio.h>
#include "Fheap.h"
#include "Fstruct.h"

int
SanityCheck1(HeapP * h, Item * i)
{
  HeapP * h1;

  if(h == NULL_HEAP)
  {
     return(TRUE);
  }

  h1 = h;
  do
  {
    if(LessThan(ITEM(h1), i))
    {
      return(FALSE);
    }
    if(!SanityCheck1(CHILD(h1), ITEM(h1)))
    {
      return(FALSE);
    }

    h1 = FORWARD(h1);
  }
  while(h1 != h);

  return(TRUE);
}

int
SanityCheck2(HeapP * h)
{
  int   sum;
  HeapP * h1;
  HeapP * h2;

  if(h == NULL_HEAP)
  {
     return(TRUE);
  }

  h1 = h;
  do
  {
    if(CHILD(h1) != NULL_HEAP)
    {
      sum = 0;
      h2 = CHILD(h1);
      do
      {
         sum += RANK(h2) + 1;

         h2 = FORWARD(h2);
      }
      while(h2 != CHILD(h1));
      if(sum != RANK(h1))
      {
        return(FALSE);
      }

      if(!SanityCheck2(CHILD(h1)))
      {
        return(FALSE);
      }
    }

    h1 = FORWARD(h1);
  }
  while(h1 != h);

  return(TRUE);
}

int
SanityCheck3(HeapP * h, int rank)
{
  int   sum;
  HeapP * h1;
  HeapP * h2;

  if((h == NULL_HEAP) && (rank == 0))
  {
     return(TRUE);
  }

  sum = 0;
  h1 = h;
  do
  {
    sum += RANK(h1) + 1;

    if(!SanityCheck3(CHILD(h1), RANK(h1)))
    {
      return(FALSE);
    }

    h1 = FORWARD(h1);
  }
  while(h1 != h);

  if(sum == rank)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}

void
PrettyPrint(HeapP * h)
{
  HeapP * h1;

  if(h == NULL_HEAP)
  {
    printf(" nil ");
    return;
  }

  printf("(");

  h1 = h;
  do
  {
    //PrintItem(ITEM(h1));
    printf("[%u] ", RANK(h1));
    PrettyPrint(CHILD(h1));
    h1 = FORWARD(h1);
  }
  while(h1 != h);

  printf(")");
}

