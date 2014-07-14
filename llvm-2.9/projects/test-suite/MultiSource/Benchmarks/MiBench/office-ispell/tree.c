#ifndef lint
static char Rcs_Id[] =
    "$Id: tree.c 33046 2007-01-09 23:57:19Z lattner $";
#endif

/*
 * tree.c - a hash style dictionary for user's personal words
 *
 * Pace Willisson, 1983
 * Hash support added by Geoff Kuenning, 1987
 *
 * Copyright 1987, 1988, 1989, 1992, 1993, Geoff Kuenning, Granada Hills, CA
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
 * Revision 1.1.1.1  2007/01/09 02:59:06  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.56  1995/01/08  23:23:49  geoff
 * Support PDICTHOME for DOS purposes.
 *
 * Revision 1.55  1994/10/25  05:46:27  geoff
 * Fix a comment that looked to some compilers like it might be nested.
 *
 * Revision 1.54  1994/01/25  07:12:15  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include <ctype.h>
#include <errno.h>
#include "config.h"
#include "ispell.h"
#include "proto.h"
#include "msgs.h"

void		treeinit P ((char * p, char * LibDict));
static FILE *	trydict P ((char * dictname, char * home,
		  char * prefix, char * suffix));
static void	treeload P ((FILE * dictf));
void		treeinsert P ((char * word, int wordlen, int keep));
static struct dent * tinsert P ((struct dent * proto));
struct dent *	treelookup P ((ichar_t * word));
#if SORTPERSONAL != 0
static int	pdictcmp P ((struct dent ** enta, struct dent **entb));
#endif /* SORTPERSONAL != 0 */
void		treeoutput P ((void));
VOID *		mymalloc P ((unsigned int size));
void		myfree P ((VOID * ptr));
#ifdef REGEX_LOOKUP
char *		do_regex_lookup P ((char * expr, int whence));
#endif /* REGEX_LOOKUP */

static int		cantexpand = 0;	/* NZ if an expansion fails */
static struct dent *	pershtab;	/* Aux hash table for personal dict */
static int		pershsize = 0;	/* Space available in aux hash table */
static int		hcount = 0;	/* Number of items in hash table */

/*
 * Hash table sizes.  Prime is probably a good idea, though in truth I
 * whipped the algorithm up on the spot rather than looking it up, so
 * who knows what's really best?  If we overflow the table, we just
 * use a double-and-add-1 algorithm.
 */
static int goodsizes[] =
    {
    53, 223, 907, 3631
    };

static char		personaldict[MAXPATHLEN];
static FILE *		dictf;
static			newwords = 0;

void treeinit (p, LibDict)
    char *		p;		/* Value specified in -p switch */
    char *		LibDict;	/* Root of default dict name */
    {
    int			abspath;	/* NZ if p is abs path name */
    char *		h;		/* Home directory name */
    char		seconddict[MAXPATHLEN]; /* Name of secondary dict */
    FILE *		secondf;	/* Access to second dict file */

    /*
    ** If -p was not specified, try to get a default name from the
    ** environment.  After this point, if p is null, the the value in
    ** personaldict is the only possible name for the personal dictionary.
    ** If p is non-null, then there is a possibility that we should
    ** prepend HOME to get the correct dictionary name.
    */
    if (p == NULL)
	p = getenv (PDICTVAR);
    /*
    ** if p exists and begins with '/' we don't really need HOME,
    ** but it's not very likely that HOME isn't set anyway (on non-DOS
    ** systems).
    */
    if ((h = getenv (HOME)) == NULL)
	{
#ifdef PDICTHOME
	h = PDICTHOME;
#else /* PDICTHOME */
	return;
#endif /* PDICTHOME */
	}

    if (p == NULL)
	{
	/*
	 * No -p and no PDICTVAR.  We will use LibDict and DEFPAFF to
	 * figure out the name of the personal dictionary and where it
	 * is.  The rules are as follows:
	 *
	 * (1) If there is a local dictionary and a HOME dictionary,
	 *     both are loaded, but changes are saved in the local one.
	 *     The dictionary to save changes in is named "personaldict".
	 * (2) Dictionaries named after the affix file take precedence
	 *     over dictionaries with the default suffix (DEFPAFF).
	 * (3) Dictionaries named with the new default names
	 *     (DEFPDICT/DEFPAFF) take precedence over the old ones
	 *     (OLDPDICT/OLDPAFF).
	 * (4) Dictionaries aren't combined unless they follow the same
	 *     naming scheme.
	 * (5) If no dictionary can be found, a new one is created in
	 *     the home directory, named after DEFPDICT and the affix
	 *     file.
	 */
	dictf = trydict (personaldict, (char *) NULL, DEFPDICT, LibDict);
	secondf = trydict (seconddict, h, DEFPDICT, LibDict);
	if (dictf == NULL  &&  secondf == NULL)
	    {
	    dictf = trydict (personaldict, (char *) NULL, DEFPDICT, DEFPAFF);
	    secondf = trydict (seconddict, h, DEFPDICT, DEFPAFF);
	    }
	if (dictf == NULL  &&  secondf == NULL)
	    {
	    dictf = trydict (personaldict, (char *) NULL, OLDPDICT, LibDict);
	    secondf = trydict (seconddict, h, OLDPDICT, LibDict);
	    }
	if (dictf == NULL  &&  secondf == NULL)
	    {
	    dictf = trydict (personaldict, (char *) NULL, OLDPDICT, OLDPAFF);
	    secondf = trydict (seconddict, h, OLDPDICT, OLDPAFF);
	    }
	if (personaldict[0] == '\0')
	    {
	    if (seconddict[0] != '\0')
		(void) strcpy (personaldict, seconddict);
	    else
		(void) sprintf (personaldict, "%s/%s%s", h, DEFPDICT, LibDict);
	    }
	if (dictf != NULL)
	    {
	    treeload (dictf);
	    (void) fclose (dictf);
	    }
	if (secondf != NULL)
	    {
	    treeload (secondf);
	    (void) fclose (secondf);
	    }
	}
    else
	{
	/*
	** Figure out if p is an absolute path name.  Note that beginning
	** with "./" and "../" is considered an absolute path, since this
	** still means we can't prepend HOME.
	*/
	abspath = (*p == '/'  ||  strncmp (p, "./", 2) == 0
	  ||  strncmp (p, "../", 3) == 0);
	if (abspath)
	    {
	    (void) strcpy (personaldict, p);
	    if ((dictf = fopen (personaldict, "r")) != NULL)
		{
		treeload (dictf);
		(void) fclose (dictf);
		}
	    }
	else
	    {
	    /*
	    ** The user gave us a relative pathname.  We will try it
	    ** locally, and if that doesn't work, we'll try the home
	    ** directory.  If neither exists, it will be created in
	    ** the home directory if words are added.
	    */
	    (void) strcpy (personaldict, p);
	    if ((dictf = fopen (personaldict, "r")) != NULL)
		{
		treeload (dictf);
		(void) fclose (dictf);
		}
	    else if (!abspath)
		{
		/* Try the home */
		(void) sprintf (personaldict, "%s/%s", h, p);
		if ((dictf = fopen (personaldict, "r")) != NULL)
		    {
		    treeload (dictf);
		    (void) fclose (dictf);
		    }
		}
	    /*
	     * If dictf is null, we couldn't open the dictionary
	     * specified in the -p switch.  Complain.
	     */
	    if (dictf == NULL)
		{
		(void) fprintf (stderr, CANT_OPEN, p);
		perror ("");
		return;
		}
	    }
	}

    if (!lflag  &&  !aflag
      &&  access (personaldict, 2) < 0  &&  errno != ENOENT)
	{
	(void) fprintf (stderr, TREE_C_CANT_UPDATE, personaldict);
	(void) sleep ((unsigned) 2);
	}
    }

/*
 * Try to open a dictionary.  As a side effect, leaves the dictionary
 * name in "filename" if one is found, and leaves a null string there
 * otherwise.
 */
static FILE * trydict (filename, home, prefix, suffix)
    char *		filename;	/* Where to store the file name */
    char *		home;		/* Home directory */
    char *		prefix;		/* Prefix for dictionary */
    char *		suffix;		/* Suffix for dictionary */
    {
    FILE *		dictf;		/* Access to dictionary file */

    if (home == NULL)
	(void) sprintf (filename, "%s%s", prefix, suffix);
    else
	(void) sprintf (filename, "%s/%s%s", home, prefix, suffix);
    dictf = fopen (filename, "r");
    if (dictf == NULL)
	filename[0] = '\0';
    return dictf;
    }

static void treeload (loadfile)
    register FILE *	loadfile;	/* File to load words from */
    {
    char		buf[BUFSIZ];	/* Buffer for reading pers dict */

    while (fgets (buf, sizeof buf, loadfile) != NULL)
	treeinsert (buf, sizeof buf, 1);
    newwords = 0;
    }

void treeinsert (word, wordlen, keep)
    char *		word;	/* Word to insert - must be canonical */
    int			wordlen; /* Length of the word buffer */
    int			keep;
    {
    register int	i;
    struct dent		wordent;
    register struct dent * dp;
    struct dent *	olddp;
#ifndef NO_CAPITALIZATION_SUPPORT
    struct dent *	newdp;
#endif
    struct dent *	oldhtab;
    int			oldhsize;
    ichar_t		nword[INPUTWORDLEN + MAXAFFIXLEN];
#ifndef NO_CAPITALIZATION_SUPPORT
    int			isvariant;
#endif

    /*
     * Expand hash table when it is MAXPCT % full.
     */
    if (!cantexpand  &&  (hcount * 100) / MAXPCT >= pershsize)
	{
	oldhsize = pershsize;
	oldhtab = pershtab;
	for (i = 0;  i < sizeof goodsizes / sizeof (goodsizes[0]);  i++)
	    {
	    if (goodsizes[i] > pershsize)
		break;
	    }
	if (i >= sizeof goodsizes / sizeof goodsizes[0])
	    pershsize += pershsize + 1;
	else
	    pershsize = goodsizes[i];
	pershtab =
	  (struct dent *) calloc ((unsigned) pershsize, sizeof (struct dent));
	if (pershtab == NULL)
	    {
	    (void) fprintf (stderr, TREE_C_NO_SPACE);
	    /*
	     * Try to continue anyway, since our overflow
	     * algorithm can handle an overfull (100%+) table,
	     * and the malloc very likely failed because we
	     * already have such a huge table, so small mallocs
	     * for overflow entries will still work.
	     */
	    if (oldhtab == NULL)
		exit (1);		/* No old table, can't go on */
	    (void) fprintf (stderr, TREE_C_TRY_ANYWAY);
	    cantexpand = 1;		/* Suppress further messages */
	    pershsize = oldhsize;	/* Put things back */
	    pershtab = oldhtab;		/* ... */
	    newwords = 1;		/* And pretend it worked */
	    }
	else
	    {
	    /*
	     * Re-insert old entries into new table
	     */
	    for (i = 0;  i < oldhsize;  i++)
		{
		dp = &oldhtab[i];
		if (dp->flagfield & USED)
		    {
#ifdef NO_CAPITALIZATION_SUPPORT
		    (void) tinsert (dp);
#else
		    newdp = tinsert (dp);
		    isvariant = (dp->flagfield & MOREVARIANTS);
#endif
		    dp = dp->next;
#ifdef NO_CAPITALIZATION_SUPPORT
		    while (dp != NULL)
			{
			(void) tinsert (dp);
			olddp = dp;
			dp = dp->next;
			free ((char *) olddp);
			}
#else
		    while (dp != NULL)
			{
			if (isvariant)
			    {
			    isvariant = dp->flagfield & MOREVARIANTS;
			    olddp = newdp->next;
			    newdp->next = dp;
			    newdp = dp;
			    dp = dp->next;
			    newdp->next = olddp;
			    }
			else
			    {
			    isvariant = dp->flagfield & MOREVARIANTS;
			    newdp = tinsert (dp);
			    olddp = dp;
			    dp = dp->next;
			    free ((char *) olddp);
			    }
			}
#endif
		    }
		}
	    if (oldhtab != NULL)
		free ((char *) oldhtab);
	    }
	}

    /*
    ** We're ready to do the insertion.  Start by creating a sample
    ** entry for the word.
    */
    if (makedent (word, wordlen, &wordent) < 0)
	return;			/* Word must be too big or something */
    if (keep)
	wordent.flagfield |= KEEP;
    /*
    ** Now see if word or a variant is already in the table.  We use the
    ** capitalized version so we'll find the header, if any.
    **/
    (void) strtoichar (nword, word, sizeof nword, 1);
    upcase (nword);
    if ((dp = lookup (nword, 1)) != NULL)
	{
	/* It exists.  Combine caps and set the keep flag. */
	if (combinecaps (dp, &wordent) < 0)
	    {
	    free (wordent.word);
	    return;
	    }
	}
    else
	{
	/* It's new. Insert the word. */
	dp = tinsert (&wordent);
#ifndef NO_CAPITALIZATION_SUPPORT
	if (captype (dp->flagfield) == FOLLOWCASE)
	   (void) addvheader (dp);
#endif
	}
    newwords |= keep;
    }

static struct dent * tinsert (proto)
    struct dent *	proto;		/* Prototype entry to copy */
    {
    ichar_t		iword[INPUTWORDLEN + MAXAFFIXLEN];
    register int	hcode;
    register struct dent * hp;		/* Next trial entry in hash table */
    register struct dent * php;		/* Prev. value of hp, for chaining */

    if (strtoichar (iword, proto->word, sizeof iword, 1))
	(void) fprintf (stderr, WORD_TOO_LONG (proto->word));
#ifdef NO_CAPITALIZATION_SUPPORT
    upcase (iword);
#endif
    hcode = hash (iword, pershsize);
    php = NULL;
    hp = &pershtab[hcode];
    if (hp->flagfield & USED)
	{
	while (hp != NULL)
	    {
	    php = hp;
	    hp = hp->next;
	    }
	hp = (struct dent *) calloc (1, sizeof (struct dent));
	if (hp == NULL)
	    {
	    (void) fprintf (stderr, TREE_C_NO_SPACE);
	    exit (1);
	    }
	}
    *hp = *proto;
    if (php != NULL)
	php->next = hp;
    hp->next = NULL;
    return hp;
    }

struct dent * treelookup (word)
    register ichar_t *	word;
    {
    register int	hcode;
    register struct dent * hp;
    char		chword[INPUTWORDLEN + MAXAFFIXLEN];

    if (pershsize <= 0)
	return NULL;
    (void) ichartostr (chword, word, sizeof chword, 1);
    hcode = hash (word, pershsize);
    hp = &pershtab[hcode];
    while (hp != NULL  &&  (hp->flagfield & USED))
	{
	if (strcmp (chword, hp->word) == 0)
	    break;
#ifndef NO_CAPITALIZATION_SUPPORT
	while (hp->flagfield & MOREVARIANTS)
	    hp = hp->next;
#endif
	hp = hp->next;
	}
    if (hp != NULL  &&  (hp->flagfield & USED))
	return hp;
    else
	return NULL;
    }

#if SORTPERSONAL != 0
/* Comparison routine for sorting the personal dictionary with qsort */
static int pdictcmp (enta, entb)
    struct dent **	enta;
    struct dent **	entb;
    {

    /* The parentheses around *enta and *entb below are NECESSARY!
    ** Otherwise the compiler reads it as *(enta->word), or
    ** enta->word[0], which is illegal (but pcc takes it and
    ** produces wrong code).
    **/
    return casecmp ((*enta)->word, (*entb)->word, 1);
    }
#endif

void treeoutput ()
    {
    register struct dent *	cent;	/* Current entry */
    register struct dent *	lent;	/* Linked entry */
#if SORTPERSONAL != 0
    int				pdictsize; /* Number of entries to write */
    struct dent **		sortlist; /* List of entries to be sorted */
    register struct dent **	sortptr; /* Handy pointer into sortlist */
#endif
    register struct dent *	ehtab;	/* End of pershtab, for fast looping */

    if (newwords == 0)
	return;

    if ((dictf = fopen (personaldict, "w")) == NULL)
	{
	(void) fprintf (stderr, CANT_CREATE, personaldict);
	return;
	}

#if SORTPERSONAL != 0
    /*
    ** If we are going to sort the personal dictionary, we must know
    ** how many items are going to be sorted.
    */
    pdictsize = 0;
    if (hcount >= SORTPERSONAL)
	sortlist = NULL;
    else
	{
	for (cent = pershtab, ehtab = pershtab + pershsize;
	  cent < ehtab;
	  cent++)
	    {
	    for (lent = cent;  lent != NULL;  lent = lent->next)
		{
		if ((lent->flagfield & (USED | KEEP)) == (USED | KEEP))
		    pdictsize++;
#ifndef NO_CAPITALIZATION_SUPPORT
		while (lent->flagfield & MOREVARIANTS)
		  lent = lent->next;
#endif
		}
	    }
	for (cent = hashtbl, ehtab = hashtbl + hashsize;
	  cent < ehtab;
	  cent++)
	    {
	    if ((cent->flagfield & (USED | KEEP)) == (USED | KEEP))
		{
		/*
		** We only want to count variant headers
		** and standalone entries.  These happen
		** to share the characteristics in the
		** test below.  This test will appear
		** several more times in this routine.
		*/
#ifndef NO_CAPITALIZATION_SUPPORT
		if (captype (cent->flagfield) != FOLLOWCASE
		  &&  cent->word != NULL)
#endif
		    pdictsize++;
		}
	    }
	sortlist = (struct dent **) malloc (pdictsize * sizeof (struct dent));
	}
    if (sortlist == NULL)
	{
#endif
	for (cent = pershtab, ehtab = pershtab + pershsize;
	  cent < ehtab;
	  cent++)
	    {
	    for (lent = cent;  lent != NULL;  lent = lent->next)
		{
		if ((lent->flagfield & (USED | KEEP)) == (USED | KEEP))
		    {
		    toutent (dictf, lent, 1);
#ifndef NO_CAPITALIZATION_SUPPORT
		    while (lent->flagfield & MOREVARIANTS)
			lent = lent->next;
#endif
		    }
		}
	    }
	for (cent = hashtbl, ehtab = hashtbl + hashsize;
	  cent < ehtab;
	  cent++)
	    {
	    if ((cent->flagfield & (USED | KEEP)) == (USED | KEEP))
		{
#ifndef NO_CAPITALIZATION_SUPPORT
		if (captype (cent->flagfield) != FOLLOWCASE
		  &&  cent->word != NULL)
#endif
		    toutent (dictf, cent, 1);
		}
	    }
#if SORTPERSONAL != 0
	return;
	}
    /*
    ** Produce dictionary in sorted order.  We used to do this
    ** destructively, but that turns out to fail because in some modes
    ** the dictionary is written more than once.  So we build an
    ** auxiliary pointer table (in sortlist) and sort that.  This
    ** is faster anyway, though it uses more memory. 
    */
    sortptr = sortlist;
    for (cent = pershtab, ehtab = pershtab + pershsize;  cent < ehtab;  cent++)
	{
	for (lent = cent;  lent != NULL;  lent = lent->next)
	    {
	    if ((lent->flagfield & (USED | KEEP)) == (USED | KEEP))
		{
		*sortptr++ = lent;
#ifndef NO_CAPITALIZATION_SUPPORT
		while (lent->flagfield & MOREVARIANTS)
		    lent = lent->next;
#endif
		}
	    }
	}
    for (cent = hashtbl, ehtab = hashtbl + hashsize;  cent < ehtab;  cent++)
	{
	if ((cent->flagfield & (USED | KEEP)) == (USED | KEEP))
	    {
#ifndef NO_CAPITALIZATION_SUPPORT
	    if (captype (cent->flagfield) != FOLLOWCASE
	      &&  cent->word != NULL)
#endif
		*sortptr++ = cent;
	    }
	}
    /* Sort the list */
    qsort ((char *) sortlist, (unsigned) pdictsize,
      sizeof (sortlist[0]),
      (int (*) P ((const void *, const void *))) pdictcmp);
    /* Write it out */
    for (sortptr = sortlist;  --pdictsize >= 0;  )
	toutent (dictf, *sortptr++, 1);
    free ((char *) sortlist);
#endif

    newwords = 0;

    (void) fclose (dictf);
    }

VOID * mymalloc (size)
    unsigned int	size;
    {

    return malloc ((unsigned) size);
    }

void myfree (ptr)
    VOID * ptr;
    {
    if (hashstrings != NULL  &&  (char *) ptr >= hashstrings
      &&  (char *) ptr <= hashstrings + hashheader.stringsize)
	return;			/* Can't free stuff in hashstrings */
    free (ptr);
    }

#ifdef REGEX_LOOKUP

/* check the hashed dictionary for words matching the regex. return the */
/* a matching string if found else return NULL */
char * do_regex_lookup (expr, whence)
    char *	expr;	/* regular expression to use in the match   */
    int		whence;	/* 0 = start at the beg with new regx, else */
			/* continue from cur point w/ old regex     */
    {
    static struct dent *    curent;
    static int		    curindex;
    static struct dent *    curpersent;
    static int		    curpersindex;
    static char *	    cmp_expr;
    char		    dummy[INPUTWORDLEN + MAXAFFIXLEN];
    ichar_t *		    is;

    if (whence == 0)
	{
	is = strtosichar (expr, 0);
	upcase (is);
	expr = ichartosstr (is, 1);
        cmp_expr = REGCMP (expr);
        curent = hashtbl;
        curindex = 0;
        curpersent = pershtab;
        curpersindex = 0;
	}
    
    /* search the dictionary until the word is found or the words run out */
    for (  ; curindex < hashsize;  curent++, curindex++)
	{
        if (curent->word != NULL
          &&  REGEX (cmp_expr, curent->word, dummy) != NULL)
	    {
	    curindex++;
	    /* Everybody's gotta write a wierd expression once in a while! */
	    return curent++->word;
	    }
	}
    /* Try the personal dictionary too */
    for (  ; curpersindex < pershsize;  curpersent++, curpersindex++)
	{
        if ((curpersent->flagfield & USED) != 0
          &&  curpersent->word != NULL
          &&  REGEX (cmp_expr, curpersent->word, dummy) != NULL)
	    {
	    curpersindex++;
	    /* Everybody's gotta write a wierd expression once in a while! */
	    return curpersent++->word;
	    }
	}
    return NULL;
    }
#endif /* REGEX_LOOKUP */
