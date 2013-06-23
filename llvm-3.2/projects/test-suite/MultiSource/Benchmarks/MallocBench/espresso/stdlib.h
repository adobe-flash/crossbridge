/* ANSI Compatible stdlib.h stub */

#ifndef __cplusplus

#include <sys/types.h>

#ifndef ultrix4
extern double atof(const char *);
extern int atoi(const char *);
extern long atol(const char *);
extern void abort(void);
extern void *calloc(size_t, size_t);
extern void exit(int);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern char *getenv(const char *);
#else
extern double atof(const char *);
extern int atoi(char *);
extern long atol(char *);
extern void abort(void);
extern void *calloc(size_t, size_t);
extern void exit(int);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern char *getenv(char *);
#endif

/* should be in stdio.h */
extern void perror(const char *);

#ifdef LINT
#undef putc
#endif

#endif
