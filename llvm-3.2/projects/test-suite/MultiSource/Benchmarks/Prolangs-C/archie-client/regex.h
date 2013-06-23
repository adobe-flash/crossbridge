/*
 * regex.h : External defs for Ozan Yigit's regex functions, for systems
 *	that don't have them builtin. See regex.c for copyright and other
 *	details.
 *
 * Note that this file can be included even if we're linking against the
 * system routines, since the interface is (deliberately) identical.
 *
 * George Ferguson, ferguson@cs.rochester.edu, 11 Sep 1991.
 */

#if defined(AUX) || defined(USG)
/* Let them use ours if they wish.  */
# ifndef NOREGEX
extern char *regcmp();
extern char *regex();
#define re_comp regcmp
#define re_exec regex
# endif
#else
extern char *re_comp();
extern int re_exec();
#endif
