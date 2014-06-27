/*
 * Fheap.h
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
 * This is an implementation of an algorithm described in the paper:
 *
 *   , by Michael L. Fredman and Robert Endre Tarjan, in
 *   Journal of Association for Computing Machinery, Vol. 34, No. 3,
 *   July 1987, Pages 596-615.
 *
 * The algorithm is theirs.  Any discrepancy between the algorithm
 * description which appears in the paper and this implementation is
 * a consequence of my misunderstanding of their intent.
 *   
 * ------------------------------------------------------------------------
 *
 * $Id: Fheap.h 4552 2002-11-05 18:52:12Z hldnbrnd $
 *
 */

#ifndef _fheap_h
#define _fheap_h

/*
 * The following "item.h" must define
 *   o a structure "Item",
 *   o the definition of NULL_ITEM,
 *   o the definition of "int LessThan(Item *, Item *);",
 *   o the definition of "int Equal(Item *, Item *);",
 *   o the definition of "Item *Subtract(Item *, void *);"
 *     this function must returm an item with its key reduced by a positive
 *     value passed as void *.
 */
#include "item.h"

typedef struct _Heap
{
  Item *     item;

  struct _Heap * parent;
  struct _Heap * child;
  struct _Heap * forward;
  struct _Heap * backward;
  int           rank;
  short         marked;
} HeapP;

#define NULL_HEAP	((void *)0)

/*
 * Initialize the package.  A single call is sufficient even if multiple
 * heaps are alive in the course of a computation.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   none
 *
 * Return values:
 *   none
 */
void  InitFHeap();

/*
 * Create a heap structure.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   none
 *
 * Return values:
 *   a heap, to be precise an empty, i.e. NULL_HEAP
 */
HeapP * MakeHeap();

/*
 * Find the item with lowest key.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the structure to access, possibly NULL_HEAP
 *
 * Return values:
 *   an item if the heap is not empty
 *   NULL_ITEM otherwise
 */
Item * FindMin(HeapP * h);

/*
 * Insert an item in a heap.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   IN/OUT:	h	the structure to access, possibly NULL_HEAP
 *   INPUT:	i	the item to insert, must be different than NULL_ITEM
 *
 * Return values:
 *   a handle to the inserted item, useful in connection with Delete()
 *   and DecreaseKey().
 */
HeapP * Insert(HeapP * * h, Item * i);

/*
 * Meld to heaps.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h1, h2	the heaps to meld, possibly NULL_HEAP
 *
 * Return values:
 *   a bigger heap, possibly NULL_HEAP
 */
HeapP * Meld(HeapP * h1, HeapP * h2);

/*
 * Remove the smallest item in a heap
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the structure to access, possibly NULL_HEAP
 *
 * Return values:
 *   a smaller heap, possibly NULL_HEAP
 */
HeapP * DeleteMin(HeapP * h);

/*
 * Decrease the key of an item in a heap.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the structure to access, must be different than
 *			NULL_HEAP
 *   INPUT:	i	the item to change, must different than NULL_HEAP
 *   INPUT:	delta	a "positive" value which will be subtracted from
 *                      the key in i.
 *
 * Return values:
 *   a heap, possibly NULL_HEAP
 */
HeapP * DecreaseKey(HeapP * h, HeapP * i, int delta);

/*
 * Delete an entry in a heap.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the structure to access, must be different than
 *			NULL_HEAP
 *   INPUT:	i	the item to delete, must be different than NULL_HEAP
 *
 * Return values:
 *   a smaller heap, possibly NULL_HEAP
 */
HeapP * Delete(HeapP * h, HeapP * i);

/*
 * Search for an item with a particular key in a heap.
 * Beware the fibonacci heaps are not efficient at searching.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the structure to access, possibly NULL_HEAP
 *   INPUT:	item	the item to search for in h (only the key in item is
 *                      accessed through Equal() and LessThan())
 *
 * Return values:
 *   an handle to the item, possibly NULL_HEAP
 */
HeapP * Find(HeapP * h, Item * item);

/*
 * Converts a item handle into an item pointer.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the item handle, must be different than NULL_HEAP
 *
 * Return values:
 *   an pointer to the item
 */
Item * ItemOf(HeapP * h);

#endif
