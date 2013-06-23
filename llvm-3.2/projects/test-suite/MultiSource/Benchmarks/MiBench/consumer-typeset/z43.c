/*@z43.c:Language Service:LanguageChange, LanguageString@*********************/
/*                                                                           */
/*  THE LOUT DOCUMENT FORMATTING SYSTEM (VERSION 3.24)                       */
/*  COPYRIGHT (C) 1991, 2000 Jeffrey H. Kingston                             */
/*                                                                           */
/*  Jeffrey H. Kingston (jeff@cs.usyd.edu.au)                                */
/*  Basser Department of Computer Science                                    */
/*  The University of Sydney 2006                                            */
/*  AUSTRALIA                                                                */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either Version 2, or (at your option)      */
/*  any later version.                                                       */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307 USA   */
/*                                                                           */
/*  FILE:         z43.c                                                      */
/*  MODULE:       Language Service                                           */
/*  EXTERNS:      LanguageInit(), LanguageDefine(), LanguageChange(),        */
/*                LanguageString(), LanguageHyph()                           */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define INIT_LANGUAGE_NUM	100


/*****************************************************************************/
/*                                                                           */
/*  LANGUAGE_TABLE                                                           */
/*                                                                           */
/*  A symbol table permitting access to language name records.               */
/*  The table will automatically enlarge to accept any number of entries.    */
/*                                                                           */
/*     ltab_new(newsize)         New empty table, newsize capacity           */
/*     ltab_insert(x, &S)        Insert new language name object x into S    */
/*     ltab_retrieve(str, S)     Retrieve language name object named str     */
/*     ltab_debug(S, fp)         Debug print of table S to file fp           */
/*                                                                           */
/*****************************************************************************/

typedef struct
{ int langtab_size;				/* size of table             */
  int langtab_count;				/* number of objects held    */
  OBJECT langtab_item[1];
} *LANGUAGE_TABLE;

#define	ltab_size(S)	(S)->langtab_size
#define	ltab_count(S)	(S)->langtab_count
#define	ltab_item(S, i)	(S)->langtab_item[i]

#define hash(pos, str, S)						\
{ FULL_CHAR *p = str;							\
  pos = *p++;								\
  while( *p ) pos += *p++;						\
  pos = pos % ltab_size(S);						\
}

static LANGUAGE_TABLE ltab_new(int newsize)
{ LANGUAGE_TABLE S;  int i;
  ifdebug(DMA, D, DebugRegisterUsage(MEM_LANG_TAB, 1,
    2*sizeof(int) + newsize * sizeof(OBJECT)));
  S = (LANGUAGE_TABLE)
	  malloc(2*sizeof(int) + newsize * sizeof(OBJECT));
  if( S == (LANGUAGE_TABLE) NULL )
    Error(43, 1, "run out of memory enlarging language table", FATAL, no_fpos);
  ltab_size(S) = newsize;
  ltab_count(S) = 0;
  for( i = 0;  i < newsize;  i++ )  ltab_item(S, i) = nilobj;
  return S;
} /* end ltab_new */

static void ltab_insert(OBJECT x, LANGUAGE_TABLE *S);

static LANGUAGE_TABLE ltab_rehash(LANGUAGE_TABLE S, int newsize)
{ LANGUAGE_TABLE NewS;  int i;
  NewS = ltab_new(newsize);
  for( i = 1;  i <= ltab_size(S);  i++ )
  { if( ltab_item(S, i) != nilobj )
      ltab_insert(ltab_item(S, i), &NewS);
  }
  free(S);
  return NewS;
} /* end ltab_rehash */

static void ltab_insert(OBJECT x, LANGUAGE_TABLE *S)
{ int pos;  OBJECT z, link, y;
  if( ltab_count(*S) == ltab_size(*S) - 1 )	/* one less since 0 unused */
    *S = ltab_rehash(*S, 2*ltab_size(*S));
  hash(pos, string(x), *S);
  if( ltab_item(*S, pos) == nilobj )  New(ltab_item(*S, pos), ACAT);
  z = ltab_item(*S, pos);
  for( link = Down(z);  link != z;  link = NextDown(link) )
  { Child(y, link);
    if( StringEqual(string(x), string(y)) )
    { Error(43, 2, "language name %s used twice (first at%s)",
	FATAL, &fpos(x), string(x), EchoFilePos(&fpos(y)));
    }
  }
  Link(ltab_item(*S, pos), x);
} /* end ltab_insert */

static OBJECT ltab_retrieve(FULL_CHAR *str, LANGUAGE_TABLE S)
{ OBJECT x, link, y;  int pos;
  hash(pos, str, S);
  x = ltab_item(S, pos);
  if( x == nilobj )  return nilobj;
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( StringEqual(str, string(y)) )  return y;
  }
  return nilobj;
} /* end ltab_retrieve */

#if DEBUG_ON
static void ltab_debug(LANGUAGE_TABLE S, FILE *fp)
{ int i;  OBJECT x, link, y;
  fprintf(fp, "  table size: %d;  current number of keys: %d\n",
    ltab_size(S), ltab_count(S));
  for( i = 0;  i < ltab_size(S);  i++ )
  { x = ltab_item(S, i);
    fprintf(fp, "ltab_item(S, %d) =", i);
    if( x == nilobj )
      fprintf(fp, " <nilobj>");
    else if( type(x) != ACAT )
      fprintf(fp, " not ACAT!");
    else for( link = Down(x);  link != x;  link = NextDown(link) )
    { Child(y, link);
      fprintf(fp, " %s",
	is_word(type(y)) ? string(y) : AsciiToFull("not-WORD!"));
    }
    fprintf(fp, "\n");
  }
} /* end ltab_debug */
#endif


static	LANGUAGE_TABLE	names_tab;		/* the language names        */
static	OBJECT		*hyph_tab;		/* arry of hyph filenames    */
static	OBJECT		*canonical_tab;		/* array of lang names       */
static	int		lang_tabsize;		/* size of prev two arrays   */
static	int		lang_count;		/* number of languages       */
static	OBJECT		lang_ends[MAX_LANGUAGE];/* sentence endings        */

/*@@**************************************************************************/
/*                                                                           */
/*  BOOLEAN LanguageSentenceEnds[]                                           */
/*                                                                           */
/*  LanguageSentenceEnds[ch] is TRUE if there exists a language in which     */
/*  character ch could occur at the end of a sentence.                       */
/*                                                                           */
/*****************************************************************************/

BOOLEAN LanguageSentenceEnds[MAX_CHARS];


/*@::LanguageInit(), LanguageDefine()@****************************************/
/*                                                                           */
/*  LanguageInit()                                                           */
/*                                                                           */
/*  Initialize this module.                                                  */
/*                                                                           */
/*****************************************************************************/

void LanguageInit(void)
{ int i;
  debug0(DLS, D, "LanguageInit()");
  names_tab = ltab_new(INIT_LANGUAGE_NUM);
  lang_count = 0;
  lang_tabsize = INIT_LANGUAGE_NUM;
  ifdebug(DMA, D, DebugRegisterUsage(MEM_LANG_TAB, 0,
    INIT_LANGUAGE_NUM * sizeof(OBJECT)));
  hyph_tab = (OBJECT *) malloc(INIT_LANGUAGE_NUM * sizeof(OBJECT));
  ifdebug(DMA, D, DebugRegisterUsage(MEM_LANG_TAB, 0,
    INIT_LANGUAGE_NUM * sizeof(OBJECT)));
  canonical_tab = (OBJECT *) malloc(INIT_LANGUAGE_NUM * sizeof(OBJECT));
  for( i = 0;  i < MAX_CHARS;  i++ )  LanguageSentenceEnds[i] = FALSE;
  debug0(DLS, D, "LanguageInit returning.");
} /* end LanguageInit */


/*****************************************************************************/
/*                                                                           */
/*  LanguageDefine(names, inside)                                            */
/*                                                                           */
/*  Define a language whose names are given by ACAT of words names, and      */
/*  whose associated hyphenation patterns file name is hyph_file.            */
/*                                                                           */
/*****************************************************************************/

void LanguageDefine(OBJECT names, OBJECT inside)
{ OBJECT link, y, hyph_file;  BOOLEAN junk;  FULL_CHAR ch;
  int len;
  assert( names != nilobj && type(names) == ACAT, "LanguageDefine: names!");
  assert( Down(names) != names, "LanguageDefine: names is empty!");
  debug2(DLS, D, "LanguageDefine(%s, %s)",
    EchoObject(names), EchoObject(inside));

  /* double table size if overflow */
  if( ++lang_count >= lang_tabsize )
  {
    ifdebug(DMA, D, DebugRegisterUsage(MEM_LANG_TAB, 0,
      -lang_tabsize * sizeof(OBJECT)));
    ifdebug(DMA, D, DebugRegisterUsage(MEM_LANG_TAB, 0,
      -lang_tabsize * sizeof(OBJECT)));
    lang_tabsize *= 2;
    ifdebug(DMA, D, DebugRegisterUsage(MEM_LANG_TAB, 0,
      lang_tabsize * sizeof(OBJECT)));
    ifdebug(DMA, D, DebugRegisterUsage(MEM_LANG_TAB, 0,
      lang_tabsize * sizeof(OBJECT)));
    hyph_tab = (OBJECT *) realloc(hyph_tab, lang_tabsize * sizeof(OBJECT) );
    canonical_tab = (OBJECT *) realloc(canonical_tab, lang_tabsize * sizeof(OBJECT) );
  }

  /* insert each language name into names_tab */
  for( link = Down(names);  link != names;  link = NextDown(link) )
  { Child(y, link);
    assert( is_word(type(y)), "LanguageDefine: type(y) != WORD!" );
    word_language(y) = lang_count;
    ltab_insert(y, &names_tab);
  }

  /* initialize canonical language name entry */
  Child(y, Down(names));
  canonical_tab[lang_count] = y;

  /* make inside an ACAT if it isn't already */
  if( type(inside) != ACAT )
  { New(y, ACAT);
    FposCopy(fpos(y), fpos(inside));
    Link(y, inside);
    inside = y;
  }

  /* initialize hyphenation file entry (first child of inside) */
  Child(hyph_file, Down(inside));
  DeleteLink(Down(inside));
  if( !is_word(type(hyph_file)) )
    Error(43, 3, "hyphenation file name expected here",
      FATAL, &fpos(inside));
  if( StringEqual(string(hyph_file), STR_EMPTY) ||
      StringEqual(string(hyph_file), STR_HYPHEN) )
  { Dispose(hyph_file);
    hyph_tab[lang_count] = nilobj;
  }
  else hyph_tab[lang_count] = hyph_file;

  /* initialize sentence ends */
  lang_ends[lang_count] = inside;
  for( link = Down(inside);  link != inside;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == GAP_OBJ )
    { link = PrevDown(link);
      DisposeChild(NextDown(link));
      continue;
    }
    if( !is_word(type(y)) )
    { debug2(DLS, D, "word patterns failing on %s %s", Image(type(y)),
	EchoObject(y));
      Error(43, 4, "expected word ending pattern here", FATAL, &fpos(y));
    }
    len = StringLength(string(y));
    if( len == 0 )
      Error(43, 5, "empty word ending pattern", FATAL, &fpos(y));
    ch = string(y)[len - 1];
    LanguageSentenceEnds[ch] = TRUE;
  }

  /* if initializing run, initialize the hyphenation table */
  if( InitializeAll )
  { if( hyph_tab[lang_count] != nilobj )
    junk = ReadHyphTable(lang_count);
  }

  debug0(DLS, D, "LanguageDefine returning.");
} /* end LanguageDefine */


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN LanguageWordEndsSentence(OBJECT wd, BOOLEAN lc_prec)             */
/*                                                                           */
/*  Returns TRUE if word ends a sentence in the current language.  This is   */
/*  so if it ends with a string in the list associated with the current      */
/*  language.  If lc_prec is TRUE, it is also necessary for the character    */
/*  preceding this suffix to be lower-case.                                  */
/*                                                                           */
/*****************************************************************************/

BOOLEAN LanguageWordEndsSentence(OBJECT wd, BOOLEAN lc_prec)
{ OBJECT x, y, link;  int pos;
  assert( is_word(type(wd)), "LanguageWordEndsSentence: wd!" );
  debug2(DLS, D, "LanguageWordEndsSentence(%d %s)",
    word_language(wd), EchoObject(wd));
  x = lang_ends[word_language(wd)];
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( StringEndsWith(string(wd), string(y)) )
    {
      if( !lc_prec )
      { debug0(DLS, D, "LanguageWordEndsSentence returning TRUE (!lc_prec)");
        return TRUE;
      }

      /* now check whether the preceding character is lower case */
      pos = StringLength(string(wd)) - StringLength(string(y)) - 1;
      if( pos >= 0 &&
	MapIsLowerCase(string(wd)[pos], FontMapping(word_font(wd), &fpos(wd))))
      {
        debug0(DLS, D, "LanguageWordEndsSentence returning TRUE (!lc_prec)");
        return TRUE;
      }
    }
  }
  debug0(DLS, D, "LanguageWordEndsSentence returning FALSE");
  return FALSE;
} /* end LanguageWordEndsSentence */


/*@::LanguageChange(), LanguageString(), LanguageHyph()@**********************/
/*                                                                           */
/*  LanguageChange(style, x)                                                 */
/*                                                                           */
/*  Change the current style to contain the language of language command x.  */
/*                                                                           */
/*****************************************************************************/

void LanguageChange(STYLE *style, OBJECT x)
{ OBJECT lname;
  debug2(DLS, D, "LanguageChange(%s, %s)", EchoStyle(style), EchoObject(x));

  /* if argument is not a word, fail and exit */
  if( !is_word(type(x)) )
  { Error(43, 6, "%s ignored (illegal left parameter)", WARN, &fpos(x),
      KW_LANGUAGE);
    debug0(DLS, D, "LanguageChange returning (language unchanged)");
    return;
  }

  /* if argument is empty, return unchanged */
  if( StringEqual(string(x), STR_EMPTY) )
  { debug0(DLS, D, "LanguageChange returning (empty, language unchanged)");
    return;
  }

  /* retrieve language record if present, else leave style unchanged */
  lname = ltab_retrieve(string(x), names_tab);
  if( lname == nilobj )
    Error(43, 7, "%s ignored (unknown language %s)", WARN, &fpos(x),
      KW_LANGUAGE, string(x));
  else language(*style) = word_language(lname);

  debug1(DLS, D, "LanguageChange returning (language = %s)", string(lname));
  ifdebug(DLS, DD, ltab_debug(names_tab, stderr));
} /* LanguageChange */


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *LanguageString(lnum)                                          */
/*                                                                           */
/*  Return the canonical name of language lnum.                              */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *LanguageString(LANGUAGE_NUM lnum)
{ FULL_CHAR *res;
  debug1(DLS, D, "LanguageString(%d)", lnum);
  assert( lnum > 0 && lnum <= lang_count, "LanguageString: unknown number" );

  res = string(canonical_tab[lnum]);

  debug1(DLS, D, "LanguageString returning %s", res);
  return res;
} /* end LanguageString */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT LanguageHyph(lnum)                                                */
/*                                                                           */
/*  Return the hyphenation file name object for language lnum.               */
/*                                                                           */
/*****************************************************************************/

OBJECT LanguageHyph(LANGUAGE_NUM lnum)
{ OBJECT res;
  debug1(DLS, D, "LanguageHyph(%d)", lnum);
  assert( lnum > 0 && lnum <= lang_count, "LanguageHyph: unknown number" );

  res = hyph_tab[lnum];

  debug1(DLS, D, "LanguageHyph returning %s", EchoObject(res));
  return res;
} /* end LanguageHyph */
