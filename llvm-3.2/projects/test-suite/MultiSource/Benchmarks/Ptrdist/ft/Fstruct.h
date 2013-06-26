/*
 * Fstruct.h
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
 * $Id: Fstruct.h 4552 2002-11-05 18:52:12Z hldnbrnd $
 *
 */

#ifndef _fstruct_h
#define _fstruct_h

#define ITEM(P)		((*(P)).item)
#define PARENT(P)	((*(P)).parent)
#define CHILD(P)	((*(P)).child)
#define FORWARD(P)	((*(P)).forward)
#define BACKWARD(P)	((*(P)).backward)
#define RANK(P)		((*(P)).rank)
#define MARKED(P)	((*(P)).marked)

#define ORPHAN(P)	(PARENT(P) == NULL_HEAP)
#define ONLY_CHILD(P)	((P) == FORWARD(P))
#define PARENT_OF(P, C)	(CHILD(P) == (C))

#define TRUE		1
#define FALSE		0
#define MAX_RANK	10000

#endif
