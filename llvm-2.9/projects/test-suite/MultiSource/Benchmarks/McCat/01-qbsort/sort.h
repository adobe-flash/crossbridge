/*           sort.h             */
/*   quicksort and bubblesort   */
/*            made              */
/*          24/09/96            */
/*             by               */
/*    Jacob Grydholt Jensen     */
/* Email: grydholt@daimi.aau.dk */
/*             &&               */
/*       Claus Brabrand         */
/* Email: zracscan@daimi.aau.dk */

#ifndef _SORT_H_
#define _SORT_H_

#define TRUE 1
#define FALSE 0
typedef int BOOL;
typedef int *ListElem;
typedef struct LinkList LinkList;
typedef struct List List;

struct LinkList
{
  int key;
  LinkList *next;
}; 

struct List
{
  int n; /* No Of Elements in the List */
  ListElem l;
};

List *BubbleSort(List *l, BOOL (*compare)(int, int));
LinkList *QuickSort(LinkList *l, BOOL (*compare)(int, int));
void PrintList(List *l);
void PrintLinkList(LinkList *l);
void FreeLinkList(LinkList *l);
#endif /* ifndef _SORT_H_ */
