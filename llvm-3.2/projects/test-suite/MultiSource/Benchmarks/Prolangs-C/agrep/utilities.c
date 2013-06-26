/* this file contains various utility functions for accessing
   and manipulating regular expression syntax trees.	*/

#include <stdio.h>
#include <stdlib.h>
#include "re.h"

/************************************************************************/
/*                                                                      */
/*  the following routines implement an abstract data type "stack".	*/
/*                                                                      */
/************************************************************************/

Stack Push(Stack *s, Re_node v)
{
    Stack node;

    node = (Stack) new_node(node);
    if (s == NULL || node == NULL) return NULL;	    /* can't allocate */
    node->next = *s;
    node->val = v;
    if (*s == NULL) node->size = 1;
    else node->size = (*s)->size + 1;
    *s = node;
    return *s;
}

Re_node Pop(Stack *s)
{
    Re_node node;
    Stack temp;

    if (s == NULL || *s == NULL) return NULL;
    else {
	temp = *s;
	node = (*s)->val;
	*s = (*s)->next;
	free(temp);
	return node;
    }
}

Re_node Top(Stack s)
{
    if (s == NULL) return NULL;
    else return s->val;
}

int Size(Stack s)
{
    if (s == NULL) return 0;
    else return s->size;
}

/************************************************************************/
/*                                                                      */
/*	the following routines manipulate sets of positions.		*/
/*                                                                      */
/************************************************************************/

int occurs_in(int n, Pset p)
{
    while (p != NULL)
	if (n == p->posnum) return 1;
	else p = p->nextpos;
    return 0;
}

/* pset_union() takes two position-sets and returns their union.    */

Pset pset_union(Pset s1, Pset s2)
{
    Pset hd, curr, new1;

    hd = NULL; curr = NULL;
    while (s1 != NULL) {
	if (!occurs_in(s1->posnum, s2)) {
	    new1 = (Pset) new_node(new1);
	    if (new1 == NULL) return NULL;
	    new1->posnum = s1->posnum;
	    if (hd == NULL) hd = new1;
	    else curr->nextpos = new1;
	}
	curr = new1;
	s1 = s1->nextpos;
    }
    if (hd == NULL) hd = s2;
    else curr->nextpos = s2;
    return hd;
}

/* create_pos() creates a position node with the position value given,
   then returns a pointer to this node.					*/

Pset create_pos(int n)
{
    Pset x;

    x = (Pset) new_node(x);
    if (x == NULL) return NULL;
    x->posnum = n;
    x->nextpos = NULL;
    return x;
}

/* eq_pset() takes two position sets and checks to see if they are
   equal.  It returns 1 if the sets are equal, 0 if they are not.	*/

int subset_pset(Pset s1, Pset s2)
{
    int subs = 1;

    while (s1 != NULL && subs != 0) {
	subs = 0;
	while (s2 != NULL && subs != 1)
	    if (s1->posnum == s2->posnum) subs = 1;
	    else s2 = s2->nextpos;
	s1 = s1->nextpos;
    }
    return subs;
}	

int eq_pset(Pset s1, Pset s2)
{
    return subset_pset(s1, s2) && subset_pset(s2, s1);
}

