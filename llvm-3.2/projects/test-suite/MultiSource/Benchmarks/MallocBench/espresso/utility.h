#ifndef UTILITY_H
#define UTILITY_H

/*
 *  assumes the memory manager is libmm.a
 *	- allows malloc(0) or realloc(obj, 0)
 *	- catches out of memory (and calls MMout_of_memory())
 *	- catch free(0) and realloc(0, size) in the macros
 */
#define NIL(type)		((type *) 0)

#ifdef BWGC 
#define ALLOC(type, num)	\
    ((type *) gc_malloc(sizeof(type) * ((num)==0?1:(num))))
#define REALLOC(type, obj, num)	\
    (obj) ? ((type *) gc_realloc((char *) obj, sizeof(type) * ((num)==0?1:(num)))) : \
	    ((type *) gc_malloc(sizeof(type) * ((num)==0?1:(num))))
#else
#define ALLOC(type, num)	\
    ((type *) malloc(sizeof(type) * (num)))
#define REALLOC(type, obj, num)	\
    (obj) ? ((type *) realloc((char *) obj, sizeof(type) * (num))) : \
	    ((type *) malloc(sizeof(type) * (num)))
#endif BWGC

#ifdef IGNOREFREE
#define FREE(obj)		\
    {};
#else
#define FREE(obj)		\
    if ((obj)) { (void) free((char *) (obj)); (obj) = 0; }
#endif IGNOREFREE

#include "ansi.h"

EXTERN long  util_cpu_time
	NULLARGS;
EXTERN char *util_path_search
	ARGS((char *program));
EXTERN char *util_file_search
	ARGS((char *file, char *path, char *mode));
EXTERN int   util_pipefork
	ARGS((char **argv, FILE **toCommand, FILE **fromCommand, int *pid));
EXTERN int   util_csystem
	ARGS((char *command));
EXTERN char *util_print_time
	ARGS((long t));
EXTERN char *util_strsav
	ARGS((char *ptr));
EXTERN char *util_tilde_expand
	ARGS((char *filename));
EXTERN char *util_tilde_compress
	ARGS((char *filename));
EXTERN void util_register_user
	ARGS((char *user, char *directory));

#ifndef NIL_FN
#define NIL_FN(type) ((type (*)()) 0)
#endif /* NIL_FN */

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif /* MAX */
#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif /* MIN */
#ifndef ABS
#define ABS(a)		((a) > 0 ? (a) : -(a))
#endif /* ABS */


#ifdef lint
#undef ALLOC			/* allow for lint -h flag */
#undef REALLOC
#define ALLOC(type, num)	(((type *) 0) + (num))
#define REALLOC(type, obj, num)	((obj) + (num))
#endif /* lint */

#endif

