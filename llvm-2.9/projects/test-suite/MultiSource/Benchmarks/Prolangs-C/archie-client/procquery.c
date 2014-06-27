/*
 * procquery.c : Routines for processing results from Archie
 *
 * Originally part of the Prospero Archie client by Cliff Neuman (bcn@isi.edu).
 * Modified by Brendan Kehoe (brendan@cs.widener.edu).
 * Re-modified by George Ferguson (ferguson@cs.rochester.edu).
 *
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.2 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 * v1.2.1 - 10/18/91 (bpk) - warning stuff
 * v1.2.0 - 09/20/91 (bpk) - VMS under Multinet 3.0 too
 * v1.1.1 - 08/20/91 (bpk) - took out archie_query from error msg
 */
#include <copyright.h>
#include <stdio.h>
#include <pfs.h>
#include <perrno.h>
#include <pmachine.h>
#include <archie.h>

# include <time.h>

extern int client_dirsrv_timeout,client_dirsrv_retry;	/* dirsend.c */
extern char *progname;


/*
 * Functions defined here
 */

extern void procquery(char *host,char *str,int max_hits,int offset,
                      char query_type,int sortflag,int listflag);

void display_link(VLINK l,int listflag);

/*
 * Data defined here
 */
extern int pwarn, perrno;
#ifdef DEBUG
int pfs_debug;
#endif
static struct tm *presenttime;
static char lastpath[MAX_VPATH] = "\001";
static char lasthost[MAX_VPATH] = "\001";

/*	-	-	-	-	-	-	-	-	*/
/*
 * display_link : Prints the contents of the given virtual link. If
 *	listflag is 0, then this uses last{host,path} to save state
 *	between calls for a less verbose output. If listflag is non-zero
 *	then all information is printed every time.
 */
void display_link(VLINK l,int listflag)
{
    PATTRIB 	ap;
    char	linkpath[MAX_VPATH];
    int		dirflag = 0;
#ifdef MSDOS
    unsigned long size = 0L;
#else
    int		size = 0;
#endif
    char	*modes = "";
    char	archie_date[20];
    char	*gt_date = "";
    int		gt_year = 0;
    int		gt_mon = 0;
    int		gt_day = 0;
    int		gt_hour = 0;
    int		gt_min = 0;
    
    /* Initialize local buffers */
    *archie_date = '\0';

    /* Remember if we're looking at a directory */
    if (sindex(l->type,"DIRECTORY"))
	dirflag = 1;
    else
	dirflag = 0;
    
    /* Extract the linkpath from the filename */
    strcpy(linkpath,l->filename);
    *(linkpath + (strlen(linkpath) - strlen(l->name) - 1)) = '\0';
    
    /* Is this a new host? */
    if (strcmp(l->host,lasthost) != 0) {
	if (!listflag)
	    printf("\nHost %s\n\n",l->host);
	strcpy(lasthost,l->host);
	*lastpath = '\001';
    }
    
    /* Is this a new linkpath (location)? */
    if(strcmp(linkpath,lastpath) != 0) {
	if (!listflag)
	    printf("    Location: %s\n",(*linkpath ? linkpath : "/"));
	strcpy(lastpath,linkpath);
    }
    
    /* Parse the attibutes of this link */
    for (ap = l->lattrib; ap; ap = ap->next) {
	if (strcmp(ap->aname,"SIZE") == 0) {
#ifdef MSDOS
	    sscanf(ap->value.ascii,"%lu",&size);
#else
	    sscanf(ap->value.ascii,"%d",&size);
#endif
	} else if(strcmp(ap->aname,"UNIX-MODES") == 0) {
	    modes = ap->value.ascii;
	} else if(strcmp(ap->aname,"LAST-MODIFIED") == 0) {
	    gt_date = ap->value.ascii;
	    sscanf(gt_date,"%4d%2d%2d%2d%2d",&gt_year,
		   &gt_mon, &gt_day, &gt_hour, &gt_min);
	    if ((12 * (presenttime->tm_year + 1900 - gt_year) + 
					presenttime->tm_mon - gt_mon) > 6) 
		sprintf(archie_date,"%s %2d %4d",month_sname(gt_mon),
			gt_day, gt_year);
	    else
		sprintf(archie_date,"%s %2d %02d:%02d",month_sname(gt_mon),
			 gt_day, gt_hour, gt_min);
	}
    }
    
    /* Print this link's information */
    if (listflag)
#if defined(MSDOS)
	printf("%s %6lu %s %s%s\n",gt_date,size,l->host,l->filename,
	       (dirflag ? "/" : ""));
#else
	printf("%s %6d %s %s%s\n",gt_date,size,l->host,l->filename,
	       (dirflag ? "/" : ""));
#endif
    else
#ifdef MSDOS
	printf("      %9s %s %10lu  %s  %s\n",(dirflag ? "DIRECTORY" : "FILE"),
					modes,size,archie_date,l->name);
#else
	printf("      %9s %s %10d  %s  %s\n",(dirflag ? "DIRECTORY" : "FILE"),
					modes,size,archie_date,l->name);
#endif /* MSDOS */


    /* Free the attibutes */
    atlfree(l->lattrib);
    l->lattrib = NULL;
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * procquery : Process the given query and display the results. If
 *	sortflag is non-zero, then the results are sorted by increasing
 *	date, else by host/filename. If listflag is non-zero then each
 *	entry is printed on a separate, complete line. Note that listflag
 *	is ignored by xarchie.
 */
void procquery(char *host,char *str,int max_hits,int offset,char query_type,
               int sortflag,int listflag)
{
    VLINK l;
    long now;

    /* initialize data structures for this query */
    (void)time(&now);
    presenttime = localtime(&now);

    /* Do the query */
    if (sortflag == 1)
	l = archie_query(host,str,max_hits,offset,query_type,AQ_INVDATECMP,0);
    else
	l = archie_query(host,str,max_hits,offset,query_type,NULL,0);

    /* Error? */
    if (perrno != PSUCCESS) {
	if (p_err_text[perrno]) {
	    if (*p_err_string)
		fprintf(stderr, "%s: failed: %s - %s\n", progname,
		        p_err_text[perrno], p_err_string);
	    else
	        fprintf(stderr, "%s failed: %s\n", progname, p_err_text[perrno]);
	} else
	    fprintf(stderr, "%s failed: Undefined error %d (prospero)", 
                    (char*) perrno,0);
    }

    /* Warning? */
    if (pwarn != PNOWARN) {
	if (*p_warn_string)
	    fprintf(stderr, "%s: Warning! %s - %s\n", progname,
		p_warn_text[pwarn], p_warn_string);
	else
	    fprintf(stderr, "%s: Warning! %s\n", progname, p_warn_text[pwarn]);
    }


    /* Display the results */

    if (l == (VLINK)NULL && pwarn == PNOWARN && perrno == PSUCCESS) {
	if (! listflag) puts ("No matches.");
	exit (1);
    }

    *lasthost = '\001';
    *lastpath = '\001';
    while (l != NULL) {
	display_link(l,listflag);
	l = l->next;
    }
}
