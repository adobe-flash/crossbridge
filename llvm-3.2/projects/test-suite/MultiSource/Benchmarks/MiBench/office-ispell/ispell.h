/*
 * $Id: ispell.h 33046 2007-01-09 23:57:19Z lattner $
 */

/*
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
 * Revision 1.1.1.1  2007/01/09 02:58:54  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.68  1995/03/06  02:42:41  geoff
 * Be vastly more paranoid about parenthesizing macro arguments.  This
 * fixes a bug in defmt.c where a complex argument was passed to
 * isstringch.
 *
 * Revision 1.67  1995/01/03  19:24:12  geoff
 * Get rid of a non-global declaration.
 *
 * Revision 1.66  1994/12/27  23:08:49  geoff
 * Fix a lot of subtly bad assumptions about the widths of ints and longs
 * which only show up on 64-bit machines like the Cray and the DEC Alpha.
 *
 * Revision 1.65  1994/11/02  06:56:10  geoff
 * Remove the anyword feature, which I've decided is a bad idea.
 *
 * Revision 1.64  1994/10/25  05:46:18  geoff
 * Add the FF_ANYWORD flag for defining an affix that will apply to any
 * word, even if not explicitly specified.  (Good for French.)
 *
 * Revision 1.63  1994/09/16  04:48:28  geoff
 * Make stringdups and laststringch unsigned ints, and dupnos a plain
 * int, so that we can handle more than 128 stringchars and stringchar
 * types.
 *
 * Revision 1.62  1994/09/01  06:06:39  geoff
 * Change erasechar/killchar to uerasechar/ukillchar to avoid
 * shared-library problems on HP systems.
 *
 * Revision 1.61  1994/08/31  05:58:35  geoff
 * Add contextoffset, used in -a mode to handle extremely long lines.
 *
 * Revision 1.60  1994/05/17  06:44:15  geoff
 * Add support for controlled compound formation and the COMPOUNDONLY
 * option to affix flags.
 *
 * Revision 1.59  1994/03/15  06:25:16  geoff
 * Change deftflag's initialization so we can tell if -t/-n appeared.
 *
 * Revision 1.58  1994/02/07  05:53:28  geoff
 * Add typecasts to the the 7-bit versions of ichar* routines
 *
 * Revision 1.57  1994/01/25  07:11:48  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include <stdio.h>

#ifdef __STDC__
#define P(x)	x
#define VOID	void
#else /* __STDC__ */
#define P(x)	()
#define VOID	char
#define const
#endif /* __STDC__ */

#ifdef NO8BIT
#define SET_SIZE	128
#else
#define SET_SIZE	256
#endif

#define MASKSIZE	(MASKBITS / MASKTYPE_WIDTH)

#ifdef lint
extern void	SETMASKBIT P ((MASKTYPE * mask, int bit));
extern void	CLRMASKBIT P ((MASKTYPE * mask, int bit));
extern int	TSTMASKBIT P ((MASKTYPE * mask, int bit));
#else /* lint */
/* The following is really testing for MASKSIZE <= 1, but cpp can't do that */
#if MASKBITS <= MASKTYPE_WIDTH
#define SETMASKBIT(mask, bit) ((mask)[0] |= (MASKTYPE) 1 << (bit))
#define CLRMASKBIT(mask, bit) ((mask)[0] &= (MASKTYPE) ~(1 << (bit)))
#define TSTMASKBIT(mask, bit) ((mask)[0] & ((MASKTYPE) 1 << (bit)))
#else
#define SETMASKBIT(mask, bit) \
		    ((mask)[(bit) / MASKTYPE_WIDTH] |= \
		      (MASKTYPE) 1 << ((bit) & (MASKTYPE_WIDTH - 1)))
#define CLRMASKBIT(mask, bit) \
		    ((mask)[(bit) / MASKTYPE_WIDTH] &= \
		      ~((MASKTYPE) 1 << ((bit) & (MASKTYPE_WIDTH - 1))))
#define TSTMASKBIT(mask, bit) \
		    ((mask)[(bit) / MASKTYPE_WIDTH] & \
		      ((MASKTYPE) 1 << ((bit) & (MASKTYPE_WIDTH - 1))))
#endif
#endif /* lint */

#if MASKBITS > 64
#define FULLMASKSET
#endif

#ifdef lint
extern int	BITTOCHAR P ((int bit));
extern int	CHARTOBIT P ((int ch));
#endif /* lint */

#if MASKBITS <= 32
# ifndef lint
#define BITTOCHAR(bit)	((bit) + 'A')
#define CHARTOBIT(ch)	((ch) - 'A')
# endif /* lint */
#define LARGESTFLAG	26	/* 5 are needed for flagfield below */
#define FLAGBASE	((MASKTYPE_WIDTH) - 6)
#else
# if MASKBITS <= 64
#  ifndef lint
#define BITTOCHAR(bit)	((bit) + 'A')
#define CHARTOBIT(ch)	((ch) - 'A')
#  endif /* lint */
#define LARGESTFLAG	(64 - 6) /* 5 are needed for flagfield below */
#define FLAGBASE	((MASKTYPE_WIDTH) - 6)
# else
#  ifndef lint
#define BITTOCHAR(bit)	(bit)
#define CHARTOBIT(ch)	(ch)
#  endif /* lint */
#define LARGESTFLAG	MASKBITS /* flagfield is a separate field */
#define FLAGBASE	0
# endif
#endif

/*
** Data type for internal word storage.  If necessary, we use shorts rather
** than chars so that string characters can be encoded as a single unit.
*/
#if (SET_SIZE + MAXSTRINGCHARS) <= 256
#ifndef lint
#define ICHAR_IS_CHAR
#endif /* lint */
#endif

#ifdef ICHAR_IS_CHAR
typedef unsigned char	ichar_t;	/* Internal character */
#define icharlen(s)	strlen ((char *) (s))
#define icharcpy(a, b)	strcpy ((char *) (a), (char *) (b))
#define icharcmp(a, b)	strcmp ((char *) (a), (char *) (b))
#define icharncmp(a, b, n) strncmp ((char *) (a), (char *) (b), (n))
#define chartoichar(x)	((ichar_t) (x))
#else
typedef unsigned short	ichar_t;	/* Internal character */
#define chartoichar(x)	((ichar_t) (unsigned char) (x))
#endif

struct dent
    {
    struct dent *	next;
    char *		word;
    MASKTYPE		mask[MASKSIZE];
#ifdef FULLMASKSET
    char		flags;
#endif
    };

/*
** Flags in the directory entry.  If FULLMASKSET is undefined, these are
** stored in the highest bits of the last longword of the mask field.  If
** FULLMASKSET is defined, they are stored in the extra "flags" field.
#ifndef NO_CAPITALIZATION_SUPPORT
**
** If a word has only one capitalization form, and that form is not
** FOLLOWCASE, it will have exactly one entry in the dictionary.  The
** legal capitalizations will be indicated by the 2-bit capitalization
** field, as follows:
**
**	ALLCAPS		The word must appear in all capitals.
**	CAPITALIZED	The word must be capitalized (e.g., London).
**			It will also be accepted in all capitals.
**	ANYCASE		The word may appear in lowercase, capitalized,
**			or all-capitals.
**
** Regardless of the capitalization flags, the "word" field of the entry
** will point to an all-uppercase copy of the word.  This is to simplify
** the large portion of the code that doesn't care about capitalization.
** Ispell will generate the correct version when needed.
**
** If a word has more than one capitalization, there will be multiple
** entries for it, linked together by the "next" field.  The initial
** entry for such words will be a dummy entry, primarily for use by code
** that ignores capitalization.  The "word" field of this entry will
** again point to an all-uppercase copy of the word.  The "mask" field
** will contain the logical OR of the mask fields of all variants.
** A header entry is indicated by a capitalization type of ALLCAPS,
** with the MOREVARIANTS bit set.
**
** The following entries will define the individual variants.  Each
** entry except the last has the MOREVARIANTS flag set, and each
** contains one of the following capitalization options:
**
**	ALLCAPS		The word must appear in all capitals.
**	CAPITALIZED	The word must be capitalized (e.g., London).
**			It will also be accepted in all capitals.
**	FOLLOWCASE	The word must be capitalized exactly like the
**			sample in the entry.  Prefix (suffix) characters
**			must be rendered in the case of the first (last)
**			"alphabetic" character.  It will also be accepted
**			in all capitals.  ("Alphabetic" means "mentioned
**			in a 'casechars' statement".)
**	ANYCASE		The word may appear in lowercase, capitalized,
**			or all-capitals.
**
** The "mask" field for the entry contains only the affix flag bits that
** are legal for that capitalization.  The "word" field will be null
** except for FOLLOWCASE entries, where it will point to the
** correctly-capitalized spelling of the root word.
**
** It is worth discussing why the ALLCAPS option is used in
** the header entry.  The header entry accepts an all-capitals
** version of the root plus every affix (this is always legal, since
** words get capitalized in headers and so forth).  Further, all of
** the following variant entries will reject any all-capitals form
** that is illegal due to an affix.
**
** Finally, note that variations in the KEEP flag can cause a multiple-variant
** entry as well.  For example, if the personal dictionary contains "ALPHA",
** (KEEP flag set) and the user adds "alpha" with the KEEP flag clear, a
** multiple-variant entry will be created so that "alpha" will be accepted
** but only "ALPHA" will actually be kept.
#endif
*/
#ifdef FULLMASKSET
#define flagfield	flags
#else
#define flagfield	mask[MASKSIZE - 1]
#endif
#define USED		((MASKTYPE) 1 << (FLAGBASE + 0))
#define KEEP		((MASKTYPE) 1 << (FLAGBASE + 1))
#ifdef NO_CAPITALIZATION_SUPPORT
#define ALLFLAGS	(USED | KEEP)
#else /* NO_CAPITALIZATION_SUPPORT */
#define ANYCASE		((MASKTYPE) 0 << (FLAGBASE + 2))
#define ALLCAPS		((MASKTYPE) 1 << (FLAGBASE + 2))
#define CAPITALIZED	((MASKTYPE) 2 << (FLAGBASE + 2))
#define FOLLOWCASE	((MASKTYPE) 3 << (FLAGBASE + 2))
#define CAPTYPEMASK	((MASKTYPE) 3 << (FLAGBASE + 2))
#define MOREVARIANTS	((MASKTYPE) 1 << (FLAGBASE + 4))
#define ALLFLAGS	(USED | KEEP | CAPTYPEMASK | MOREVARIANTS)
#define captype(x)	((x) & CAPTYPEMASK)
#endif /* NO_CAPITALIZATION_SUPPORT */

/*
 * Language tables used to encode prefix and suffix information.
 */
struct flagent
    {
    ichar_t *		strip;			/* String to strip off */
    ichar_t *		affix;			/* Affix to append */
    short		flagbit;		/* Flag bit this ent matches */
    short		stripl;			/* Length of strip */
    short		affl;			/* Length of affix */
    short		numconds;		/* Number of char conditions */
    short		flagflags;		/* Modifiers on this flag */
    char		conds[SET_SIZE + MAXSTRINGCHARS]; /* Adj. char conds */
    };

/*
 * Bits in flagflags
 */
#define FF_CROSSPRODUCT	(1 << 0)		/* Affix does cross-products */
#define FF_COMPOUNDONLY	(1 << 1)		/* Afx works in compounds */

union ptr_union					/* Aid for building flg ptrs */
    {
    struct flagptr *	fp;			/* Pointer to more indexing */
    struct flagent *	ent;			/* First of a list of ents */
    };

struct flagptr
    {
    union ptr_union	pu;			/* Ent list or more indexes */
    int			numents;		/* If zero, pu.fp is valid */
    };

/*
 * Description of a single string character type.
 */
struct strchartype
    {
    char *		name;			/* Name of the type */
    char *		deformatter;		/* Deformatter to use */
    char *		suffixes;		/* File suffixes, null seps */
    };

/*
 * Header placed at the beginning of the hash file.
 */
struct hashheader
    {
    unsigned short magic;    	    	    	/* Magic number for ID */
    unsigned short compileoptions;		/* How we were compiled */
    short maxstringchars;			/* Max # strchrs we support */
    short maxstringcharlen;			/* Max strchr len supported */
    short compoundmin;				/* Min lth of compound parts */
    short compoundbit;				/* Flag 4 compounding roots */
    int stringsize;				/* Size of string table */
    int lstringsize;				/* Size of lang. str tbl */
    int tblsize;				/* No. entries in hash tbl */
    int stblsize;				/* No. entries in sfx tbl */
    int ptblsize;				/* No. entries in pfx tbl */
    int sortval;				/* Largest sort ID assigned */
    int nstrchars;				/* No. strchars defined */
    int nstrchartype;				/* No. strchar types */
    int strtypestart;				/* Start of strtype table */
    char nrchars[5];				/* Nroff special characters */
    char texchars[13];				/* TeX special characters */
    char compoundflag;				/* Compund-word handling */
    char defhardflag;				/* Default tryveryhard flag */
    char flagmarker;				/* "Start-of-flags" char */
    unsigned short sortorder[SET_SIZE + MAXSTRINGCHARS]; /* Sort ordering */
    ichar_t lowerconv[SET_SIZE + MAXSTRINGCHARS]; /* Lower-conversion table */
    ichar_t upperconv[SET_SIZE + MAXSTRINGCHARS]; /* Upper-conversion table */
    char wordchars[SET_SIZE + MAXSTRINGCHARS]; /* NZ for chars found in wrds */
    char upperchars[SET_SIZE + MAXSTRINGCHARS]; /* NZ for uppercase chars */
    char lowerchars[SET_SIZE + MAXSTRINGCHARS]; /* NZ for lowercase chars */
    char boundarychars[SET_SIZE + MAXSTRINGCHARS]; /* NZ for boundary chars */
    char stringstarts[SET_SIZE];		/* NZ if char can start str */
    char stringchars[MAXSTRINGCHARS][MAXSTRINGCHARLEN + 1]; /* String chars */
    unsigned int stringdups[MAXSTRINGCHARS];	/* No. of "base" char */
    int dupnos[MAXSTRINGCHARS];			/* Dup char ID # */
    unsigned short magic2;			/* Second magic for dbl chk */
    };

/* hash table magic number */
#define MAGIC			0x9602

/* compile options, put in the hash header for consistency checking */
#ifdef NO8BIT
# define MAGIC8BIT		0x01
#else
# define MAGIC8BIT		0x00
#endif
#ifdef NO_CAPITALIZATION_SUPPORT
# define MAGICCAPITALIZATION	0x00
#else
# define MAGICCAPITALIZATION	0x02
#endif
#if MASKBITS <= 32
# define MAGICMASKSET		0x00
#else
# if MASKBITS <= 64
#  define MAGICMASKSET		0x04
# else
#  if MASKBITS <= 128
#   define MAGICMASKSET		0x08
#  else
#   define MAGICMASKSET		0x0C
#  endif
# endif
#endif

#define COMPILEOPTIONS	(MAGIC8BIT | MAGICCAPITALIZATION | MAGICMASKSET)

/*
 * Structure used to record data about successful lookups; these values
 * are used in the ins_root_cap routine to produce correct capitalizations.
 */
struct success
    {
    struct dent *	dictent;	/* Header of dict entry chain for wd */
    struct flagent *	prefix;		/* Prefix flag used, or NULL */
    struct flagent *	suffix;		/* Suffix flag used, or NULL */
    };

/*
** Offsets into the nroff special-character array
*/
#define NRLEFTPAREN	hashheader.nrchars[0]
#define NRRIGHTPAREN	hashheader.nrchars[1]
#define NRDOT		hashheader.nrchars[2]
#define NRBACKSLASH	hashheader.nrchars[3]
#define NRSTAR		hashheader.nrchars[4]

/*
** Offsets into the TeX special-character array
*/
#define TEXLEFTPAREN	hashheader.texchars[0]
#define TEXRIGHTPAREN	hashheader.texchars[1]
#define TEXLEFTSQUARE	hashheader.texchars[2]
#define TEXRIGHTSQUARE	hashheader.texchars[3]
#define TEXLEFTCURLY	hashheader.texchars[4]
#define TEXRIGHTCURLY	hashheader.texchars[5]
#define TEXLEFTANGLE	hashheader.texchars[6]
#define TEXRIGHTANGLE	hashheader.texchars[7]
#define TEXBACKSLASH	hashheader.texchars[8]
#define TEXDOLLAR	hashheader.texchars[9]
#define TEXSTAR		hashheader.texchars[10]
#define TEXDOT		hashheader.texchars[11]
#define TEXPERCENT	hashheader.texchars[12]

/*
** Values for compoundflag
*/
#define COMPOUND_NEVER		0	/* Compound words are never good */
#define COMPOUND_ANYTIME	1	/* Accept run-together words */
#define COMPOUND_CONTROLLED	2	/* Compounds controlled by afx flags */

/*
** The isXXXX macros normally only check ASCII range, and don't support
** the character sets of other languages.  These private versions handle
** whatever character sets have been defined in the affix files.
*/
#ifdef lint
extern int	myupper P ((unsigned int ch));
extern int	mylower P ((unsigned int ch));
extern int	myspace P ((unsigned int ch));
extern int	iswordch P ((unsigned int ch));
extern int	isboundarych P ((unsigned int ch));
extern int	isstringstart P ((unsigned int ch));
extern ichar_t	mytolower P ((unsigned int ch));
extern ichar_t	mytoupper P ((unsigned int ch));
#else /* lint */
#define myupper(X)	(hashheader.upperchars[(X)])
#define mylower(X)	(hashheader.lowerchars[(X)])
#define myspace(X)	(((X) > 0)  &&  ((X) < 0x80) \
			  &&  isspace((unsigned char) (X)))
#define iswordch(X)	(hashheader.wordchars[(X)])
#define isboundarych(X) (hashheader.boundarychars[(X)])
#define isstringstart(X) (hashheader.stringstarts[(unsigned char) (X)])
#define mytolower(X)	(hashheader.lowerconv[(X)])
#define mytoupper(X)	(hashheader.upperconv[(X)])
#endif /* lint */

/*
** These macros are similar to the ones above, but they take into account
** the possibility of string characters.  Note well that they take a POINTER,
** not a character.
**
** The "l_" versions set "len" to the length of the string character as a
** handy side effect.  (Note that the global "laststringch" is also set,
** and sometimes used, by these macros.)
**
** The "l1_" versions go one step further and guarantee that the "len"
** field is valid for *all* characters, being set to 1 even if the macro
** returns false.  This macro is a great example of how NOT to write
** readable C.
*/
#define isstringch(ptr, canon)	(isstringstart (*(ptr)) \
				  &&  stringcharlen ((ptr), (canon)) > 0)
#define l_isstringch(ptr, len, canon)	\
				(isstringstart (*(ptr)) \
				  &&  (len = stringcharlen ((ptr), (canon))) \
				    > 0)
#define l1_isstringch(ptr, len, canon)	\
				(len = 1, \
				  isstringstart (*(ptr)) \
				    &&  ((len = \
					  stringcharlen ((ptr), (canon))) \
					> 0 \
				      ? 1 : (len = 1, 0)))

/*
 * Sizes of buffers returned by ichartosstr/strtosichar.
 */
#define ICHARTOSSTR_SIZE (INPUTWORDLEN + 4 * MAXAFFIXLEN + 4)
#define STRTOSICHAR_SIZE ((INPUTWORDLEN + 4 * MAXAFFIXLEN + 4) \
			  * sizeof (ichar_t))

/*
 * termcap variables
 */
#ifdef MAIN
# define EXTERN /* nothing */
#else
# define EXTERN extern
#endif

EXTERN char *	BC;	/* backspace if not ^H */
EXTERN char *	cd;	/* clear to end of display */
EXTERN char *	cl;	/* clear display */
EXTERN char *	cm;	/* cursor movement */
EXTERN char *	ho;	/* home */
EXTERN char *	nd;	/* non-destructive space */
EXTERN char *	so;	/* standout */
EXTERN char *	se;	/* standout end */
EXTERN int	sg;	/* space taken by so/se */
EXTERN char *	ti;	/* terminal initialization sequence */
EXTERN char *	te;	/* terminal termination sequence */
EXTERN int	li;	/* lines */
EXTERN int	co;	/* columns */

EXTERN int	contextsize;	/* number of lines of context to show */
EXTERN char	contextbufs[MAXCONTEXT][BUFSIZ]; /* Context of current line */
EXTERN int	contextoffset;	/* Offset of line start in contextbufs[0] */
EXTERN char *	currentchar;	/* Location in contextbufs */
EXTERN char	ctoken[INPUTWORDLEN + MAXAFFIXLEN]; /* Current token as char */
EXTERN ichar_t	itoken[INPUTWORDLEN + MAXAFFIXLEN]; /* Ctoken as ichar_t str */

EXTERN char	termcap[2048];	/* termcap entry */
EXTERN char	termstr[2048];	/* for string values */
EXTERN char *	termptr;	/* pointer into termcap, used by tgetstr */

EXTERN int	numhits;	/* number of hits in dictionary lookups */
EXTERN struct success
		hits[MAX_HITS]; /* table of hits gotten in lookup */

EXTERN char *	hashstrings;	/* Strings in hash table */
EXTERN struct hashheader
		hashheader;	/* Header of hash table */
EXTERN struct dent *
		hashtbl;	/* Main hash table, for dictionary */
EXTERN int	hashsize;	/* Size of main hash table */

EXTERN char	hashname[MAXPATHLEN]; /* Name of hash table file */

EXTERN int	aflag;		/* NZ if -a or -A option specified */
EXTERN int	cflag;		/* NZ if -c (crunch) option */
EXTERN int	lflag;		/* NZ if -l (list) option */
EXTERN int	incfileflag;	/* whether xgets() acts exactly like gets() */
EXTERN int	nodictflag;	/* NZ if dictionary not needed */

EXTERN int	uerasechar;	/* User's erase character, from stty */
EXTERN int	ukillchar;	/* User's kill character */

EXTERN unsigned int laststringch; /* Number of last string character */
EXTERN int	defdupchar;	/* Default duplicate string type */

EXTERN int	numpflags;		/* Number of prefix flags in table */
EXTERN int	numsflags;		/* Number of suffix flags in table */
EXTERN struct flagptr pflagindex[SET_SIZE + MAXSTRINGCHARS];
					/* Fast index to pflaglist */
EXTERN struct flagent *	pflaglist;	/* Prefix flag control list */
EXTERN struct flagptr sflagindex[SET_SIZE + MAXSTRINGCHARS];
					/* Fast index to sflaglist */
EXTERN struct flagent *	sflaglist;	/* Suffix flag control list */

EXTERN struct strchartype *		/* String character type collection */
		chartypes;

EXTERN FILE *	infile;			/* File being corrected */
EXTERN FILE *	outfile;		/* Corrected copy of infile */

EXTERN char *	askfilename;		/* File specified in -f option */

EXTERN int	changes;		/* NZ if changes made to cur. file */
EXTERN int	readonly;		/* NZ if current file is readonly */
EXTERN int	quit;			/* NZ if we're done with this file */

#define MAXPOSSIBLE	100	/* Max no. of possibilities to generate */

EXTERN char	possibilities[MAXPOSSIBLE][INPUTWORDLEN + MAXAFFIXLEN];
				/* Table of possible corrections */
EXTERN int	pcount;		/* Count of possibilities generated */
EXTERN int	maxposslen;	/* Length of longest possibility */
EXTERN int	easypossibilities; /* Number of "easy" corrections found */
				/* ..(defined as those using legal affixes) */

/*
 * The following array contains a list of characters that should be tried
 * in "missingletter."  Note that lowercase characters are omitted.
 */
EXTERN int	Trynum;		/* Size of "Try" array */
EXTERN ichar_t	Try[SET_SIZE + MAXSTRINGCHARS];

/*
 * Initialized variables.  These are generated using macros so that they
 * may be consistently declared in all programs.  Numerous examples of
 * usage are given below.
 */
#ifdef MAIN
#define INIT(decl, init)	decl = init
#else
#define INIT(decl, init)	extern decl
#endif

#ifdef MINIMENU
INIT (int minimenusize, 2);		/* MUST be either 2 or zero */
#else /* MINIMENU */
INIT (int minimenusize, 0);		/* MUST be either 2 or zero */
#endif /* MINIMENU */

INIT (int eflag, 0);			/* NZ for expand mode */
INIT (int dumpflag, 0);			/* NZ to do dump mode */
INIT (int fflag, 0);			/* NZ if -f specified */
#ifndef USG
INIT (int sflag, 0);			/* NZ to stop self after EOF */
#endif
INIT (int vflag, 0);			/* NZ to display characters as M-xxx */
INIT (int xflag, DEFNOBACKUPFLAG);	/* NZ to suppress backups */
INIT (int deftflag, -1);		/* NZ for TeX mode by default */
INIT (int tflag, DEFTEXFLAG);		/* NZ for TeX mode in current file */
INIT (int prefstringchar, -1);		/* Preferred string character type */

INIT (int terse, 0);			/* NZ for "terse" mode */

INIT (char tempfile[MAXPATHLEN], "");	/* Name of file we're spelling into */

INIT (int minword, MINWORD);		/* Longest always-legal word */
INIT (int sortit, 1);			/* Sort suggestions alphabetically */
INIT (int compoundflag, -1);		/* How to treat compounds: see above */
INIT (int tryhardflag, -1);		/* Always call tryveryhard */

INIT (char * currentfile, NULL);	/* Name of current input file */

/* Odd numbers for math mode in LaTeX; even for LR or paragraph mode */
INIT (int math_mode, 0);
/* P -- paragraph or LR mode
 * b -- parsing a \begin statement
 * e -- parsing an \end statement
 * r -- parsing a \ref type of argument.
 * m -- looking for a \begin{minipage} argument.
 */
INIT (char LaTeX_Mode, 'P');
