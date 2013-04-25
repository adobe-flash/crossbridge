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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "popt.h"
#include "poptint.h"

#ifdef _WIN32
#define SLASH '\\'
#else
#define SLASH '/'
#endif

static void displayArgs(poptContext con, enum poptCallbackReason foo, 
			struct poptOption * key, 
			const char * arg, void * data) {
    if (key->shortName== '?')
	poptPrintHelp(con, stdout, 0);
    else
	poptPrintUsage(con, stdout, 0);
    exit(0);
}

struct poptOption poptHelpOptions[] = {
    { NULL, '\0', POPT_ARG_CALLBACK, &displayArgs, '\0', NULL },
    { "help", '?', 0, NULL, '?', N_("Show this help message") },
    { "usage", '\0', 0, NULL, 'u', N_("Display brief usage message") },
    { NULL, '\0', 0, NULL, 0 }
} ;

static const char * getArgDescrip(const struct poptOption * opt) {
    if (!(opt->argInfo & POPT_ARG_MASK)) return NULL;

    if (opt == (poptHelpOptions + 1) || opt == (poptHelpOptions + 2))
	if (opt->argDescrip) return POPT_(opt->argDescrip);

    if (opt->argDescrip) return _(opt->argDescrip);
    return POPT_("ARG");
}

static void singleOptionHelp(FILE * f, int maxLeftCol, 
			     const struct poptOption * opt) {
    int indentLength = maxLeftCol + 5;
    int lineLength = 79 - indentLength;
    const char * help = _(opt->descrip);
    int helpLength;
    const char * ch;
    char * left = alloca(maxLeftCol + 1);
    const char * argDescrip = getArgDescrip(opt);

    *left = '\0';
    if (opt->longName && opt->shortName)
	sprintf(left, "-%c, --%s", opt->shortName, opt->longName);
    else if (opt->shortName) 
	sprintf(left, "-%c", opt->shortName);
    else if (opt->longName)
	sprintf(left, "--%s", opt->longName);
    if (!*left) return ;
    if (argDescrip) {
	strcat(left, "=");
	strcat(left, argDescrip);
    }

    if (help)
	fprintf(f,"  %-*s   ", maxLeftCol, left);
    else {
	fprintf(f,"  %s\n", left); 
	return;
    }

    helpLength = strlen(help);
    while (helpLength > lineLength) {
	ch = help + lineLength - 1;
	while (ch > help && !isspace(*ch)) ch--;
	if (ch == help) break;		/* give up */
	while (ch > (help + 1) && isspace(*ch)) ch--;
	ch++;

	fprintf(f, "%.*s\n%*s", (int) (ch - help), help, indentLength, " ");

	help = ch;
	while (isspace(*help) && *help) help++;
	helpLength = strlen(help);
    }

    if (helpLength) fprintf(f, "%s\n", help);
}

static int maxArgWidth(const struct poptOption * opt) {
    int max = 0;
    int this;
    const char * s;
    
    while (opt->longName || opt->shortName || opt->arg) {
	if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INCLUDE_TABLE) {
	    this = maxArgWidth(opt->arg);
	    if (this > max) max = this;
	} else if (!(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN)) {
	    this = opt->shortName ? 2 : 0;
	    if (opt->longName) {
		if (this) this += 2;
		this += strlen(opt->longName) + 2;
	    }

	    s = getArgDescrip(opt);
	    if (s)
		this += strlen(s) + 1;
	    if (this > max) max = this;
	}

	opt++;
    }
    
    return max;
}

static void singleTableHelp(FILE * f, const struct poptOption * table, 
			    int left) {
    const struct poptOption * opt;

    opt = table;
    while (opt->longName || opt->shortName || opt->arg) {
	if ((opt->longName || opt->shortName) && 
	    !(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN))
	    singleOptionHelp(f, left, opt);
	opt++;
    }

    opt = table;
    while (opt->longName || opt->shortName || opt->arg) {
	if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INCLUDE_TABLE) {
	    if (opt->descrip)
		fprintf(f, "\n%s\n", _(opt->descrip));
	    singleTableHelp(f, opt->arg, left);
	}
	opt++;
    }
}

static int showHelpIntro(poptContext con, FILE * f) {
    int len = 6;
    char * fn;

    fprintf(f, POPT_("Usage:"));
    if (!(con->flags & POPT_CONTEXT_KEEP_FIRST)) {
	fn = con->optionStack->argv[0];
	if (strchr(fn, SLASH)) fn = strchr(fn, SLASH) + 1;
	fprintf(f, " %s", fn);
	len += strlen(fn) + 1;
    }

    return len;
}

void poptPrintHelp(poptContext con, FILE * f, int flags) {
    int leftColWidth;

    showHelpIntro(con, f);
    if (con->otherHelp)
	fprintf(f, " %s", con->otherHelp);
    else
	fprintf(f, " %s", POPT_("[OPTION...]"));

    fprintf(f, " [LIBRARIES]\n");
    leftColWidth = maxArgWidth(con->options);
    singleTableHelp(f, con->options, leftColWidth);
}

static int singleOptionUsage(FILE * f, int cursor, 
			      const struct poptOption * opt) {
    int len = 3;
    char shortStr[2];
    const char * item = shortStr;
    const char * argDescrip = getArgDescrip(opt);

    if (opt->shortName) {
	if (!(opt->argInfo & POPT_ARG_MASK)) 
	    return cursor;	/* we did these already */
	len++;
	*shortStr = opt->shortName;
	shortStr[1] = '\0';
    } else if (opt->longName) {
	len += 1 + strlen(opt->longName);
	item = opt->longName;
    }

    if (len == 3) return cursor;

    if (argDescrip) 
	len += strlen(argDescrip) + 1;

    if ((cursor + len) > 79) {
	fprintf(f, "\n       ");
	cursor = 7;
    } 

    fprintf(f, " [-%s%s%s%s]", opt->shortName ? "" : "-", item,
	    argDescrip ? (opt->shortName ? " " : "=") : "",
	    argDescrip ? argDescrip : "");

    return cursor + len + 1;
}

int singleTableUsage(FILE * f, int cursor, const struct poptOption * table) {
    const struct poptOption * opt;
    
    opt = table;
    while (opt->longName || opt->shortName || opt->arg) {
	if ((opt->longName || opt->shortName) && 
	    !(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN))
	    cursor = singleOptionUsage(f, cursor, opt);
	else if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INCLUDE_TABLE) 
	    cursor = singleTableUsage(f, cursor, opt->arg);
	opt++;
    }

    return cursor;
}

static int showShortOptions(const struct poptOption * opt, FILE * f, 
			    char * str) {
    char s[300];		/* this is larger then the ascii set, so
				   it should do just fine */

    if (!str) {
	str = s;
	memset(str, 0, sizeof(str));
    }

    while (opt->longName || opt->shortName || opt->arg) {
	if (opt->shortName && !(opt->argInfo & POPT_ARG_MASK))
	    str[strlen(str)] = opt->shortName;
	else if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INCLUDE_TABLE)
	    showShortOptions(opt->arg, f, str);

	opt++;
    } 

    if (s != str || !*s)
	return 0;

    fprintf(f, " [-%s]", s);
    return strlen(s) + 4;
}

void poptPrintUsage(poptContext con, FILE * f, int flags) {
    int cursor;

    cursor = showHelpIntro(con, f);
    cursor += showShortOptions(con->options, f, NULL);
    singleTableUsage(f, cursor, con->options);

    if (con->otherHelp) {
	cursor += strlen(con->otherHelp) + 1;
	if (cursor > 79) fprintf(f, "\n       ");
	fprintf(f, " %s", con->otherHelp);
    }
    fprintf(f, "\n        [LIBRARIES]\n");
}

void poptSetOtherOptionHelp(poptContext con, const char * text) {
    if (con->otherHelp) free(con->otherHelp);
    con->otherHelp = strdup(text);
}
