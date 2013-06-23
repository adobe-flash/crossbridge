#ifndef lint
static char Rcs_Id[] =
    "$Id: ispell.c 33046 2007-01-09 23:57:19Z lattner $";
#endif

#define MAIN

/*
 * ispell.c - An interactive spelling corrector.
 *
 * Copyright (c), 1983, by Pace Willisson
 *
 * Copyright 1992, 1993, Geoff Kuenning, Granada Hills, CA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All modifications to the source code must be clearly marked as
 *    such.  Binary redistributions based on modified source code
 *    must be clearly marked as modified versions in the documentation
 *    and/or other materials provided with the distribution.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *      This product includes software developed by Geoff Kuenning and
 *      other unpaid contributors.
 * 5. The name of Geoff Kuenning may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:58:51  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.133  1995/10/11  04:30:29  geoff
 * Get rid of an unused variable.
 *
 * Revision 1.132  1995/08/05  23:19:36  geoff
 * If the DICTIONARY environment variable is set, derive the default
 * personal-dictionary name from it.
 *
 * Revision 1.131  1995/01/08  23:23:39  geoff
 * Support variable hashfile suffixes for DOS purposes.  Report all the
 * new configuration variables in the -vv switch.  Do some better error
 * checking for mktemp failures.  Support the rename system call.  All of
 * this is to help make DOS porting easier.
 *
 * Revision 1.130  1995/01/03  19:24:08  geoff
 * When constructing a personal-dictioary name from the hash file name,
 * don't stupidly include path directory components.
 *
 * Revision 1.129  1995/01/03  02:23:19  geoff
 * Disable the setbuf call on BSDI systems, sigh.
 *
 * Revision 1.128  1994/10/26  05:12:28  geoff
 * Include boundary characters in the list of characters to be tried in
 * corrections.
 *
 * Revision 1.127  1994/10/25  05:46:07  geoff
 * Allow the default dictionary to be specified by an environment
 * variable (DICTIONARY) as well as a switch.
 *
 * Revision 1.126  1994/09/16  03:32:34  geoff
 * Issue an error message for bad affix flags
 *
 * Revision 1.125  1994/07/28  05:11:36  geoff
 * Log message for previous revision: fix backup-file checks to correctly
 * test for exceeding MAXNAMLEN.
 *
 * Revision 1.124  1994/07/28  04:53:39  geoff
 *
 * Revision 1.123  1994/05/17  06:44:12  geoff
 * Add support for controlled compound formation and the COMPOUNDONLY
 * option to affix flags.
 *
 * Revision 1.122  1994/04/27  01:50:37  geoff
 * Print MAX_CAPS in -vv mode.
 *
 * Revision 1.121  1994/03/16  03:49:10  geoff
 * Fix -vv to display the value of NO_STDLIB_H.
 *
 * Revision 1.120  1994/03/15  06:24:28  geoff
 * Allow the -t, -n, and -T switches to override each other, as follows:
 * if no switches are given, the deformatter and string characters are
 * chosen based on the file suffix.  If only -t/-n are given, the
 * deformatter is forced but string cahracters come from the file suffix.
 * If only -T is given, the deformatter is chosen based on the value
 * given in the -T switch.  Finally, if both -T and -t/-n are given,
 * string characters are controlled by -T and the deformatter by -t/-n.
 *
 * Revision 1.119  1994/03/15  05:58:07  geoff
 * Get rid of a gcc warning
 *
 * Revision 1.118  1994/03/15  05:30:37  geoff
 * Get rid of an unused-variable complaint by proper ifdeffing
 *
 * Revision 1.117  1994/03/12  21:26:48  geoff
 * Correctly limit maximum name lengths for files that have directory paths
 * included.  Also don't use a wired-in 256 for the size of the backup file
 * name.
 *
 * Revision 1.116  1994/02/07  08:10:44  geoff
 * Print GENERATE_LIBRARY_PROTOS in the -vv switch.
 *
 * Revision 1.115  1994/01/26  07:44:47  geoff
 * Make yacc configurable through local.h.
 *
 * Revision 1.114  1994/01/25  07:11:44  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include "config.h"
#include "ispell.h"
#include "proto.h"
#include "msgs.h"
#include "version.h"
#include <ctype.h>
#include <sys/stat.h>

static void	usage P ((void));
static void	initckch P ((char * wchars));
int		main P ((int argc, char * argv[]));
static void	dofile P ((char * filename));
static void	update_file P ((char * filename, struct stat * statbuf));
static void	expandmode P ((int printorig));

static char *	Cmd;
static char *	LibDict = NULL;		/* Pointer to name of $(LIBDIR)/dict */

static void usage ()
    {

    (void) fprintf (stderr, ISPELL_C_USAGE1, Cmd);
    (void) fprintf (stderr, ISPELL_C_USAGE2, Cmd);
    (void) fprintf (stderr, ISPELL_C_USAGE3, Cmd);
    (void) fprintf (stderr, ISPELL_C_USAGE4, Cmd);
    (void) fprintf (stderr, ISPELL_C_USAGE5, Cmd);
    (void) fprintf (stderr, ISPELL_C_USAGE6, Cmd);
    (void) fprintf (stderr, ISPELL_C_USAGE7, Cmd);
    givehelp (0);
    exit (1);
    }

static void initckch (wchars)
    char *		wchars;		/* Characters in -w option, if any */
    {
    register ichar_t	c;
    char		num[4];

    for (c = 0; c < (ichar_t) (SET_SIZE + hashheader.nstrchars); ++c)
	{
	if (iswordch (c))
	    {
	    if (!mylower (c))
		{
		Try[Trynum] = c;
		++Trynum;
		}
	    }
	else if (isboundarych (c))
	    {
	    Try[Trynum] = c;
	    ++Trynum;
	    }
	}
    if (wchars != NULL)
	{
	while (Trynum < SET_SIZE  &&  *wchars != '\0')
	    {
	    if (*wchars != 'n'  &&  *wchars != '\\')
		{
		c = *wchars;
		++wchars;
		}
	    else
		{
		++wchars;
		num[0] = '\0'; 
		num[1] = '\0'; 
		num[2] = '\0'; 
		num[3] = '\0';
		if (isdigit (wchars[0]))
		    {
		    num[0] = wchars[0];
		    if (isdigit (wchars[1]))
			{
			num[1] = wchars[1];
			if (isdigit (wchars[2]))
			    num[2] = wchars[2];
			}
		    }
		if (wchars[-1] == 'n')
		    {
		    wchars += strlen (num);
		    c = atoi (num);
		    }
		else
		    {
		    wchars += strlen (num);
		    c = 0;
		    if (num[0])
			c = num[0] - '0';
		    if (num[1])
			{
			c <<= 3;
			c += num[1] - '0';
			}
		    if (num[2])
			{
			c <<= 3;
			c += num[2] - '0';
			}
		    }
		}
	    c &= NOPARITY;
	    if (!hashheader.wordchars[c])
		{
		hashheader.wordchars[c] = 1;
		hashheader.sortorder[c] = hashheader.sortval++;
		Try[Trynum] = c;
		++Trynum;
		}
	    }
	}
    }

int main (argc, argv)
    int		argc;
    char *	argv[];
    {
    char *	p;
    char *	cpd;
    char **	versionp;
    char *	wchars = NULL;
    char *	preftype = NULL;
    static char	libdictname[sizeof DEFHASH];
    static char	outbuf[BUFSIZ];
    int		argno;
    int		arglen;

    Cmd = *argv;

    Trynum = 0;

    p = getenv ("DICTIONARY");
    if (p != NULL)
	{
	if (index (p, '/') != NULL)
	    (void) strcpy (hashname, p);
	else
	    (void) sprintf (hashname, "%s/%s", LIBDIR, p);
	(void) strcpy (libdictname, p);
	p = rindex (p, '.');
	if (p == NULL  ||  strcmp (p, HASHSUFFIX) != 0)
	    (void) strcat (hashname, HASHSUFFIX);
	LibDict = rindex (libdictname, '/');
	if (LibDict != NULL)
	    LibDict++;
	else
	    LibDict = libdictname;
	p = rindex (libdictname, '.');
	if (p != NULL)
	    *p = '\0';
	}
    else
	(void) sprintf (hashname, "%s/%s", LIBDIR, DEFHASH);

    cpd = NULL;

    argv++;
    argc--;
    while (argc && **argv == '-')
	{
	/*
	 * Trying to add a new flag?  Can't remember what's been used?
	 * Here's a handy guide:
	 *
	 * Used:
	 *
	 *	ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789
	 *	^^^^       ^^^ ^  ^^ ^^
	 *	abcdefghijklmnopqrstuvwxyz
	 *	^^^^^^     ^^^ ^  ^^ ^^^
	 */
	arglen = strlen (*argv);
	switch ((*argv)[1])
	    {
	    case 'v':
		if (arglen > 3)
		    usage ();
		for (versionp = Version_ID;  *versionp;  )
		    {
		    p = *versionp++;
		    if (strncmp (p, "(#) ", 5) == 0)
		      p += 5;
		    (void) printf ("%s\n", p);
		    }
		if ((*argv)[2] == 'v')
		    {
		    (void) printf (ISPELL_C_OPTIONS_ARE);
#ifdef USG
		    (void) printf ("\tUSG\n");
#else /* USG */
		    (void) printf ("\t!USG (BSD)\n");
#endif /* USG */
		    (void) printf ("\tBAKEXT = \"%s\"\n", BAKEXT);
		    (void) printf ("\tBINDIR = \"%s\"\n", BINDIR);
#ifdef BOTTOMCONTEXT
		    (void) printf ("\tBOTTOMCONTEXT\n");
#else /* BOTTOMCONTEXT */
		    (void) printf ("\t!BOTTOMCONTEXT\n");
#endif /* BOTTOMCONTEXT */
#if TERM_MODE == CBREAK
		    (void) printf ("\tCBREAK\n");
#endif /* TERM_MODE */
		    (void) printf ("\tCC = \"%s\"\n", CC);
		    (void) printf ("\tCFLAGS = \"%s\"\n", CFLAGS);
#ifdef COMMANDFORSPACE
		    (void) printf ("\tCOMMANDFORSPACE\n");
#else /* COMMANDFORSPACE */
		    (void) printf ("\t!COMMANDFORSPACE\n");
#endif /* COMMANDFORSPACE */
#ifdef CONTEXTROUNDUP
		    (void) printf ("\tCONTEXTROUNDUP\n");
#else /* CONTEXTROUNDUP */
		    (void) printf ("\t!CONTEXTROUNDUP\n");
#endif /* CONTEXTROUNDUP */
		    (void) printf ("\tCONTEXTPCT = %d\n", CONTEXTPCT);
		    (void) printf ("\tCOUNTSUFFIX = \"%s\"\n", COUNTSUFFIX);
		    (void) printf ("\tDEFHASH = \"%s\"\n", DEFHASH);
		    (void) printf ("\tDEFINCSTR = \"%s\"\n", DEFINCSTR);
		    (void) printf ("\tDEFLANG = \"%s\"\n", DEFLANG);
		    (void) printf ("\tDEFNOBACKUPFLAG = %d\n",
		      DEFNOBACKUPFLAG);
		    (void) printf ("\tDEFPAFF = \"%s\"\n", DEFPAFF);
		    (void) printf ("\tDEFPDICT = \"%s\"\n", DEFPDICT);
		    (void) printf ("\tDEFTEXFLAG = %d\n", DEFTEXFLAG);
		    (void) printf ("\tEGREPCMD = \"%s\"\n", EGREPCMD);
		    (void) printf ("\tELISPDIR = \"%s\"\n", ELISPDIR);
		    (void) printf ("\tEMACS = \"%s\"\n", EMACS);
#ifdef EQUAL_COLUMNS
		    (void) printf ("\tEQUAL_COLUMNS\n");
#else /* EQUAL_COLUMNS */
		    (void) printf ("\t!EQUAL_COLUMNS\n");
#endif /* EQUAL_COLUMNS */
#ifdef GENERATE_LIBRARY_PROTOS
		    (void) printf ("\tGENERATE_LIBRARY_PROTOS\n");
#else /* GENERATE_LIBRARY_PROTOS */
		    (void) printf ("\t!GENERATE_LIBRARY_PROTOS\n");
#endif /* GENERATE_LIBRARY_PROTOS */
#ifdef HAS_RENAME
		    (void) printf ("\tHAS_RENAME\n");
#else /* HAS_RENAME */
		    (void) printf ("\t!HAS_RENAME\n");
#endif /* HAS_RENAME */
		    (void) printf ("\tHASHSUFFIX = \"%s\"\n", HASHSUFFIX);
		    (void) printf ("\tHOME = \"%s\"\n", HOME);
#ifdef IGNOREBIB
		    (void) printf ("\tIGNOREBIB\n");
#else /* IGNOREBIB */
		    (void) printf ("\t!IGNOREBIB\n");
#endif /* IGNOREBIB */
		    (void) printf ("\tINCSTRVAR = \"%s\"\n", INCSTRVAR);
		    (void) printf ("\tINPUTWORDLEN = %d\n", INPUTWORDLEN);
		    (void) printf ("\tLANGUAGES = \"%s\"\n", LANGUAGES);
		    (void) printf ("\tLIBDIR = \"%s\"\n", LIBDIR);
		    (void) printf ("\tLIBES = \"%s\"\n", LIBES);
		    (void) printf ("\tLINT = \"%s\"\n", LINT);
		    (void) printf ("\tLINTFLAGS = \"%s\"\n", LINTFLAGS);
#ifndef REGEX_LOOKUP
		    (void) printf ("\tLOOK = \"%s\"\n", LOOK);
#endif /* REGEX_LOOKUP */
		    (void) printf ("\tMAKE_SORTTMP = \"%s\"\n", MAKE_SORTTMP);
		    (void) printf ("\tMALLOC_INCREMENT = %d\n",
		      MALLOC_INCREMENT);
		    (void) printf ("\tMAN1DIR = \"%s\"\n", MAN1DIR);
		    (void) printf ("\tMAN1EXT = \"%s\"\n", MAN1EXT);
		    (void) printf ("\tMAN4DIR = \"%s\"\n", MAN4DIR);
		    (void) printf ("\tMAN4EXT = \"%s\"\n", MAN4EXT);
		    (void) printf ("\tMASKBITS = %d\n", MASKBITS);
		    (void) printf ("\tMASKTYPE = %s\n", MASKTYPE_STRING);
		    (void) printf ("\tMASKTYPE_WIDTH = %d\n", MASKTYPE_WIDTH);
		    (void) printf ("\tMASTERHASH = \"%s\"\n", MASTERHASH);
		    (void) printf ("\tMAXAFFIXLEN = %d\n", MAXAFFIXLEN);
		    (void) printf ("\tMAXCONTEXT = %d\n", MAXCONTEXT);
		    (void) printf ("\tMAXINCLUDEFILES = %d\n",
		      MAXINCLUDEFILES);
		    (void) printf ("\tMAXNAMLEN = %d\n", MAXNAMLEN);
		    (void) printf ("\tMAXPATHLEN = %d\n", MAXPATHLEN);
		    (void) printf ("\tMAXPCT = %d\n", MAXPCT);
		    (void) printf ("\tMAXSEARCH = %d\n", MAXSEARCH);
		    (void) printf ("\tMAXSTRINGCHARLEN = %d\n",
		      MAXSTRINGCHARLEN);
		    (void) printf ("\tMAXSTRINGCHARS = %d\n", MAXSTRINGCHARS);
		    (void) printf ("\tMAX_CAPS = %d\n", MAX_CAPS);
		    (void) printf ("\tMAX_HITS = %d\n", MAX_HITS);
		    (void) printf ("\tMAX_SCREEN_SIZE = %d\n",
		      MAX_SCREEN_SIZE);
		    (void) printf ("\tMINCONTEXT = %d\n", MINCONTEXT);
#ifdef MINIMENU
		    (void) printf ("\tMINIMENU\n");
#else /* MINIMENU */
		    (void) printf ("\t!MINIMENU\n");
#endif /* MINIMENU */
		    (void) printf ("\tMINWORD = %d\n", MINWORD);
		    (void) printf ("\tMSDOS_BINARY_OPEN = 0x%x\n",
		      (unsigned int) MSDOS_BINARY_OPEN);
		    (void) printf ("\tMSGLANG = %s\n", MSGLANG);
#ifdef NO_CAPITALIZATION_SUPPORT
		    (void) printf ("\tNO_CAPITALIZATION_SUPPORT\n");
#else /* NO_CAPITALIZATION_SUPPORT */
		    (void) printf ("\t!NO_CAPITALIZATION_SUPPORT\n");
#endif /* NO_CAPITALIZATION_SUPPORT */
#ifdef NO_STDLIB_H
		    (void) printf ("\tNO_STDLIB_H\n");
#else /* NO_STDLIB_H */
		    (void) printf ("\t!NO_STDLIB_H (STDLIB_H)\n");
#endif /* NO_STDLIB_H */
#ifdef NO8BIT
		    (void) printf ("\tNO8BIT\n");
#else /* NO8BIT */
		    (void) printf ("\t!NO8BIT (8BIT)\n");
#endif /* NO8BIT */
		    (void) printf ("\tNRSPECIAL = \"%s\"\n", NRSPECIAL);
		    (void) printf ("\tOLDPAFF = \"%s\"\n", OLDPAFF);
		    (void) printf ("\tOLDPDICT = \"%s\"\n", OLDPDICT);
#ifdef PDICTHOME
		    (void) printf ("\tPDICTHOME = \"%s\"\n", PDICTHOME);
#else /* PDICTHOME */
		    (void) printf ("\tPDICTHOME = (undefined)\n");
#endif /* PDICTHOME */
		    (void) printf ("\tPDICTVAR = \"%s\"\n", PDICTVAR);
#ifdef PIECEMEAL_HASH_WRITES
		    (void) printf ("\tPIECEMEAL_HASH_WRITES\n");
#else /* PIECEMEAL_HASH_WRITES */
		    (void) printf ("\t!PIECEMEAL_HASH_WRITES\n");
#endif /* PIECEMEAL_HASH_WRITES */
#if TERM_MODE != CBREAK
		    (void) printf ("\tRAW\n");
#endif /* TERM_MODE */
#ifdef REGEX_LOOKUP
		    (void) printf ("\tREGEX_LOOKUP\n");
#else /* REGEX_LOOKUP */
		    (void) printf ("\t!REGEX_LOOKUP\n");
#endif /* REGEX_LOOKUP */
		    (void) printf ("\tREGLIB = \"%s\"\n", REGLIB);
		    (void) printf ("\tSIGNAL_TYPE = %s\n", SIGNAL_TYPE_STRING);
		    (void) printf ("\tSORTPERSONAL = %d\n", SORTPERSONAL);
		    (void) printf ("\tSTATSUFFIX = \"%s\"\n", STATSUFFIX);
		    (void) printf ("\tTEMPNAME = \"%s\"\n", TEMPNAME);
		    (void) printf ("\tTERMLIB = \"%s\"\n", TERMLIB);
		    (void) printf ("\tTEXINFODIR = \"%s\"\n", TEXINFODIR);
		    (void) printf ("\tTEXSPECIAL = \"%s\"\n", TEXSPECIAL);
#ifdef TRUNCATEBAK
		    (void) printf ("\tTRUNCATEBAK\n");
#else /* TRUNCATEBAK */
		    (void) printf ("\t!TRUNCATEBAK\n");
#endif /* TRUNCATEBAK */
#ifdef USESH
		    (void) printf ("\tUSESH\n");
#else /* USESH */
		    (void) printf ("\t!USESH\n");
#endif /* USESH */
		    (void) printf ("\tWORDS = \"%s\"\n", WORDS);
		    (void) printf ("\tYACC = \"%s\"\n", YACC);
		    }
		exit (0);
		break;
	    case 'n':
		if (arglen > 2)
		    usage ();
		tflag = 0;		/* nroff/troff mode */
		deftflag = 0;
		if (preftype == NULL)
		    preftype = "nroff";
		break;
	    case 't':			/* TeX mode */
		if (arglen > 2)
		    usage ();
		tflag = 1;
		deftflag = 1;
		if (preftype == NULL)
		    preftype = "tex";
		break;
	    case 'T':			/* Set preferred file type */
		p = (*argv)+2;
		if (*p == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    p = *argv;
		    }
		preftype = p;
		break;
	    case 'A':
		if (arglen > 2)
		    usage ();
		incfileflag = 1;
		aflag = 1;
		break;
	    case 'a':
		if (arglen > 2)
		    usage ();
		aflag++;
		break;
	    case 'D':
		if (arglen > 2)
		    usage ();
		dumpflag++;
		nodictflag++;
		break;
	    case 'e':
		if (arglen > 3)
		    usage ();
		eflag = 1;
		if ((*argv)[2] == 'e')
		    eflag = 2;
		else if ((*argv)[2] >= '1'  &&  (*argv)[2] <= '4')
		    eflag = (*argv)[2] - '0';
		else if ((*argv)[2] != '\0')
		    usage ();
		nodictflag++;
		break;
	    case 'c':
		if (arglen > 2)
		    usage ();
		cflag++;
		lflag++;
		nodictflag++;
		break;
	    case 'b':
		if (arglen > 2)
		    usage ();
		xflag = 0;		/* Keep a backup file */
		break;
	    case 'x':
		if (arglen > 2)
		    usage ();
		xflag = 1;		/* Don't keep a backup file */
		break;
	    case 'f':
		fflag++;
		p = (*argv)+2;
		if (*p == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    p = *argv;
		    }
		askfilename = p;
		if (*askfilename == '\0')
		    askfilename = NULL;
		break;
	    case 'L':
		p = (*argv)+2;
		if (*p == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    p = *argv;
		    }
		contextsize = atoi (p);
		break;
	    case 'l':
		if (arglen > 2)
		    usage ();
		lflag++;
		break;
#ifndef USG
	    case 's':
		if (arglen > 2)
		    usage ();
		sflag++;
		break;
#endif
	    case 'S':
		if (arglen > 2)
		    usage ();
		sortit = 0;
		break;
	    case 'B':		/* -B:  report missing blanks */
		if (arglen > 2)
		    usage ();
		compoundflag = COMPOUND_NEVER;
		break;
	    case 'C':		/* -C:  compound words are acceptable */
		if (arglen > 2)
		    usage ();
		compoundflag = COMPOUND_ANYTIME;
		break;
	    case 'P':		/* -P:  don't gen non-dict poss's */
		if (arglen > 2)
		    usage ();
		tryhardflag = 0;
		break;
	    case 'm':		/* -m:  make all poss affix combos */
		if (arglen > 2)
		    usage ();
		tryhardflag = 1;
		break;
	    case 'N':		/* -N:  suppress minimenu */
		if (arglen > 2)
		    usage ();
		minimenusize = 0;
		break;
	    case 'M':		/* -M:  force minimenu */
		if (arglen > 2)
		    usage ();
		minimenusize = 2;
		break;
	    case 'p':
		cpd = (*argv)+2;
		if (*cpd == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    cpd = *argv;
		    if (*cpd == '\0')
			cpd = NULL;
		    }
		LibDict = NULL;
		break;
	    case 'd':
		p = (*argv)+2;
		if (*p == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    p = *argv;
		    }
		if (index (p, '/') != NULL)
		    (void) strcpy (hashname, p);
		else
		    (void) sprintf (hashname, "%s/%s", LIBDIR, p);
		if (cpd == NULL  &&  *p != '\0')
		    LibDict = p;
		p = rindex (p, '.');
		if (p != NULL  &&  strcmp (p, HASHSUFFIX) == 0)
		    *p = '\0';	/* Don't want ext. in LibDict */
		else
		    (void) strcat (hashname, HASHSUFFIX);
		if (LibDict != NULL)
		    {
		    p = rindex (LibDict, '/');
		    if (p != NULL)
			LibDict = p + 1;
		    }
		break;
	    case 'V':		/* Display 8-bit characters as M-xxx */
		if (arglen > 2)
		    usage ();
		vflag = 1;
		break;
	    case 'w':
		wchars = (*argv)+2;
		if (*wchars == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    wchars = *argv;
		    }
		break;
	    case 'W':
		if ((*argv)[2] == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    minword = atoi (*argv);
		    }
		else
		    minword = atoi (*argv + 2);
		break;
	    default:
		usage ();
	    }
	argv++;
	argc--;
	}

    if (!argc  &&  !lflag  &&  !aflag   &&  !eflag  &&  !dumpflag)
	usage ();

    /*
     * Because of the high cost of reading the dictionary, we stat
     * the files specified first to see if they exist.  If at least
     * one exists, we continue.
     */
    for (argno = 0;  argno < argc;  argno++)
	{
	if (access (argv[argno], 4) >= 0)
	    break;
	}
    if (argno >= argc  &&  !lflag  &&  !aflag  &&  !eflag  &&  !dumpflag)
	{
	(void) fprintf (stderr,
	  argc == 1 ? ISPELL_C_NO_FILE : ISPELL_C_NO_FILES);
	exit (1);
	}
    if (linit () < 0)
	exit (1);

    if (preftype != NULL)
	{
	prefstringchar =
	  findfiletype (preftype, 1, deftflag < 0 ? &deftflag : (int *) NULL);
	if (prefstringchar < 0
	  &&  strcmp (preftype, "tex") != 0
	  &&  strcmp (preftype, "nroff") != 0)
	    {
	    (void) fprintf (stderr, ISPELL_C_BAD_TYPE, preftype);
	    exit (1);
	    }
	}
    if (prefstringchar < 0)
	defdupchar = 0;
    else
	defdupchar = prefstringchar;

    if (compoundflag < 0)
	compoundflag = hashheader.compoundflag;
    if (tryhardflag < 0)
	tryhardflag = hashheader.defhardflag;

    initckch(wchars);

    if (LibDict == NULL)	
	{
	(void) strcpy (libdictname, DEFHASH);
	LibDict = libdictname;
	p = rindex (libdictname, '.');
	if (p != NULL  &&  strcmp (p, HASHSUFFIX) == 0)
	    *p = '\0';	/* Don't want ext. in LibDict */
	}
    if (!nodictflag)
	treeinit (cpd, LibDict);

    if (aflag)
	{
	askmode ();
	treeoutput ();
	exit (0);
	}
    else if (eflag)
	{
	expandmode (eflag);
	exit (0);
	}
    else if (dumpflag)
	{
	dumpmode ();
	exit (0);
	}

#ifndef __bsdi__
    setbuf (stdout, outbuf);
#endif /* __bsdi__ */
    if (lflag)
	{
	infile = stdin;
	outfile = stdout;
	checkfile ();
	exit (0);
	}

    terminit ();

    while (argc--)
	dofile (*argv++);

    done (0);
    /* NOTREACHED */
    return 0;
    }

static void dofile (filename)
    char *	filename;
    {
    struct stat	statbuf;
    char *	cp;

    currentfile = filename;

    /* See if the file is a .tex file.  If so, set the appropriate flags. */
    tflag = deftflag;
    if (tflag < 0)
	tflag =
	  (cp = rindex (filename, '.')) != NULL  &&  strcmp (cp, ".tex") == 0;

    if (prefstringchar < 0)
	{
	defdupchar =
	  findfiletype (filename, 0, deftflag < 0 ? &tflag : (int *) NULL);
	if (defdupchar < 0)
	    defdupchar = 0;
	}

    if ((infile = fopen (filename, "r")) == NULL)
	{
	(void) fprintf (stderr, CANT_OPEN, filename);
	(void) sleep ((unsigned) 2);
	return;
	}

    readonly = access (filename, 2) < 0;
    if (readonly)
	{
	(void) fprintf (stderr, ISPELL_C_CANT_WRITE, filename);
	(void) sleep ((unsigned) 2);
	}

    (void) fstat (fileno (infile), &statbuf);
    (void) strcpy (tempfile, TEMPNAME);
    if (mktemp (tempfile) == NULL  ||  tempfile[0] == '\0'
      ||  (outfile = fopen (tempfile, "w")) == NULL)
	{
	(void) fprintf (stderr, CANT_CREATE,
	  (tempfile == NULL  ||  tempfile[0] == '\0')
	    ? "temporary file" : tempfile);
	(void) sleep ((unsigned) 2);
	return;
	}
    (void) chmod (tempfile, statbuf.st_mode);

    quit = 0;
    changes = 0;

    checkfile ();

    (void) fclose (infile);
    (void) fclose (outfile);

    if (!cflag)
	treeoutput ();

    if (changes && !readonly)
	update_file (filename, &statbuf);
    (void) unlink (tempfile);
    }

static void update_file (filename, statbuf)
    char *		filename;
    struct stat *	statbuf;
    {
    char		bakfile[MAXPATHLEN];
    int			c;
    char *		pathtail;

    if ((infile = fopen (tempfile, "r")) == NULL)
	{
	(void) fprintf (stderr, ISPELL_C_TEMP_DISAPPEARED, tempfile);
	(void) sleep ((unsigned) 2);
	return;
	}

#ifdef TRUNCATEBAK
    (void) strncpy (bakfile, filename, sizeof bakfile - 1);
    bakfile[sizeof bakfile - 1] = '\0';
    if (strcmp(BAKEXT, filename + strlen(filename) - sizeof BAKEXT - 1) != 0)
	{
	pathtail = rindex (bakfile, '/');
	if (pathtail == NULL)
	    pathtail = bakfile;
	else
	    pathtail++;
	if (strlen (pathtail) > MAXNAMLEN - sizeof BAKEXT - 1)
	    pathtail[MAXNAMLEN - sizeof BAKEXT -1] = '\0';
	(void) strcat (pathtail, BAKEXT);
	}
#else
    (void) sprintf (bakfile, "%.*s%s", (int) (sizeof bakfile - sizeof BAKEXT),
      filename, BAKEXT);
#endif

    pathtail = rindex (bakfile, '/');
    if (pathtail == NULL)
	pathtail = bakfile;
    else
	pathtail++;
    if (strncmp (filename, bakfile, pathtail - bakfile + MAXNAMLEN) != 0)
	(void) unlink (bakfile);	/* unlink so we can write a new one. */
#ifdef HAS_RENAME
    (void) rename (filename, bakfile);
#else /* HAS_RENAME */
    if (link (filename, bakfile) == 0)
	(void) unlink (filename);
#endif /* HAS_RENAME */

    /* if we can't write new, preserve .bak regardless of xflag */
    if ((outfile = fopen (filename, "w")) == NULL)
	{
	(void) fprintf (stderr, CANT_CREATE, filename);
	(void) sleep ((unsigned) 2);
	return;
	}

    (void) chmod (filename, statbuf->st_mode);

    while ((c = getc (infile)) != EOF)
	(void) putc (c, outfile);

    (void) fclose (infile);
    (void) fclose (outfile);

    if (xflag
      &&  strncmp (filename, bakfile, pathtail - bakfile + MAXNAMLEN) != 0)
	(void) unlink (bakfile);
    }

static void expandmode (option)
    int			option;		/* How to print: */
					/* 1 = expansions only */
					/* 2 = original line + expansions */
					/* 3 = original paired w/ expansions */
					/* 4 = add length ratio */
    {
    char		buf[BUFSIZ];
    int			explength;	/* Total length of all expansions */
    register char *	flagp;		/* Pointer to next flag char */
    ichar_t		ibuf[BUFSIZ];
    MASKTYPE		mask[MASKSIZE];
    char		origbuf[BUFSIZ]; /* Original contents of buf */
    char		ratiobuf[20];	/* Expansion/root length ratio */
    int			rootlength;	/* Length of root word */
    register int	temp;

    while (xgets (buf, sizeof buf, stdin) != NULL)
	{
	rootlength = strlen (buf);
	if (buf[rootlength - 1] == '\n')
	  buf[--rootlength] = '\0';
	(void) strcpy (origbuf, buf);
	if ((flagp = index (buf, hashheader.flagmarker)) != NULL)
	    {
	    rootlength = flagp - buf;
	    *flagp++ = '\0';
	    }
	if (option == 2  ||  option == 3  ||  option == 4)
	    (void) printf ("%s ", origbuf);
	if (flagp != NULL)
	    {
	    if (flagp - buf > INPUTWORDLEN)
		buf[INPUTWORDLEN] = '\0';
	    }
	else
	    {
	    if ((int) strlen (buf) > INPUTWORDLEN - 1)
		buf[INPUTWORDLEN] = '\0';
	    }
	(void) fputs (buf, stdout);
	if (flagp != NULL)
	    {
	    (void) bzero ((char *) mask, sizeof (mask));
	    while (*flagp != '\0'  &&  *flagp != '\n')
		{
		temp = CHARTOBIT ((unsigned char) *flagp);
		if (temp >= 0  &&  temp <= LARGESTFLAG)
		    SETMASKBIT (mask, temp);
		else
		    (void) fprintf (stderr, BAD_FLAG, (unsigned char) *flagp);
		flagp++;
		/* Accept old-format dicts with extra slashes */
		if (*flagp == hashheader.flagmarker)
		    flagp++;
		}
	    if (strtoichar (ibuf, buf, sizeof ibuf, 1))
		(void) fprintf (stderr, WORD_TOO_LONG (buf));
	    explength = expand_pre (origbuf, ibuf, mask, option, "");
	    explength += expand_suf (origbuf, ibuf, mask, 0, option, "");
	    explength += rootlength;
	    if (option == 4)
		{
		(void) sprintf (ratiobuf, " %f",
		  (double) explength / (double) rootlength);
		(void) fputs (ratiobuf, stdout);
		(void) expand_pre (origbuf, ibuf, mask, 3, ratiobuf);
		(void) expand_suf (origbuf, ibuf, mask, 0, 3, ratiobuf);
		}
	    }
	(void) putchar ('\n');
	}
    }
