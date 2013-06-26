/* "p2c", a Pascal to C translator.
   Copyright (C) 1989, 1990, 1991 Free Software Foundation.
   Author's address: daveg@csvax.caltech.edu; 256-80 Caltech/Pasadena CA 91125.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (any version).

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */



#define PROTO_COMMENT_C
#include "trans.h"



Static int cmttablesize;
Static uchar *cmttable;

Static int grabbed_comment;




/* Special comment forms:

   \001\001\001...      Blank line(s), one \001 char per blank line
   \002text...          Additional line for previous comment
   \003text...          Additional comment line, absolutely indented
   \004text...		Note or warning line, unindented

*/




void setup_comment()
{
    curcomments = NULL;
    cmttablesize = 200;
    cmttable = ALLOC(cmttablesize, uchar, misc);
    grabbed_comment = 0;
}





int commentlen(cmt)
Strlist *cmt;
{
    if (cmt)
	if (*(cmt->s))
	    return strlen(cmt->s) + 4;
	else
	    return 5;
    else
	return 0;
}


int commentvisible(cmt)
Strlist *cmt;
{
    return (cmt &&
	    getcommentkind(cmt) != CMT_DONE &&
	    ((eatcomments != 1 && eatcomments != 2) ||
	     isembedcomment(cmt)));
}






/* If preceding statement's POST comments include blank lines,
   steal all comments after longest stretch of blank lines as
   PRE comments for the next statement. */

void steal_comments(olds, news, always)
long olds, news;
int always;
{
    Strlist *cmt, *cmtfirst = NULL, *cmtblank = NULL;
    int len, longest;

    for (cmt = curcomments; cmt; cmt = cmt->next) {
	if ((cmt->value & CMT_MASK) == olds &&
	    getcommentkind(cmt) == CMT_POST) {
	    if (!cmtfirst)
		cmtfirst = cmt;
	} else {
	    cmtfirst = NULL;
	}
    }
    if (cmtfirst) {
	if (!always) {
	    longest = 0;
	    for (cmt = cmtfirst; cmt; cmt = cmt->next) {
		if (cmt->s[0] == '\001') {   /* blank line(s) */
		    len = strlen(cmt->s);
		    if (len > longest) {
			longest = len;
			cmtblank = cmt;
		    }
		}
	    }
	    if (longest > 0) {
		if (blankafter)
		    cmtfirst = cmtblank->next;
		else
		    cmtfirst = cmtblank;
	    } else if (commentafter == 1)
		cmtfirst = NULL;
	}
	changecomments(cmtfirst, CMT_POST, olds, CMT_PRE, news);
    }
}



Strlist *fixbeginendcomment(cmt)
Strlist *cmt;
{
    char *cp, *cp2;

    if (!cmt)
	return NULL;
    cp = cmt->s;
    while (isspace(*cp))
	cp++;
    if (!strcincmp(cp, "procedure ", 10)) {    /* remove "PROCEDURE" keyword */
	strcpy(cp, cp+10);
    } else if (!strcincmp(cp, "function ", 9)) {
	strcpy(cp, cp+9);
    }
    while (isspace(*cp))
	cp++;
    if (!*cp)
	return NULL;
    if (getcommentkind(cmt) == CMT_ONBEGIN) {
	cp2 = curctx->sym->name;
	while (*cp2) {
	    if (toupper(*cp2++) != toupper(*cp++))
		break;
	}
	while (isspace(*cp))
	    cp++;
	if (!*cp2 && !*cp)
	    return NULL;     /* eliminate function-begin comment */
    }
    return cmt;
}




Static void attach_mark(sp)
Stmt *sp;
{
    long serial;

    while (sp) {
	serial = sp->serial;
	if (serial >= 0 && serial < cmttablesize) {
	    cmttable[serial]++;
	    if (sp->kind == SK_IF && serial+1 < cmttablesize)
		cmttable[serial+1]++;   /* the "else" branch */
	}
	attach_mark(sp->stm1);
	attach_mark(sp->stm2);
	sp = sp->next;
    }
}



void attach_comments(sbase)
Stmt *sbase;
{
    Strlist *cmt;
    long serial, i, j;
    int kind;

    if (spitorphancomments)
	return;
    if (serialcount >= cmttablesize) {
	cmttablesize = serialcount + 100;
	cmttable = REALLOC(cmttable, cmttablesize, uchar);
    }
    for (i = 0; i < cmttablesize; i++)
	cmttable[i] = 0;
    attach_mark(sbase);
    for (cmt = curcomments; cmt; cmt = cmt->next) {
	serial = cmt->value & CMT_MASK;
	kind = getcommentkind(cmt);
	if (serial < 0 || serial >= cmttablesize || cmttable[serial])
	    continue;
	i = 0;
	j = 0;
	do {
	    if (commentafter == 1) {
		j++;
		if (j % 3 == 0)
		    i++;
	    } else if (commentafter == 0) {
		i++;
		if (i % 3 == 0)
		    j++;
	    } else {
		i++;
		j++;
	    }
	    if (serial+i < cmttablesize && cmttable[serial+i]) {
		setcommentkind(cmt, CMT_PRE);
		cmt->value += i;
		break;
	    }
	    if (serial-j > 0 && cmttable[serial-j]) {
		setcommentkind(cmt, CMT_POST);
		cmt->value -= j;
		break;
	    }
	} while (serial+i < cmttablesize || serial-j > 0);
    }
}




void setcommentkind(cmt, kind)
Strlist *cmt;
int kind;
{
    cmt->value = (cmt->value & CMT_MASK) | (kind << CMT_SHIFT);
}



void commentline(kind)
int kind;
{
    char *cp;
    Strlist *sl;

    if (grabbed_comment) {
	grabbed_comment = 0;
	return;
    }
    if (blockkind == TOK_IMPORT || skipping_module)
	return;
    if (eatcomments == 1)
	return;
    for (cp = curtokbuf; (cp = my_strchr(cp, '*')) != NULL; ) {
	if (*++cp == '/') {
	    cp[-1] = '%';
	    note("Changed \"* /\" to \"% /\" in comment [140]");
	}
    }
    sl = strlist_append(&curcomments, curtokbuf);
    sl->value = curserial;
    setcommentkind(sl, kind);
}



void addnote(msg, serial)
char *msg;
long serial;
{
    int len1, len2, xextra, extra;
    int defer = (notephase > 0 && spitcomments == 0);
    Strlist *sl, *base = NULL, **pbase = (defer) ? &curcomments : &base;
    char *prefix;

    if (defer && (outf != stdout || !quietmode))
	printf("%s, line %d: %s\n", infname, inf_lnum, msg);
    else if (outf != stdout)
	printf("%s, line %d/%d: %s\n", infname, inf_lnum, outf_lnum, msg);
    if (verbose)
	fprintf(logf, "%s, %d/%d: %s\n", infname, inf_lnum, outf_lnum, msg);
    if (notephase == 2 || regression)
	prefix = format_s("\004 p2c: %s:", infname);
    else
	prefix = format_sd("\004 p2c: %s, line %d:", infname, inf_lnum);
    len1 = strlen(prefix);
    len2 = strlen(msg) + 2;
    if (len1 + len2 < linewidth-4) {
	msg = format_ss("%s %s ", prefix, msg);
    } else {
	extra = xextra = 0;
	while (len2 - extra > linewidth-6) {
	    while (extra < len2 && !isspace(msg[extra]))
		extra++;
	    xextra = extra;
	    while (extra < len2 && isspace(msg[extra]))
		extra++;
	}
	prefix = format_sds("%s %.*s", prefix, xextra, msg);
	msg += extra;
	sl = strlist_append(pbase, prefix);
	sl->value = serial;
	setcommentkind(sl, CMT_POST);
	msg = format_s("\003 * %s ", msg);
    }
    sl = strlist_append(pbase, msg);
    sl->value = serial;
    setcommentkind(sl, CMT_POST);
    outputmode++;
    outcomments(base);
    outputmode--;
}





/* Grab a comment off the end of the current line */
Strlist *grabcomment(kind)
int kind;
{
    char *cp, *cp2;
    Strlist *cmt, *savecmt;

    if (grabbed_comment || spitcomments == 1)
	return NULL;
    cp = inbufptr;
    while (isspace(*cp))
	cp++;
    if (*cp == ';' || *cp == ',' || *cp == '.')
	cp++;
    while (isspace(*cp))
	cp++;
    cp2 = curtokbuf;
    if (*cp == '{') {
	cp++;
	while (*cp && *cp != '}')
	    *cp2++ = *cp++;
	if (!*cp)
	    return NULL;
	cp++;
    } else if (*cp == '(' && cp[1] == '*') {
	cp += 2;
	while (*cp && (*cp != '*' || cp[1] != ')'))
	    *cp2++ = *cp++;
	if (!*cp)
	    return NULL;
	cp += 2;
    } else
	return NULL;
    while (isspace(*cp))
	cp++;
    if (*cp)
	return NULL;
    *cp2 = 0;
    savecmt = curcomments;
    curcomments = NULL;
    commentline(kind);
    cmt = curcomments;
    curcomments = savecmt;
    grabbed_comment = 1;
    if (cmtdebug > 1)
	fprintf(outf, "Grabbed comment [%d] \"%s\"\n", cmt->value & CMT_MASK, cmt->s);
    return cmt;
}



int matchcomment(cmt, kind, stamp)
Strlist *cmt;
int kind, stamp;
{
    if (spitcomments == 1 && (cmt->value & CMT_MASK) != 10000 &&
	*cmt->s != '\001' && (kind >= 0 || stamp >= 0))
	return 0;
    if (!cmt || getcommentkind(cmt) == CMT_DONE)
	return 0;
    if (stamp >= 0 && (cmt->value & CMT_MASK) != stamp)
	return 0;
    if (kind >= 0) {
	if (kind & CMT_NOT) {
	    if (getcommentkind(cmt) == kind - CMT_NOT)
		return 0;
	} else {
	    if (getcommentkind(cmt) != kind)
		return 0;
	}
    }
    return 1;
}



Strlist *findcomment(cmt, kind, stamp)
Strlist *cmt;
int kind, stamp;
{
    while (cmt && !matchcomment(cmt, kind, stamp))
	cmt = cmt->next;
    if (cmt && cmtdebug > 1)
	fprintf(outf, "Found comment [%d] \"%s\"\n", cmt->value & CMT_MASK, cmt->s);
    return cmt;
}



Strlist *extractcomment(cmt, kind, stamp)
Strlist **cmt;
int kind, stamp;
{
    Strlist *base, **last, *sl;

    last = &base;
    while ((sl = *cmt)) {
	if (matchcomment(sl, kind, stamp)) {
	    if (cmtdebug > 1)
		fprintf(outf, "Extracted comment [%d] \"%s\"\n",
		        sl->value & CMT_MASK, sl->s);
	    *cmt = sl->next;
	    *last = sl;
	    last = &sl->next;
	} else
	    cmt = &sl->next;
    }
    *last = NULL;
    return base;
}


void changecomments(cmt, okind, ostamp, kind, stamp)
Strlist *cmt;
int okind, ostamp, kind, stamp;
{
    while (cmt) {
	if (matchcomment(cmt, okind, ostamp)) {
	    if (cmtdebug > 1)
		fprintf(outf, "Changed comment [%s:%d] \"%s\" ",
			CMT_NAMES[getcommentkind(cmt)],
			cmt->value & CMT_MASK, cmt->s);
	    if (kind >= 0)
		setcommentkind(cmt, kind);
	    if (stamp >= 0)
		cmt->value = (cmt->value & ~CMT_MASK) | stamp;
	    if (cmtdebug > 1)
		fprintf(outf, " to [%s:%d]\n",
			CMT_NAMES[getcommentkind(cmt)], cmt->value & CMT_MASK);
	}
	cmt = cmt->next;
    }
}






/* End. */

