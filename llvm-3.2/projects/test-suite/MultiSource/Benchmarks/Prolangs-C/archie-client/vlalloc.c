/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>
#include <stdlib.h>

#include <pfs.h>
#include <pmachine.h>

static VLINK	lfree = NULL;
int		vlink_count = 0;
int		vlink_max = 0;

extern void bzero(char *b,int length);

/*
 * vlalloc - allocate and initialize vlink structure
 *
 *    VLALLOC returns a pointer to an initialized structure of type
 *    VLINK.  If it is unable to allocate such a structure, it
 *    returns NULL.
 */
VLINK vlalloc(void)
    {
	VLINK	vl;
	if(lfree) {
	    vl = lfree;
	    lfree = lfree->next;
	}
	else {
	    vl = (VLINK) malloc(sizeof(VLINK_ST));
	    if (!vl) return(NULL);
	    vlink_max++;
	}

	vlink_count++;

	/* Initialize and fill in default values */
	/* Since all but four are set to a zero-value,
	   why not just wipe it clean?  */
	ZERO(vl);

	vl->linktype = 'L';
	vl->type = stcopy("FILE");
	vl->hosttype = stcopy("INTERNET-D");
	vl->nametype = stcopy("ASCII");

	return(vl);
    }

/*
 * vlfree - free a VLINK structure
 *
 *    VLFREE takes a pointer to a VLINK structure and adds it to
 *    the free list for later reuse.
 */

void vlfree(VLINK vl)
    {
        extern int string_count;

	if(vl->dontfree) return;
	/* many of these don't need to call stfree(); since a check
	   for pointer validity's already done before even calling
	   it, we can just call free() here then do one big decrement
	   of string_count at the end.  */
	if(vl->name) free(vl->name);
	stfree(vl->type);
	if(vl->replicas) vllfree(vl->replicas);
	stfree(vl->hosttype);
	if(vl->host) free(vl->host);
	stfree(vl->nametype);
	if(vl->filename) free(vl->filename);
	if(vl->args) free(vl->args);
	if(vl->lattrib) atlfree(vl->lattrib);
	/* No allocation routines for f_info yet */
	vl->f_info = NULL;
	vl->next = lfree;
	vl->previous = NULL;
	lfree = vl;
	vlink_count--;
	string_count -= 4; /* freed name, host, filename, and args */
    }

/*
 * vllfree - free a VLINK structure
 *
 *    VLLFREE takes a pointer to a VLINK structure frees it and any linked
 *    VLINK structures.  It is used to free an entrie list of VLINK
 *    structures.
 */

void vllfree(VLINK vl)
    {
	VLINK	nxt;

	while((vl != NULL) && !vl->dontfree) {
	    nxt = vl->next;
	    vlfree(vl);
	    vl = nxt;
	}
    }

