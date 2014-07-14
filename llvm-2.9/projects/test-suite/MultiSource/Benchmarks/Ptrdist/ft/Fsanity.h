/*
 * Fsanity.h
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
 * $Id: Fsanity.h 4552 2002-11-05 18:52:12Z hldnbrnd $
 *
 */

#ifndef _fsanity_h
#define _fsanity_h

/*
 * Check the entry ordering in the structure.
 *
 * Special external object accessed:
 *   LessThan()
 *
 * Arguments:
 *   INPUT:	h	the structure to check
 *   INPUT:	i	an item whose key is less than or equal to the minimal
 *                      element in h.  Use for instance the value return by
 *                      FindMin().
 *
 * Return values:
 *   FALSE:	check failed
 *   TRUE:	check succeeded
 */
int  SanityCheck1(Heap * h, Item * i);

/*
 * Check the rank fields in the structure.
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the structure to check
 *
 * Return values:
 *   FALSE:	check failed
 *   TRUE:	check succeeded
 */
int  SanityCheck2(Heap * h);

/*
 * Check the rank fields in the structure.
 * If you know how many elements are in the heap, this version is probably
 * better than SanityCheck2().
 *
 * Special external object accessed:
 *   none
 *
 * Arguments:
 *   INPUT:	h	the structure to check
 *   INPUT:	rank	expected rank of the structure
 *
 * Return values:
 *   FALSE:	check failed
 *   TRUE:	check succeeded
 */
int  SanityCheck3(Heap * h, int rank);

/*
 * Print the structure in some human readable form.  It is printed in a
 * list like format.  The entry consists of whatever PrintItem() output,
 * and the rank in square brackets. 
 *
 * Special external object accessed:
 *   PrintItem()
 *
 * Arguments:
 *   INPUT:	h	the structure to print
 *
 * Return values:
 *   none
 */
void PrettyPrint(Heap * h);

#endif
