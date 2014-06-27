/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>
#include <stdlib.h>

#include <pfs.h>
#include <pmachine.h> /* for correct definition of ZERO */

static PATTRIB	lfree = NULL;
int		pattrib_count = 0;
int		pattrib_max = 0;

extern void bzero(char *b,int length);

/*
 * atalloc - allocate and initialize vlink structure
 *
 *    ATALLOC returns a pointer to an initialized structure of type
 *    PATTRIB.  If it is unable to allocate such a structure, it
 *    returns NULL.
 */

PATTRIB atalloc(void)
    {
	PATTRIB	at;
	if(lfree) {
	    at = lfree;
	    lfree = lfree->next;
	}
	else {
	    at = (PATTRIB) malloc(sizeof(PATTRIB_ST));
	    if (!at) return(NULL);
	    pattrib_max++;
	}

	pattrib_count++;

	ZERO(at);
	/* Initialize and fill in default values; all items are
	   0 [or NULL] save precedence */
	at->precedence = ATR_PREC_OBJECT;

	return(at);
    }

/*
 * atfree - free a PATTRIB structure
 *
 *    ATFREE takes a pointer to a PATTRRIB structure and adds it to
 *    the free list for later reuse.
 */

void atfree(PATTRIB at)
    {
	if(at->aname) stfree(at->aname);

	if((strcmp(at->avtype,"ASCII") == 0) && at->value.ascii) 
	    stfree(at->value.ascii);
	if((strcmp(at->avtype,"LINK") == 0) && at->value.link) 
	    vlfree(at->value.link);
	
	if(at->avtype) stfree(at->avtype);

	at->next = lfree;
	at->previous = NULL;
	lfree = at;
	pattrib_count--;
    }

/*
 * atlfree - free a PATTRIB structure
 *
 *    ATLFREE takes a pointer to a PATTRIB structure frees it and any linked
 *    PATTRIB structures.  It is used to free an entrie list of PATTRIB
 *    structures.
 */

void atlfree(PATTRIB at)
    {
	PATTRIB	nxt;

	while(at != NULL) {
	    nxt = at->next;
	    atfree(at);
	    at = nxt;
	}
    }
