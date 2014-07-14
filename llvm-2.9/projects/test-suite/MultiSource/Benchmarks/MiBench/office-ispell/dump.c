#ifndef lint
static char Rcs_Id[] =
    "$Id: dump.c 33046 2007-01-09 23:57:19Z lattner $";
#endif

/*
 * dump.c - Ispell's dump mode
 *
 * This code originally resided in ispell.c, but was moved here to keep
 * file sizes smaller.
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
 * Revision 1.1.1.1  2007/01/09 02:58:50  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.14  1994/01/25  07:11:27  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include "config.h"
#include "ispell.h"
#include "proto.h"

void		dumpmode P ((void));
static void	tbldump P ((struct flagent * flagp, int numflags));
static void	entdump P ((struct flagent * flagp));
static void	setdump P ((char * setp, int mask));
static void	subsetdump P ((char * setp, int mask, int dumpval));

void dumpmode ()
    {

    if (hashheader.flagmarker == '\\'
      ||  hashheader.flagmarker == '#'
      ||  hashheader.flagmarker == '>'
      ||  hashheader.flagmarker == ':'
      ||  hashheader.flagmarker == '-'
      ||  hashheader.flagmarker == ','
      ||  hashheader.flagmarker == '[') /* ] */
	(void) printf ("flagmarker \\%c\n", hashheader.flagmarker);
    else if (hashheader.flagmarker < ' '  ||  hashheader.flagmarker >= 0177)
	(void) printf ("flagmarker \\%3.3o\n",
	 (unsigned int) hashheader.flagmarker & 0xFF);
    else	    
	(void) printf ("flagmarker %c\n", hashheader.flagmarker);
    if (numpflags)
	{
	(void) printf ("prefixes\n");
	tbldump (pflaglist, numpflags);
	}
    if (numsflags)
	{
	(void) printf ("suffixes\n");
	tbldump (sflaglist, numsflags);
	}
    }

static void tbldump (flagp, numflags)	/* Dump a flag table */
    register struct flagent *	flagp;	/* First flag entry to dump */
    register int		numflags; /* Number of flags to dump */
    {

    while (--numflags >= 0)
	entdump (flagp++);
    }

static void entdump (flagp)		/* Dump one flag entry */
    register struct flagent *	flagp;	/* Flag entry to dump */
    {
    register int		cond;	/* Condition number */

    (void) printf ("  flag %s%c: ",
      (flagp->flagflags & FF_CROSSPRODUCT) ? "*" : " ",
      BITTOCHAR (flagp->flagbit));
    for (cond = 0;  cond < flagp->numconds;  cond++)
	{
	setdump (flagp->conds, 1 << cond);
	if (cond < flagp->numconds - 1)
	    (void) putc (' ', stdout);
	}
    if (cond == 0)			/* No conditions at all? */
	(void) putc ('.', stdout);
    (void) printf ("\t> ");
    (void) putc ('\t', stdout);
    if (flagp->stripl)
	(void) printf ("-%s,", ichartosstr (flagp->strip, 1));
    (void) printf ("%s\n", flagp->affl ? ichartosstr (flagp->affix, 1) : "-");
    }

static void setdump (setp, mask)	/* Dump a set specification */
    register char *		setp;	/* Set to be dumped */
    register int		mask;	/* Mask for bit to be dumped */
    {
    register int		cnum;	/* Next character's number */
    register int		firstnz; /* Number of first NZ character */
    register int		numnz;	/* Number of NZ characters */

    firstnz = numnz = 0;
    for (cnum = SET_SIZE;  --cnum >= 0;  )
	{
	if (setp[cnum] & mask)
	    {
	    numnz++;
	    firstnz = cnum;
	    }
	}
    if (numnz == 1)
	(void) putc (firstnz, stdout);
    else if (numnz == SET_SIZE)
	(void) putc ('.', stdout);
    else if (numnz > SET_SIZE / 2)
	{
	(void) printf ("[^");
	subsetdump (setp, mask, 0);
	(void) putc (']', stdout);
	}
    else
	{
	(void) putc ('[', stdout);
	subsetdump (setp, mask, mask);
	(void) putc (']', stdout);
	}
    }

static void subsetdump (setp, mask, dumpval) /* Dump part of a set spec */
    register char *		setp;	/* Set to be dumped */
    register int		mask;	/* Mask for bit to be dumped */
    register int		dumpval; /* Value to be printed */
    {
    register int		cnum;	/* Next character's number */
    register int		rangestart; /* Value starting a range */

    for (cnum = 0;  cnum < SET_SIZE;  setp++, cnum++)
	{
	if (((*setp ^ dumpval) & mask) == 0)
	    {
	    for (rangestart = cnum;  cnum < SET_SIZE;  setp++, cnum++)
		{
		if ((*setp ^ dumpval) & mask)
		    break;
		}
	    if (cnum == rangestart + 1)
		(void) putc (rangestart, stdout);
	    else if (cnum <= rangestart + 3)
		{
		while (rangestart < cnum)
		    {
		    (void) putc (rangestart, stdout);
		    rangestart++;
		    }
		}
	    else
		(void) printf ("%c-%c", rangestart, cnum - 1);
	    }
	}
    }
