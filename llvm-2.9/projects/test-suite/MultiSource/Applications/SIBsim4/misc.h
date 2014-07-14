/* $Id: misc.h 34335 2007-02-15 23:46:09Z nicholas $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2004 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 * Copyright (C) 1998-2001  Scott Schwartz.
 */

#ifndef SIM_MISC_H
#define SIM_MISC_H

#ifdef __GNUC__
void
fatal(const char *fmt, ...)
     __attribute__ ((format (printf, 1, 2) , __noreturn__));
#else
void
fatal(const char *fmt, ...);
#endif

void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
void *xrealloc(void *ptr, size_t size);

#endif
