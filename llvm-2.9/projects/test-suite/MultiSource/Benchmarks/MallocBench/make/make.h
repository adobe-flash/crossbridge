/* Copyright (C) 1988-1991 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>

#ifndef	isblank
#define	isblank(c)	((c) == ' ' || (c) == '\t')
#endif

#if	!defined(NSIG) && defined(_NSIG)
#define	NSIG	_NSIG
#endif

#ifdef	__STDC__
#define	SIGHANDLER	void *
#else
#define	SIGHANDLER	int (*)()
#endif
#define	SIGNAL(sig, handler) \
  ((SIGHANDLER) signal((sig), (SIGHANDLER) (handler)))

#ifndef	sigmask
#define	sigmask(sig)	(1 << ((sig) - 1))
#endif

#if	defined(POSIX) || defined(__GNU_LIBRARY__)
#include <limits.h>
#ifndef	PATH_MAX
#define	GET_PATH_MAX	pathconf ("/", _PC_PATH_MAX)
#endif
#else	/* Not POSIX.  */
#include <sys/param.h>
#ifndef	PATH_MAX
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif	/* No MAXPATHLEN.  */
#define	PATH_MAX	MAXPATHLEN
#endif	/* No PATH_MAX.  */
#endif	/* POSIX.  */
#ifdef	PATH_MAX
#define	PATH_VAR(var)	char var[PATH_MAX]
#else
#define	PATH_VAR(var)	char *var = (char *) alloca (GET_PATH_MAX)
#endif

#ifdef	uts
#ifdef	S_ISREG
#undef	S_ISREG
#endif
#ifdef	S_ISDIR
#undef	S_ISDIR
#endif
#endif	/* uts.  */

#ifndef	S_ISREG
#define	S_ISREG(mode)	(((mode) & S_IFMT) == S_IFREG)
#endif
#ifndef	S_ISDIR
#define	S_ISDIR(mode)	(((mode) & S_IFMT) == S_IFDIR)
#endif


#if	(defined(STDC_HEADERS) || defined(__GNU_LIBRARY__) || \
	   defined(POSIX) || defined(__FreeBSD__) || defined(__APPLE__) \
     defined(__OpenBSD__))
#include <stdlib.h>
#include <string.h>
#define	ANSI_STRING
#else	/* No standard headers.  */

#ifdef	USG

#include <string.h>
#include <memory.h>
#define	ANSI_STRING

#else	/* Not USG.  */

#ifdef	NeXT

#include <string.h>

#else	/* Not NeXT.  */

#include <strings.h>

#ifndef	bcmp
extern int bcmp ();
#endif
#ifndef	bzero
extern void bzero ();
#endif
#ifndef	bcopy
extern void bcopy ();
#endif

#endif	/* NeXT. */

#endif	/* USG.  */

extern char *malloc (), *realloc ();
extern void free ();

#endif	/* Standard headers.  */

#ifdef	ANSI_STRING
#define	index(s, c)	strchr((s), (c))
#define	rindex(s, c)	strrchr((s), (c))

#define bcmp(s1, s2, n)	memcmp ((s1), (s2), (n))
#define bzero(s, n)	memset ((s), 0, (n))
#define bcopy(s, d, n)	memcpy ((d), (s), (n))
#endif
#undef	ANSI_STRING


#ifdef	__GNUC__
#undef	alloca
#define	alloca(n)	__builtin_alloca (n)
#else	/* Not GCC.  */
#ifdef	sparc
#include <alloca.h>
#else	/* Not sparc.  */
extern char *alloca ();
#endif	/* sparc.  */
#endif	/* GCC.  */

#ifndef	iAPX286
#define streq(a, b) \
  ((a) == (b) || \
   (*(a) == *(b) && (*(a) == '\0' || !strcmp ((a) + 1, (b) + 1))))
#else
/* Buggy compiler can't handle this.  */
#define streq(a, b) (strcmp ((a), (b)) == 0)
#endif

/* Add to VAR the hashing value of C, one character in a name.  */
#define	HASH(var, c) \
  ((var += (c)), (var = ((var) << 7) + ((var) >> 20)))

#if defined(__GNUC__) || defined(ENUM_BITFIELDS)
#define	ENUM_BITFIELD(bits)	:bits
#else
#define	ENUM_BITFIELD(bits)
#endif

extern void die ();
extern void message (), fatal (), error ();
extern void makefile_error (), makefile_fatal ();
extern void pfatal_with_name (), perror_with_name ();
extern char *savestring (), *concat ();
extern char *xmalloc (), *xrealloc ();
extern char *find_next_token (), *next_token (), *end_of_token ();
extern void collapse_continuations (), remove_comments ();
extern char *sindex (), *lindex ();
extern int alpha_compare ();
extern void print_spaces ();
extern struct dep *copy_dep_chain ();
extern char *find_percent ();

#ifndef	NO_ARCHIVES
extern int ar_name ();
extern void ar_parse_name ();
extern int ar_touch ();
extern time_t ar_member_date ();
#endif

extern void dir_load ();
extern int dir_file_exists_p (), file_exists_p (), file_impossible_p ();
extern void file_impossible ();
extern char *dir_name ();

extern void define_default_variables ();
extern void set_default_suffixes (), install_default_implicit_rules ();
extern void convert_to_pattern (), count_implicit_rule_limits ();
extern void create_pattern_rule ();

extern void build_vpath_lists (), construct_vpath_list ();
extern int vpath_search ();

extern void construct_include_path ();
extern void uniquize_deps ();

extern int update_goal_chain ();
extern void notice_finished_file ();

extern void user_access (), make_access (), child_access ();


#if	defined(USG) && !defined(HAVE_VFORK)
#define	vfork	fork
#define	VFORK_NAME	"fork"
#else	/* Have vfork or not USG.  */
#define	VFORK_NAME	"vfork"
#endif	/* USG and don't have vfork.  */

#if	defined(__GNU_LIBRARY__) || defined(POSIX) || defined(__CYGWIN__) || \
    defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)

#include <unistd.h>
#include <signal.h>

#else

extern int kill ();
extern void abort (), exit ();
extern int unlink (), stat ();
extern void qsort ();
extern int atoi ();
extern int pipe (), close (), read (), write (), open ();
extern long int lseek ();
extern char *ctime ();
#endif	/* GNU C library or POSIX.  */

#if	defined(USG) || defined(POSIX) && !defined(__GNU_LIBRARY__)
extern char *getcwd ();
#ifdef	PATH_MAX
#define	getwd(buf)	getcwd (buf, PATH_MAX - 2)
#else
#define	getwd(buf)	getcwd (buf, GET_PATH_MAX - 2)
#endif
#else	/* USG or POSIX and not GNU C library.  */
extern char *getwd ();
#endif	/* Not USG or POSIX, or maybe GNU C library.  */

#if !defined(__GNU_LIBRARY__) && (!defined(vfork) || !defined(POSIX))
#ifdef	POSIX
extern pid_t vfork ();
#else
extern int vfork ();
#endif
#endif

extern char **environ;

extern char *reading_filename;
extern unsigned int *reading_lineno_ptr;

extern int just_print_flag, silent_flag, ignore_errors_flag, keep_going_flag;
extern int debug_flag, print_data_base_flag, question_flag, touch_flag;
extern int env_overrides, no_builtin_rules_flag, print_version_flag;
extern int print_directory_flag;

extern unsigned int job_slots;
extern double max_load_average;

extern char *program;

extern unsigned int makelevel;


#define DEBUGPR(msg)							\
  if (debug_flag) { print_spaces (depth); printf (msg, file->name);	\
		    fflush (stdout);  } else
