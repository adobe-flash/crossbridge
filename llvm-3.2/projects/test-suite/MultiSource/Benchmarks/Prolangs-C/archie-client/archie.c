/*
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.1 - 11/21/91 (bkc) - added CUTCP library support for MSDOS
 * v1.2.0 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 * v1.1.2 - 08/27/91 (bpk) - added <pmachine.h> for index()
 * v1.1.1 - 08/22/91 (bpk) - added 0-9 as arguments
 */

#include <string.h>
#include <copyright.h>

/*
 * Archie client using the Prospero protocol.
 *
 * Suggestions and improvements to Brendan Kehoe (brendan@cs.widener.edu).
 */

#include <stdio.h>
#if defined(OS2)
# include <pctcp.h>
#endif
#ifdef MSDOS
# include <string.h>
# include <stdlib.h>
# ifdef CUTCP
#  include <msdos/cutcp.h>
#  include <msdos/hostform.h>
# endif
#endif

#include <pfs.h>
#include <rdgram.h>
#include <archie.h>
#include <pmachine.h>

int		listflag = 0;
int		sortflag = 0;   /* 1 = by date                    */
char		*progname;
#ifdef DEBUG
extern int	pfs_debug;
#endif
extern int	rdgram_priority;

/* To keep the code clean.. */
#ifdef VMS
# define NFLAG	"\"N\""
# define LFLAG	"\"L\""
# define A_EXIT	SS$_NORMAL
#else
# define NFLAG	"N"
# define LFLAG	"L"
# define A_EXIT	1
#endif
#ifdef CUTCP
# define HFLAG	"[H config.tel]]"
#else
# define HFLAG	"]"
#endif

#define ARCHIE_HOST "archie.rutgers.edu"

int main(int argc,char *argv[])
    {
	char		*cur_arg;
	char		qtype = '=';    /* Default to exact string match  */
	char		etype = '=';	/* Type if only -e is specified   */
	int		eflag = 0;	/* Exact flag specified		  */
	int		max_hits = MAX_HITS;
	int		offset = 0;
	int		exitflag = 0;	/* Display release identifier     */
	int		tmp;
	char		*host = ARCHIE_HOST;
	char		*p;
	static char *archies[] = { ARCHIES };
#ifdef CUTCP
	int		hostset = 0;
#endif
	char		*getenv();

	progname = "archie";
	argc--; argv++;

#ifdef	CUTCP
	if(getenv("CONFIGTEL"))
		if(Shostfile(getenv("CONFIGTEL")) < 0) {
			fprintf(stderr,"Error, couldn't open configtel file %s\n",
				getenv("CONFIGTEL"));
			exit(1);
		}
#endif

	if ((p = getenv("ARCHIE_HOST")) != (char *)NULL)
	  host = p;
	    
	while (argc > 0 && **argv == '-') {
	    cur_arg = argv[0]+1;

	    /* If a - by itself, or --, then no more arguments */
	    if(!*cur_arg || ((*cur_arg == '-') && (!*(cur_arg+1)))) {
	        argc--, argv++;
		goto scandone;
	    }

	    while (*cur_arg) {
		switch (*cur_arg++) {
#ifdef DEBUG		
		case 'D':  /* Debug level */
		    pfs_debug = 1; /* Default debug level */
		    if(*cur_arg && strchr("0123456789",(int) *cur_arg)) {
			sscanf(cur_arg,"%d",&pfs_debug);
			cur_arg += strspn(cur_arg,"0123456789");
		    }
		    else if(argc > 2) {
		        tmp = sscanf(argv[1],"%d",&pfs_debug);
			if (tmp == 1) {argc--;argv++;}
		    }
		    break;
#endif
#ifdef	CUTCP
		case 'H' :
			if(Shostfile(argv[1]) < 0) {
				fprintf(stderr,"Error, couldn't open configtel file %s\n",argv[1]);
				exit(1);
			}
		 	argc--;argv++;
			break;
#endif
#ifndef XARCHIE
		case 'L':
		    printf("Known archie servers:\n");
		    for (tmp = 0; tmp < NARCHIES; tmp++)
			printf("\t%s\n", archies[tmp]);
		    printf(" * %s is the default Archie server.\n", ARCHIE_HOST);
		    printf(" * For the most up-to-date list, write to an Archie server and give it\n   the command `servers'.\n");
		    exitflag = 1;
		    break;
#endif

		case 'N':  /* Priority (nice) */
		    rdgram_priority = RDGRAM_MAX_PRI; /* Use this if no # */
		    if(*cur_arg && strchr("-0123456789",(int) *cur_arg)) {
			sscanf(cur_arg,"%d",&rdgram_priority);
			cur_arg += strspn(cur_arg,"-0123456789");
		    }
		    else if(argc > 2) {
		        tmp = sscanf(argv[1],"%d",&rdgram_priority);
			if (tmp == 1) {argc--;argv++;}
		    }
		    if(rdgram_priority > RDGRAM_MAX_SPRI) 
			rdgram_priority = RDGRAM_MAX_PRI;
		    if(rdgram_priority < RDGRAM_MIN_PRI) 
			rdgram_priority = RDGRAM_MIN_PRI;
  		    break;

		case 'c':  /* substring (case sensitive) */
		    qtype = 'C';
		    etype = 'c';
		    break;

		case 'e':  /* Exact match */
		    /* If -e specified by itself, then we use the  */
		    /* default value of etype which must be '='    */
		    eflag++;
		    break;

		case 'h':  /* Host */
		    host = argv[1];
#ifdef CUTCP
		    hostset++;
#endif
		    argc--; argv++;
		    break;

		case 'l':  /* List one match per line */
		    listflag++;
		    break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		    cur_arg--;
		case 'm':  /* Max hits */
		    max_hits = -1;  
		    if(*cur_arg && strchr("0123456789",(int) *cur_arg)) {
			sscanf(cur_arg,"%d",&max_hits);
			cur_arg += strspn(cur_arg,"0123456789");
		    }
		    else if(argc > 1) {
		        tmp = sscanf(argv[1],"%d",&max_hits);
			if (tmp == 1) {argc--;argv++;}
		    }
		    if (max_hits < 1) {
			fprintf(stderr, "%s: -m option requires a value for max hits (>= 1)\n",
				progname);
			exit(A_EXIT);
		    }
		    break;

		case 'o':  /* Offset */
		    if(argc > 1) {
		      tmp = sscanf(argv[1],"%d",&offset);
		      if (tmp != 1)
			argc = -1;
		      else {
			argc--; argv++;
		      }
		    }
		    break;

		case 'r':  /* Regular expression search */
		    qtype = 'R';
		    etype = 'r';
		    break;

		case 's':  /* substring (case insensitive) */
		    qtype = 'S';
		    etype = 's';
		    break;

		case 't':  /* Sort inverted by date */
		    sortflag = 1;
		    break;

		case 'v':  /* Display version */
		    fprintf(stderr,
			"Client version %s based upon Prospero version %s\n",
			    CLIENT_VERSION, PFS_RELEASE);
		    exitflag++;
		    break;

		default:
		    fprintf(stderr,"Usage: %s [-[cers][l][t][m#][h host][%s][%s#]%s string\n", progname, LFLAG, NFLAG, HFLAG);
		    exit(A_EXIT);
		}
	    }
	    argc--; argv++;
	}

      scandone:

	if (eflag) qtype = etype;

	if ((argc != 1) && exitflag) exit(0);

	if (argc != 1) {
	    fprintf(stderr, "Usage: %s [-[cers][l][t][m#][h host][%s][%s#]%s string\n", progname, LFLAG, NFLAG, HFLAG);
	    fprintf(stderr,"       -c : case sensitive substring search\n");
	    fprintf(stderr,"       -e : exact string match (default)\n");
	    fprintf(stderr,"       -r : regular expression search\n");
	    fprintf(stderr,"       -s : case insensitive substring search\n");
	    fprintf(stderr,"       -l : list one match per line\n");
	    fprintf(stderr,"       -t : sort inverted by date\n");
	    fprintf(stderr,"      -m# : specifies maximum number of hits to return (default %d)\n", max_hits);
	    fprintf(stderr,"  -h host : specifies server host\n");
	    fprintf(stderr,"       -%s : list known servers and current default\n", LFLAG);
	    fprintf(stderr,"      -%s# : specifies query niceness level (0-35765)\n", NFLAG);
#ifdef CUTCP
	    fprintf(stderr,"-H config.tel: specify location of config.tel file\n");
#endif
	    exit(A_EXIT);
	}

#ifdef	CUTCP
	if(argc = Snetinit()) {
	       	fprintf(stderr,"Error %d from SNetinit (bad or missing config.tel ?)\n",argc);
		if(argc == -2)
		       	netshut();	/* rarp lookup failure */
		exit(1);
	}
	if(!hostset) {		/* if no host on command line, look in config.tel file
	       			   for name=archie */
		struct machinfo *mp;

	 	mp = Shostlook("archie");
		if(mp) {
		       	host = mp->hname ? mp->hname : mp->sname;
		}
       }
#endif

	procquery(host, argv[0], max_hits, offset, qtype, sortflag, listflag);

#ifdef CUTCP
	netshut();
#endif
	exit(0);
        return 0;
    }
