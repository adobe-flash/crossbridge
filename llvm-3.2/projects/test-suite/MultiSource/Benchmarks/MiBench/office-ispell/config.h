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
 * This is the configuration file for ispell.  Thanks to Bob McQueer
 * for creating it and making the necessary changes elsewhere to
 * support it, and to George Sipe for figuring out how to make it easier
 * to use.
 *
 * WARNING:  The Makefile edits this file (config.X) to produce config.h.
 * If you are looking at config.h, you're in the wrong file.
 *
 * Look through this file from top to bottom.  If anything needs changing,
 * create the header file "local.h" and define the correct values there;
 * they will override this file.  If you don't make any changes to this
 * file, future patching will be easier.
 */

/*
 * $Id: config.h 35855 2007-04-10 02:59:31Z jeffc $
 *
 * $Log$
 * Revision 1.3  2007/04/10 02:59:31  jeffc
 * Use correct dirent.h header file.
 *
 * Revision 1.2  2007/03/29 17:00:52  jeffc
 * Fix for src != obj.
 * Don't use obsolete header file.
 *
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:58:48  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.71  1995/01/08  23:23:28  geoff
 * Add some more configuration variables: HAS_RENAME, MSDOS_BINARY_OPEN,
 * HOME, PDICTHOME, HASHSUFFIX, STATSUFFIX, and COUNTSUFFIX.  These are
 * all to make it easier to port ispell to MS-DOS.  Change DEFPAFF back
 * to "words" so that only .ispell_words will be independent of language.
 *
 * Revision 1.70  1994/10/25  05:45:57  geoff
 * Fix a tiny typo in a comment.  Add a configurable install command.
 *
 * Revision 1.69  1994/09/01  06:06:30  geoff
 * Improve the the documentation of LANGUAGES to include working examples.
 *
 * Revision 1.68  1994/07/28  05:11:34  geoff
 * Log message for previous revision: force MASKBITS to greater than or
 * equal to MASKTYPE_WIDTH (simplifies configuration for 64-bit
 * machines).
 *
 * Revision 1.67  1994/07/28  04:53:34  geoff
 *
 * Revision 1.66  1994/04/27  02:50:46  geoff
 * Change the documentation and defaults for the languages variable to
 * reflect the new method of making American and British dictionary
 * variants.
 *
 * Revision 1.65  1994/04/27  01:50:28  geoff
 * Add MAX_CAPS.
 *
 * Revision 1.64  1994/02/07  08:10:42  geoff
 * Add GENERATE_LIBRARY_PROTOS as a special variable for use only by me.
 *
 * Revision 1.63  1994/01/26  07:44:45  geoff
 * Make yacc configurable through local.h.
 *
 * Revision 1.62  1994/01/25  07:11:20  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

/* You may wish to specify your local definitions in this file: */

#include "local.h"	/* local definitions for options */

/*
** Major-differences selection.  The default system is BSD;  for USG
** or non-UNIX systems you should add the appropriate #define to local.h.
*/
#ifndef USG
#undef USG		/* Define this in local.h for System V machines */
#endif /* USG */

#include <sys/param.h>
#include <sys/types.h>
#ifndef USG
#include <dirent.h>
#endif /* USG */

/*
** Things that normally go in a Makefile.  Define these just like you
** might in the Makefile, except you should use #define instead of
** make's assignment syntax.  Everything must be double-quoted, and
** (unlike make) you can't use any sort of $-syntax to pick up the
** values of other definitions.
*/
#ifndef CC
#define CC	"cc"
#endif /* CC */
#ifndef EMACS
#define EMACS	"emacs"
#endif /* EMACS */
#ifndef LINT
#define LINT	"lint"
#endif /* LINT */
#ifndef CFLAGS
#define CFLAGS	"-O"
#endif /* CFLAGS */
#ifndef LINTFLAGS
#define LINTFLAGS ""
#endif /* LINTFLAGS */
#ifndef YACC
#define YACC	"yacc"
#endif /* YACC */

/*
** Libraries that may need to be added to the cc line to get ispell to
** link.  Normally, this should be null.
*/
#ifndef LIBES
#define LIBES	""
#endif

/*
** TERMLIB - where to get the termcap library.  Should be -ltermcap or
** -lcurses on most systems.
*/
#ifndef TERMLIB
#define TERMLIB ""
#endif

/*
** REGLIB - where to get the regular-expression routines, if
** REGEX_LOOKUP is defined.  Should be -lPW on USG systems, null on
** BSD systems.
*/
#ifndef REGLIB
#define REGLIB	""
#endif

/*
** Where to install various components of ispell.  BINDIR contains
** binaries.  LIBDIR contains hash tables and affix files.  ELISPDIR
** contains emacs lisp files (if any) and TEXINFODIR contains emacs
** TeXinfo files.  MAN1DIR and MAN4DIR will hold the chapter-1 and
** chapter-4 manual pages, respectively.
**
** If you intend to use multiple dictionary files, I would suggest
** LIBDIR be a directory which will contain nothing else, so sensible
** names can be constructed for the -d option without conflict.
*/
#ifndef BINDIR
#define BINDIR	"/usr/local/bin"
#endif
#ifndef LIBDIR
#define LIBDIR	""
#endif
#ifndef ELISPDIR
#define ELISPDIR "/usr/local/lib/emacs/site-lisp"
#endif
#ifndef TEXINFODIR
#define TEXINFODIR "/usr/local/info"
#endif
#ifndef MAN1DIR
#define MAN1DIR	"/usr/local/man/man1"
#endif
#ifndef MAN4DIR
#define MAN4DIR	"/usr/local/man/man4"
#endif

/*
** Extensions to put on manual pages.  Usually these are ".1" or ".1l".
*/
#ifndef MAN1EXT
#define MAN1EXT	".1"
#endif
#ifndef MAN4EXT
#define MAN4EXT	".4"
#endif

/*
** List of all hash files (languages) which will be supported by ispell.
**
** This variable has a complex format so that many options can be
** specified.  The format is as follows:
**
**	<language>[,<make-options>...] [<language> [,<make-options> ...] ...]
**
** where
**
**	language	is the name of a subdirectory of the
**			"languages" directory
**	make-options	are options that are to be passed to "make" in
**			the specified directory.  The make-options
**			should not, in general, specify a target, as
**			this will be provided by the make process.
**
** For example, if LANGUAGES is:
**
**	"{american,MASTERDICTS=american.med+,HASHFILES=americanmed+.hash,EXTRADICT=/usr/dict/words /usr/dict/web2} {deutsch,DICTALWAYS=deutsch.sml,DICTOPTIONS=}"
**
** then the American-English and Deutsch (German) languages will be supported,
** and the following variable settings will be passed to the two Makefiles:
**
**	American:
**
**	    MASTERDICTS='american.med+'
**	    HASHFILES='americanmed+.hash'
**	    EXTRADICT='/usr/dict/words /usr/dict/web2'
**
**	Deutsch:
**
**	    DICTALWAYS='deutsch.sml'
**	    DICTOPTIONS=''
**
** Notes on the syntax: The makefile is not very robust.  If you have
** make problems, or if make seems to fail in the language-subdirs
** dependency, check your syntax.  The makefile adds single quotes to
** the individual variables in the LANGUAGES specification, so don't
** use quotes of any kind.
**
** In the future, the first language listed in this variable will
** become the default, and the DEFHASH, DEFLANG, and DEFPAFF,
** variables will all become obsolete.  So be sure to put your default
** language first, to make later conversion easier!
**
** Notes on options for the various languages will be found in the
** Makefiles for those languages.  Some of those languages may require
** you to also change various limits limits like MASKBITS or the
** length parameters.
**
** A special note on the English language: because the British and
** American dialects use different spelling, you should usually select
** one or the other of these.  If you select both, the setting of
** MASTERHASH will determine which becomes the language linked to
** DEFHASH (which will usually be named english.hash).
*/
#ifndef LANGUAGES
#define LANGUAGES "{american,MASTERDICTS=american.med+,HASHFILES=americanmed+.hash,EXTRADICT=words}"
#endif /* LANGUAGES */

/*
** Master hash file for DEFHASH.  This is the name of a hash file
** built by a language Makefile.  It should be the most-popular hash
** file on your system, because it is the one that will be used by
** default.  It must be listed in LANGUAGES, above.
*/
#ifndef MASTERHASH
#define MASTERHASH	"americanmed+.hash"
#endif

/*
** Default native-language hash file.  This is the name given to the
** hash table that will be used if no language is specified to
** ispell.  It is a link to MASTERHASH, above.
*/
#ifndef DEFHASH
#define DEFHASH "english.hash"
#endif

/*
** Language tables for the default language.  This must be the name of
** the affix file that was used to generate the MASTERHASH/DEFHASH,
** above.
*/
#ifndef DEFLANG
#define DEFLANG	"english.aff"
#endif

/*
** Language to use for error messages.  If there are no messages in this
** language, English will be used instead.
*/
#ifndef MSGLANG
#define MSGLANG	"english"
#endif /* MSGLANG */

/*
** If your sort command accepts the -T switch to set temp file
** locations (try it out; it exists but is undocumented on some
** systems), make the following variable the null string.  Otherwise
** leave it as the sed script.
*/
#ifndef SORTTMP
#define SORTTMP	"-e '/!!SORTTMP!!/s/=.*$/=/'"
#endif

/*
** If your sort command accepts the -T switch (see above), make the
** following variable refer to a temporary directory with lots of
** space.  Otherwise make it the null string.
*/
#ifndef MAKE_SORTTMP
#define MAKE_SORTTMP "-T ${TMPDIR-/tmp}"
#endif

/*
** INSTALL program. Could be a copy program like cp or something fancier
** like /usr/ucb/install -c
*/
#ifndef INSTALL
#define INSTALL		"cp"
#endif

/*
** If your system has the rename(2) system call, define HAS_RENAME and
** ispell will use that call to rename backup files.  Otherwise, it
** will use link/unlink.  There is no harm in this except on MS-DOS,
** which doesn't support link/unlink.
*/
#ifndef HAS_RENAME
#undef HAS_RENAME
#endif /* HAS_RENAME */

/* Aliases for some routines */
#ifdef USG
#define bcopy(s, d, n)	(void) memcpy (d, s, n)
#define bzero(d, n)	(void) memset (d, 0, n)
#define index strchr
#define rindex strrchr
#endif

/* type given to signal() by signal.h */
#ifndef SIGNAL_TYPE
#define SIGNAL_TYPE void
#endif

/* environment variable for user's word list */
#ifndef PDICTVAR
#define PDICTVAR "WORDLIST"
#endif

/* prefix part of default word list */
#ifndef DEFPDICT
#define DEFPDICT ".ispell_"
#endif

/*
** suffix part of default word list
*/
#ifndef DEFPAFF
#define DEFPAFF "words"
#endif

/* old place to look for default word list */
#ifndef OLDPDICT
#define OLDPDICT ".ispell_"
#endif /* OLDPDICT */
#ifndef OLDPAFF
#define OLDPAFF "words"
#endif /* OLDPAFF */

/* environment variable for include file string */
#ifndef INCSTRVAR
#define INCSTRVAR "INCLUDE_STRING"
#endif

/* default include string */
#ifndef DEFINCSTR
#define DEFINCSTR "&Include_File&"
#endif

/* mktemp template for temporary file - MUST contain 6 consecutive X's */
#ifndef TEMPNAME
#define TEMPNAME "/tmp/ispellXXXXXX"
#endif

/*
** If REGEX_LOOKUP is undefined, the lookup command (L) will use the look(1)
** command (if available) or the egrep command.  If REGEX_LOOKUP is defined,
** the lookup command will use the internal dictionary and the
** regular-expression library (which you must supply separately.  There is
** a public-domain library available;  libraries are also distributed with
** both BSD and System V.
**
** The advantage of no REGEX_LOOKUP is that it is often much faster, especially
** if the look(1) command is available, that the words found are presented
** in alphabetical order, and that the list of words searched is larger.
** The advantage of REGEX_LOOKUP is that ispell doesn't need to spawn another
** program, and the list of words searched is exactly the list of (root) words
** that ispell will accept.  (However, note that words formed with affixes will
** not be found;  this can produce some artifacts.  For example, since
** "brother" can be formed as "broth+er", a lookup command might fail to
** find "brother.")
*/
#ifndef REGEX_LOOKUP
#undef REGEX_LOOKUP
#endif /* REGEX_LOOKUP */

/*
** Choose the proper type of regular-expression routines here.  BSD
** and public-domain systems have routines called re_comp and re_exec;
** System V uses regcmp and regex.
*/
#ifdef REGEX_LOOKUP
#ifndef REGCMP
#ifdef USG
#define REGCMP(str)		regcmp (str, (char *) 0)
#define REGEX(re, str, dummy)	regex (re, str, dummy, dummy, dummy, dummy, \
				    dummy, dummy, dummy, dummy, dummy, dummy)
#else /* USG */
#define REGCMP(str)		re_comp (str)
#define REGEX(re, str, dummy)	re_exec (str)
#endif /* USG */
#endif /* REGCMP */
#endif /* REGEX_LOOKUP */

/* look command (if look(1) MAY BE available - ignored if not) */
#ifndef REGEX_LOOKUP
#ifndef LOOK
#define	LOOK	"look -df"
#endif
#endif /* REGEX_LOOKUP */

/* path to egrep (use speeded up version if available) */
#ifndef EGREPCMD
#ifdef	USG
#define EGREPCMD "/bin/egrep"
#else
#define EGREPCMD "/usr/bin/egrep -i"
#endif
#endif

/* path to wordlist for Lookup command (typically /usr/dict/{words|web2}) */
/* note that /usr/dict/web2 is usually a bad idea due to obscure words */
#ifndef WORDS
#define WORDS	"/usr/dict/words"
#endif

/* buffer size to use for file names if not in sys/param.h */
#ifndef MAXPATHLEN
#define MAXPATHLEN 240
#endif

/* max file name length (will truncate to fit BAKEXT) if not in sys/param.h */
#ifndef MAXNAMLEN
#define MAXNAMLEN 14
#endif

/* define if you want .bak file names truncated to MAXNAMLEN characters */
#ifndef TRUNCATEBAK
#undef TRUNCATEBAK
#endif /* TRUNCATEBAK */

/* largest word accepted from a file by any input routine, plus one */
#ifndef	INPUTWORDLEN
#define INPUTWORDLEN 100
#endif

/* largest amount that a word might be extended by adding affixes */
#ifndef MAXAFFIXLEN
#define MAXAFFIXLEN 20
#endif

/*
** Define the following to suppress the 8-bit character feature.
** Unfortunately, this doesn't work as well as it might, because ispell
** only strips the 8th bit in some places.  For example, it calls strcmp()
** quite often without first stripping parity.  However, I really wonder
** about the utility of this option, so I haven't bothered to fix it.  If
** you think the stripping feature of NO8BIT is useful, you might let me
** (Geoff Kuenning) know.
**
** Nevertheless, NO8BIT is a useful option for other reasons.  If NO8BIT
** is defined, ispell will probably use 8-bit characters internally;
** this improves time efficiency and saves a small amount of space
** in the hash file.  Thus, I recommend you specify NO8BIT unless you
** actually expect to be spelling files which use a 256-character set.
*/ 
#ifndef NO8BIT
#undef NO8BIT
#endif /* NO8BIT */

/*
** Number of mask bits (affix flags) supported.  Must be 32, 64, 128, or
** 256.  If MASKBITS is 32 or 64, there are really only 26 or 58 flags
** available, respectively.  If it is 32, the flags are named with the
** 26 English uppercase letters;  lowercase will be converted to uppercase.
** If MASKBITS is 64, the 58 flags are named 'A' through 'z' in ASCII
** order, including the 6 special characters from 'Z' to 'a': "[\]^_`".
** If MASKBITS is 128 or 256, all the 7-bit or 8-bit characters,
** respectively, are theoretically available, though a few (newline, slash,
** null byte) are pretty hard to actually use successfully.
**
** Note that a number of non-English affix files depend on having a
** larger value for MASKBITS.  See the affix files for more
** information.
*/
#ifndef MASKBITS
#define MASKBITS	32
#endif

/*
** C type to use for masks.  This should be a type that the processor
** accesses efficiently.
**
** MASKTYPE_WIDTH must correctly reflect the number of bits in a
** MASKTYPE.  Unfortunately, it is also required to be a constant at
** preprocessor time, which means you can't use the sizeof operator to
** define it.
**
** Note that MASKTYPE *must* match MASKTYPE_WIDTH or you may get
** division-by-zero errors! 
*/
#ifndef MASKTYPE
#define MASKTYPE	long
#endif
#ifndef MASKTYPE_WIDTH
#define MASKTYPE_WIDTH	32
#endif
#if MASKBITS < MASKTYPE_WIDTH
#undef MASKBITS
#define MASKBITS	MASKTYPE_WIDTH
#endif /* MASKBITS < MASKTYPE_WIDTH */


/* maximum number of include files supported by xgets;  set to 0 to disable */
#ifndef MAXINCLUDEFILES
#define MAXINCLUDEFILES	5
#endif

/*
** Maximum hash table fullness percentage.  Larger numbers trade space
** for time.
**/
#ifndef MAXPCT
#define MAXPCT	70		/* Expand table when 70% full */
#endif

/*
** Maximum number of "string" characters that can be defined in a
** language (affix) file.  Don't forget that an upper/lower string
** character counts as two!
*/
#ifndef MAXSTRINGCHARS
#define MAXSTRINGCHARS 100
#endif /* MAXSTRINGCHARS */

/*
** Maximum length of a "string" character.  The default is appropriate for
** nroff-style characters starting with a backslash.
*/
#ifndef MAXSTRINGCHARLEN
#define MAXSTRINGCHARLEN 10
#endif /* MAXSTRINGCHARLEN */

/*
** the NOPARITY mask is applied to user input characters from the terminal
** in order to mask out the parity bit.
*/
#ifdef NO8BIT
#define NOPARITY 0x7f
#else
#define NOPARITY 0xff
#endif


/*
** the terminal mode for ispell, set to CBREAK or RAW
**
*/
#ifndef TERM_MODE
#define TERM_MODE	CBREAK
#endif

/*
** Define this if you want your columns of words to be of equal length.
** This will spread short word lists across the screen instead of down it.
*/
#ifndef EQUAL_COLUMNS
#undef EQUAL_COLUMNS
#endif /* EQUAL_COLUMNS */

/*
** This is the extension that will be added to backup files
*/
#ifndef	BAKEXT
#define	BAKEXT	".bak"
#endif

/*
** Define this if you want to suppress the capitalization-checking
** feature.  This will reduce the size of the hashed dictionary on
** most 16-bit and some 32-bit machines.  This option is not
** recommended.
*/
#ifndef NO_CAPITALIZATION_SUPPORT
#undef NO_CAPITALIZATION_SUPPORT
#endif /* NO_CAPITALIZATION_SUPPORT */

/*
** Define this if you want your personal dictionary sorted.  This may take
** a long time for very large dictionaries.  Dictionaries larger than
** SORTPERSONAL words will not be sorted.  Define SORTPERSONAL as zero
** to disable this feature.
*/
#ifndef SORTPERSONAL
#define SORTPERSONAL	1000
#endif

/*
** Define this if you want to use the shell for interpretation of commands
** issued via the "L" command, "^Z" under System V, and "!".  If this is
** not defined then a direct fork()/exec() will be used in place of the
** normal system().  This may speed up these operations greately on some
** systems.
*/
#ifndef USESH
#undef USESH
#endif /* USESH */

/*
** Maximum language-table search size.  Smaller numbers make ispell
** run faster, at the expense of more memory (the lowest reasonable value
** is 2).  If a given character appears in a significant position in
** more than MAXSEARCH suffixes, it will be given its own index table.
** If you change this, define INDEXDUMP in lookup.c to be sure your
** index table looks reasonable.
*/
#ifndef MAXSEARCH
#define MAXSEARCH 4
#endif

/*
** Define this if you want to be able to type any command at a "type space
** to continue" prompt.
*/
#ifndef COMMANDFORSPACE
#undef COMMANDFORSPACE
#endif /* COMMANDFORSPACE */

/*
** Memory-allocation increment.  Buildhash allocates memory in chunks
** of this size, and then subdivides it to get its storage.  This saves
** much malloc execution time.  A good number for this is the system
** page size less the malloc storage overhead.
**
** Define this to zero to revert to using malloc/realloc.  This is normally
** useful only on systems with limited memory.
*/
#ifndef MALLOC_INCREMENT
#define MALLOC_INCREMENT	(4096 - 8)
#endif

/*
** Maximum number of "hits" expected on a word.  This is basically the
** number of different ways different affixes can produce the same word.
** For example, with "english.aff", "brothers" can be produced 3 ways:
** "brothers," "brother+s", or "broth+ers".  If this is too low, no major
** harm will be done, but ispell may occasionally forget a capitalization.
*/
#ifndef MAX_HITS
#define MAX_HITS	10
#endif

/*
** Maximum number of capitalization variations expected in any word.
** Besides the obvious all-lower, all-upper, and capitalized versions,
** this includes followcase variants.  If this is too low, no real
** harm will be done, but ispell may occasionally fail to suggest a
** correct capitalization.
*/
#ifndef MAX_CAPS
#define MAX_CAPS	10
#endif /* MAX_CAPS */

/* Define this to ignore spelling check of entire LaTeX bibliography listings */
#ifndef IGNOREBIB
#undef IGNOREBIB
#endif

/*
** Default nroff and TeX special characters.  Normally, you won't want to
** change this;  instead you would override it in the language-definition
** file.
*/
#ifndef TEXSPECIAL
#define TEXSPECIAL	"()[]{}<>\\$*.%"
#endif

#ifndef NRSPECIAL
#define NRSPECIAL	"().\\*"
#endif

/*
** Defaults for certain command-line flags.
*/
#ifndef DEFNOBACKUPFLAG
#define DEFNOBACKUPFLAG	0		    /* Don't suppress backup file */
#endif
#ifndef DEFTEXFLAG
#define DEFTEXFLAG	0		    /* Default to nroff mode */
#endif

/*
** Define this if you want ispell to place a limitation on the maximum
** size of the screen.  On windowed workstations with very large windows,
** the size of the window can be too much of a good thing, forcing the
** user to look back and forth between the bottom and top of the screen.
** If MAX_SCREEN_SIZE is nonzero, screens larger than this will be treated
** as if they have only MAX_SCREEN_SIZE lines.  A good value for this
** variable is 24 or 30.  Define it as zero to suppress the feature.
*/
#ifndef MAX_SCREEN_SIZE
#define MAX_SCREEN_SIZE 0
#endif

/*
** The next three variables are used to provide a variable-size context
** display at the bottom of the screen.  Normally, the user will see
** a number of lines equal to CONTEXTPCT of his screen, rounded down
** (thus, with CONTEXTPCT == 10, a 24-line screen will produce two lines
** of context).  The context will never be greater than MAXCONTEXT or
** less than MINCONTEXT.  To disable this feature entirely, set MAXCONTEXT
** and MINCONTEXT to the same value.  To round context percentages up,
** define CONTEXTROUNDUP.
**
** Warning: don't set MAXCONTEXT ridiculously large.  There is a
** static buffer of size MAXCONTEXT*BUFSIZ; since BUFSIZ is frequently
** 1K or larger, this can create a remarkably large executable.
*/
#ifndef CONTEXTPCT
#define CONTEXTPCT	10	/* Use 10% of the screen for context */
#endif
#ifndef MINCONTEXT
#define MINCONTEXT	2	/* Always show at least 2 lines of context */
#endif
#ifndef MAXCONTEXT
#define MAXCONTEXT	10	/* Never show more than 10 lines of context */
#endif
#ifndef CONTEXTROUNDUP
#undef CONTEXTROUNDUP		/* Don't round context up */
#endif

/*
** Define this if you want the context lines to be displayed at the
** bottom of the screen, the way they used to be, rather than at the top.
*/
#ifndef BOTTOMCONTEXT
#undef BOTTOMCONTEXT
#endif /* BOTTOMCONTEXT */

/*
** Define this if you want the "mini-menu," which gives the most important
** options at the bottom of the screen, to be the default (in any case, it
** can be controlled with the "-M" switch).
*/
#ifndef MINIMENU
#undef MINIMENU
#endif

/*
** You might want to change this to zero if your users want to check
** single-letter words against the dictionary.  However, you should try
** some sample runs using the -W switch before you try it out;  you'd
** be surprised how many single letters appear in documents.  If you increase
** MINWORD beyond 1, don't say I didn't warn you that it was a bad idea.
*/
#ifndef MINWORD
#define MINWORD		1	/* Words this short and shorter are always ok */
#endif

/*
** ANSI C compilers are supposed to provide an include file,
** "stdlib.h", which gives function prototypes for all library
** routines.  Define NO_STDLIB_H if you have a compiler that claims to
** be ANSI, but doesn't provide this include file.
*/
#ifndef NO_STDLIB_H
#ifndef __STDC__
#define NO_STDLIB_H
#endif /* __STDC__ */
#endif /* NO_STDLIB_H */

/*
** The following define is used by the ispell developer to help
** double-check the software.  Leave it undefined on other systems
** unless you are especially fond of warning messages, or are pursuing
** an inexplicable bug that might be related to a type mismatch.
*/
#ifndef GENERATE_LIBARARY_PROTOS
#undef GENERATE_LIBARARY_PROTOS
#endif /* GENERATE_LIBARARY_PROTOS */

/*
** Symbols below this point are generally intended to cater to
** idiosyncracies of specific machines and operating systems.
**
** MS-DOS users should also define HAS_RENAME, above, if appropriate.
**
** Define PIECEMEAL_HASH_WRITES if your system can't handle huge write
** operations.  This is known to be a problem on some MS-DOS systems.
*/
#ifndef PIECEMEAL_HASH_WRITES
#undef PIECEMEAL_HASH_WRITES
#endif /* PIECEMEAL_HASH_WRITES */

/*
** Redefine GETKEYSTROKE() to getkey() on some MS-DOS systems where
** getchar() doesn't operate properly in raw mode.
*/
#ifndef GETKEYSTROKE
#define GETKEYSTROKE()	getchar ()
#endif /* GETKEYSTROKE */

/*
** Define MSDOS_BINARY_OPEN to 0x8000 on MS-DOS systems.  This can be
** done by adding "#include fcntl.h" to your local.h file.
*/
#ifndef MSDOS_BINARY_OPEN
#ifdef O_BINARY
#define MSDOS_BINARY_OPEN   O_BINARY
#else /* O_BINARY */
#define MSDOS_BINARY_OPEN   0
#endif /* O_BINARY */
#endif /* MSDOS_BINARY_OPEN */

/*
** Environment variable to use to locate the home directory.  On DOS
** systems you might want to set this to ISPELL_HOME to avoid
** conflicts with other programs that look for a HOME environment
** variable; on all other systems it should be just HOME.
*/
#ifndef HOME
#define HOME	"HOME"
#endif /* HOME */

/*
** On MS-DOS systems, define PDICTHOME to be the name of a directory
** to be used to contain the personal dictionary (.ispell_english,
** etc.).  On other systems, you can leave it undefined.
*/
#ifndef PDICTHOME
#undef PDICTHOME
#endif /* PDICTHOME */

/*
** On MS-DOS systems, you can rename the following variables so that
** ispell's files have 3-character suffixes.  Note that, if you do
** this, you will have to redefine any variable above that
** incorporates one of the suffixes.
*/
#ifndef HASHSUFFIX
#define HASHSUFFIX	".hash"
#endif /* HASHSUFFIX */
#ifndef STATSUFFIX
#define STATSUFFIX	".stat"
#endif /* STATSUFFIX */
#ifndef COUNTSUFFIX
#define COUNTSUFFIX	".cnt"
#endif /* COUNTSUFFIX */

/* AUTOMATICALLY-GENERATED SYMBOLS */
#define SIGNAL_TYPE_STRING "void"
#define MASKTYPE_STRING "long"
