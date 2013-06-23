
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

/*         readlist.c           */
/*   quicksort and bubblesort   */
/*            made              */
/*          24/09/96            */
/*             by               */
/*    Jacob Grydholt Jensen     */
/* Email: grydholt@daimi.aau.dk */
/*             &&               */
/*       Claus Brabrand         */
/* Email: zracscan@daimi.aau.dk */

/* Reads a list of the form:

   <list> ::= "[]" | '[' <ints>* <int> ']'
   <ints> ::= <int> ',' */

#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "readlist.h"

int ReadList(LinkList **ll, List **l)
{
  int i,n,x;
  char c;
  LinkList *NewElement;
  List *NewList;
  LinkList *p;
  
  for (c='-'; c!='['; /* empty */)
    if (scanf("%c",&c)==EOF) return(READ_EOF);
  c=',';
  /* Make linked list */
  *ll=NULL;
  for (n=0; c!=']'; n++) /* count no of elements */
    {
      if (c!=',') return(COMMA_EXPECTED);
      if (scanf("%d",&x) == EOF) return(READ_EOF);
      /* Make Structure */
      NewElement=(LinkList*) malloc(sizeof(LinkList));
      NewElement->key=x;
      NewElement->next=*ll;
      *ll=NewElement;
      for (c=' '; ((c==' ') || (c=='\n'));/* empty */)
	if (scanf("%c",&c) == EOF) return (READ_EOF);
    }
  /* Make list */
  NewList=(List*) malloc(sizeof(List));
  NewList->n=n;
  NewList->l=(int*) malloc(sizeof(int)*n);
  for (p=*ll,i=0; i<n; p=p->next, i++)
    NewList->l[i]=p->key;
  *l=NewList;
  return 0;
}
