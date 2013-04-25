/*
Copyright (c) 1998  Red Hat Software
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "popt.h"

int poptParseArgvString(char * s, int * argcPtr, char *** argvPtr) {
    char * buf = strcpy(alloca(strlen(s) + 1), s);
    char * bufStart = buf;
    char * src, * dst;
    char quote = '\0';
    int argvAlloced = 5;
    char ** argv = malloc(sizeof(*argv) * argvAlloced);
    char ** argv2;
    int argc = 0;
    int i;

    src = s;
    dst = buf;
    argv[argc] = buf;

    memset(buf, '\0', strlen(s) + 1);

    while (*src) {
	if (quote == *src) {
	    quote = '\0';
	} else if (quote) {
	    if (*src == '\\') {
		src++;
		if (!*src) {
		    free(argv);
		    return POPT_ERROR_BADQUOTE;
		}
		if (*src != quote) *buf++ = '\\';
	    }
	    *buf++ = *src;
	} else if (isspace(*src)) {
	    if (*argv[argc]) {
		buf++, argc++;
		if (argc == argvAlloced) {
		    argvAlloced += 5;
		    argv = realloc(argv, sizeof(*argv) * argvAlloced);
		}
		argv[argc] = buf;
	    }
	} else switch (*src) {
	  case '"':
	  case '\'':
	    quote = *src;
	    break;
	  case '\\':
	    src++;
	    if (!*src) {
		free(argv);
		return POPT_ERROR_BADQUOTE;
	    }
	    /* fallthrough */
	  default:
	    *buf++ = *src;
	}

	src++;
    }

    if (strlen(argv[argc])) {
	argc++, buf++;
    }

    dst = malloc(argc * sizeof(*argv) + (buf - bufStart));
    argv2 = (void *) dst;
    dst += argc * sizeof(*argv);
    memcpy(argv2, argv, argc * sizeof(*argv));
    memcpy(dst, bufStart, buf - bufStart);

    for (i = 0; i < argc; i++) {
	argv2[i] = dst + (argv[i] - bufStart);
    }

    free(argv);

    *argvPtr = argv2;
    *argcPtr = argc;

    return 0;
}
