#ifndef lint
static char Rcs_Id[] =
    "$Id: correct.c 33095 2007-01-12 00:01:41Z reid $";
#endif

/*
 * correct.c - Routines to manage the higher-level aspects of spell-checking
 *
 * This code originally resided in ispell.c, but was moved here to keep
 * file sizes smaller.
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
 * Revision 1.2  2007/01/12 00:01:41  reid
 * Rename getline so it doesn't conflict with the Linux version.
 *
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:58:50  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.59  1995/08/05  23:19:43  geoff
 * Fix a bug that caused offsets for long lines to be confused if the
 * line started with a quoting uparrow.
 *
 * Revision 1.58  1994/11/02  06:56:00  geoff
 * Remove the anyword feature, which I've decided is a bad idea.
 *
 * Revision 1.57  1994/10/26  05:12:39  geoff
 * Try boundary characters when inserting or substituting letters, except
 * (naturally) at word boundaries.
 *
 * Revision 1.56  1994/10/25  05:46:30  geoff
 * Fix an assignment inside a conditional that could generate spurious
 * warnings (as well as being bad style).  Add support for the FF_ANYWORD
 * option.
 *
 * Revision 1.55  1994/09/16  04:48:24  geoff
 * Don't pass newlines from the input to various other routines, and
 * don't assume that those routines leave the input unchanged.
 *
 * Revision 1.54  1994/09/01  06:06:41  geoff
 * Change erasechar/killchar to uerasechar/ukillchar to avoid
 * shared-library problems on HP systems.
 *
 * Revision 1.53  1994/08/31  05:58:38  geoff
 * Add code to handle extremely long lines in -a mode without splitting
 * words or reporting incorrect offsets.
 *
 * Revision 1.52  1994/05/25  04:29:24  geoff
 * Fix a bug that caused line widths to be calculated incorrectly when
 * displaying lines containing tabs.  Fix a couple of places where
 * characters were sign-extended incorrectly, which could cause 8-bit
 * characters to be displayed wrong.
 *
 * Revision 1.51  1994/05/17  06:44:05  geoff
 * Add support for controlled compound formation and the COMPOUNDONLY
 * option to affix flags.
 *
 * Revision 1.50  1994/04/27  05:20:14  geoff
 * Allow compound words to be formed from more than two components
 *
 * Revision 1.49  1994/04/27  01:50:31  geoff
 * Add support to correctly capitalize words generated as a result of a
 * missing-space suggestion.
 *
 * Revision 1.48  1994/04/03  23:23:02  geoff
 * Clean up the code in missingspace() to be a bit simpler and more
 * efficient.
 *
 * Revision 1.47  1994/03/15  06:24:23  geoff
 * Fix the +/-/~ commands to be independent.  Allow the + command to
 * receive a suffix which is a deformatter type (currently hardwired to
 * be either tex or nroff/troff).
 *
 * Revision 1.46  1994/02/21  00:20:03  geoff
 * Fix some bugs that could cause bad displays in the interaction between
 * TeX parsing and string characters.  Show_char now will not overrun
 * the inverse-video display area by accident.
 *
 * Revision 1.45  1994/02/14  00:34:51  geoff
 * Fix correct to accept length parameters for ctok and itok, so that it
 * can pass them to the to/from ichar routines.
 *
 * Revision 1.44  1994/01/25  07:11:22  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include <ctype.h>
#include "config.h"
#include "ispell.h"
#include "proto.h"
#include "msgs.h"
#include "version.h"

void		givehelp P ((int interactive));
void		checkfile P ((void));
void		correct P ((char * ctok, int ctokl, ichar_t * itok, int itokl,
		  char ** curchar));
static void	show_line P ((char * line, char * invstart, int invlen));
static int	show_char P ((char ** cp, int linew, int output, int maxw));
static int	line_size P ((char * buf, char * bufend));
static void	inserttoken P ((char * buf, char * start, char * tok,
		  char ** curchar));
static int	posscmp P ((char * a, char * b));
int		casecmp P ((char * a, char * b, int canonical));
void		makepossibilities P ((ichar_t * word));
static int	insert P ((ichar_t * word));
#ifndef NO_CAPITALIZATION_SUPPORT
static void	wrongcapital P ((ichar_t * word));
#endif /* NO_CAPITALIZATION_SUPPORT */
static void	wrongletter P ((ichar_t * word));
static void	extraletter P ((ichar_t * word));
static void	missingletter P ((ichar_t * word));
static void	missingspace P ((ichar_t * word));
int		compoundgood P ((ichar_t * word, int pfxopts));
static void	transposedletter P ((ichar_t * word));
static void	tryveryhard P ((ichar_t * word));
static int	ins_cap P ((ichar_t * word, ichar_t * pattern));
static int	save_cap P ((ichar_t * word, ichar_t * pattern,
		  ichar_t savearea[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN]));
int		ins_root_cap P ((ichar_t * word, ichar_t * pattern,
		  int prestrip, int preadd, int sufstrip, int sufadd,
		  struct dent * firstdent, struct flagent * pfxent,
		  struct flagent * sufent));
static void	save_root_cap P ((ichar_t * word, ichar_t * pattern,
		  int prestrip, int preadd, int sufstrip, int sufadd,
		  struct dent * firstdent, struct flagent * pfxent,
		  struct flagent * sufent,
		  ichar_t savearea[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN],
		  int * nsaved));
static char *	getline_ispell P ((char * buf));
void		askmode P ((void));
void		copyout P ((char ** cc, int cnt));
static void	lookharder P ((char * string));
#ifdef REGEX_LOOKUP
static void	regex_dict_lookup P ((char * cmd, char * grepstr));
#endif /* REGEX_LOOKUP */

void givehelp (interactive)
    int		    interactive;	/* NZ for interactive-mode help */
    {
#ifdef COMMANDFORSPACE
    char ch;
#endif
    register FILE *helpout;	/* File to write help to */

    if (interactive)
	{
	erase ();
	helpout = stdout;
	}
    else
	helpout = stderr;

    (void) fprintf (helpout, CORR_C_HELP_1);
    (void) fprintf (helpout, CORR_C_HELP_2);
    (void) fprintf (helpout, CORR_C_HELP_3);
    (void) fprintf (helpout, CORR_C_HELP_4);
    (void) fprintf (helpout, CORR_C_HELP_5);
    (void) fprintf (helpout, CORR_C_HELP_6);
    (void) fprintf (helpout, CORR_C_HELP_7);
    (void) fprintf (helpout, CORR_C_HELP_8);
    (void) fprintf (helpout, CORR_C_HELP_9);

    (void) fprintf (helpout, CORR_C_HELP_COMMANDS);

    (void) fprintf (helpout, CORR_C_HELP_R_CMD);
    (void) fprintf (helpout, CORR_C_HELP_BLANK);
    (void) fprintf (helpout, CORR_C_HELP_A_CMD);
    (void) fprintf (helpout, CORR_C_HELP_I_CMD);
    (void) fprintf (helpout, CORR_C_HELP_U_CMD);
    (void) fprintf (helpout, CORR_C_HELP_0_CMD);
    (void) fprintf (helpout, CORR_C_HELP_L_CMD);
    (void) fprintf (helpout, CORR_C_HELP_X_CMD);
    (void) fprintf (helpout, CORR_C_HELP_Q_CMD);
    (void) fprintf (helpout, CORR_C_HELP_BANG);
    (void) fprintf (helpout, CORR_C_HELP_REDRAW);
    (void) fprintf (helpout, CORR_C_HELP_SUSPEND);
    (void) fprintf (helpout, CORR_C_HELP_HELP);

    if (interactive)
	{
	(void) fprintf (helpout, "\r\n\r\n");
	(void) fprintf (helpout, CORR_C_HELP_TYPE_SPACE);
	(void) fflush (helpout);
#ifdef COMMANDFORSPACE
	ch = GETKEYSTROKE ();
	if (ch != ' ' && ch != '\n' && ch != '\r')
	    (void) ungetc (ch, stdin);
#else
	while (GETKEYSTROKE () != ' ')
	    ;
#endif
	}
    }

void checkfile ()
    {
    int		bufno;
    int		bufsize;
    int		ch;

    for (bufno = 0;  bufno < contextsize;  bufno++)
	contextbufs[bufno][0] = '\0';

    for (  ;  ;  )
	{
	for (bufno = contextsize;  --bufno > 0;  )
	    (void) strcpy (contextbufs[bufno],
	      contextbufs[bufno - 1]);
	if (quit)	/* quit can't be set in l mode */
	    {
	    while (fgets (contextbufs[0],
	      sizeof contextbufs[0], infile) != NULL)
		(void) fputs (contextbufs[0], outfile);
	    break;
	    }
	/*
	 * Only read in enough characters to fill half this buffer so that any
	 * corrections we make are not likely to cause an overflow.
	 */
	if (fgets (contextbufs[0], (sizeof contextbufs[0]) / 2, infile)
	  == NULL)
	    break;
	/*
	 * If we didn't read to end-of-line, we may have ended the
	 * buffer in the middle of a word.  So keep reading until we
	 * see some sort of character that can't possibly be part of a
	 * word. (or until the buffer is full, which fortunately isn't
	 * all that likely).
	 */
	bufsize = strlen (contextbufs[0]);
	if (bufsize == (sizeof contextbufs[0]) / 2 - 1)
	    {
	    ch = (unsigned char) contextbufs[0][bufsize - 1];
	    while (bufsize < sizeof contextbufs[0] - 1
	      &&  (iswordch ((ichar_t) ch)  ||  isboundarych ((ichar_t) ch)
	      ||  isstringstart (ch)))
		{
		ch = getc (infile);
		if (ch == EOF)
		    break;
		contextbufs[0][bufsize++] = (char) ch;
		contextbufs[0][bufsize] = '\0';
		}
	    }
	checkline (outfile);
	}
    }

void correct (ctok, ctokl, itok, itokl, curchar)
    char *		ctok;
    int			ctokl;
    ichar_t *		itok;
    int			itokl;
    char **		curchar;
    {
    register int	c;
    register int	i;
    int			col_ht;
    int			ncols;
    char *		start_l2;
    char *		begintoken;

    begintoken = *curchar - strlen (ctok);

    if (icharlen (itok) <= minword)
	return;			/* Accept very short words */

checkagain:
    if (good (itok, 0, 0, 0, 0)  ||  compoundgood (itok, 0))
	return;

    erase ();
    (void) printf ("    %s", ctok);
    if (currentfile)
	(void) printf (CORR_C_FILE_LABEL, currentfile);
    if (readonly)
	(void) printf (" %s", CORR_C_READONLY);
    (void) printf ("\r\n\r\n");

    makepossibilities (itok);

    /*
     * Make sure we have enough room on the screen to hold the
     * possibilities.  Reduce the list if necessary.  co / (maxposslen + 8)
     * is the maximum number of columns that will fit.  col_ht is the
     * height of the columns.  The constant 4 allows 2 lines (1 blank) at
     * the top of the screen, plus another blank line between the
     * columns and the context, plus a final blank line at the bottom
     * of the screen for command entry (R, L, etc).
     */
    col_ht = li - contextsize - 4 - minimenusize;
    ncols = co / (maxposslen + 8);
    if (pcount > ncols * col_ht)
	pcount = ncols * col_ht;

#ifdef EQUAL_COLUMNS
    /*
     * Equalize the column sizes.  The last column will be short.
     */
    col_ht = (pcount + ncols - 1) / ncols;
#endif

    for (i = 0; i < pcount; i++)
	{
#ifdef BOTTOMCONTEXT
	move (2 + (i % col_ht), (maxposslen + 8) * (i / col_ht));
#else /* BOTTOMCONTEXT */
	move (3 + contextsize + (i % col_ht), (maxposslen + 8) * (i / col_ht));
#endif /* BOTTOMCONTEXT */
	if (i >= easypossibilities)
	    (void) printf ("??: %s", possibilities[i]);
	else if (easypossibilities >= 10  &&  i < 10)
	    (void) printf ("0%d: %s", i, possibilities[i]);
	else
	    (void) printf ("%2d: %s", i, possibilities[i]);
	}

#ifdef BOTTOMCONTEXT
    move (li - contextsize - 1 - minimenusize, 0);
#else /* BOTTOMCONTEXT */
    move (2, 0);
#endif /* BOTTOMCONTEXT */
    for (i = contextsize;  --i > 0;  )
	show_line (contextbufs[i], contextbufs[i], 0);

    start_l2 = contextbufs[0];
    if (line_size (contextbufs[0], *curchar) > co - (sg << 1) - 1)
	{
	start_l2 = begintoken - (co / 2);
	while (start_l2 < begintoken)
	    {
	    i = line_size (start_l2, *curchar) + 1;
	    if (i + (sg << 1) <= co)
		break;
	    start_l2 += i - co;
	    }
	if (start_l2 > begintoken)
	    start_l2 = begintoken;
	if (start_l2 < contextbufs[0])
	    start_l2 = contextbufs[0];
	}
    show_line (start_l2, begintoken, (int) strlen (ctok));

    if (minimenusize != 0)
	{
	move (li - 2, 0);
	(void) printf (CORR_C_MINI_MENU);
	}

    for (  ;  ;  )
	{
	(void) fflush (stdout);
	switch (c = (GETKEYSTROKE () & NOPARITY))
	    {
	    case 'Z' & 037:
		stop ();
		erase ();
		goto checkagain;
	    case ' ':
		erase ();
		(void) fflush (stdout);
		return;
	    case 'q': case 'Q':
		if (changes)
		    {
		    (void) printf (CORR_C_CONFIRM_QUIT);
		    (void) fflush (stdout);
		    c = (GETKEYSTROKE () & NOPARITY);
		    }
		else
		    c = 'y';
		if (c == 'y' || c == 'Y')
		    {
		    erase ();
		    (void) fflush (stdout);
		    done (0);
		    }
		goto checkagain;
	    case 'i': case 'I':
		treeinsert (ichartosstr (strtosichar (ctok, 0), 1),
		 ICHARTOSSTR_SIZE, 1);
		erase ();
		(void) fflush (stdout);
		changes = 1;
		return;
	    case 'u': case 'U':
		itok = strtosichar (ctok, 0);
		lowcase (itok);
		treeinsert (ichartosstr (itok, 1), ICHARTOSSTR_SIZE, 1);
		erase ();
		(void) fflush (stdout);
		changes = 1;
		return;
	    case 'a': case 'A':
		treeinsert (ichartosstr (strtosichar (ctok, 0), 1),
		  ICHARTOSSTR_SIZE, 0);
		erase ();
		(void) fflush (stdout);
		return;
	    case 'L' & 037:
		goto checkagain;
	    case '?':
		givehelp (1);
		goto checkagain;
	    case '!':
		{
		char	buf[200];

		move (li - 1, 0);
		(void) putchar ('!');
		if (getline_ispell (buf) == NULL)
		    {
		    (void) putchar (7);
		    erase ();
		    (void) fflush (stdout);
		    goto checkagain;
		    }
		(void) printf ("\r\n");
		(void) fflush (stdout);
#ifdef	USESH
		shescape (buf);
#else
		(void) shellescape (buf);
#endif
		erase ();
		goto checkagain;
		}
	    case 'r': case 'R':
		move (li - 1, 0);
		if (readonly)
		    {
		    (void) putchar (7);
		    (void) printf ("%s ", CORR_C_READONLY);
		    }
		(void) printf (CORR_C_REPLACE_WITH);
		if (getline_ispell (ctok) == NULL)
		    {
		    (void) putchar (7);
		    /* Put it back */
		    (void) ichartostr (ctok, itok, ctokl, 0);
		    }
		else
		    {
		    inserttoken (contextbufs[0],
		      begintoken, ctok, curchar);
		    if (strtoichar (itok, ctok, itokl, 0))
			{
			(void) putchar (7);
			(void) printf (WORD_TOO_LONG (ctok));
			}
		    changes = 1;
		    }
		erase ();
		if (icharlen (itok) <= minword)
		    return;		/* Accept very short replacements */
		goto checkagain;
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		i = c - '0';
		if (easypossibilities >= 10)
		    {
		    c = GETKEYSTROKE () & NOPARITY;
		    if (c >= '0'  &&  c <= '9')
			i = i * 10 + c - '0';
		    else if (c != '\r'  &&  c != '\n')
			{
			(void) putchar (7);
			break;
			}
		    }
		if (i < easypossibilities)
		    {
		    (void) strcpy (ctok, possibilities[i]);
		    changes = 1;
		    inserttoken (contextbufs[0],
			begintoken, ctok, curchar);
		    erase ();
		    if (readonly)
			{
			move (li - 1, 0);
			(void) putchar (7);
			(void) printf ("%s", CORR_C_READONLY);
			(void) fflush (stdout);
			(void) sleep ((unsigned) 2);
			}
		    return;
		    }
		(void) putchar (7);
		break;
	    case '\r':	/* This makes typing \n after single digits */
	    case '\n':	/* ..less obnoxious */
		break;
	    case 'l': case 'L':
		{
		char	buf[100];
		move (li - 1, 0);
		(void) printf (CORR_C_LOOKUP_PROMPT);
		if (getline_ispell (buf) == NULL)
		    {
		    (void) putchar (7);
		    erase ();
		    goto checkagain;
		    }
		(void) printf ("\r\n");
		(void) fflush (stdout);
		lookharder (buf);
		erase ();
		goto checkagain;
		}
	    case 'x': case 'X':
		quit = 1;
		erase ();
		(void) fflush (stdout);
		return;
	    default:
		(void) putchar (7);
		break;
	    }
	}
    }

static void show_line (line, invstart, invlen)
    char *		line;
    register char *	invstart;
    register int	invlen;
    {
    register int	width;

    width = invlen ? (sg << 1) : 0;
    while (line < invstart  &&  width < co - 1)
	width += show_char (&line, width, 1, invstart - line);
    if (invlen)
	{
	inverse ();
	invstart += invlen;
	while (line < invstart  &&  width < co - 1)
	    width += show_char (&line, width, 1, invstart - line);
	normal ();
	}
    while (*line  &&  width < co - 1)
	width += show_char (&line, width, 1, 0);
    (void) printf ("\r\n");
    }

static int show_char (cp, linew, output, maxw)
    register char **	cp;
    int			linew;
    int			output;		/* NZ to actually do output */
    int			maxw;		/* NZ to limit width shown */
    {
    register int	ch;
    register int	i;
    int			len;
    ichar_t		ichar;
    register int	width;

    ch = (unsigned char) **cp;
    if (l1_isstringch (*cp, len, 0))
	ichar = SET_SIZE + laststringch;
    else
	ichar = chartoichar (ch);
    if (!vflag  &&  iswordch (ichar)  &&  len == 1)
	{
	if (output)
	    (void) putchar (ch);
	(*cp)++;
	return 1;
	}
    if (ch == '\t')
	{
	if (output)
	    (void) putchar ('\t');
	(*cp)++;
	return 8 - (linew & 0x07);
	}
    /*
     * Character is non-printing, or it's ISO and vflag is set.  Display
     * it in "cat -v" form.  For string characters, display every element
     * separately in that form.
     */
    width = 0;
    if (maxw != 0  &&  len > maxw)
	len = maxw;			/* Don't show too much */
    for (i = 0;  i < len;  i++)
	{
	ch = (unsigned char) *(*cp)++;
	if (ch > '\177')
	    {
	    if (output)
		{
		(void) putchar ('M');
		(void) putchar ('-');
		}
	    width += 2;
	    ch &= 0x7f;
	    }
	if (ch < ' '  ||  ch == '\177')
	    {
	    if (output)
		{
		(void) putchar ('^');
		if (ch == '\177')
		    (void) putchar ('?');
		else
		    (void) putchar (ch + 'A' - '\001');
		}
	    width += 2;
	    }
	else
	    {
	    if (output)
		(void) putchar (ch);
	    width += 1;
	    }
	}
    return width;
    }

static int line_size (buf, bufend)
    char *		buf;
    register char *	bufend;
    {
    register int	width;

    for (width = 0;  buf < bufend  &&  *buf != '\0';  )
	width += show_char (&buf, width, 0, bufend - buf);
    return width;
    }

static void inserttoken (buf, start, tok, curchar)
    char *		buf;
    char *		start; 
    register char *	tok;
    char **		curchar;
    {
    char		copy[BUFSIZ];
    register char *	p;
    register char *	q;
    char *		ew;

    (void) strcpy (copy, buf);

    for (p = buf, q = copy; p != start; p++, q++)
	*p = *q;
    q += *curchar - start;
    ew = skipoverword (tok);
    while (tok < ew)
	*p++ = *tok++;
    *curchar = p;
    if (*tok)
	{

	/*
	** The token changed to two words.  Split it up and save the
	** second one for later.
	*/

	*p++ = *tok;
	*tok++ = '\0';
	while (*tok)
	    *p++ = *tok++;
	}
    while ((*p++ = *q++) != '\0')
	;
    }

static int posscmp (a, b)
    char *		a;
    char *		b;
    {

    return casecmp (a, b, 0);
    }

int casecmp (a, b, canonical)
    char *		a;
    char *		b;
    int			canonical;	/* NZ for canonical string chars */
    {
    register ichar_t *	ap;
    register ichar_t *	bp;
    ichar_t		inta[INPUTWORDLEN + 4 * MAXAFFIXLEN + 4];
    ichar_t		intb[INPUTWORDLEN + 4 * MAXAFFIXLEN + 4];

    (void) strtoichar (inta, a, sizeof inta, canonical);
    (void) strtoichar (intb, b, sizeof intb, canonical);
    for (ap = inta, bp = intb;  *ap != 0;  ap++, bp++)
	{
	if (*ap != *bp)
	    {
	    if (*bp == '\0')
		return hashheader.sortorder[*ap];
	    else if (mylower (*ap))
		{
		if (mylower (*bp)  ||  mytoupper (*ap) != *bp)
		    return (int) hashheader.sortorder[*ap]
		      - (int) hashheader.sortorder[*bp];
		}
	    else
		{
		if (myupper (*bp)  ||  mytolower (*ap) != *bp)
		    return (int) hashheader.sortorder[*ap]
		      - (int) hashheader.sortorder[*bp];
		}
	    }
	}
    if (*bp != '\0')
	return -(int) hashheader.sortorder[*bp];
    for (ap = inta, bp = intb;  *ap;  ap++, bp++)
	{
	if (*ap != *bp)
	    {
	    return (int) hashheader.sortorder[*ap]
	      - (int) hashheader.sortorder[*bp];
	    }
	}
    return 0;
    }

void makepossibilities (word)
    register ichar_t *	word;
    {
    register int	i;

    for (i = 0; i < MAXPOSSIBLE; i++)
	possibilities[i][0] = 0;
    pcount = 0;
    maxposslen = 0;
    easypossibilities = 0;

#ifndef NO_CAPITALIZATION_SUPPORT
    wrongcapital (word);
#endif

/* 
 * according to Pollock and Zamora, CACM April 1984 (V. 27, No. 4),
 * page 363, the correct order for this is:
 * OMISSION = TRANSPOSITION > INSERTION > SUBSTITUTION
 * thus, it was exactly backwards in the old version. -- PWP
 */

    if (pcount < MAXPOSSIBLE)
	missingletter (word);		/* omission */
    if (pcount < MAXPOSSIBLE)
	transposedletter (word);	/* transposition */
    if (pcount < MAXPOSSIBLE)
	extraletter (word);		/* insertion */
    if (pcount < MAXPOSSIBLE)
	wrongletter (word);		/* substitution */

    if ((compoundflag != COMPOUND_ANYTIME)  &&  pcount < MAXPOSSIBLE)
	missingspace (word);	/* two words */

    easypossibilities = pcount;
    if (easypossibilities == 0  ||  tryhardflag)
	tryveryhard (word);

    if ((sortit  ||  (pcount > easypossibilities))  &&  pcount)
	{
	if (easypossibilities > 0  &&  sortit)
	    qsort ((char *) possibilities,
	      (unsigned) easypossibilities,
	      sizeof (possibilities[0]),
	      (int (*) P ((const void *, const void *))) posscmp);
	if (pcount > easypossibilities)
	    qsort ((char *) &possibilities[easypossibilities][0],
	      (unsigned) (pcount - easypossibilities),
	      sizeof (possibilities[0]),
	      (int (*) P ((const void *, const void *))) posscmp);
	}
    }

static int insert (word)
    register ichar_t *	word;
    {
    register int	i;
    register char *	realword;

    realword = ichartosstr (word, 0);
    for (i = 0; i < pcount; i++)
	{
	if (strcmp (possibilities[i], realword) == 0)
	    return (0);
	}

    (void) strcpy (possibilities[pcount++], realword);
    i = strlen (realword);
    if (i > maxposslen)
	maxposslen = i;
    if (pcount >= MAXPOSSIBLE)
	return (-1);
    else
	return (0);
    }

#ifndef NO_CAPITALIZATION_SUPPORT
static void wrongcapital (word)
    register ichar_t *	word;
    {
    ichar_t		newword[INPUTWORDLEN + MAXAFFIXLEN];

    /*
    ** When the third parameter to "good" is nonzero, it ignores
    ** case.  If the word matches this way, "ins_cap" will recapitalize
    ** it correctly.
    */
    if (good (word, 0, 1, 0, 0))
	{
	(void) icharcpy (newword, word);
	upcase (newword);
	(void) ins_cap (newword, word);
	}
    }
#endif

static void wrongletter (word)
    register ichar_t *	word;
    {
    register int	i;
    register int	j;
    register int	n;
    ichar_t		savechar;
    ichar_t		newword[INPUTWORDLEN + MAXAFFIXLEN];

    n = icharlen (word);
    (void) icharcpy (newword, word);
#ifndef NO_CAPITALIZATION_SUPPORT
    upcase (newword);
#endif

    for (i = 0; i < n; i++)
	{
	savechar = newword[i];
	for (j=0; j < Trynum; ++j)
	    {
	    if (Try[j] == savechar)
		continue;
	    else if (isboundarych (Try[j])  &&  (i == 0  ||  i == n - 1))
		continue;
	    newword[i] = Try[j];
	    if (good (newword, 0, 1, 0, 0))
		{
		if (ins_cap (newword, word) < 0)
		    return;
		}
	    }
	newword[i] = savechar;
	}
    }

static void extraletter (word)
    register ichar_t *	word;
    {
    ichar_t		newword[INPUTWORDLEN + MAXAFFIXLEN];
    register ichar_t *	p;
    register ichar_t *	r;

    if (icharlen (word) < 2)
	return;

    (void) icharcpy (newword, word + 1);
    for (p = word, r = newword;  *p != 0;  )
	{
	if (good (newword, 0, 1, 0, 0))
	    {
	    if (ins_cap (newword, word) < 0)
		return;
	    }
	*r++ = *p++;
	}
    }

static void missingletter (word)
    ichar_t *		word;
    {
    ichar_t		newword[INPUTWORDLEN + MAXAFFIXLEN + 1];
    register ichar_t *	p;
    register ichar_t *	r;
    register int	i;

    (void) icharcpy (newword + 1, word);
    for (p = word, r = newword;  *p != 0;  )
	{
	for (i = 0;  i < Trynum;  i++)
	    {
	    if (isboundarych (Try[i])  &&  r == newword)
		continue;
	    *r = Try[i];
	    if (good (newword, 0, 1, 0, 0))
		{
		if (ins_cap (newword, word) < 0)
		    return;
		}
	    }
	*r++ = *p++;
	}
    for (i = 0;  i < Trynum;  i++)
	{
	if (isboundarych (Try[i]))
	    continue;
	*r = Try[i];
	if (good (newword, 0, 1, 0, 0))
	    {
	    if (ins_cap (newword, word) < 0)
		return;
	    }
	}
    }

static void missingspace (word)
    ichar_t *		word;
    {
    ichar_t		firsthalf[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN];
    int			firstno;	/* Index into first */
    ichar_t *		firstp;		/* Ptr into current firsthalf word */
    ichar_t		newword[INPUTWORDLEN + MAXAFFIXLEN + 1];
    int			nfirsthalf;	/* No. words saved in 1st half */
    int			nsecondhalf;	/* No. words saved in 2nd half */
    register ichar_t *	p;
    ichar_t		secondhalf[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN];
    int			secondno;	/* Index into second */

    /*
    ** We don't do words of length less than 3;  this keeps us from
    ** splitting all two-letter words into two single letters.  We
    ** also don't do maximum-length words, since adding the space
    ** would exceed the size of the "possibilities" array.
    */
    nfirsthalf = icharlen (word);
    if (nfirsthalf < 3  ||  nfirsthalf >= INPUTWORDLEN + MAXAFFIXLEN - 1)
	return;
    (void) icharcpy (newword + 1, word);
    for (p = newword + 1;  p[1] != '\0';  p++)
	{
	p[-1] = *p;
	*p = '\0';
	if (good (newword, 0, 1, 0, 0))
	    {
	    /*
	     * Save_cap must be called before good() is called on the
	     * second half, because it uses state left around by
	     * good().  This is unfortunate because it wastes a bit of
	     * time, but I don't think it's a significant performance
	     * problem.
	     */
	    nfirsthalf = save_cap (newword, word, firsthalf);
	    if (good (p + 1, 0, 1, 0, 0))
		{
		nsecondhalf = save_cap (p + 1, p + 1, secondhalf);
		for (firstno = 0;  firstno < nfirsthalf;  firstno++)
		    {
		    firstp = &firsthalf[firstno][p - newword];
		    for (secondno = 0;  secondno < nsecondhalf;  secondno++)
			{
			*firstp = ' ';
			(void) icharcpy (firstp + 1, secondhalf[secondno]);
			if (insert (firsthalf[firstno]) < 0)
			    return;
			*firstp = '-';
			if (insert (firsthalf[firstno]) < 0)
			    return;
			}
		    }
		}
	    }
	}
    }

int compoundgood (word, pfxopts)
    ichar_t *		word;
    int			pfxopts;	/* Options to apply to prefixes */
    {
    ichar_t		newword[INPUTWORDLEN + MAXAFFIXLEN];
    register ichar_t *	p;
    register ichar_t	savech;
    long		secondcap;	/* Capitalization of 2nd half */

    /*
    ** If compoundflag is COMPOUND_NEVER, compound words are never ok.
    */
    if (compoundflag == COMPOUND_NEVER)
	return 0;
    /*
    ** Test for a possible compound word (for languages like German that
    ** form lots of compounds).
    **
    ** This is similar to missingspace, except we quit on the first hit,
    ** and we won't allow either member of the compound to be a single
    ** letter.
    **
    ** We don't do words of length less than 2 * compoundmin, since
    ** both halves must at least compoundmin letters.
    */
    if (icharlen (word) < 2 * hashheader.compoundmin)
	return 0;
    (void) icharcpy (newword, word);
    p = newword + hashheader.compoundmin;
    for (  ;  p[hashheader.compoundmin - 1] != 0;  p++)
	{
	savech = *p;
	*p = 0;
	if (good (newword, 0, 0, pfxopts, FF_COMPOUNDONLY))
	    {
	    *p = savech;
	    if (good (p, 0, 1, FF_COMPOUNDONLY, 0)
	      ||  compoundgood (p, FF_COMPOUNDONLY))
		{
		secondcap = whatcap (p);
		switch (whatcap (newword))
		    {
		    case ANYCASE:
		    case CAPITALIZED:
		    case FOLLOWCASE:	/* Followcase can have l.c. suffix */
			return secondcap == ANYCASE;
		    case ALLCAPS:
			return secondcap == ALLCAPS;
		    }
		}
	    }
	else
	    *p = savech;
	}
    return 0;
    }

static void transposedletter (word)
    register ichar_t *	word;
    {
    ichar_t		newword[INPUTWORDLEN + MAXAFFIXLEN];
    register ichar_t *	p;
    register ichar_t	temp;

    (void) icharcpy (newword, word);
    for (p = newword;  p[1] != 0;  p++)
	{
	temp = *p;
	*p = p[1];
	p[1] = temp;
	if (good (newword, 0, 1, 0, 0))
	    {
	    if (ins_cap (newword, word) < 0)
		return;
	    }
	temp = *p;
	*p = p[1];
	p[1] = temp;
	}
    }

static void tryveryhard (word)
    ichar_t *		word;
    {
    (void) good (word, 1, 0, 0, 0);
    }

/* Insert one or more correctly capitalized versions of word */
static int ins_cap (word, pattern)
    ichar_t *		word;
    ichar_t *		pattern;
    {
    int			i;		/* Index into savearea */
    int			nsaved;		/* No. of words saved */
    ichar_t		savearea[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN];

    nsaved = save_cap (word, pattern, savearea);
    for (i = 0;  i < nsaved;  i++)
	{
	if (insert (savearea[i]) < 0)
	    return -1;
	}
    return 0;
    }

/* Save one or more correctly capitalized versions of word */
static int save_cap (word, pattern, savearea)
    ichar_t *		word;		/* Word to save */
    ichar_t *		pattern;	/* Prototype capitalization pattern */
    ichar_t		savearea[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN];
					/* Room to save words */
    {
    int			hitno;		/* Index into hits array */
    int			nsaved;		/* Number of words saved */
    int			preadd;		/* No. chars added to front of root */
    int			prestrip;	/* No. chars stripped from front */
    int			sufadd;		/* No. chars added to back of root */
    int			sufstrip;	/* No. chars stripped from back */

    if (*word == 0)
	return 0;

    for (hitno = numhits, nsaved = 0;  --hitno >= 0  &&  nsaved < MAX_CAPS;  )
	{
	if (hits[hitno].prefix)
	    {
	    prestrip = hits[hitno].prefix->stripl;
	    preadd = hits[hitno].prefix->affl;
	    }
	else
	    prestrip = preadd = 0;
	if (hits[hitno].suffix)
	    {
	    sufstrip = hits[hitno].suffix->stripl;
	    sufadd = hits[hitno].suffix->affl;
	    }
	else
	    sufadd = sufstrip = 0;
	save_root_cap (word, pattern, prestrip, preadd,
	    sufstrip, sufadd,
	    hits[hitno].dictent, hits[hitno].prefix, hits[hitno].suffix,
	    savearea, &nsaved);
	}
    return nsaved;
    }

int ins_root_cap (word, pattern, prestrip, preadd, sufstrip, sufadd,
  firstdent, pfxent, sufent)
    register ichar_t *	word;
    register ichar_t *	pattern;
    int			prestrip;
    int			preadd;
    int			sufstrip;
    int			sufadd;
    struct dent *	firstdent;
    struct flagent *	pfxent;
    struct flagent *	sufent;
    {
    int			i;		/* Index into savearea */
    ichar_t		savearea[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN];
    int			nsaved;		/* Number of words saved */

    nsaved = 0;
    save_root_cap (word, pattern, prestrip, preadd, sufstrip, sufadd,
      firstdent, pfxent, sufent, savearea, &nsaved);
    for (i = 0;  i < nsaved;  i++)
	{
	if (insert (savearea[i]) < 0)
	    return -1;
	}
    return 0;
    }

/* ARGSUSED */
static void save_root_cap (word, pattern, prestrip, preadd, sufstrip, sufadd,
  firstdent, pfxent, sufent, savearea, nsaved)
    register ichar_t *	word;		/* Word to be saved */
    register ichar_t *	pattern;	/* Capitalization pattern */
    int			prestrip;	/* No. chars stripped from front */
    int			preadd;		/* No. chars added to front of root */
    int			sufstrip;	/* No. chars stripped from back */
    int			sufadd;		/* No. chars added to back of root */
    struct dent *	firstdent;	/* First dent for root */
    struct flagent *	pfxent;		/* Pfx-flag entry for word */
    struct flagent *	sufent;		/* Sfx-flag entry for word */
    ichar_t		savearea[MAX_CAPS][INPUTWORDLEN + MAXAFFIXLEN];
					/* Room to save words */
    int *		nsaved;		/* Number saved so far (updated) */
    {
#ifndef NO_CAPITALIZATION_SUPPORT
    register struct dent * dent;
#endif /* NO_CAPITALIZATION_SUPPORT */
    int			firstisupper;
    ichar_t		newword[INPUTWORDLEN + 4 * MAXAFFIXLEN + 4];
#ifndef NO_CAPITALIZATION_SUPPORT
    register ichar_t *	p;
    int			len;
    int			i;
    int			limit;
#endif /* NO_CAPITALIZATION_SUPPORT */

    if (*nsaved >= MAX_CAPS)
	return;
    (void) icharcpy (newword, word);
    firstisupper = myupper (pattern[0]);
#ifdef NO_CAPITALIZATION_SUPPORT
    /*
    ** Apply the old, simple-minded capitalization rules.
    */
    if (firstisupper)
	{
	if (myupper (pattern[1]))
	    upcase (newword);
	else
	    {
	    lowcase (newword);
	    newword[0] = mytoupper (newword[0]);
	    }
	}
    else
	lowcase (newword);
    (void) icharcpy (savearea[*nsaved], newword);
    (*nsaved)++;
    return;
#else /* NO_CAPITALIZATION_SUPPORT */
#define flagsareok(dent)    \
    ((pfxent == NULL \
	||  TSTMASKBIT (dent->mask, pfxent->flagbit)) \
      &&  (sufent == NULL \
	||  TSTMASKBIT (dent->mask, sufent->flagbit)))

    dent = firstdent;
    if ((dent->flagfield & (CAPTYPEMASK | MOREVARIANTS)) == ALLCAPS)
	{
	upcase (newword);	/* Uppercase required */
	(void) icharcpy (savearea[*nsaved], newword);
	(*nsaved)++;
	return;
	}
    for (p = pattern;  *p;  p++)
	{
	if (mylower (*p))
	    break;
	}
    if (*p == 0)
	{
	upcase (newword);	/* Pattern was all caps */
	(void) icharcpy (savearea[*nsaved], newword);
	(*nsaved)++;
	return;
	}
    for (p = pattern + 1;  *p;  p++)
	{
	if (myupper (*p))
	    break;
	}
    if (*p == 0)
	{
	/*
	** The pattern was all-lower or capitalized.  If that's
	** legal, insert only that version.
	*/
	if (firstisupper)
	    {
	    if (captype (dent->flagfield) == CAPITALIZED
	      ||  captype (dent->flagfield) == ANYCASE)
		{
		lowcase (newword);
		newword[0] = mytoupper (newword[0]);
		(void) icharcpy (savearea[*nsaved], newword);
		(*nsaved)++;
		return;
		}
	    }
	else
	    {
	    if (captype (dent->flagfield) == ANYCASE)
		{
		lowcase (newword);
		(void) icharcpy (savearea[*nsaved], newword);
		(*nsaved)++;
		return;
		}
	    }
	while (dent->flagfield & MOREVARIANTS)
	    {
	    dent = dent->next;
	    if (captype (dent->flagfield) == FOLLOWCASE
	      ||  !flagsareok (dent))
		continue;
	    if (firstisupper)
		{
		if (captype (dent->flagfield) == CAPITALIZED)
		    {
		    lowcase (newword);
		    newword[0] = mytoupper (newword[0]);
		    (void) icharcpy (savearea[*nsaved], newword);
		    (*nsaved)++;
		    return;
		    }
		}
	    else
		{
		if (captype (dent->flagfield) == ANYCASE)
		    {
		    lowcase (newword);
		    (void) icharcpy (savearea[*nsaved], newword);
		    (*nsaved)++;
		    return;
		    }
		}
	    }
	}
    /*
    ** Either the sample had complex capitalization, or the simple
    ** capitalizations (all-lower or capitalized) are illegal.
    ** Insert all legal capitalizations, including those that are
    ** all-lower or capitalized.  If the prototype is capitalized,
    ** capitalized all-lower samples.  Watch out for affixes.
    */
    dent = firstdent;
    p = strtosichar (dent->word, 1);
    len = icharlen (p);
    if (dent->flagfield & MOREVARIANTS)
	dent = dent->next;	/* Skip place-holder entry */
    for (  ;  ;  )
	{
	if (flagsareok (dent))
	    {
	    if (captype (dent->flagfield) != FOLLOWCASE)
		{
		lowcase (newword);
		if (firstisupper  ||  captype (dent->flagfield) == CAPITALIZED)
		    newword[0] = mytoupper (newword[0]);
		(void) icharcpy (savearea[*nsaved], newword);
		(*nsaved)++;
		if (*nsaved >= MAX_CAPS)
		    return;
		}
	    else
		{
		/* Followcase is the tough one. */
		p = strtosichar (dent->word, 1);
		(void) bcopy ((char *) (p + prestrip),
		  (char *) (newword + preadd),
		  (len - prestrip - sufstrip) * sizeof (ichar_t));
		if (myupper (p[prestrip]))
		    {
		    for (i = 0;  i < preadd;  i++)
			newword[i] = mytoupper (newword[i]);
		    }
		else
		    {
		    for (i = 0;  i < preadd;  i++)
			newword[i] = mytolower (newword[i]);
		    }
		limit = len + preadd + sufadd - prestrip - sufstrip;
		i = len + preadd - prestrip - sufstrip;
		p += len - sufstrip - 1;
		if (myupper (*p))
		    {
		    for (p = newword + i;  i < limit;  i++, p++)
			*p = mytoupper (*p);
		    }
		else
		    {
		    for (p = newword + i;  i < limit;  i++, p++)
		      *p = mytolower (*p);
		    }
		(void) icharcpy (savearea[*nsaved], newword);
		(*nsaved)++;
		if (*nsaved >= MAX_CAPS)
		    return;
		}
	    }
	if ((dent->flagfield & MOREVARIANTS) == 0)
	    break;		/* End of the line */
	dent = dent->next;
	}
    return;
#endif /* NO_CAPITALIZATION_SUPPORT */
    }

static char * getline_ispell (s)
    register char *	s;
    {
    register char *	p;
    register int	c;

    p = s;

    for (  ;  ;  )
	{
	(void) fflush (stdout);
	c = (GETKEYSTROKE () & NOPARITY);
	if (c == '\\')
	    {
	    (void) putchar ('\\');
	    (void) fflush (stdout);
	    c = (GETKEYSTROKE () & NOPARITY);
	    backup ();
	    (void) putchar (c);
	    *p++ = (char) c;
	    }
	else if (c == ('G' & 037))
	    return (NULL);
	else if (c == '\n' || c == '\r')
	    {
	    *p = 0;
	    return (s);
	    }
	else if (c == uerasechar)
	    {
	    if (p != s)
		{
		p--;
		backup ();
		(void) putchar (' ');
		backup ();
		}
	    }
	else if (c == ukillchar)
	    {
	    while (p != s)
		{
		p--;
		backup ();
		(void) putchar (' ');
		backup ();
		}
	    }
	else
	    {
	    *p++ = (char) c;
	    (void) putchar (c);
	    }
	}
    }

void askmode ()
    {
    int			bufsize;	/* Length of contextbufs[0] */
    int			ch;		/* Next character read from input */
    register char *	cp1;
    register char *	cp2;
    ichar_t *		itok;		/* Ichar version of current word */
    int			hadnl;		/* NZ if \n was at end of line */

    if (fflag)
	{
	if (freopen (askfilename, "w", stdout) == NULL)
	    {
	    (void) fprintf (stderr, CANT_CREATE, askfilename);
	    exit (1);
	    }
	}

    (void) printf ("%s\n", Version_ID[0]);

    contextoffset = 0;
    while (1)
	{
	(void) fflush (stdout);
	/*
	 * Only read in enough characters to fill half this buffer so that any
	 * corrections we make are not likely to cause an overflow.
	 */
	if (contextoffset == 0)
	    {
	    if (xgets (contextbufs[0], (sizeof contextbufs[0]) / 2, stdin)
	      == NULL)
		break;
	    }
	else
	    {
	    if (fgets (contextbufs[0], (sizeof contextbufs[0]) / 2, stdin)
	      == NULL)
		break;
	    }
	/*
	 * If we didn't read to end-of-line, we may have ended the
	 * buffer in the middle of a word.  So keep reading until we
	 * see some sort of character that can't possibly be part of a
	 * word. (or until the buffer is full, which fortunately isn't
	 * all that likely).
	 */
	bufsize = strlen (contextbufs[0]);
	if (contextbufs[0][bufsize - 1] == '\n')
	    {
	    hadnl = 1;
	    contextbufs[0][--bufsize] = '\0';
	    }
	else
	    hadnl = 0;
	if (bufsize == (sizeof contextbufs[0]) / 2 - 1)
	    {
	    ch = (unsigned char) contextbufs[0][bufsize - 1];
	    while (bufsize < sizeof contextbufs[0] - 1
	      &&  (iswordch ((ichar_t) ch)  ||  isboundarych ((ichar_t) ch)
	      ||  isstringstart (ch)))
		{
		ch = getc (stdin);
		if (ch == EOF)
		    break;
		contextbufs[0][bufsize++] = (char) ch;
		contextbufs[0][bufsize] = '\0';
		}
	    }
	/*
	** *line is like `i', @line is like `a', &line is like 'u'
	** `#' is like `Q' (writes personal dictionary)
	** `+' sets tflag, `-' clears tflag
	** `!' sets terse mode, `%' clears terse
	** `~' followed by a filename sets parameters according to file name
	** `^' causes rest of line to be checked after stripping 1st char
	*/
	if (contextoffset != 0)
	    checkline (stdout);
	else
	    {
	    if (contextbufs[0][0] == '*'  ||  contextbufs[0][0] == '@')
		treeinsert(ichartosstr (strtosichar (contextbufs[0] + 1, 0), 1),
		  ICHARTOSSTR_SIZE,
		  contextbufs[0][0] == '*');
	    else if (contextbufs[0][0] == '&')
		{
		itok = strtosichar (contextbufs[0] + 1, 0);
		lowcase (itok);
		treeinsert (ichartosstr (itok, 1), ICHARTOSSTR_SIZE, 1);
		}
	    else if (contextbufs[0][0] == '#')
		{
		treeoutput ();
		math_mode = 0;
		LaTeX_Mode = 'P';
		}
	    else if (contextbufs[0][0] == '!')
		terse = 1;
	    else if (contextbufs[0][0] == '%')
		terse = 0;
	    else if (contextbufs[0][0] == '-')
		{
		math_mode = 0;
		LaTeX_Mode = 'P';
		tflag = 0;
		}
	    else if (contextbufs[0][0] == '+')
		{
		math_mode = 0;
		LaTeX_Mode = 'P';
		tflag = strcmp (&contextbufs[0][1], "nroff") != 0
		  &&  strcmp (&contextbufs[0][1], "troff") != 0;
		}
	    else if (contextbufs[0][0] == '~')
		{
		defdupchar = findfiletype (&contextbufs[0][1], 1, (int *) NULL);
		if (defdupchar < 0)
		    defdupchar = 0;
		}
	    else
		{
		if (contextbufs[0][0] == '^')
		    {
		    /* Strip off leading uparrow */
		    for (cp1 = contextbufs[0], cp2 = contextbufs[0] + 1;
		      (*cp1++ = *cp2++) != '\0';
		      )
			;
		    contextoffset++;
		    bufsize--;
		    }
		checkline (stdout);
		}
	    }
	if (hadnl)
	    contextoffset = 0;
	else
	    contextoffset += bufsize;
#ifndef USG
	if (sflag)
	    {
	    stop ();
	    if (fflag)
		{
		rewind (stdout);
		(void) creat (askfilename, 0666);
		}
	    }
#endif
	}
    }

/* Copy/ignore "cnt" number of characters pointed to by *cc. */
void copyout (cc, cnt)
    register char **	cc;
    register int	cnt;
    {

    while (--cnt >= 0)
	{
	if (**cc == '\0')
	    break;
	if (!aflag && !lflag)
	    (void) putc (**cc, outfile);
	(*cc)++;
	}
    }

static void lookharder (string)
    char *		string;
    {
    char		cmd[150];
    char		grepstr[100];
    register char *	g;
    register char *	s;
#ifndef REGEX_LOOKUP
    register int	wild = 0;
#ifdef LOOK
    static int		look = -1;
#endif /* LOOK */
#endif /* REGEX_LOOKUP */

    g = grepstr;
    for (s = string; *s != '\0'; s++)
	{
	if (*s == '*')
	    {
#ifndef REGEX_LOOKUP
	    wild++;
#endif /* REGEX_LOOKUP */
	    *g++ = '.';
	    *g++ = '*';
	    }
	else
	    *g++ = *s;
	}
    *g = '\0';
    if (grepstr[0])
	{
#ifdef REGEX_LOOKUP
	regex_dict_lookup (cmd, grepstr);
#else /* REGEX_LOOKUP */
#ifdef LOOK
	/* now supports automatic use of look - gms */
	if (!wild && look)
	    {
	    /* no wild and look(1) is possibly available */
	    (void) sprintf (cmd, "%s %s %s", LOOK, grepstr, WORDS);
	    if (shellescape (cmd))
		return;
	    else
		look = 0;
	    }
#endif /* LOOK */
	/* string has wild card chars or look not avail */
	if (!wild)
	    (void) strcat (grepstr, ".*");	/* work like look */
	(void) sprintf (cmd, "%s ^%s$ %s", EGREPCMD, grepstr, WORDS);
	(void) shellescape (cmd);
#endif /* REGEX_LOOKUP */
	}
    }

#ifdef REGEX_LOOKUP
static void regex_dict_lookup (cmd, grepstr)
    char *		cmd;
    char *		grepstr;
    {
    char *		rval;
    int			whence = 0;
    int			quitlookup = 0;
    int			count = 0;
    int			ch;

    (void) sprintf (cmd, "^%s$", grepstr);
    while (!quitlookup  &&  (rval = do_regex_lookup (cmd, whence)) != NULL)
	{
	whence = 1;
        (void) printf ("%s\r\n", rval);;
	if ((++count % (li - 1)) == 0)
	    {
	    inverse ();
	    (void) printf (CORR_C_MORE_PROMPT);
	    normal ();
	    (void) fflush (stdout);
	    if ((ch = GETKEYSTROKE ()) == 'q'
	      ||  ch == 'Q'  ||  ch == 'x'  ||  ch == 'X' )
	         quitlookup = 1;
	    /*
	     * The following line should blank out the -- more -- even on
	     * magic-cookie terminals.
	     */
	    (void) printf (CORR_C_BLANK_MORE);
	    (void) fflush (stdout);
	    }
	}
    if ( rval == NULL )
	{
	inverse ();
	(void) printf (CORR_C_END_LOOK);
	normal ();
	(void) fflush (stdout);
	(void) GETKEYSTROKE ();    
	}
    }

#endif /* REGEX_LOOKUP */
