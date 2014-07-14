
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

/*           sort.c             */
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
#include "sort.h"


List *BubbleSort(List *l, BOOL (*compare)(int, int))
{
  int i,j;
  int temp; /* temporary variable */

  for (i=0; i<l->n; i++)
    for (j=0; j<l->n-i-1; j++)
      if (compare(l->l[j+1],l->l[j]))
	{
	  /* Swap the two elements */
	  temp=l->l[j];
	  l->l[j]=l->l[j+1];
	  l->l[j+1]=temp;
	};
  return(l);
}

LinkList *QuickSort(LinkList *l, BOOL (*compare)(int, int))
{
  LinkList *p;
  LinkList *SortList;
  LinkList *InfList=NULL, *SupList=NULL;
  LinkList *NewElement;
  LinkList *FirstElement;

  if (l==NULL) return(NULL);
  else
    {
      FirstElement=(LinkList*) malloc(sizeof(LinkList));
      *FirstElement=*l;
      /* Build the Inf- and the SupList */
      for (p=l->next; p!=NULL; p=p->next)
	{
	  if (compare(p->key,FirstElement->key))
	    {
	      /* Add the element to the InfList (in front) */
	      NewElement=(LinkList*) malloc(sizeof(LinkList));
	      NewElement->key=p->key;
	      NewElement->next=InfList;
	      InfList =NewElement;
	    }
	  else
	    {
	      /* Add the element to the SupList (in front) */
	      NewElement=(LinkList*) malloc(sizeof(LinkList));
	      NewElement->key=p->key;
	      NewElement->next=SupList;
	      SupList =NewElement;
	    }
	}
      /* Sort Inf- and SupList by means of recursion */ 
      SortList=QuickSort(SupList, compare);
      FreeLinkList(SupList);
      SupList = SortList;
      SortList=QuickSort(InfList, compare);
      FreeLinkList(InfList);
      /* Join Lists to form quicksorted list */
      if (SortList!=NULL)
	{
	  /* Fast forward to the end of SortList */
	  for (p=SortList; p->next!=NULL; p=p->next)
	    /* empty */;
	  p->next=FirstElement;
	}
      else
	SortList=FirstElement;
      FirstElement->next=SupList;
    }
  return(SortList);
}
 
void PrintList(List *l)
{
  int i;

  printf("[");
  for (i=0; i<l->n-1; i++)
    printf("%d,",l->l[i]);
  printf("%d]",l->l[l->n-1]);
}

void PrintLinkList(LinkList *l)
{
  if (l==NULL)
    printf("[]");
  else
    {
      printf("[");
      for (/* empty*/; l->next!=NULL; l=l->next)
	printf("%d,",l->key);
      printf("%d]",l->key);
    }
}

void FreeLinkList(LinkList *l)
{
  LinkList* next;
  for (; l!=NULL; l=next) {
    next = l->next;
    free(l);
  }
}
