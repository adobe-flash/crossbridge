/*
 * $Id: proto.h 33046 2007-01-09 23:57:19Z lattner $
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
 *
 */

/*
 * $Log$
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:59:04  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.15  1994/10/25  05:46:38  geoff
 * Protoize bzero the way 4.1.1 does it (which I hope is the standard).
 *
 * Revision 1.14  1994/05/24  06:23:10  geoff
 * Make cap_ok a global routine.
 *
 * Revision 1.13  1994/05/17  06:44:20  geoff
 * Add the new arguments to chk_aff, good, and compoundgood.
 *
 * Revision 1.12  1994/03/16  03:49:15  geoff
 * Add an ifdef so that there won't be a conflict with the definition of
 * free() on braindamaged Sun systems.
 *
 * Revision 1.11  1994/02/14  00:34:55  geoff
 * Add new arguments to the prototype for correct().
 *
 * Revision 1.10  1994/02/08  05:45:34  geoff
 * Don't undef P unless we're going to redefine it
 *
 * Revision 1.9  1994/02/07  08:10:47  geoff
 * Add the GENERATE_LIBRARY_PROTOS option.  Put the definitions of
 * index/rindex back the way they were, because that's what's needed on
 * my system (sigh).
 *
 * Revision 1.8  1994/02/07  05:45:25  geoff
 * Change the second parameter of index/rindex to be a char
 *
 * Revision 1.7  1994/01/25  07:12:05  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

extern int	addvheader P ((struct dent * ent));
extern void	askmode P ((void));
extern void	backup P ((void));
#ifndef NO_CAPITALIZATION_SUPPORT
extern int	cap_ok P ((ichar_t * word, struct success * hit, int len));
#endif /* NO_CAPITALIZATION_SUPPORT */
extern int	casecmp P ((char * a, char * b, int canonical));
extern void	chupcase P ((char * s));
extern void	checkfile P ((void));
extern void	checkline P ((FILE * ofile));
extern void	chk_aff P ((ichar_t * word, ichar_t * ucword, int len,
		  int ignoreflagbits, int allhits, int pfxopts, int sfxopts));
extern int	combinecaps P ((struct dent * hdr, struct dent * newent));
extern int	compoundgood P ((ichar_t * word, int pfxopts));
extern void	copyout P ((char ** cc, int cnt));
extern void	correct P ((char * ctok, int ctokl, ichar_t * itok, int itokl,
		  char ** curchar));
extern char *	do_regex_lookup P ((char * expr, int whence));
extern SIGNAL_TYPE done P ((int));
extern void	dumpmode P ((void));
extern void	erase P ((void));
extern int	expand_pre P ((char * croot, ichar_t * rootword,
		  MASKTYPE mask[], int option, char *extra));
extern int	expand_suf P ((char * croot, ichar_t * rootword,
		  MASKTYPE mask[], int crossonly, int option, char * extra));
extern int	findfiletype P ((char * name, int searchnames,
		  int * deformatter));
extern void	flagpr P ((ichar_t * word, int preflag, int prestrip,
		  int preadd, int sufflag, int sufadd));
extern void	givehelp P ((int interactive));
extern int	good P ((ichar_t * word, int ignoreflagbits, int allhits,
		  int pfxopts, int sfxopts));
extern int	hash P ((ichar_t * word, int hashtablesize));
#ifndef ICHAR_IS_CHAR
extern int	icharcmp P ((ichar_t * s1, ichar_t * s2));
extern ichar_t * icharcpy P ((ichar_t * out, ichar_t * in));
extern int	icharlen P ((ichar_t * str));
extern int	icharncmp P ((ichar_t * s1, ichar_t * s2, int n));
#endif /* ICHAR_IS_CHAR */
extern int	ichartostr P ((char * out, ichar_t * in, int outlen,
		  int canonical));
extern char *	ichartosstr P ((ichar_t * in, int canonical));
extern int	ins_root_cap P ((ichar_t * word, ichar_t * pattern,
		  int prestrip, int preadd, int sufstrip, int sufadd,
		  struct dent * firstdent, struct flagent * pfxent,
		  struct flagent * sufent));
extern void	inverse P ((void));
extern int	linit P ((void));
extern struct dent * lookup P ((ichar_t * word, int dotree));
extern void	lowcase P ((ichar_t * string));
extern int	makedent P ((char * lbuf, int lbuflen, struct dent * d));
extern void	makepossibilities P ((ichar_t * word));
extern void	move P ((int row, int col));
extern void	normal P ((void));
extern char *	printichar P ((int in));
#ifdef USESH
extern int	shellescape P ((char * buf));
extern void	shescape P ((char * buf));
#else /* USESH */
#ifndef REGEX_LOOKUP
extern int	shellescape P ((char * buf));
#endif /* REGEX_LOOKUP */
#endif /* USESH */
extern char *	skipoverword P ((char * bufp));
extern void	stop P ((void));
extern int	stringcharlen P ((char * bufp, int canonical));
extern int	strtoichar P ((ichar_t * out, char * in, int outlen,
		  int canonical));
extern ichar_t * strtosichar P ((char * in, int canonical));
extern void	terminit P ((void));
extern void	toutent P ((FILE * outfile, struct dent * hent,
		  int onlykeep));
extern void	treeinit P ((char * persdict, char * LibDict));
extern void	treeinsert P ((char * word, int wordlen, int keep));
extern struct dent * treelookup P ((ichar_t * word));
extern void	treeoutput P ((void));
extern void	upcase P ((ichar_t * string));
#ifndef NO_CAPITALIZATION_SUPPORT
extern long	whatcap P ((ichar_t * word));
#endif
extern char *	xgets P ((char * string, int size, FILE * stream));
extern void	yyinit P ((void));
extern int	yyopen P ((char * file));
extern int	yyparse P ((void));

extern void	myfree P ((VOID * area));
extern VOID *	mymalloc P ((unsigned int));
extern VOID *	myrealloc P ((VOID * area, unsigned int size,
		  unsigned int oldsize));

/*
 * C library functions.  If possible, we get these from stdlib.h.
 *
 * Even if stdlib.h doesn't exist, we don't generate proper prototypes
 * on most systems.  This protects us against minor differences in
 * declarations that break the compilation unnecessarily.
 * GENERATE_LIBRARY_PROTOS is mostly for the benefit of the ispell
 * developer.
 */
#ifndef GENERATE_LIBRARY_PROTOS
#undef P
#define P(x)	()
#endif /* GENERATE_LIBRARY_PROTOS */

#ifdef NO_STDLIB_H
extern int	access P ((const char * file, int mode));
extern int	atoi P ((const char * string));
#ifndef USG
extern VOID *	bcopy P ((const VOID * src, VOID * dest, unsigned int size));
extern void	bzero P ((VOID * dest, int size));
#endif /* USG */
extern VOID *	calloc P ((unsigned int nelems, unsigned int elemsize));
#ifdef _POSIX_SOURCE
extern int	chmod P ((const char * file, unsigned int mode));
#else /* _POSIX_SOURCE */
extern int	chmod P ((const char * file, unsigned long mode));
#endif /* POSIX_SOURCE */
extern int	close P ((int fd));
extern int	creat P ((const char * file, int mode));
extern int	execvp P ((const char * name, const char * argv[]));
extern void	_exit P ((int status));
extern void	exit P ((int status));
extern char *	fgets P ((char * string, int size, FILE * stream));
extern int	fork P ((void));
#ifdef __STDC__
/*
 * Some flaming cretin at Sun decided that free() should be declared
 * as returning an int in /usr/include/malloc.h, so the following
 * declaration causes a conflict.  Fortunately, it doesn't really do a
 * lot of harm to leave it undeclared, since (a) we always properly
 * ignore the return value and (b) any machine that really needs
 * special code to handle ignoring the return value is likely to also
 * provide a correct declaration.
 *
 * (Why is this ifdef'ed on __STDC__?  Because I want it to be correct
 * on my development machine, so I can catch lint problems.)
 *
 * A pox on those who violate long-established standards!
 */
extern void	free P ((VOID * area));
#endif /* __STDC__ */
extern char *	getenv P ((const char * varname));
extern int	ioctl P ((int fd, int func, char * arg));
extern int	kill P ((int pid, int sig));
extern int	link P ((const char * existing, const char * new));
extern long	lseek P ((int fd, long offset, int whence));
extern VOID *	malloc P ((unsigned int size));
#ifdef USG
extern VOID *	memcpy P ((VOID * dest, const VOID * src));
extern VOID *	memset P ((VOID * dest, int val, unsigned int len));
#endif /* USG */
extern char *	mktemp P ((char * prototype));
extern int	open P ((const char * file, int mode));
extern void	perror P ((const char * msg));
extern void	qsort P ((VOID * array, unsigned int nelems,
		  unsigned int elemsize,
		  int (*cmp) (const VOID * a, const VOID * b)));
extern int	read P ((int fd, VOID * buf, unsigned int n));
extern VOID *	realloc P ((VOID * area, unsigned int size));
extern unsigned int
		sleep P ((unsigned int));
extern char *	strcat P ((char * dest, const char * src));
#ifdef USG
extern char *	strchr P ((const char * string, int ch));
#endif /* USG */
extern int	strcmp P ((const char * s1, const char * s2));
extern char *	strcpy P ((char * dest, const char * src));
extern unsigned	int
		strlen P ((const char * str));
extern int	strncmp P ((const char * s1, const char * s2,
		  unsigned int len));
#ifdef USG
extern char *	strrchr P ((const char * string, int ch));
#endif /* USG */
extern int	system P ((const char * command));
extern int	unlink P ((const char * file));
extern int	wait P ((int * statusp));
#else /* NO_STDLIB_H */
#include <stdlib.h>
#include <string.h>
#endif /* NO_STDLIB_H */

#ifndef USG
extern char *	index P ((const char * string, int ch));
extern char *	rindex P ((const char * string, int ch));
#endif /* USG */
#ifdef REGEX_LOOKUP
#ifdef USG
extern char *	regcmp P ((const char * expr, const char * terminator, ...));
extern char *	regex P ((const char * pat, const char * subject, ...));
#else /* USG */
extern char *	re_comp P ((const char * expr));
extern int *	re_exec P ((const char * pat));
#endif /* USG */
#endif /* REGEX_LOOKUP */
extern int	tgetent P ((char * buf, const char * termname));
extern int	tgetnum P ((const char * id));
extern char *	tgetstr P ((const char * id, char ** area));
extern char *	tgoto P ((const char * cm, int col, int row));
extern char *	tputs P ((const char * str, int pad, int (*func) (int ch)));

#ifndef GENERATE_LIBRARY_PROTOS
#ifdef __STDC__
#undef P
#define P(x)	x
#endif /* __STDC__ */
#endif /* GENERATE_LIBRARY_PROTOS */
