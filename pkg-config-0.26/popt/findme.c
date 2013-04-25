/* 
 * Copyright (C) 1998 Red Hat Inc.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#else
# ifdef _AIX
#  pragma alloca
# endif
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef __NeXT
/* access macros are not declared in non posix mode in unistd.h -
 don't try to use posix on NeXTstep 3.3 ! */ 
#include <libc.h>
#endif

#include "findme.h"

#if !defined(X_OK)
#define X_OK 1
#endif

char * findProgramPath(char * argv0) {
    char * path = getenv("PATH");
    char * pathbuf;
    char * start, * chptr;
    char * buf;

    /* If there is a / in the argv[0], it has to be an absolute
       path */
    if (strchr(argv0, '/'))
	return strdup(argv0);

    if (!path) return NULL;

    start = pathbuf = alloca(strlen(path) + 1);
    buf = malloc(strlen(path) + strlen(argv0) + 2);
    strcpy(pathbuf, path);

    chptr = NULL;
    do {
	if ((chptr = strchr(start, ':')))
	    *chptr = '\0';
	sprintf(buf, "%s/%s", start, argv0);

	if (!access(buf, X_OK))
	    return buf;

	if (chptr) 
	    start = chptr + 1;
	else
	    start = NULL;
    } while (start && *start);

    free(buf);

    return NULL;
}
