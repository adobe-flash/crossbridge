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




/* This needs to go before trans.h (and thus p2c.proto) is read */

typedef struct S_paren {
    struct S_paren *next;
    int pos, indent, qmindent, flags;
} Paren;



#define PROTO_OUT_C
#include "trans.h"


#ifndef USETIME
# if defined(BSD) || defined(hpux)
#  define USETIME 1
# else
#  define USETIME 0
# endif
#endif

#if USETIME
# include <sys/time.h>
#else
# include <time.h>
#endif




/* Output control characters:

   \001  \B  Possible break point
   \002  \X  Break point in parentheses
   \003  \(  Invisible open paren
   \004  \)  Invisible close paren
   \005  \T  Set left margin
   \006  \F  Forced break point
   \007  \A  Preceding paren requires all-or-none breaking
   \010  \[  Invisible open paren, becomes visible if not all on one line
   \011  \S  Break point after last "special argument" of a function
   \012  \n  (newline)
   \013  \E  Preceding break has extra penalty
   \014  \f  (form-feed)
   \015  \H  Hang-indent the preceding operator
   \016  \.  (unused)
   \017  \C  Break point for last : of a ?: construct

*/

char spchars[] = ".BX()TFA[SnEfH.C................";



Static int testinglinebreaker = 0;

Static int deltaindent, thisindent, thisfutureindent;
Static int sectionsize, blanklines, codesectsize, hdrsectsize;
Static int codelnum, hdrlnum;

#define MAXBREAKS  200
Static int numbreaks, bestnumbreaks;
Static double bestbadness;
Static int breakpos[MAXBREAKS], breakindent[MAXBREAKS];
Static int breakcount[MAXBREAKS], breakparen[MAXBREAKS];
Static int bestbreakpos[MAXBREAKS], bestbreakindent[MAXBREAKS];
Static int breakerrorflag;

#define MAXEDITS  200
Static int numedits, bestnumedits;
Static int editpos[MAXEDITS], besteditpos[MAXEDITS];
Static char editold[MAXEDITS], editnew[MAXEDITS];
Static char besteditold[MAXEDITS], besteditnew[MAXEDITS];

Static Paren *parenlist;

Static long numalts, bestnumalts;
Static int randombreaks;

Static char *outbuf;
Static int outbufpos, outbufcount, outbufsize;
Static int suppressnewline, lastlinelength;
Static int eatblanks;
Static int embeddedcode;
Static int showingsourcecode = 0;

#define BIGBADNESS  (1e20)



void setup_out()
{
    end_source();
    if (!nobanner)
	fprintf(outf, "/* From input file \"%s\" */\n", infname);
    outf_lnum++;
    hdrlnum = 1;
    outindent = 0;
    deltaindent = 0;
    thisindent = 0;
    thisfutureindent = -1;
    sectionsize = 2;
    blanklines = 0;
    dontbreaklines = 0;
    embeddedcode = 0;
    outputmode = 0;
    suppressnewline = 0;
    eatblanks = 0;
    outbufsize = 1000;
    outbuf = ALLOC(outbufsize, char, misc);
    outbufpos = 0;
    outbufcount = 0;
    srand(17);
}



void select_outfile(fp)
FILE *fp;
{
    if (outf == codef) {
        codesectsize = sectionsize;
	codelnum = outf_lnum;
    } else {
        hdrsectsize = sectionsize;
	hdrlnum = outf_lnum;
    }
    outf = fp;
    if (outf == codef) {
        sectionsize = codesectsize;
	outf_lnum = codelnum;
    } else {
        sectionsize = hdrsectsize;
	outf_lnum = hdrlnum;
    }
}



void start_source()
{
    if (!showingsourcecode) {
	fprintf(outf, "\n#ifdef Pascal\n");
	showingsourcecode = 1;
    }
}

void end_source()
{
    if (showingsourcecode) {
	fprintf(outf, "#endif /*Pascal*/\n\n");
	showingsourcecode = 0;
    }
}



int line_start()
{
    return (outbufcount == 0);
}


int cur_column()
{
    if (outbufpos == 0)
	return outindent;
    else
	return thisindent + outbufcount;
}



int lookback(n)
int n;
{
    if (n <= 0 || n > outbufpos)
	return 0;
    else
	return outbuf[outbufpos - n];
}


int lookback_prn(n)
int n;
{
    for (;;) {
	if (n <= 0 || n > outbufpos)
	    return 0;
	else if (outbuf[outbufpos - n] >= ' ')
	    return outbuf[outbufpos - n];
	else
	    n++;
    }
}



/* Combine two indentation adjustments */
int adddeltas(d1, d2)
int d1, d2;
{
    if (d2 >= 1000)
	return d2;
    else
	return d1 + d2;
}


/* Apply an indentation delta */
int applydelta(i, d)
int i, d;
{
    if (d >= 1000)
	return d - 1000;
    else
	return i + d;
}


/* Adjust the current indentation by delta */
void moreindent(delta)
int delta;
{
    outindent = applydelta(outindent, delta);
}


/* Adjust indentation for just this line */
void singleindent(delta)
int delta;
{
    deltaindent = adddeltas(deltaindent, delta);
}


/* Predict indentation for next line */
void futureindent(num)
int num;
{
    thisfutureindent = applydelta(applydelta(outindent, deltaindent), num);
}


int parsedelta(cp, def)
char *cp;
int def;
{
    if (!cp || !*cp)
	return def;
    if ((*cp == '+' || *cp == '-') && isdigit(cp[1]))
	return atoi(cp);
    if (*cp == '*' && isdigit(cp[1]))
	return 2000 + atoi(cp+1);
    else
	return 1000 + atoi(cp);
}




Static void leading_tab(col)
int col;
{
    if (col > maxlinewidth)
	return;    /* something wrong happened! */
    if (phystabsize > 0) {
	while (col >= phystabsize) {
	    putc('\t', outf);
	    col -= phystabsize;
	}
    }
    while (col > 0) {
	putc(' ', outf);
	col--;
    }
}



void eatblanklines()
{
    eatblanks = 1;
}



Static void flush_outbuf(numbreaks, breakpos, breakindent,
			 numedits, editpos, editold, editnew)
int numbreaks, *breakpos, *breakindent, numedits, *editpos;
char *editold, *editnew;
{
    unsigned char ch, ch2;
    char *cp;
    int i, j, linelen = 0, spaces, hashline;
    int editsaves[MAXEDITS];

    end_source();
    if (outbufcount > 0) {
	for (i = 0; i < numedits; i++) {
	    editsaves[i] = outbuf[editpos[i]];
	    outbuf[editpos[i]] = editnew[i];
	}
	leading_tab(thisindent);
	cp = outbuf;
	hashline = (*cp == '#');    /* a preprocessor directive */
	spaces = 0;
	j = 1;
	for (i = 0; i < outbufpos; ) {
	    if (j < numbreaks && i == breakpos[j]) {
		if (hashline)
		    fprintf(outf, " \\");   /* trailing backslash required */
		putc('\n', outf);
		outf_lnum++;
		leading_tab(breakindent[j]);
		linelen = breakindent[j];
		j++;
		while (i < outbufpos && *cp == ' ')
		    i++, cp++;   /* eat leading spaces */
		spaces = 0;      /* eat trailing spaces */
	    } else {
		ch = *cp++;
		if (ch == ' ') {
		    spaces++;
		} else if (ch > ' ') {
		    linelen += spaces;
		    while (spaces > 0)
			putc(' ', outf), spaces--;
		    linelen++;
		    if (ch == '\\' && embeddedcode) {
			if (*cp == '[') {
			    putc('{', outf);
			    cp++, i++;
			} else if (*cp == ']') {
			    putc('}', outf);
			    cp++, i++;
			} else
			    putc(ch, outf);
		    } else
			putc(ch, outf);
		} else if (testinglinebreaker >= 3) {
		    linelen += spaces;
		    while (spaces > 0)
			putc(' ', outf), spaces--;
		    linelen++;
		    putc('\\', outf);
		    ch2 = spchars[ch];
		    if (ch2 != '.')
			putc(ch2, outf);
		    else {
			putc('0' + ((ch >> 6) & 7), outf);
			putc('0' + ((ch >> 3) & 7), outf);
			putc('0' + (ch & 7), outf);
		    }
		}
		i++;
	    }
	}
	for (i = 0; i < numedits; i++)
	    outbuf[editpos[i]] = editsaves[i];
	eatblanks = 0;
    } else if (eatblanks) {
	return;
    }
    if (suppressnewline) {
	lastlinelength = linelen;
    } else
	putc('\n', outf);
    outf_lnum++;
}



#define ISQUOTE(ch)  ((ch)=='"' || (ch)=='\'')
#define ISOPENP(ch)  ((ch)=='(' || (ch)=='[' || (ch)=='\003' || (ch)=='\010')
#define ISCLOSEP(ch) ((ch)==')' || (ch)==']' || (ch)=='\004')
#define ISBREAK(ch)  ((ch)=='\001' || (ch)=='\002' || (ch)=='\006' || (ch)=='\011' || (ch)=='\017')

Static int readquotes(posp, err)
int *posp, err;
{
    int pos;
    char quote;

    pos = *posp;
    quote = outbuf[pos++];
    while (pos < outbufpos && outbuf[pos] != quote) {
	if (outbuf[pos] == '\\')
	    pos++;
	pos++;
    }
    if (pos >= outbufpos) {
	if (err && breakerrorflag) {
	    intwarning("output", "Mismatched quotes [248]");
	    breakerrorflag = 0;
	}
	return 0;
    } else {
	*posp = pos;
	return 1;
    }    
}


Static int maxdepth;

Static int readparens(posp, err)
int *posp, err;
{
    char ch, closing;
    int pos, level;

    pos = *posp;
    switch (outbuf[pos]) {
      case '(':
	closing = ')';
	break;
      case '[':
	closing = ']';
	break;
      case '\003':
      case '\010':
	closing = '\004';
	break;
      default:
	closing = 0;
	break;
    }
    level = 0;
    for (;;) {
	pos++;
	if (pos >= outbufpos)
	    break;
	ch = outbuf[pos];
	if (ISOPENP(ch)) {
	    level++;
	    if (level > maxdepth)
		maxdepth = level;
	} else if (ISCLOSEP(ch)) {
	    level--;
	    if (level < 0) {
		if (closing && outbuf[pos] != closing)
		    break;
		*posp = pos;
		return 1;
	    }
	} else if (ISQUOTE(ch)) {
	    if (!readquotes(&pos, err))
		return 0;
	}
    }
    if (err && breakerrorflag) {
	switch (closing) {
	  case ')':
	    intwarning("output", "Mismatched parentheses [249]");
	    break;
	  case ']':
	    intwarning("output", "Mismatched brackets [249]");
	    break;
	  default:
	    intwarning("output", "Mismatched clauses [250]");
	    break;
	}
	breakerrorflag = 0;
    }
    return 0;
}



Static int measurechars(first, last)
int first, last;
{
    int count = 0;

    while (first <= last) {
	if (outbuf[first] >= ' ')
	    count++;
	first++;
    }
    return count;
}



Static void makeedit(pos, ch)
int pos, ch;
{
    editpos[numedits] = pos;
    editold[numedits] = outbuf[pos];
    editnew[numedits] = ch;
    outbuf[pos] = ch;
    numedits++;
}

Static void unedit()
{
    numedits--;
    outbuf[editpos[numedits]] = editold[numedits];
}


Static int parencount(par)
Paren *par;
{
    int count = 0;

    while (par) {
	count++;
	par = par->next;
    }
    return count;
}





/* The following routine explores the tree of all possible line breaks,
   pruning according to the fact that "badness" and "extra" are
   increasing functions.  The object is to find the set of breaks and
   indentation with the least total badness.
   (The basic idea was borrowed from Donald Knuth's "TeX".)
*/

/* As an additional optimization, the concept of a "simple" line is used,
   i.e., a line with a structure such that the best break is sure to be
   the straightforward left-to-right fill used by a simple word processor.
   (For example, a long line with nothing but comma-breakpoints is simple.)

   Also, if the line is very long a few initial random passes are made just
   to scope out an estimate of the eventual badness of the line.  This
   combined with the badness cull helps keep the breaker from using up its
   quota of tries before even considering a key break point!  Note that
   when randombreaks==1, each call to trybreakline is fast since only one
   branch is taken at each decision point.
*/


#define randtest(lim)  (!randombreaks ? -1    \
			: randombreaks > 0    \
			    ? parencount(parens) < randombreaks-1   \
			: randombreaks == -2  \
			    ? 0 \
			: (rand() & 0xfff) < (lim))

#define TB_BRKCOUNT   0x0ff
#define TB_FORCEBRK   0x100
#define TB_NOBREAK    0x200
#define TB_ALREADYBRK 0x400
#define TB_ALLORNONE  0x800
#define TB_EXTRAIND   0x1000
#define TB_EXTRAIND2  0x2000

#define TBR_ABORT     0x1
#define TBR_SIMPLE    0x2
#define TBR_REACHED   0x4

Static int trybreakline(pos, count, indent, badness, flags, parens)
int pos, count, indent, flags;
double badness;
Paren *parens;
{
    int edited;
    int i, j, jmask, f, pos2, r;
    char ch, ch2, closing;
    double extra, penalty;
    Paren *pp;

#if 0
    { static double save = -1;
      if (showbadlimit != save) printf("Showbadlimit = %g\n", showbadlimit);
      save = showbadlimit;
    }
#endif

    if (numalts >= maxalts)
	return TBR_ABORT;
    jmask = -1;
    for (;;) {
	if (numbreaks >= MAXBREAKS) {   /* must leave rest of line alone */
	    count += measurechars(pos, outbufpos-1);
	    pos = outbufpos;
	}
	i = count - breakcount[numbreaks-1] +
	    breakindent[numbreaks-1] - linewidth;
	if (i <= 0)
	    extra = 0;
	else {
	    if (i + linewidth >= maxlinewidth || randombreaks == -2)
		return 0;   /* absolutely too long! */
	    extra = overwidepenalty + ((long)i*i)*overwideextrapenalty;
	    jmask &= ~TBR_SIMPLE;
	    if (extra < 0)
		extra = 0;
	}
	if ((testinglinebreaker > 1 && showbadlimit > 0) ?
	    (badness + extra >= showbadlimit) :
	    (badness + extra >= bestbadness)) {
	    numalts++;
	    return 0;   /* no point in going on, badness will only increase */
	}
	if (pos >= outbufpos)
	    break;
	if (parens && pos >= parens->pos) {
	    indent = parens->indent;
	    flags = parens->flags;
	    parens = parens->next;
	}
	ch = outbuf[pos++];
	if (ch >= ' ')
	    count++;
	switch (ch) {

	  case '(':
	  case '[':
	  case '\003':     /* "invisible open paren" */
	  case '\010':     /* "semi-invisible open paren" */
	    pos2 = pos - 1;
	    if (!readparens(&pos2, 1))
		break;
	    i = measurechars(pos, pos2);
	    if (count + i - breakcount[numbreaks-1] +
		breakindent[numbreaks-1] <= linewidth) {
		/* it fits, so leave it on one line */
#if 0  /* I don't think this is necessary */
		while (pos <= pos2) {
		    if (outbuf[pos] == '\002') {
			jmask &= ~TBR_SIMPLE;
			pos = pos2 + 1;
			break;
		    }
		    pos++;
		}
#else
		pos = pos2 + 1;
#endif
		count += i;
		break;
	    }
	    pp = ALLOC(1, Paren, parens);   /* doesn't fit, try poss breaks */
	    pp->next = parens;
	    pp->pos = pos2;
	    pp->indent = indent;
	    pp->qmindent = indent;
	    pp->flags = flags;
	    parens = pp;
	    flags = 0;
	    if (ch == '\010' &&       /* change to real parens when broken */
		numedits+1 < MAXEDITS) {    /* (assume it will be broken!) */
		makeedit(pos-1, '(');
		makeedit(pos2, ')');
		count++;    /* count the new open paren */
		edited = 1;
	    } else
		edited = 0;
	    i = breakindent[numbreaks-1] + count - breakcount[numbreaks-1];
	    if (i <= thisindent)
		r = 0;  /* e.g., don't break top-level assignments */
	    else if (i == indent + extraindent)
		r = 1;  /* don't waste time on identical operations */
	    else
		r = randtest(0xc00);
	    if (r != 0) {
		j = trybreakline(pos, count, i,
				 badness + MAX(- extraindentpenalty,0),
				 flags, parens);
	    } else
		j = 0;
	    if (r != 1) {
		j &= trybreakline(pos, count, indent + extraindent,
				  badness + MAX(extraindentpenalty,0),
				  flags | TB_EXTRAIND, parens);
	    }
	    if (!randombreaks && bumpindent != 0) {
		if (i == thisfutureindent) {
		    j &= trybreakline(pos, count, i + bumpindent,
				      badness + MAX(- extraindentpenalty,0)
				              + bumpindentpenalty,
				      flags, parens);
		} else if (indent + extraindent == thisfutureindent) {
		    j &= trybreakline(pos, count,
				      indent + extraindent + bumpindent,
				      badness + MAX(extraindentpenalty,0)
				              + bumpindentpenalty,
				      flags | TB_EXTRAIND, parens);
		}
	    }
	    if (edited) {
		unedit();
		unedit();
	    }
	    FREE(pp);
	    return j & jmask;

	  case '\005':   /* "set left margin" */
	    indent = breakindent[numbreaks-1] +
		     count - breakcount[numbreaks-1];
	    break;

	  case '\007':   /* "all-or-none breaking" */
	    flags |= TB_ALLORNONE;
	    break;

	  case '\001':   /* "possible break point" */
	  case '\002':   /* "break point in parens" */
	  case '\006':   /* "forced break point" */
	  case '\011':   /* "break point after special args" */
	  case '\017':   /* "break point for final : operator" */
	    /* first try the non-breaking case */
	    if (ch != '\001' && ch != '\006')
		jmask &= ~TBR_SIMPLE;
	    if ((flags & TB_BRKCOUNT) != TB_BRKCOUNT)
		flags++;   /* increment TB_BRKCOUNT field */
	    if (outbuf[pos] == '?' && parens)
		parens->qmindent = breakindent[numbreaks-1] +
		                   count - breakcount[numbreaks-1];
	    j = TBR_REACHED;
	    if (ch == '\006' || (flags & TB_FORCEBRK)) {
		/* don't try the non-breaking case */
	    } else {
		if (ch == '\011') {
		    i = breakindent[numbreaks-1] +
			count - breakcount[numbreaks-1] + 2;
		} else {
		    i = indent;
		}
		f = flags;
		if (f & TB_ALLORNONE)
		    f |= TB_NOBREAK;
		r = randtest(0x800);
		if (r != 1 || (flags & TB_NOBREAK)) {
		    j = trybreakline(pos, count, i, badness, f, parens) &
			jmask;
		    if (randombreaks == -2 && !(j & TBR_REACHED)) {
			r = -1;
			j |= TBR_REACHED;
		    }
		    if (r == 0 || (j & TBR_SIMPLE))
			flags |= TB_NOBREAK;
		}
	    }
	    if (flags & TB_NOBREAK)
		return j;
	    if (flags & TB_ALLORNONE)
		flags |= TB_FORCEBRK;
	    if (flags & TB_EXTRAIND) {
		flags &= ~TB_EXTRAIND;
		flags |= TB_EXTRAIND2;
	    }
	    /* now try breaking here */
	    if (ch == '\017')
		indent = parens->qmindent;
	    if (indent < 0)
		indent = 0;
	    breakpos[numbreaks] = pos;
	    breakcount[numbreaks] = count;
	    breakindent[numbreaks] = indent;
	    breakparen[numbreaks] = parens ? parens->pos : 0;
	    numbreaks++;
	    penalty = extra;
	    if (indent == thisfutureindent) {
		i = pos;
		while (i < outbufpos-1 && outbuf[i] <= ' ')
		    i++;
		ch2 = outbuf[i];   /* first character on next line */
		if (ch2 != '(' && ch2 != '!' && ch2 != '~' && ch2 != '-')
		    penalty += nobumpindentpenalty;
	    }
	    switch (ch) {
	      case '\001':
		penalty += commabreakpenalty;
		if (flags & TB_ALREADYBRK)
		    penalty += morebreakpenalty;
		break;
	      case '\011':
		i = parencount(parens);
		penalty += specialargbreakpenalty + commabreakextrapenalty*i;
		break;
	      case '\002':
	      case '\017':
		i = parencount(parens);
		if (outbuf[pos-2] == '(')
		    penalty += parenbreakpenalty + parenbreakextrapenalty*i;
		else if (outbuf[pos-2] == ',')
		    penalty += commabreakpenalty + commabreakextrapenalty*i;
		else if (((outbuf[pos] == '&' || outbuf[pos] == '|') &&
			  outbuf[pos+1] == outbuf[pos]) ||
			 ((outbuf[pos-3] == '&' || outbuf[pos-3] == '|') &&
			  outbuf[pos-3] == outbuf[pos-2]))
		    penalty += logbreakpenalty + logbreakextrapenalty*i;
		else if (((outbuf[pos] == '<' || outbuf[pos] == '>') &&
			  outbuf[pos+1] != outbuf[pos]) ||
			 ((outbuf[pos] == '=' || outbuf[pos] == '!') &&
			  outbuf[pos+1] == '=') ||
			 ((outbuf[pos-2] == '<' || outbuf[pos-2] == '>') &&
			  outbuf[pos-3] != outbuf[pos-2]) ||
			 ((outbuf[pos-3] == '<' || outbuf[pos-3] == '>' ||
			   outbuf[pos-3] == '=' || outbuf[pos-3] == '!') &&
			  outbuf[pos-2] == '='))
		    penalty += relbreakpenalty + relbreakextrapenalty*i;
		else if (outbuf[pos-2] == '=')
		    penalty += assignbreakpenalty + assignbreakextrapenalty*i;
		else if (outbuf[pos] == '?') {
		    penalty += qmarkbreakpenalty + qmarkbreakextrapenalty*i;
		    if (parens)
			parens->qmindent = breakindent[numbreaks-1] +
			                   count - breakcount[numbreaks-1];
		} else
		    penalty += opbreakpenalty + opbreakextrapenalty*i;
		if (outbuf[pos-2] == '-')
		    penalty += exhyphenpenalty;
		if (flags & TB_ALREADYBRK)
		    penalty += morebreakpenalty + morebreakextrapenalty*i;
		break;
	      default:
		break;
	    }
	    while (pos < outbufpos && outbuf[pos] == '\013') {
		penalty += wrongsidepenalty;
		pos++;
	    }
	    penalty -= earlybreakpenalty*(flags & TB_BRKCOUNT);
	    /* the following test is not quite right, but it's not too bad. */
	    if (breakindent[numbreaks-2] == breakindent[numbreaks-1] &&
		breakparen[numbreaks-2] != breakparen[numbreaks-1])
		penalty += sameindentpenalty;
#if 0
	    else if (ch == '\002' && parens &&  /*don't think this is needed*/
		     parens->indent == breakindent[numbreaks-1] &&
		     parens->pos != breakparen[numbreaks-1])
		penalty += sameindentpenalty + 0.001;   /***/
#endif
	    penalty += (breakindent[numbreaks-1] - thisindent) *
		       indentamountpenalty;
	    if (penalty < 1) penalty = 1;
	    pos2 = pos;
	    while (pos2 < outbufpos && outbuf[pos2] == ' ')
		pos2++;
	    flags |= TB_ALREADYBRK;
	    j = trybreakline(pos2, count, indent, badness + penalty,
			     flags, parens) & jmask;
	    numbreaks--;
	    return j;
	    
	  case '\015':    /* "hang-indent operator" */
	    if (count <= breakcount[numbreaks-1] + 2 &&
		!(flags & TB_EXTRAIND2)) {
		breakindent[numbreaks-1] -= count - breakcount[numbreaks-1];
		pos2 = pos;
		while (pos2 < outbufpos && outbuf[pos2] <= ' ') {
		    if (outbuf[pos2] == ' ')
			breakindent[numbreaks-1]--;
		    pos2++;
		}
	    }
	    break;

	  case '"':
	  case '\'':
	    closing = ch;
	    while (pos < outbufpos && outbuf[pos] != closing) {
		if (outbuf[pos] == '\\')
		    pos++, count++;
		pos++;
		count++;
	    }
	    if (pos >= outbufpos) {
		intwarning("output", "Mismatched quotes [248]");
		continue;
	    }
	    pos++;
	    count++;
	    break;

	  case '/':
	    if (pos < outbufpos && (outbuf[pos] == '*' ||
				    (outbuf[pos] == '/' && cplus > 0))) {
		count += measurechars(pos, outbufpos-1);
		pos = outbufpos;   /* assume comment is at end of line */
	    }
	    break;

	}
    }
    numalts++;
    badness += extra;
    if (testinglinebreaker > 1) {
	if (badness >= bestbadness &&
	    (badness < showbadlimit || showbadlimit == 0)) {
	    fprintf(outf, "\n#if 0   /* rejected #%ld, badness = %g >= %g */\n", numalts, badness, bestbadness);
	    flush_outbuf(numbreaks, breakpos, breakindent,
			 numedits, editpos, editold, editnew);
	    fprintf(outf, "#endif\n");
	    return TBR_SIMPLE & jmask;
	} else if ((bestbadness < showbadlimit || showbadlimit == 0) &&
		   bestnumalts > 0) {
	    fprintf(outf, "\n#if 0   /* rejected #%ld, badness = %g > %g */\n", bestnumalts, bestbadness, badness);
	    flush_outbuf(bestnumbreaks, bestbreakpos, bestbreakindent,
			 bestnumedits, besteditpos,
			 besteditold, besteditnew);
	    fprintf(outf, "#endif\n");
	}
    }
    bestbadness = badness;
    bestnumbreaks = numbreaks;
    bestnumalts = numalts;
    for (i = 0; i < numbreaks; i++) {
	bestbreakpos[i] = breakpos[i];
	bestbreakindent[i] = breakindent[i];
    }
    bestnumedits = numedits;
    for (i = 0; i < numedits; i++) {
	besteditpos[i] = editpos[i];
	besteditold[i] = editold[i];
	besteditnew[i] = editnew[i];
    }
    return TBR_SIMPLE & jmask;
}




int parse_breakstr(cp)
char *cp;
{
    short val = 0;

    if (isdigit(*cp))
	return atoi(cp);
    while (*cp && !isspace(*cp) && *cp != '}') {
	switch (toupper(*cp++)) {

	  case 'N':
	  case '=':
	    break;

	  case 'L':
	    val |= BRK_LEFT;
	    break;

	  case 'R':
	    val |= BRK_RIGHT;
	    break;

	  case 'H':
	    val |= BRK_HANG | BRK_LEFT;
	    break;

	  case '>':
	    if (val & BRK_LEFT)
		val |= BRK_LPREF;
	    else if (val & BRK_RIGHT)
		val |= BRK_RPREF;
	    else
		return -1;
	    break;

	  case '<':
	    if (val & BRK_LEFT)
		val |= BRK_RPREF;
	    else if (val & BRK_RIGHT)
		val |= BRK_LPREF;
	    else
		return -1;
	    break;

	  case 'A':
	    val |= BRK_ALLNONE;
	    break;

	  default:
	    return -1;

	}
    }
    return val;
}




long getcurtime()
{
#if USETIME
    static unsigned long starttime = 0;
    struct timeval t;
    struct timezone tz;

    gettimeofday(&t, &tz);
    if (starttime == 0)
	starttime = t.tv_sec;
    t.tv_sec -= starttime;
    return (t.tv_sec*1000 + t.tv_usec/1000);
#else
    static unsigned long starttime = 0;
    if (!starttime) starttime = time(NULL);
    return (time(NULL) - starttime) * 1000;
#endif
}



void output(msg)
register char *msg;
{
    unsigned char ch;
    double savelimit;
    int i, savemaxlw, maxdp;
    long alts;
    long time0, time0a, time1;

    debughook();
    if (outputmode) {
	end_source();
	while ((ch = *msg++) != 0) {
	    if (ch >= ' ') {
		putc(ch, outf);
	    } else if (ch == '\n') {
		putc('\n', outf);
		outf_lnum++;
	    }
	}
	return;
    }
    while ((ch = *msg++) != 0) {
	if (ch == '\n') {
	    if (outbufpos == 0) {      /* blank line */
		thisfutureindent = -1;
		blanklines++;
		continue;
	    }
	    if (sectionsize > blanklines)
		blanklines = sectionsize;
	    sectionsize = 0;
	    if (eatblanks)
		blanklines = 0;
            while (blanklines > 0) {
                blanklines--;
		end_source();
                putc('\n', outf);
		outf_lnum++;
            }
	    if (thisindent + outbufcount >= linewidth && !dontbreaklines) {
		numbreaks = 1;
		bestnumbreaks = 0;
		bestbadness = BIGBADNESS;
		breakpos[0] = 0;
		breakindent[0] = thisindent;
		breakcount[0] = 0;
		breakerrorflag = 1;
		numedits = 0;
		bestnumedits = 0;
		savelimit = showbadlimit;
		numalts = 0;
		bestnumalts = 0;
		savemaxlw = maxlinewidth;
		time0 = time0a = getcurtime();
		if (regression)
		    srand(17);
		if (thisindent + outbufcount > linewidth*3/2) {
		    i = 0;
		    maxdepth = 0;
		    readparens(&i, 0);
		    maxdp = maxdepth;
		    for (;;) {    /* try some simple fixed methods first... */
			for (i = 1; i <= 20; i++) {
			    randombreaks = -1;
			    trybreakline(0, 0, thisindent, 0.0, 0, NULL);
			}
			randombreaks = -2;
			trybreakline(0, 0, thisindent, 0.0, 0, NULL);
			for (i = 0; i <= maxdp+1; i++) {
			    randombreaks = i+1;
			    trybreakline(0, 0, thisindent, 0.0, 0, NULL);
			}
			if (bestbadness == BIGBADNESS && maxlinewidth < 9999) {
			    maxlinewidth = 9999;   /* no choice but to relax */
			    numalts = 0;
			} else
			    break;
		    }
		    time0a = getcurtime();
		}
		randombreaks = 0;
		trybreakline(0, 0, thisindent, 0.0, 0, NULL);
		if (bestbadness == BIGBADNESS && maxlinewidth < 9999) {
		    numalts = 0;
		    maxlinewidth = 9999;   /* no choice but to relax this */
		    trybreakline(0, 0, thisindent, 0.0, 0, NULL);
		}
		time1 = getcurtime() - time0;
		alts = numalts;
		if (testinglinebreaker) {
		    if (savelimit < 0 && testinglinebreaker > 1) {
			showbadlimit = bestbadness * (-savelimit);
			numalts = 0;
			bestnumalts = 0;
			trybreakline(0, 0, thisindent, 0.0, 0, NULL);
		    }
		    fprintf(outf, "\n#if 1   /* accepted #%ld, badness = %g, tried %ld in %.3f sec */\n", bestnumalts, bestbadness, alts, time1/1000.0);
		}
		showbadlimit = savelimit;
		maxlinewidth = savemaxlw;
		flush_outbuf(bestnumbreaks, bestbreakpos, bestbreakindent,
			     bestnumedits, besteditpos,
			     besteditold, besteditnew);
		if (((USETIME && time1 > 1000) || alts >= maxalts) &&
		    !regression) {
		    sprintf(outbuf, "Line breaker spent %.1f",
			    (time1 + time0 - time0a) / 1000.0);
		    if (time0 != time0a)
			sprintf(outbuf + strlen(outbuf),
				"+%.2f", (time0a - time0) / 1000.0);
		    sprintf(outbuf + strlen(outbuf),
			    " seconds, %ld tries on line %d [251]", alts, outf_lnum);
		    note(outbuf);
		} else if (verbose) {
		    fprintf(logf, "%s, %d/%d: Line breaker spent %ld tries\n",
			    infname, inf_lnum, outf_lnum, alts);
		}
		if (testinglinebreaker)
		    fprintf(outf, "#endif\n\n");
	    } else {
		if (testinglinebreaker < 2)
		    flush_outbuf(0, NULL, NULL, 0, NULL, NULL, NULL);
	    }
	    thisfutureindent = -1;
	    outbufpos = 0;
	    outbufcount = 0;
	} else {
	    if (outbufpos == 0) {
		if (ch == ' ' && !dontbreaklines)    /* eat leading spaces */
		    continue;
		thisindent = applydelta(outindent, deltaindent);
		deltaindent = 0;
	    }
	    if (outbufpos == outbufsize) {
		outbufsize *= 2;
		outbuf = REALLOC(outbuf, outbufsize, char);
	    }
	    outbuf[outbufpos++] = ch;
	    if (ch >= ' ')
		outbufcount++;
	}
    }
}



void out_n_spaces(n)
int n;
{
    while (--n >= 0)
	output(" ");
}



void out_spaces(spc, over, len, delta)
int spc, over, len, delta;
{
    int n;

    if (spc == -999)
	spc = commentindent;
    if (spc < 0) {               /* right-justify */
	n = (-spc) - cur_column() - len;
	if (n < minspcthresh)
	    n = minspacing;
	else
	    over = 1000;
    } else if (spc >= 2000) {    /* tab to multiple */
	spc -= 2000;
	n = (spc-1) - ((cur_column()+spc-1) % spc);
	if (n < minspcthresh)
	    n += spc;
    } else if (spc >= 1000) {    /* absolute column */
	spc -= 1000;
	n = spc - cur_column();
	if (n < minspcthresh)
	    n = minspacing;
    } else                       /* relative spacing */
	n = spc;
    if (line_start()) {
	singleindent(n);
    } else if (len > 0 && over != 1000 && cur_column() + n + len > linewidth) {
	output("\n");
	out_spaces(over, 1000, len, 0);
	singleindent(delta);
    } else {
	out_n_spaces(n);
    }
}




void testlinebreaker(lev, fn)
int lev;
char *fn;
{
    char buf[256], *bp, *cp;
    int first, indent;

    testinglinebreaker = lev;
    if (!fn)
	return;
    inf = fopen(fn, "r");
    if (!inf) {
	perror(fn);
	exit(1);
    }
    sprintf(buf, "%s.br", fn);
    outf = fopen(buf, "w");
    if (!outf) {
	perror(buf);
	exit(1);
    }
    setup_out();
    outindent = 4;
    first = 1;
    while (fgets(buf, 256, inf)) {
	cp = buf + strlen(buf) - 2;
	if (cp >= buf) {
	    bp = buf;
	    indent = 0;
	    while (isspace(*bp))
		if (*bp++ == '\t')
		    indent += 8;
		else
		    indent++;
	    if (first) {
		first = 0;
		outindent = indent;
	    }
	    if (!(*cp == '{' ||
		  *cp == ')' ||
		  *cp == ';') ||
		  (*cp == '/' && cp[-1] == '*')) {
		cp[1] = '\001';   /* eat the \n */
	    } else {
		first = 1;
	    }
	    output(bp);
	}
    }
    fclose(outf);
    fclose(inf);
}





void outsection(size)
int size;
{
    if (size > sectionsize)
        sectionsize = size;
}



int isembedcomment(cmt)
Strlist *cmt;
{
    int len = strlen(embedcomment);
    return (cmt && len > 0 && !strncmp(cmt->s, embedcomment, len) &&
	    (isspace(cmt->s[len]) ||
	     (!cmt->s[len] && cmt->next &&
	      (*cmt->next->s == '\002' || *cmt->next->s == '\003'))));
}


Strlist *outcomments(cmt)
Strlist *cmt;
{
    char *cp;
    int saveindent = outindent, savesingle = deltaindent, theindent;
    int saveeat = eatcomments;
    int i = 0;

    if (!cmt)
	return NULL;
    if (!commentvisible(cmt)) {
	setcommentkind(cmt, CMT_DONE);
	return cmt->next;
    }
    if (*cmt->s == '\001') {
	if (cmtdebug)
	    output(format_sd("[]  [%s:%d]",
			     CMT_NAMES[getcommentkind(cmt)],
			     cmt->value & CMT_MASK));
	for (cp = cmt->s; *cp; cp++) {
	    output("\n");
	    if (cmtdebug && cp[1])
		output("[]");
	}
	setcommentkind(cmt, CMT_DONE);
	return cmt->next;
    }
    dontbreaklines++;
    if (isembedcomment(cmt)) {
	embeddedcode = 1;
	eatcomments = 0;
	if (!strcmp(cmt->s, embedcomment)) {
	    cmt = cmt->next;
	    theindent = 0;
	    cp = cmt/*->next*/->s + 1;
	    while (*cp++ == ' ')
		theindent++;
	} else {
	    strcpy(cmt->s, cmt->s + strlen(embedcomment) + 1);
	    moreindent(deltaindent);
	    theindent = outindent;
	    deltaindent = 0;
	}
    } else {
	moreindent(deltaindent);
	if (cmt->s[0] == '\004')
	    outindent = 0;
	theindent = outindent;
	deltaindent = 0;
	output("/*");
    }
    cp = cmt->s;
    for (;;) {
	if (*cp == '\002')
	    cp++;
	else if (*cp == '\003' || *cp == '\004') {
	    outindent = 0;
	    cp++;
	}
	if (embeddedcode) {
	    for (i = 0; *cp == ' ' && i < theindent; i++)
		cp++;
	    i = *cp;
	    if (*cp == '#')
		outindent = 0;
	}
	output(cp);
	if (cmtdebug)
	    output(format_sd(" [%s:%d] ",
			     CMT_NAMES[getcommentkind(cmt)],
			     cmt->value & CMT_MASK));
	setcommentkind(cmt, CMT_DONE);
	cmt = cmt->next;
	if (!cmt || !commentvisible(cmt))
	    break;
	cp = cmt->s;
	if (*cp != '\002' && *cp != '\003')
	    break;
	output("\n");
	if (!embeddedcode) {
	    outindent = (*cp == '\002') ? theindent : 0;
	    deltaindent = 0;
	}
    }
    if (embeddedcode) {
	embeddedcode = 0;
	if (i) {   /* eat final blank line */
	    output("\n");
	}
    } else {
	output("*/\n");
    }
    outindent = saveindent;
    deltaindent = savesingle;
    dontbreaklines--;
    eatcomments = saveeat;
    return cmt;
}



void outcomment(cmt)
Strlist *cmt;
{
    Strlist *savenext;
    
    if (cmt) {
	savenext = cmt->next;
	cmt->next = NULL;
	outcomments(cmt);
	cmt->next = savenext;
    }
}



void outtrailcomment(cmt, serial, indent)
Strlist *cmt;
int serial, indent;
{
    int savedelta = deltaindent;

#if 0
    suppressnewline = 1;
    output("\n");
    suppressnewline = 0;
#endif
    cmt = findcomment(cmt, CMT_TRAIL, serial);
    if (commentvisible(cmt)) {
	out_spaces(indent, commentoverindent, commentlen(cmt), 0);
	outcomment(cmt);
	deltaindent = savedelta;
    } else
	output("\n");
}



void flushcomments(cmt, kind, serial)
Strlist **cmt;
int kind, serial;
{
    Strlist *cmt2, *cmt3;
    int saveindent, savesingle, saveeat;

    if (!cmt)
	cmt = &curcomments;
    cmt2 = extractcomment(cmt, kind, serial);
    saveindent = outindent;
    savesingle = deltaindent;
    moreindent(deltaindent);
    deltaindent = 0;
    saveeat = eatcomments;
    if (eatcomments == 2)
	eatcomments = 0;
    cmt3 = cmt2;
    while (cmt3)
	cmt3 = outcomments(cmt3);
    eatcomments = saveeat;
    outindent = saveindent;
    deltaindent = savesingle;
    strlist_empty(&cmt2);
}





char *rawCstring(fmt, s, len, special)
char *fmt;
register char *s;
int len, special;
{
    char buf[500];
    register char *cp;
    register unsigned char ch;

    cp = buf;
    while (--len >= 0) {
        ch = *((unsigned char *) s);
        s++;
        if (ch == 0 && (len == 0 || !isdigit(*s))) {
            *cp++ = '\\';
            *cp++ = '0';
        } else if (ch == '\n') {
            *cp++ = '\\';
            *cp++ = 'n';
        } else if (ch == '\b') {
            *cp++ = '\\';
            *cp++ = 'b';
        } else if (ch == '\t') {
            *cp++ = '\\';
            *cp++ = 't';
        } else if (ch == '\f') {
            *cp++ = '\\';
            *cp++ = 'f';
#if 0
        } else if (ch == '\r') {
            *cp++ = '\\';
            *cp++ = 'r';
#endif
        } else if (ch < ' ' || ch >= 127) {
            *cp++ = '\\';
            *cp++ = '0' + (ch>>6);
            *cp++ = '0' + ((ch>>3) & 7);
            *cp++ = '0' + (ch & 7);
        } else if (ch == special) {
            switch (ch) {
                case '%':
                    *cp++ = ch;
                    *cp++ = ch;
                    break;
            }
        } else {
            if (ch == '"' || ch == '\\')
                *cp++ = '\\';
            *cp++ = ch;
        }
    }
    *cp = 0;
    return format_s(fmt, buf);
}


char *makeCstring(s, len)
register char *s;
int len;
{
    return rawCstring("\"%s\"", s, len, 0);
}



char *makeCchar(ich)
int ich;
{
    char buf[500];
    register char *cp;
    register unsigned char ch = (ich & 0xff);

    if (ich < 0 || ich > 255 || (ich == 0 && !nullcharconst))
        return format_d("%d", ich);
    cp = buf;
    if (ch == 0) {
        *cp++ = '\\';
        *cp++ = '0';
    } else if (ch == '\n') {
        *cp++ = '\\';
        *cp++ = 'n';
    } else if (ch == '\b') {
        *cp++ = '\\';
        *cp++ = 'b';
    } else if (ch == '\t') {
        *cp++ = '\\';
        *cp++ = 't';
    } else if (ch == '\f') {
        *cp++ = '\\';
        *cp++ = 'f';
#if 0
    } else if (ch == '\r') {
        *cp++ = '\\';
        *cp++ = 'r';
#endif
    } else if (ch < ' ' || ch >= 127) {
        *cp++ = '\\';
        *cp++ = '0' + (ch>>6);
        *cp++ = '0' + ((ch>>3) & 7);
        *cp++ = '0' + (ch & 7);
    } else {
        if (ch == '\'' || ch == '\\')
            *cp++ = '\\';
        *cp++ = ch;
    }
    *cp = 0;
    return format_s("'%s'", buf);
}






/* End. */


