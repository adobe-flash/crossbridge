/*
 * Miscellaneous system dependencies.
 *
 * I kept the name pmachine.h because it was already in all of the files...this
 * barely resembles the pmachine.h that comes with the real Prospero, tho.
 *
 * v1.2.1 - 12/13/91 (bpk) - added 3b2
 * v1.2.0 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 */

#ifdef u3b2
# define USG
# define NOREGEX
# define MAXPATHLEN 1024	/* There's no maxpathlen in any 3b2 .h file.  */
#endif

#ifdef hpux
# ifndef bcopy
#  define FUNCS			/* HP/UX 8.0 has the fns.  */
# endif
# define NOREGEX
# define NEED_STRING_H
#endif

#if defined(USG) || defined(SYSV)
# define FUNCS
#endif

#ifdef ISC
# define FUNCS
# define STRSPN
# define NOREGEX
#endif

#ifdef CUTCP
# define FUNCS
# define NOREGEX
# define NEED_STRING_H
# define SELECTARG int
# define MSDOS
#endif

#ifdef OS2
# define NOREGEX
# include <pctcp.h>
#endif
#ifdef MSDOS
# define USG
# define NOREGEX
# include <string.h>
# include <stdlib.h>
#endif

#ifdef _AIX
# ifdef u370
#  define FUNCS
# endif /* AIX/370 */
# define _NONSTD_TYPES
# define _BSD_INCLUDES
# define NEED_STRING_H
# define NEED_SELECT_H
# define NEED_TIME_H
#endif

/* ==== */
#ifdef FUNCS
# define index		strchr
/* According to mycroft. */
# ifdef _IBMR2
char *strchr();
# endif
# define rindex		strrchr
# ifndef AUX
#  define bcopy(a,b,n)	memcpy(b,a,n)
#  define bzero(a,n)	memset(a,0,n)
# ifdef _IBMR2
char *memset();
# endif
# endif
#endif

#if defined(_IBMR2) || defined(_BULL_SOURCE)
# define NEED_SELECT_H
#endif
#if defined(USG) || defined(UTS)
# define NEED_STRING_H
#endif
#if defined(USG) || defined(UTS) || defined(AUX)
# define NEED_TIME_H
#endif

#ifdef VMS
/* Get the system status stuff.  */
# include <ssdef.h>
#endif /* VMS */

/*
 * FD_SET: lib/pfs/dirsend.c, user/vget/ftp.c
 */
#ifndef CUTCP

#define SELECTARG fd_set
#if !defined(FD_SET) && !defined(VMS) && !defined(NEED_SELECT_H)
#define	FD_SETSIZE	32
#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif

#endif /* not CUTCP */
