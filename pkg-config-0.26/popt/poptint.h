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

#ifndef H_POPTINT
#define H_POPTINT

struct optionStackEntry {
    int argc;
    char ** argv;
    int next;
    char * nextArg;
    char * nextCharArg;
    struct poptAlias * currAlias;
    int stuffed;
};

struct execEntry {
    char * longName;
    char shortName;
    char * script;
};

struct poptContext_s {
    struct optionStackEntry optionStack[POPT_OPTION_DEPTH], * os;
    char ** leftovers;
    int numLeftovers;
    int nextLeftover;
    const struct poptOption * options;
    int restLeftover;
    char * appName;
    struct poptAlias * aliases;
    int numAliases;
    int flags;
    struct execEntry * execs;
    int numExecs;
    char ** finalArgv;
    int finalArgvCount;
    int finalArgvAlloced;
    struct execEntry * doExec;
    char * execPath;
    int execAbsolute;
    char * otherHelp;
};

#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#endif

#ifdef HAVE_GETTEXT
#define _(foo) gettext(foo)
#else
#define _(foo) (foo)
#endif

#ifdef HAVE_DGETTEXT
#define POPT_(foo) dgettext("popt", foo)
#else
#define POPT_(foo) (foo)
#endif

#define N_(foo) (foo)

#endif
