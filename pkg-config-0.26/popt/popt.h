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

#ifndef H_POPT
#define H_POPT

#include <stdio.h>			/* for FILE * */

#define POPT_OPTION_DEPTH	10

#define POPT_ARG_NONE		0
#define POPT_ARG_STRING		1
#define POPT_ARG_INT		2
#define POPT_ARG_LONG		3
#define POPT_ARG_INCLUDE_TABLE	4	/* arg points to table */
#define POPT_ARG_CALLBACK	5	/* table-wide callback... must be
					   set first in table; arg points 
					   to callback, descrip points to 
					   callback data to pass */
#define POPT_ARG_MASK		0x0000FFFF
#define POPT_ARGFLAG_ONEDASH	0x80000000  /* allow -longoption */
#define POPT_ARGFLAG_DOC_HIDDEN 0x40000000  /* don't show in help/usage */
#define POPT_CBFLAG_PRE		0x80000000  /* call the callback before parse */
#define POPT_CBFLAG_POST	0x40000000  /* call the callback after parse */
#define POPT_CBFLAG_INC_DATA	0x20000000  /* use data from the include line,
					       not the subtable */

#define POPT_ERROR_NOARG	-10
#define POPT_ERROR_BADOPT	-11
#define POPT_ERROR_OPTSTOODEEP	-13
#define POPT_ERROR_BADQUOTE	-15	/* only from poptParseArgString() */
#define POPT_ERROR_ERRNO	-16	/* only from poptParseArgString() */
#define POPT_ERROR_BADNUMBER	-17
#define POPT_ERROR_OVERFLOW	-18

/* poptBadOption() flags */
#define POPT_BADOPTION_NOALIAS  (1 << 0)  /* don't go into an alias */

/* poptGetContext() flags */
#define POPT_CONTEXT_NO_EXEC	(1 << 0)  /* ignore exec expansions */
#define POPT_CONTEXT_KEEP_FIRST	(1 << 1)  /* pay attention to argv[0] */
#define POPT_CONTEXT_POSIXMEHARDER (1 << 2) /* options can't follow args */

struct poptOption {
    const char * longName;	/* may be NULL */
    char shortName;		/* may be '\0' */
    int argInfo;
    void * arg;			/* depends on argInfo */
    int val;			/* 0 means don't return, just update flag */
    char * descrip;		/* description for autohelp -- may be NULL */
    char * argDescrip;		/* argument description for autohelp */
};

struct poptAlias {
    char * longName;		/* may be NULL */
    char shortName;		/* may be '\0' */
    int argc;
    char ** argv;		/* must be free()able */
};

extern struct poptOption poptHelpOptions[];
#define POPT_AUTOHELP { NULL, '\0', POPT_ARG_INCLUDE_TABLE, poptHelpOptions, \
			0, "Help options", NULL },

typedef struct poptContext_s * poptContext;
#ifndef __cplusplus
typedef struct poptOption * poptOption;
#endif

enum poptCallbackReason { POPT_CALLBACK_REASON_PRE, 
			  POPT_CALLBACK_REASON_POST,
			  POPT_CALLBACK_REASON_OPTION };
typedef void (*poptCallbackType)(poptContext con, 
				 enum poptCallbackReason reason,
			         const struct poptOption * opt,
				 const char * arg, void * data);

poptContext poptGetContext(char * name, int argc, char ** argv, 
			   const struct poptOption * options, int flags);
void poptResetContext(poptContext con);

/* returns 'val' element, -1 on last item, POPT_ERROR_* on error */
int poptGetNextOpt(poptContext con);
/* returns NULL if no argument is available */
char * poptGetOptArg(poptContext con);
/* returns NULL if no more options are available */
char * poptGetArg(poptContext con);
char * poptPeekArg(poptContext con);
char ** poptGetArgs(poptContext con);
/* returns the option which caused the most recent error */
char * poptBadOption(poptContext con, int flags);
void poptFreeContext(poptContext con);
int poptStuffArgs(poptContext con, char ** argv);
int poptAddAlias(poptContext con, struct poptAlias alias, int flags);
int poptReadConfigFile(poptContext con, char * fn);
/* like above, but reads /etc/popt and $HOME/.popt along with environment 
   vars */
int poptReadDefaultConfig(poptContext con, int useEnv);
/* argv should be freed -- this allows ', ", and \ quoting, but ' is treated
   the same as " and both may include \ quotes */
int poptParseArgvString(char * s, int * argcPtr, char *** argvPtr);
const char * poptStrerror(const int error);
void poptSetExecPath(poptContext con, const char * path, int allowAbsolute);
void poptPrintHelp(poptContext con, FILE * f, int flags);
void poptPrintUsage(poptContext con, FILE * f, int flags);
void poptSetOtherOptionHelp(poptContext con, const char * text);
const char * poptGetInvocationName(poptContext con);

#endif
