/*@z10.c:Cross References:CrossInit(), CrossMake()@***************************/
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
/*  FILE:         z10.c                                                      */
/*  MODULE:       Cross References                                           */
/*  EXTERNS:      CrossInit(), CrossMake(), GallTargEval(), CrossAddTag(),   */
/*                CrossExpand(), CrossSequence(), CrossClose()               */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define	NO_TARGET	0
#define	SEEN_TARGET	1
#define	WRITTEN_TARGET	2
#define INIT_CROSSREF_NUM	100

static OBJECT RootCross = nilobj;		/* header for all crs        */

/*****************************************************************************/
/*                                                                           */
/*  CROSSREF_TABLE                                                           */
/*                                                                           */
/*  A symbol table permitting access to cross reference generated tags by    */
/*  a mapping (symbol x file) -> current tag.                                */
/*                                                                           */
/*     crtab_getnext(sym, fnum, S)   Get next value associated with sym,fnum */
/*     crtab_debug(S, fp)            Debug print of table S to file fp       */
/*                                                                           */
/*****************************************************************************/

typedef struct crossref_rec
{ struct crossref_rec	*crtab_next;
  OBJECT		crtab_sym;
  FILE_NUM		crtab_fnum;
  int			crtab_value;
} *CROSSREF_ENTRY;

typedef struct
{ int tab_size;					/* size of table             */
  int tab_count;				/* number of entries held    */
  CROSSREF_ENTRY tab_chains[1];			/* the chains of entries     */
} *CROSSREF_TABLE;

#define	crtab_size(S)	(S)->tab_size
#define	crtab_count(S)	(S)->tab_count
#define	crtab_chain(S,i) (S)->tab_chains[i]

#define hash(pos, sym, fnum, S)						\
{ pos = ( ((unsigned long) sym) + fnum ) % crtab_size(S);				\
}

static CROSSREF_TABLE crtab_new(int newsize)
{ CROSSREF_TABLE S;  int i;
  ifdebug(DMA, D, DebugRegisterUsage(MEM_CROSSREF, 1,
    2*sizeof(int) + newsize*sizeof(CROSSREF_ENTRY)));
  S = (CROSSREF_TABLE)
    malloc(2*sizeof(int) + newsize*sizeof(CROSSREF_ENTRY));
  if( S == (CROSSREF_TABLE) NULL )
    Error(10, 1, "run out of memory enlarging crossref table", FATAL, no_fpos);
  crtab_size(S) = newsize;
  crtab_count(S) = 0;
  for( i = 0;  i < newsize;  i++ )
    crtab_chain(S, i) = (CROSSREF_ENTRY) nilobj;
  return S;
} /* end crtab_new */

static CROSSREF_TABLE crtab_rehash(CROSSREF_TABLE S, int newsize)
{ CROSSREF_TABLE NewS;  int i;  unsigned long newpos;  CROSSREF_ENTRY p, q;
  NewS = crtab_new(newsize);
  for( i = 0;  i < crtab_size(S);  i++ )
  { p = crtab_chain(S, i);
    while( p != NULL )
    { q = p->crtab_next;
      hash(newpos, p->crtab_sym, p->crtab_fnum, NewS);
      p->crtab_next = crtab_chain(NewS, newpos);
      crtab_chain(NewS, newpos) = p;
      crtab_count(NewS)++;
      p = q;
    }
  }
  ifdebug(DMA, D, DebugRegisterUsage(MEM_CROSSREF, -1,
   -(2*sizeof(int) + crtab_size(S)*sizeof(CROSSREF_ENTRY))));
  free(S);
  return NewS;
} /* end crtab_rehash */

static int crtab_getnext(OBJECT sym, FILE_NUM fnum, CROSSREF_TABLE *S)
{ CROSSREF_ENTRY x;  OBJECT t;  unsigned long pos;

  /* if S is NULL, create a new table */
  if( *S == NULL )  *S = crtab_new(INIT_CROSSREF_NUM);

  /* if (sym, fnum) exists, increment its value and return it */
  hash(pos, sym, fnum, *S);
  for( x = crtab_chain(*S, pos);  x != NULL;  x = x->crtab_next )
  { if( x->crtab_sym == sym && x->crtab_fnum == fnum )
    return ++x->crtab_value;
  }

  /* if table is full, rehash */
  if( crtab_count(*S) == crtab_size(*S) )
  { *S = crtab_rehash(*S, 2*crtab_size(*S));
    hash(pos, sym, fnum, *S);
  }

  /* insert a new entry for (sym, fnum) with value 1 */
  GetMem(t, sizeof(struct crossref_rec), no_fpos);
  x = (CROSSREF_ENTRY) t;
  x->crtab_sym = sym;
  x->crtab_fnum = fnum;
  x->crtab_next = crtab_chain(*S, pos);
  crtab_chain(*S, pos) = x;
  crtab_count(*S)++;
  return x->crtab_value = 1;

} /* end crtab_getnext */

#if DEBUG_ON
static void crtab_debug(CROSSREF_TABLE S, FILE *fp)
{ int i;  CROSSREF_ENTRY x;
  if( S == NULL )
  { fprintf(fp, "  null table\n");
    return;
  }
  fprintf(fp, "  table size: %d;  current count: %d\n",
    crtab_size(S), crtab_count(S));
  for( i = 0;  i < crtab_size(S);  i++ )
  { fprintf(fp, "crtab_chain(S, %d) =", i);
    for( x = crtab_chain(S, i);  x != NULL;  x = x->crtab_next )
    { fprintf(fp, " %s:%s,%d",
	SymName(x->crtab_sym), FileName(x->crtab_fnum), x->crtab_value);
    }
    fprintf(fp, "\n");
  }
} /* end crtab_debug */
#endif

static CROSSREF_TABLE crossref_tab = NULL;


/*@@**************************************************************************/
/*                                                                           */
/*  CrossInit(sym)     Initialize cross_sym(sym).                            */
/*                                                                           */
/*****************************************************************************/

void CrossInit(OBJECT sym)
{ OBJECT cs;
  New(cs, CROSS_SYM);
  target_state(cs) = NO_TARGET;  target_seq(cs) = 0;
  /* cr_file(cs) = NO_FILE; unused */
  gall_seq(cs) = 0;  gall_tag(cs) = nilobj;
  gall_tfile(cs) = NO_FILE;
  symb(cs) = sym;  cross_sym(sym) = cs;
  if( RootCross == nilobj )  New(RootCross, CR_ROOT);  Link(RootCross, cs);
}


/*****************************************************************************/
/*                                                                           */
/*  OBJECT CrossMake(sym, val, ctype)                                        */
/*                                                                           */
/*  Make a cross-reference with the given sym and tag value (NB no fpos).    */
/*                                                                           */
/*****************************************************************************/

OBJECT CrossMake(OBJECT sym, OBJECT val, int ctype)
{ OBJECT v1, res;
  debug3(DCR, DD, "CrossMake(%s, %s, %s)", SymName(sym),
    EchoObject(val), Image(ctype));
  New(res, CROSS);  cross_type(res) = ctype;  threaded(res) = FALSE;
  New(v1, CLOSURE);  actual(v1) = sym;
  Link(res, v1);  Link(res, val);
  debug1(DCR, DD, "CrossMake returning %s", EchoObject(res));
  return res;
}

/*@::GallTargEval(), CrossGenTag()@*******************************************/
/*                                                                           */
/*  OBJECT GallTargEval(sym, dfpos)                                          */
/*                                                                           */
/*  Produce a suitable cross-reference for a galley target.                  */
/*                                                                           */
/*****************************************************************************/

OBJECT GallTargEval(OBJECT sym, FILE_POS *dfpos)
{ OBJECT cs, res;
  FULL_CHAR buff[MAX_BUFF], *str;
  debug2(DCR, DD, "GallTargEval( %s,%s )", SymName(sym), EchoFilePos(dfpos));
  if( cross_sym(sym) == nilobj )  CrossInit(sym);
  cs = cross_sym(sym);
  if( file_num(*dfpos) != gall_tfile(cs) )
  { gall_tfile(cs) = file_num(*dfpos);
    gall_seq(cs)   = 0;
  }
  str = FileName(gall_tfile(cs));
  ++gall_seq(cs);
  if( StringLength(str) + 6 >= MAX_BUFF )
    Error(10, 2, "automatically generated tag %s&%d is too long",
	FATAL, dfpos, str, gall_seq(cs));
  StringCopy(buff, str);
  StringCat(buff, AsciiToFull("&"));
  StringCat(buff, StringInt(gall_seq(cs)));
  res = CrossMake(sym, MakeWord(WORD, buff, dfpos), GALL_TARG);
  debug1(DCR, DD, "GallTargEval returning %s", EchoObject(res));
  return res;
} /* end GallTargEval */


/*****************************************************************************/
/*                                                                           */
/*  static OBJECT CrossGenTag(x)                                             */
/*                                                                           */
/*  Generate a tag suitable for labelling closure x, in such a way that      */
/*  the same tag is likely to be generated on subsequent runs.               */
/*                                                                           */
/*****************************************************************************/

static OBJECT CrossGenTag(OBJECT x)
{ FULL_CHAR buff[MAX_BUFF],  *file_name;
  OBJECT sym, res;  FILE_NUM fnum;
  int seq;
  debug1(DCR, DD, "CrossGenTag( %s )", SymName(actual(x)));
  sym = actual(x);
  if( cross_sym(sym) == nilobj )  CrossInit(sym);
  fnum = file_num(fpos(x));
  file_name = FileName(fnum);
  seq = crtab_getnext(sym, fnum, &crossref_tab);
  debug3(DCR, DDD, "%d = crtab_getnext(%s, %s, S); S =",
    seq, SymName(sym), FileName(fnum));
  ifdebug(DCR, DDD, crtab_debug(crossref_tab, stderr));
  if( StringLength(file_name) + 20 >= MAX_BUFF )
    Error(10, 3, "automatically generated tag is too long (contains %s)",
      FATAL, &fpos(x), file_name);
  sprintf( (char *) buff, "%d.%d.%s.%d",
    file_num(fpos(sym)), line_num(fpos(sym)), file_name, seq);
  res = MakeWord(QWORD, buff, &fpos(x));
  debug2(DCR, DD, "CrossGenTag( %s ) returning %s", SymName(actual(x)), string(res));
  return res;
} /* end CrossGenTag */


/*@::CrossAddTag()@***********************************************************/
/*                                                                           */
/*  CrossAddTag(x)                                                           */
/*                                                                           */
/*  Add an automatically generated @Tag parameter to closure x if required.  */
/*                                                                           */
/*****************************************************************************/

void CrossAddTag(OBJECT x)
{ OBJECT link, par, ppar, y;
  debug1(DCR, DD, "CrossAddTag( %s )", EchoObject(x));

  /* search the parameter list of x for a @Tag parameter */
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(par, link);
    if( type(par) == PAR && is_tag(actual(par)) )
    {
      /* has tag, but if value is empty object, delete it */
      Child(y, Down(par));
      if( is_word(type(y)) && StringEqual(string(y), STR_EMPTY) )
      { DisposeChild(link);
	link = x;
      }
      break;
    }
  }
  if( link == x )
  { 
      /* search the definition of x for name of its @Tag parameter */
      ppar = nilobj;
      for( link=Down(actual(x));  link != actual(x);  link = NextDown(link) )
      {	Child(y, link);
	if( is_par(type(y)) && is_tag(y) )
	{ ppar = y;
	  break;
	}
      }
      if( ppar != nilobj ) /* should always hold */
      {
	/* prepare new PAR containing generated tag */
	New(par, PAR);
	actual(par) = ppar;
	y = CrossGenTag(x);
	Link(par, y);

	/* find the right spot, then link it to x */
	switch( type(ppar) )
	{
	  case LPAR:	link = Down(x);
			break;

	  case NPAR:	link = Down(x);
			if( Down(x) != x )
			{ Child(y, Down(x));
			  if( type(y) == PAR && type(actual(y)) == LPAR )
				link = NextDown(link);
			}
			break;

	  case RPAR:	for( link = Down(x); link != x; link = NextDown(link) )
			{ Child(y, link);
			  if( type(y) != PAR )  break;
			}
			break;
	}
	Link(link, par);
      }
  }
  debug1(DCR, DD, "CrossAddTag returning %s", EchoObject(x));
} /* end CrossAddTag */


/*@::CrossExpand()@***********************************************************/
/*                                                                           */
/*  OBJECT CrossExpand(x, env, style, crs, res_env)                          */
/*                                                                           */
/*  Return the value of cross-reference x, with environment *res_env.  If    */
/*  x has a non-literal tag, it must be tracked, so an object is added to    */
/*  *crs for this purpose.  The result replaces x, which is disposed.        */
/*                                                                           */
/*****************************************************************************/
static OBJECT nbt[2] = { nilobj, nilobj };
static OBJECT nft[2] = { nilobj, nilobj };
static OBJECT ntarget = nilobj;
static OBJECT nenclose = nilobj;

OBJECT CrossExpand(OBJECT x, OBJECT env, STYLE *style,
OBJECT *crs, OBJECT *res_env)
{ OBJECT sym, res, tag, y, cs, link, db, tmp, index;
  int ctype, count, i;  FULL_CHAR buff[MAX_BUFF], seq[MAX_BUFF], *str;
  FILE_NUM fnum, dfnum;  BOOLEAN tagerror = FALSE;
  long cont, dfpos;  int dlnum;
  assert( is_cross(type(x)), "CrossExpand: x!" );
  debug2(DCR, DD, "[ CrossExpand( %s, env, style, %s, res_env )",
    EchoObject(x), EchoObject(*crs));
  assert( NextDown(Down(x)) == LastDown(x), "CrossExpand: #args!" );

  /* manifest and tidy the right parameter */
  Child(tag, LastDown(x));
  debug0(DOM, D, "  [ calling Manifest from CrossExpand");
  tag = Manifest(tag, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, FALSE);
  debug0(DOM, D, "  ] returning from Manifest");
  tag = ReplaceWithTidy(tag, TRUE);   /* && */

  /* extract sym (the symbol name) and tag (the tag value) from x */
  Child(y, Down(x));
  assert( type(y) == CLOSURE, "ClosureExpand: type(y) != CLOSURE!" );
  sym = actual(y);
  ctype = !is_word(type(tag)) ? 1 :
	  StringEqual(string(tag), STR_EMPTY) ? 2 :
	  StringEqual(string(tag), KW_PRECEDING) ? CROSS_PREC :
	  StringEqual(string(tag), KW_FOLL_OR_PREC) ? CROSS_FOLL_OR_PREC :
	  StringEqual(string(tag), KW_FOLLOWING) ? CROSS_FOLL : CROSS_LIT;

  res = nilobj;
  switch( ctype )
  {

    case 1:

      Error(10, 4, "value of right parameter of %s is not a simple word",
	WARN, &fpos(tag), KW_CROSS);
      break;


    case 2:
    
      Error(10, 5, "value of right parameter of %s is an empty word",
	WARN, &fpos(tag), KW_CROSS);
      break;


    case CROSS_LIT:
    
      debug2(DCR, DD, "  CROSS_LIT sym %s, tag %s", SymName(sym), string(tag));
      if( cross_sym(sym) == nilobj )  CrossInit(sym);
      cs = cross_sym(sym);
      if( sym == MomentSym && StringEqual(string(tag), KW_NOW) )
      {	/* this is a request for the current time */
	res = StartMoment();
      }
      else
      { if( !has_tag(sym) )
	{ Error(10, 6, "symbol %s used in cross reference has no %s parameter",
	    WARN, &fpos(x), SymName(sym), KW_TAG);
	  tagerror = TRUE;
	}
	for( link = NextUp(Up(cs));  link != cs;  link = NextUp(link) )
        { Parent(db, link);
	  assert( is_word(type(db)), "CrossExpand: db!" );
	  if( DbRetrieve(db, FALSE, sym, string(tag), seq, &dfnum, &dfpos,
	      &dlnum, &cont) )
	  {
	    SwitchScope(nilobj);
	    count = 0;
	    /* condition db != OldCrossDb added to fix inconsistency with */
	    /* the call to AttachEnv below, which always carried it; but  */
	    /* there may still be a problem when db != OldCrossDb because */
	    /* in that case all symbols currently visible are declared    */
	    /* visible in the database entry; perhaps InitialEnvironment  */
	    /* would be best */
	    if( db != OldCrossDb )
	    { SetScope(env, &count, FALSE);
	      debug2(DCR, DD, "Retrieving %s, env = %s", SymName(sym),
	        EchoObject(env));
	    }
	    else
	    { debug1(DCR, DD, "Retrieving %s, env = nilobj", SymName(sym));
	    }
	    res = ReadFromFile(dfnum, dfpos, dlnum);
	    for( i = 1;  i <= count;  i++ )  PopScope();
	    UnSwitchScope(nilobj);
	    if( db != OldCrossDb )  AttachEnv(env, res);
	    break;
	  }
	}
      }
      break;


    case CROSS_PREC:
    case CROSS_FOLL:
    case CROSS_FOLL_OR_PREC:
    
      if( has_tag(sym) )
      { int new_seq;
	if( cross_sym(sym) == nilobj )  CrossInit(sym);
        cs = cross_sym(sym);
        assert( cs != nilobj, "CrossExpand/CROSS_FOLL: cs == nilobj!" );
        assert( type(cs) == CROSS_SYM, "CrossExpand/CROSS_FOLL: type(cs)!" );

	/* generate literal tag buff, used to track this cross reference */
        fnum = file_num(fpos(tag));
	new_seq = crtab_getnext(sym, fnum, &crossref_tab);
	str = FileName(fnum);

        if( StringLength(str) + 5 >= MAX_BUFF )
	  Error(10, 7, "automatically generated tag %s_%d is too long",
	    FATAL, &fpos(x), str, new_seq); /* was cr_seq(cs) */
        StringCopy(buff, str);
        StringCat(buff, AsciiToFull("_"));
        StringCat(buff, StringInt(new_seq)); /* was cr_seq(cs) */
	debug1(DCR, DD, "  CROSS_PREC or CROSS_FOLL generated tag %s", buff);

	/* generate tracking cross reference and index, and add to *crs */
        tmp = CrossMake(sym, MakeWord(WORD, buff, &fpos(tag)), ctype);
        New(index, ctype);
        actual(index) = tmp;
        Link(index, tmp);
        if( *crs == nilobj )  New(*crs, CR_LIST);
	Link(*crs, index);

	/* read tracking cross ref from previous run from cross-ref database */
        if( AllowCrossDb &&
	    DbRetrieve(OldCrossDb, FALSE, sym, buff, seq, &dfnum, &dfpos,
	      &dlnum, &cont) )
	{
	  SwitchScope(nilobj);
	  res = ReadFromFile(dfnum, dfpos, dlnum);
	  UnSwitchScope(nilobj);
	}
      }
      else
      {	Error(10, 8, "symbol %s used in cross reference has no %s parameter",
	  WARN, &fpos(x), SymName(sym), KW_TAG);
	tagerror = TRUE;
      }
      break;


    default:
    
      assert(FALSE, "CrossExpand ctype");
      break;


  } /* end switch */
  if( res == nilobj )
  { OBJECT envt;
    /* *** reporting this now whether or not crs_wanted
    if( ctype > 1 && !tagerror && crs_wanted )
    *** */
    if( ctype > 1 && !tagerror )
    { debug3(DCR, DD, "  reporting unresolved cross reference %s%s%s",
	SymName(sym), KW_CROSS, string(tag));
      Error(10, 9, "unresolved cross reference %s%s%s",
	WARN, &fpos(x), SymName(sym), KW_CROSS, string(tag));
    }

    /* build dummy result with environment attached */
    /* nb at present we are not adding dummy import closures to this! */
    New(res, CLOSURE);  actual(res) = sym;
    y = res;
    debug1(DCR, DD, "First y = %s", SymName(actual(y)));
    while( enclosing(actual(y)) != StartSym )
    { New(tmp, CLOSURE);
      actual(tmp) = enclosing(actual(y));
      debug0(DCR, DDD, "  calling SetEnv from CrossExpand (a)");
      envt = SetEnv(tmp, nilobj);
      AttachEnv(envt, y);
      y = tmp;
      debug1(DCR, DD, "Later y = %s", SymName(actual(y)));
    }
    New(envt, ENV);  Link(y, envt);
  }

  /* set environment, replace x by res, debug and exit */
  *res_env = DetachEnv(res);
  ReplaceNode(res, x);
  DisposeObject(x);
  assert( type(res) == CLOSURE, "CrossExpand: type(res) != CLOSURE!" );
  assert( actual(res) == sym, "CrossExpand: actual(res) != sym!" );
  debug1(DCR, DD, "] CrossExpand returning %s", EchoObject(res));
  debug1(DCR, DD, "  *crs = %s", EchoObject(*crs));
  debug1(DCR, DD, "  *res_env = %s", EchoObject(*res_env));
  return res;
} /* end CrossExpand */


/*@::CrossSequence()@*********************************************************/
/*                                                                           */
/*  CrossSequence(x)                                                         */
/*                                                                           */
/*  Object x is an insinuated cross-reference that has just been popped off  */
/*  the top of the root galley.  Resolve it with the sequence of others.     */
/*                                                                           */
/*****************************************************************************/

void CrossSequence(OBJECT x)
{ OBJECT sym, tag, val, tmp, cs, par, key, hold_key, link, y, env, hold_env;
  unsigned ctype;  FULL_CHAR buff[MAX_BUFF], *seq;
  FILE_NUM dfnum;  int dfpos, dlnum;

  /* if suppressing cross-referencing, dispose x and quit */
  if( !AllowCrossDb )
  { if( Up(x) == x )  DisposeObject(x);
    debug0(DCR, DD, "CrossSequence returning (!AllowCrossDb).");
    return;
  }

  /* get interesting fragments from x */
  debugcond1(DCR, DD, !is_cross(type(x)), "  type(x) = %s, x =", Image(type(x)));
  ifdebugcond(DCR, DD, !is_cross(type(x)), DebugObject(x));
  assert( is_cross(type(x)), "CrossSequence: type(x)!" );
  ctype = cross_type(x);
  Child(tmp, Down(x));
  assert( type(tmp) == CLOSURE, "CrossSequence: type(tmp)!" );
  sym = actual(tmp);
  if( cross_sym(sym) == nilobj )  CrossInit(sym);
  cs = cross_sym(sym);
  assert( type(cs) == CROSS_SYM, "CrossSequence: cs!" );

  /* debug output */
  debug2(DCR, D, "[ CrossSequence %s %s", Image(ctype), EchoObject(x));
  debug1(DCR, DD, "  x = %s", EchoObject(x));
  ifdebug(DCR, D, DebugObject(cs));

  /* delete as much of x as possible */
  Child(tag, NextDown(Down(x)));
  DeleteLink(NextDown(Down(x)));
  if( Up(x) == x )  DisposeObject(x);

  switch( ctype )
  {
    case GALL_FOLL:
    case GALL_FOLL_OR_PREC:
    case GALL_PREC:

      /* find the value of key of the galley, if any */
      val = tag;  key = hold_key = nilobj;
      assert( type(val) == CLOSURE, "CrossSequence/GALL_FOLL: type(val)!" );
      if( has_key(actual(val)) )
      { for( link=Down(actual(val)); link != actual(val); link=NextDown(link) )
	{ Child(y, link);
	  if( is_key(y) )
	  { OBJECT nbt[2], nft[2], crs, ntarget, nenclose;
	    nbt[COLM] = nft[COLM] = nbt[ROWM] = nft[ROWM] = nilobj;
	    crs = ntarget = nenclose = nilobj;
	    New(key, CLOSURE);
	    actual(key) = y;
	    New(hold_key, ACAT);
	    Link(hold_key, key);
	    New(env, ENV);
	    Link(env, val);
	    New(hold_env, ACAT);
	    Link(hold_env, env);
	    debug0(DOM, D, "  [ calling Manifest from CrossSequence");
	    key = Manifest(key, env, &save_style(val), nbt, nft,
	      &ntarget, &crs, FALSE, TRUE, &nenclose, FALSE);
	    debug0(DOM, D, "  ] returning from Manifest");
	    key = ReplaceWithTidy(key, TRUE);
	    DeleteLink(Down(env));
	    DisposeObject(hold_env);
	  }
	}
      }

      /* write out the galley */
      dfnum = DatabaseFileNum(&fpos(val));
      AppendToFile(val, dfnum, &dfpos, &dlnum);

      /* determine the sequence number or string of this galley */
      if( key == nilobj )
      {	++gall_seq(cs);
	StringCopy(buff, StringFiveInt(gall_seq(cs)));
	seq = buff;
      }
      else if( !is_word(type(key)) )
      {	Error(10, 10, "%s parameter is not a word", WARN, &fpos(key), KW_KEY);
	debug1(DCR, DD, "key = %s", EchoObject(key));
	seq = STR_BADKEY;
      }
      else if( StringEqual(string(key), STR_EMPTY) )
      {	Error(10, 11, "%s parameter is an empty word", WARN,&fpos(key),KW_KEY);
	seq = STR_BADKEY;
      }
      else seq = string(key);

      /* either write out the index immediately or store it for later */
      /* if( ctype == GALL_PREC || ctype == GALL_FOLL_OR_PREC ) */
      if( ctype == GALL_PREC )
      {	if( gall_tag(cs) == nilobj )
	{
	  if( ctype == GALL_PREC )
	    Error(10, 12, "no %s galley target precedes this %s%s%s", WARN,
	      &fpos(val), SymName(sym), SymName(sym), KW_CROSS, KW_PRECEDING);
	  else
	    Error(10, 22, "no %s galley target follows or precedes this %s%s%s",
	      WARN, &fpos(val), SymName(sym), SymName(sym), KW_CROSS,
	      KW_FOLL_OR_PREC);
	  debug0(DCR, DD, "  ... so substituting \"none\"");
	  gall_tag(cs) = MakeWord(WORD, STR_NONE, &fpos(val));
	}
	assert( is_word(type(gall_tag(cs))) &&
	  !StringEqual(string(gall_tag(cs)), STR_EMPTY),
	  "CrossSequence: gall_tag!" );
	debug4(DCR, DD, "  inserting galley (%s) %s&%s %s",
	  ctype == GALL_PREC ? "GALL_PREC" : "GALL_FOLL_OR_PREC", SymName(sym),
	  string(gall_tag(cs)), seq);
	DbInsert(NewCrossDb, TRUE, sym, string(gall_tag(cs)), no_fpos, seq,
			dfnum, (long) dfpos, dlnum, FALSE);
      }
      else
      {	tmp = MakeWord(WORD, seq, &fpos(val));
	cs_type(tmp) = ctype;
	cs_fnum(tmp) = dfnum;
	cs_pos(tmp) = dfpos;
	cs_lnum(tmp) = dlnum;
	Link(cs, tmp);
	debug2(DCR, D, "  saving galley (foll) %s&? %s", SymName(sym), seq);
      }
      DisposeObject(val);
      if( hold_key != nilobj )  DisposeObject(hold_key);
      break;


    case GALL_TARG:

      if( gall_tag(cs) != nilobj )  DisposeObject(gall_tag(cs));
      if( !is_word(type(tag)) || StringEqual(string(tag), STR_EMPTY) )
      {
	debug2(DCR, D, "  GALL_TARG %s put none for %s",
	  SymName(sym), EchoObject(tag));
	DisposeObject(tag);
	gall_tag(cs) = MakeWord(WORD, STR_NONE, no_fpos);
      }
      else gall_tag(cs) = tag;
      debug2(DCR, D, "  have new %s gall_targ %s", SymName(sym),
	  EchoObject(gall_tag(cs)));
      for( link = Down(cs);  link != cs;  link = NextDown(link) )
      {	Child(y, link);
	assert( is_word(type(y)) && !StringEqual(string(y), STR_EMPTY),
				"CrossSequence: GALL_TARG y!" );
	switch( cs_type(y) )
	{

	  case GALL_PREC:
	  case GALL_FOLL:
	  case GALL_FOLL_OR_PREC:

	    debug4(DCR, D, "  inserting galley (%s) %s&%s %s",
	      Image(cs_type(y)), SymName(sym), string(gall_tag(cs)), string(y));
	    if( Down(y) != y )
	      Child(val, Down(y));
            else
	      val = nilobj;
	    DbInsert(NewCrossDb, TRUE, sym, string(gall_tag(cs)), no_fpos,
	      string(y), cs_fnum(y), (long) cs_pos(y), cs_lnum(y), FALSE);
	    link = PrevDown(link);
	    DisposeChild(NextDown(link));
	    break;


	  case CROSS_LIT:
	  case CROSS_PREC:
	  case CROSS_FOLL:
	  case CROSS_FOLL_OR_PREC:

	    break;


	  default:

	    assert(FALSE, "CrossSequence: cs_type!");
	    break;
	}
      }
      break;


    case CROSS_PREC:

      if( target_state(cs) == NO_TARGET )
      {	Error(10, 13, "no %s precedes this %s%s%s", WARN, &fpos(tag),
	  SymName(sym), SymName(sym), KW_CROSS, KW_PRECEDING);
	break;
      }
      if( target_state(cs) == SEEN_TARGET )
      {
	debug2(DCR, DD, "  inserting %s cross_targ %s",
	  SymName(sym), target_val(cs));
	AppendToFile(target_val(cs), target_file(cs), &target_pos(cs),
	  &target_lnum(cs));
	DisposeObject(target_val(cs));
	target_val(cs) = nilobj;
	target_state(cs) = WRITTEN_TARGET;
      }
      if( !is_word(type(tag)) || StringEqual(string(tag), STR_EMPTY) )
      {
	debug2(DCR, DD, "  GALL_TARG %s put none for %s", SymName(sym),
		EchoObject(tag));
	DisposeObject(tag);
	tag = MakeWord(WORD, STR_NONE, no_fpos);
      }
      debug3(DCR, DD, "  inserting cross (prec) %s&%s %s", SymName(sym),
	    string(tag), "0");
      DbInsert(NewCrossDb, FALSE, sym, string(tag), &fpos(tag), STR_ZERO,
	target_file(cs), (long) target_pos(cs), target_lnum(cs), TRUE);
      DisposeObject(tag);
      break;


    case CROSS_FOLL:
    case CROSS_FOLL_OR_PREC:

      if( !is_word(type(tag)) )
      {	Error(10, 14, "tag of %s is not a simple word",
	  WARN, &fpos(tag), SymName(symb(cs)));
	debug1(DCR, DD, "  tag = %s", EchoObject(tag));
      }
      else if( StringEqual(string(tag), STR_EMPTY) )
      {
        debug1(DCR, DD, "  ignoring cross (foll) %s (empty tag)", SymName(sym));
      }
      else
      { Link(cs, tag);
	cs_fnum(tag) = file_num(fpos(tag));
	cs_type(tag) = ctype;
        debug4(DCR, DD, "  storing cross (%s) %s&%s %s", Image(ctype),
	  SymName(sym), string(tag), "?");
      }
      break;


    case CROSS_TARG:

      /* get rid of old target, if any, and add new one */
      if( target_state(cs) == SEEN_TARGET )
      {
	debug2(DCR, DD, "  disposing unused %s cross_targ %s", SymName(sym),
	  target_val(cs));
	DisposeObject(target_val(cs));
      }
      debug2(DCR, DD, "  remembering new %s cross_targ %s", SymName(sym),
	EchoObject(tag));
      target_val(cs) = tag;
      assert( Up(tag) == tag, "CrossSeq: Up(tag)!" );

      target_file(cs) = DatabaseFileNum(&fpos(tag));
      target_state(cs) = SEEN_TARGET;

      /* store tag of the galley, if any, and delete excessive right pars */
      tag = nilobj;
      assert( type(target_val(cs)) == CLOSURE, "CrossSequence: target_val!" );
      link = Down(target_val(cs));
      for( ;  link != target_val(cs);  link = NextDown(link) )
      {	Child(par, link);
	if( type(par) == PAR )
	{
	  assert( Down(par) != par, "CrossSequence: Down(PAR)!" );
	  if( is_tag(actual(par)) )
	  {
	    /* sort out the value of this tag now */
	    Child(tag, Down(par));
	    tag = ReplaceWithTidy(tag, TRUE);  /* && */
	    if( !is_word(type(tag)) )
	    { Error(10, 15, "tag of %s is not a simple word",
	        WARN, &fpos(tag), SymName(actual(target_val(cs))));
	      debug1(DCR, DD, "  tag = %s", EchoObject(tag));
	    }
	    else if( StringEqual(string(tag), STR_EMPTY) )
	    {
              debug1(DCR, DD, "  ignoring cross (own tag) %s (empty tag)",
		  SymName(sym));
	    }
	    else
	    {
	      cs_fnum(tag) = file_num(fpos(tag));
	      cs_type(tag) = CROSS_LIT;
	      Link(cs, tag);
              debug4(DCR, DD, "  storing cross (%s) %s&%s %s",
		Image(cs_type(tag)), SymName(sym), string(tag), "?");
	    }
	  }
	  else if( type(actual(par)) == RPAR )
	  {
	    /* replace any oversized right parameter by question marks */
	    Child(y, Down(par));
	    switch( type(y) )
	    {
	      case WORD:
	      case QWORD:
	      case ACAT:
	      case OPEN:
	      case NEXT:
	      case NULL_CLOS:
	      case CROSS:
	      case FORCE_CROSS:
	      case TAGGED:

		/* leave objects of these types as is */
		break;


	      default:

		/* replace all other types by three question marks */
		tmp = MakeWord(WORD, AsciiToFull("???"), &fpos(y));
		ReplaceNode(tmp, y);
		DisposeObject(y);
		break;

	    }
	  }
	}
      }

      /* if new target is already writable, write it */
      if( Down(cs) != cs )
      {
	debug2(DCR, DD, "  writing %s cross_targ %s", SymName(sym),
		EchoObject(target_val(cs)));
	AppendToFile(target_val(cs), target_file(cs), &target_pos(cs),
	  &target_lnum(cs));
	DisposeObject(target_val(cs));
	target_val(cs) = nilobj;
	for( link = Down(cs);  link != cs;  link = NextDown(link) )
	{ Child(tag, link);
	  assert( is_word(type(tag)) && !StringEqual(string(tag), STR_EMPTY),
			"CrossSeq: non-WORD or empty tag!" );
	  switch( cs_type(tag) )
	  {

	    case CROSS_LIT:
	    case CROSS_FOLL:
	    case CROSS_FOLL_OR_PREC:

	      debug3(DCR, DD, "  inserting cross (foll) %s&%s %s", SymName(sym),
	        string(tag), "0");
	      DbInsert(NewCrossDb, FALSE, sym, string(tag), &fpos(tag), 
	        STR_ZERO, target_file(cs), (long) target_pos(cs),
		target_lnum(cs), TRUE);
	      link = PrevDown(link);
	      DisposeChild(NextDown(link));
	      break;


	    case GALL_FOLL:
	    case GALL_PREC:
	    case GALL_FOLL_OR_PREC:

	      break;


	    default:

	      assert(FALSE, "CrossSequence: cs_type!");
	      break;
	  }
	}
	target_state(cs) = WRITTEN_TARGET;
      }
      break;


    default:

      assert1(FALSE, "CrossSequence:", Image(ctype));
      break;

  } /* end switch */
  debug0(DCR, D, "] CrossSequence returning.");
  debug0(DCR, D, "   cs =");
  ifdebug(DCR, DD, DebugObject(cs));
} /* end CrossSequence */


/*@::CrossClose()@************************************************************/
/*                                                                           */
/*  CrossClose()                                                             */
/*                                                                           */
/*  Check for dangling forward references, and convert old cross reference   */
/*  database to new one.                                                     */
/*                                                                           */
/*****************************************************************************/

void CrossClose(void)
{ OBJECT link, cs, ylink, y, sym;  BOOLEAN g;  int len, count;
  FILE_NUM dfnum;  long dfpos, cont;  int dlnum;
  FULL_CHAR buff[MAX_BUFF], seq[MAX_BUFF], tag[MAX_BUFF];
  debug0(DCR, D, "[ CrossClose()");
  ifdebug(DCR, DD, if( RootCross != nilobj ) DebugObject(RootCross));

  /* if suppressing cross referencing, return */
  if( !AllowCrossDb )
  { debug0(DCR, DD, "CrossClose returning (!AllowCrossDb).");
    return;
  }

  /* check for dangling forward references and dispose cross ref structures */
  if( RootCross != nilobj )
  { for( link = Down(RootCross);  link != RootCross;  link = NextDown(link) )
    { Child(cs, link);
      sym = symb(cs);
      assert( type(cs) == CROSS_SYM, "CrossClose: type(cs)!" );
      count = 0;
      for( ylink = Down(cs);  ylink != cs;  ylink = NextDown(ylink) )
      {	Child(y, ylink);
	assert( is_word(type(y)) && !StringEqual(string(y), STR_EMPTY),
				"CrossClose: GALL_TARG y!" );
	switch( cs_type(y) )
	{

	  case CROSS_FOLL:

	    debug2(DCR, DD, "cs_type(y) = %s, y = %s",
	      Image(cs_type(y)), EchoObject(y));
	    if( count < 5 )
	      Error(10, 16, "no %s follows this %s%s%s", WARN, &fpos(y),
	        SymName(sym), SymName(sym), KW_CROSS, KW_FOLLOWING);
            else if( count == 5 )
	      Error(10, 17, "and more undefined %s%s%s", WARN, no_fpos,
	        SymName(sym), KW_CROSS, KW_FOLLOWING);
	    count++;
	    break;


	  case CROSS_FOLL_OR_PREC:

	    /* no following target, so switch to preceding */
	    if( target_state(cs) == NO_TARGET )
	    { Error(10, 18, "no %s follows or precedes this %s%s%s", WARN,
		&fpos(y), SymName(sym), SymName(sym),KW_CROSS,KW_FOLL_OR_PREC);
		break;
	    }
	    if( target_state(cs) == SEEN_TARGET )
	    {
	      debug2(DCR, DD, "  inserting %s cross_targ %s",
	        SymName(sym), target_val(cs));
	      AppendToFile(target_val(cs), target_file(cs), &target_pos(cs),
		&target_lnum(cs));
	      DisposeObject(target_val(cs));
	      target_val(cs) = nilobj;
	      target_state(cs) = WRITTEN_TARGET;
	    }
	    if( !is_word(type(y)) || StringEqual(string(y), STR_EMPTY) )
	    {
	      debug2(DCR, DD, "  CROSS_FOLL_OR_PREC %s put none for %s",
		SymName(sym), EchoObject(y));
	      y = MakeWord(WORD, STR_NONE, no_fpos);
	    }
	    debug4(DCR, DD, "  inserting cross (%s) %s&%s %s",
	      Image(cs_type(y)), SymName(sym), string(y), "0");
	    DbInsert(NewCrossDb, FALSE, sym, string(y), &fpos(y), STR_ZERO,
	      target_file(cs), (long) target_pos(cs), target_lnum(cs), TRUE);
	    break;


	  case GALL_FOLL:

	    debug2(DCR, DD, "cs_type(y) = %s, y = %s",
	      Image(cs_type(y)), EchoObject(y));
	    if( count < 5 )
	      Error(10, 19, "no %s follows this %s%s%s", WARN, &fpos(y),
	        SymName(sym), SymName(sym), KW_CROSS, KW_FOLLOWING);
            else if( count == 5 )
	      Error(10, 20, "and more undefined %s%s%s", WARN, no_fpos,
	        SymName(sym), KW_CROSS, KW_FOLLOWING);
	    DbInsert(NewCrossDb, TRUE, sym, STR_NONE, no_fpos,
	      string(y), cs_fnum(y), (long) cs_pos(y), cs_lnum(y), FALSE);
	    count++;
	    break;


	  case GALL_FOLL_OR_PREC:

	    if( gall_tag(cs) == nilobj )
	    { Error(10, 21, "no %s precedes or follows this %s%s%s", WARN,
		&fpos(y), SymName(sym), SymName(sym),KW_CROSS,KW_FOLL_OR_PREC);
	      gall_tag(cs) = MakeWord(WORD, STR_NONE, no_fpos);
	    }
	    debug3(DCR, DD, "  inserting galley (foll_or_prec) %s&%s %s",
	      SymName(sym), string(gall_tag(cs)), string(y));
	    DbInsert(NewCrossDb, TRUE, sym, string(gall_tag(cs)), no_fpos,
	      string(y), cs_fnum(y), (long) cs_pos(y), cs_lnum(y), FALSE);
	    break;


	  default:

	    debug1(DCR, DD, "CrossClose: unknown cs_type %s",
	      Image(cs_type(y)));
	    assert(FALSE, "CrossClose: unknown cs_type!");
	    break;
	}
      }
      ifdebug(ANY, D,
	if( target_state(cs) == SEEN_TARGET )  DisposeObject(target_val(cs));
	if( gall_tag(cs) != nilobj )  DisposeObject(gall_tag(cs));
      );
    }
    ifdebug(ANY, D, DisposeObject(RootCross); );
  }

  /* add to NewCrossDb those entries of OldCrossDb from other source files */
  /* but set check to FALSE so that we don't worry about duplication there */
  cont = 0L;  len = StringLength(DATA_SUFFIX);
  while( DbRetrieveNext(OldCrossDb,&g,&sym,tag,seq,&dfnum,&dfpos,&dlnum,&cont))
  { if( g ) continue;
    StringCopy(buff, FileName(dfnum));
    StringCopy(&buff[StringLength(buff) - len], STR_EMPTY);
    if( FileNum(buff, STR_EMPTY) == NO_FILE )
      DbInsert(NewCrossDb, FALSE, sym, tag, no_fpos, seq, dfnum, dfpos,
	dlnum, FALSE);
  }

  /* close OldCrossDb's .li file so that NewCrossDb can use its name */
  DbClose(OldCrossDb);

  /* make NewCrossDb readable, for next run */
  DbConvert(NewCrossDb, TRUE);

  debug0(DCR, D, "] CrossClose returning.");
  ifdebug(DCR, DD, crtab_debug(crossref_tab, stderr));
} /* end CrossClose */
