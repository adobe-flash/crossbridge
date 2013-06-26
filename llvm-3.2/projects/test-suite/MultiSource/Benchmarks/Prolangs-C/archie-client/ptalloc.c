/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.0 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 */

#include <copyright.h>
#include <stdio.h>
#include <stdlib.h>

#include <pfs.h>
#include <pmachine.h> /* for correct definition of ZERO */
#ifdef MSDOS
# define free _pfree   /* otherwise we get conflicts with free() */
#endif

static PTEXT	free1 = NULL;
int 		ptext_count = 0;
int		ptext_max = 0;

/*
 * ptalloc - allocate and initialize ptext structure
 *
 *    PTALLOC returns a pointer to an initialized structure of type
 *    PTEXT.  If it is unable to allocate such a structure, it
 *    returns NULL.
 */

extern void bzero(char *b,int length);

PTEXT ptalloc(void)
    {
	PTEXT	vt;
	if(free1) {
	    vt = free1;
	    free1 = free1->next;
	}
	else {
	    vt = (PTEXT) malloc(sizeof(PTEXT_ST));
	    if (!vt) return(NULL);
	    ptext_max++;
	}
	ptext_count++;

	/* nearly all parts are 0 [or NULL] */
	ZERO(vt);
	/* The offset is to leave room for additional headers */
	vt->start = vt->dat + MAX_PTXT_HDR;

	return(vt);
    }

/*
 * ptfree - free a VTEXT structure
 *
 *    VTFREE takes a pointer to a VTEXT structure and adds it to
 *    the free list for later reuse.
 */

void ptfree(PTEXT vt)
    {
	vt->next = free1;
	vt->previous = NULL;
	free1 = vt;
	ptext_count--;
    }

/*
 * ptlfree - free a VTEXT structure
 *
 *    VTLFREE takes a pointer to a VTEXT structure frees it and any linked
 *    VTEXT structures.  It is used to free an entrie list of VTEXT
 *    structures.
 */

void ptlfree(PTEXT vt)
    {
	PTEXT	nxt;

	while(vt != NULL) {
	    nxt = vt->next;
	    ptfree(vt);
	    vt = nxt;
	}
    }
