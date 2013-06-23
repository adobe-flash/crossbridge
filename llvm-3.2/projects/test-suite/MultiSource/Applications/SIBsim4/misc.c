/* $Id: misc.c 34335 2007-02-15 23:46:09Z nicholas $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2005 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 * Copyright (C) 1998-2001  Scott Schwartz.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "misc.h"

/* format message, print it, and die */
void
fatal(const char *fmt, ...)
{
  extern char *argv0;
  extern char dna_seq_head[256];
  extern char rna_seq_head[256];
  va_list ap;
  va_start(ap, fmt);
  fflush(stdout);
  if (argv0) {
    char *p = strrchr(argv0, '/');
    fprintf(stderr, "%s: ", p ? p+1 : argv0);
  }
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n while processing:\n%.256s\n%.256s\n",
	  dna_seq_head, rna_seq_head);
#ifdef DEBUG
  abort();
#else
  exit(1);
#endif
}

void *
xmalloc(size_t size)
{
  void *res = malloc(size);
  if (res == NULL)
    fatal("malloc of %zd failed: %s (%d)\n", size, strerror(errno),
	  errno);
  return res;
}

void *
xcalloc(size_t nmemb, size_t size)
{
  void *res = calloc(nmemb, size);
  if (res == NULL)
    fatal("calloc of %zd, %zd failed: %s (%d)\n", nmemb, size,
	  strerror(errno), errno);
  return res;
}

void *
xrealloc(void *ptr, size_t size)
{
  void *res = realloc(ptr, size);
  if (res == NULL)
    fatal("realloc of %p to %zd failed: %s (%d)\n", ptr, size,
	  strerror(errno), errno);
  return res;
}
