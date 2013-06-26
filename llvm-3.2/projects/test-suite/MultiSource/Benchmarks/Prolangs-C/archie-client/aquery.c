/*
 * aquery.c : Programmatic Prospero interface to Archie
 *
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * Originally part of the Prospero Archie client by Clifford 
 * Neuman (bcn@isi.edu).  Modifications, addition of programmatic interface,
 * and new sorting code by George Ferguson (ferguson@cs.rochester.edu) 
 * and Brendan Kehoe (brendan@cs.widener.edu).  MSDOS and OS2 modifications
 * to use with PC/TCP by Mark Towfiq (towfiq@FTP.COM).
 *
 * v1.2.1 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 * v1.2.0 - 09/17/91 (bpk) - added BULL & USG stuff, thanks to Jim Sillas
 * v1.1.3 - 08/30/91 (bpk) - cast index()
 * v1.1.2 - 08/20/91 (bcn) - make it do it properly (new invdatecmplink)
 * v1.1.1 - 08/20/91 (bpk) - made sorting go inverted as we purport it does
 */
#include <copyright.h>

#include <stdio.h>

#include <pfs.h>
#include <perrno.h>
#include <archie.h>

#include <pmachine.h>
#ifdef NEED_STRING_H
# include <string.h>			/* for char *index() */
#else
# include <strings.h>			/* for char *index() */
#endif

static void translateArchieResponse();

extern int pwarn;
extern char p_warn_string[];

/*
 * archie_query : Sends a request to _host_, telling it to search for
 *                _string_ using _query_type_ as the search method.
 *                No more than _max_hits_ matches are to be returned
 *                skipping over _offset_ matches.
 *
 *		  archie_query returns a linked list of virtual links. 
 *                If _flags_ does not include AQ_NOTRANS, then the Archie
 *                responses will be translated. If _flags_ does not include 
 *                AQ_NOSORT, then the list will be sorted using _cmp_proc_ to
 *                compare pairs of links.  If _cmp_proc_ is NULL or AQ_DEFCMP,
 *                then the default comparison procedure, defcmplink(), is used
 *                sorting by host, then filename. If cmp_proc is AQ_INVDATECMP
 *                then invdatecmplink() is used, sorting inverted by date.
 *                otherwise a user-defined comparison procedure is called.
 *
 *                archie_query returns NULL and sets perrno if the query
 *                failed. Note that it can return NULL with perrno == PSUCCESS
 *                if the query didn't fail but there were simply no matches.
 *
 *   query_type:  S  Substring search ignoring case   
 *                C  Substring search with case significant
 *                R  Regular expression search
 *                =  Exact String Match
 *            s,c,e  Tries exact match first and falls back to S, C, or R 
 *                   if not found.
 *
 *     cmp_proc:  AQ_DEFCMP      Sort by host, then filename
 *                AQ_INVDATECMP  Sort inverted by date
 *
 *        flags:  AQ_NOSORT      Don't sort results
 *                AQ_NOTRANS     Don't translate results
 */

extern int get_vdir(char *dhost,char *dfile,char *components,VDIR1 dir,
                    long flags,VLINK filters,char *acomp);

VLINK archie_query(char *host,char *string,int max_hits,int offset,
                   char query_type,int (*cmp_proc)(),int flags)
    {
	char qstring[MAX_VPATH];    /* For construting the query  */
	VLINK	links;		/* Matches returned by server */
	VDIR_ST	dir_st;         /* Filled in by get_vdir      */
	VDIR1	dir= &dir_st;
	
	VLINK	p,q,r,lowest,nextp,pnext,pprev;
	int	tmp;

	/* Set the cmp_proc if not given */
	if (cmp_proc == NULL) cmp_proc = defcmplink;

	/* Make the query string */
	sprintf(qstring,"ARCHIE/MATCH(%d,%d,%c)/%s",
		max_hits,offset,query_type,string);

	/* Initialize Prospero structures */
	perrno = PSUCCESS; *p_err_string = '\0';
	pwarn = PNOWARN; *p_warn_string = '\0';
	vdir_init(dir);
	
	/* Retrieve the list of matches, return error if there was one */
#if defined(MSDOS)
	if(tmp = get_vdir(host, qstring, "", dir, (long)GVD_ATTRIB|GVD_NOSORT,
		NULL, NULL)) {
#else
	if((tmp = get_vdir(host,qstring,"",dir,GVD_ATTRIB|GVD_NOSORT,NULL,NULL))) {
# endif
	    perrno = tmp;
	    return(NULL);
	}

	/* Save the links, and clear in dir in case it's used again   */
	links = dir->links; dir->links = NULL;

	/* As returned, list is sorted by suffix, and conflicting     */
	/* suffixes appear on a list of "replicas".  We want to       */
	/* create a one-dimensional list sorted by host then filename */
	/* and maybe by some other parameter                          */

	/* First flatten the doubly-linked list */
	for (p = links; p != NULL; p = nextp) {
	    nextp = p->next;
	    if (p->replicas != NULL) {
		p->next = p->replicas;
		p->next->previous = p;
		for (r = p->replicas; r->next != NULL; r = r->next)
		    /*EMPTY*/ ;
		r->next = nextp;
		nextp->previous = r;
		p->replicas = NULL;
	    }
	}

	/* Translate the filenames unless NOTRANS was given */
	if (!(flags & AQ_NOTRANS))
	    for (p = links; p != NULL; p = p->next)
		translateArchieResponse(p);

	/* If NOSORT given, then just hand it back */
	if (flags & AQ_NOSORT) {
	    perrno = PSUCCESS;
	    return(links);
	}

	/* Otherwise sort it using a selection sort and the given cmp_proc */
	for (p = links; p != NULL; p = nextp) {
	    nextp = p->next;
	    lowest = p;
	    for (q = p->next; q != NULL; q = q->next)
		if ((*cmp_proc)(q,lowest) < 0)
		    lowest = q;
	    if (p != lowest) {
		/* swap the two links */
		pnext = p->next;
		pprev = p->previous;
		if (lowest->next != NULL)
		    lowest->next->previous = p;
		p->next = lowest->next;
		if (nextp == lowest) {
		    p->previous = lowest;
		} else {
		    lowest->previous->next = p;
		    p->previous = lowest->previous;
		}
		if (nextp == lowest) {
		    lowest->next = p;
		} else {
		    pnext->previous = lowest;
		    lowest->next = pnext;
		}
		if (pprev != NULL)
		    pprev->next = lowest;
		lowest->previous = pprev;
		/* keep the head of the list in the right place */
		if (links == p)
		    links = lowest;
	    }
	}

	/* Return the links */
	perrno = PSUCCESS;
	return(links);
    }

/*
 * translateArchieResponse: 
 *
 *   If the given link is for an archie-pseudo directory, fix it. 
 *   This is called unless AQ_NOTRANS was given to archie_query().
 */
static void translateArchieResponse(VLINK l)
    {
	char *slash;

	if (strcmp(l->type,"DIRECTORY") == 0) {
	    if (strncmp(l->filename,"ARCHIE/HOST",11) == 0) {
		l->type = stcopyr("EXTERNAL(AFTP,DIRECTORY)",l->type);
		l->host = stcopyr(l->filename+12,l->host);
		/*
		slash = (char *)index(l->host,'/');
		*/
		slash = strchr(l->host,(int) '/');
		if (slash) {
		    l->filename = stcopyr(slash,l->filename);
		    *slash++ = '\0';
		} else
		    l->filename = stcopyr("",l->filename);
	    }
	}
    }

/*
 * defcmplink: The default link comparison function for sorting. Compares
 *	       links p and q first by host then by filename. Returns < 0 if p
 *             belongs before q, > 0 if p belongs after q, and == 0 if their
 *             host and filename fields are identical.
 */

int defcmplink(VLINK p,VLINK q)
    {
	int result;

	if ((result=strcmp(p->host,q->host)) != 0)
	    return(result);
	else
	    return(strcmp(p->filename,q->filename));
    }

/*
 * invdatecmplink: An alternative comparison function for sorting that
 *	           compares links p and q first by LAST-MODIFIED date,
 *                 if they both have that attribute. If both links
 *                 don't have that attribute or the dates are the
 *                 same, it then calls defcmplink() and returns its 
 *		   value.
 */

int invdatecmplink(VLINK p,VLINK q)
    {
	PATTRIB pat,qat;
	char *pdate,*qdate;
	int result;
	
	pdate = qdate = NULL;
	for (pat = p->lattrib; pat; pat = pat->next)
	    if(strcmp(pat->aname,"LAST-MODIFIED") == 0)
		pdate = pat->value.ascii;
	for (qat = q->lattrib; qat; qat = qat->next)
	    if(strcmp(qat->aname,"LAST-MODIFIED") == 0)
		qdate = qat->value.ascii;
	if(!pdate && !qdate) return(defcmplink(p,q));
	if(!pdate) return(1); 
	if(!qdate) return(-1);
	if((result=strcmp(qdate,pdate)) == 0) return(defcmplink(p,q));
	else return(result);
    }
