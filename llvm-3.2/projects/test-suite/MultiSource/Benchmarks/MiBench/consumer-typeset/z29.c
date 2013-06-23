/*@z29.c:Symbol Table:Declarations, hash()@***********************************/
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
/*  FILE:         z29.c                                                      */
/*  MODULE:       Symbol Table                                               */
/*  EXTERNS:      InitSym(), PushScope(), PopScope(), SuppressVisible(),     */
/*                UnSuppressVisible(), SuppressScope(), UnSuppressScope(),   */
/*                SwitchScope(), UnSwitchScope(), BodyParAllowed(),          */
/*                BodyParNotAllowed(), InsertSym(), SearchSym(),             */
/*                SymName(), FullSymName(), ChildSym(), CheckSymSpread(),    */
/*                DeleteEverySym()                                           */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

#define	MAX_STACK	300		/* size of scope stack               */
#define	MAX_TAB		1783		/* size of hash table                */

#define	length(x)	word_font(x)

static	OBJECT		scope[MAX_STACK];		/* the scope stack   */
static	BOOLEAN		npars_only[MAX_STACK];		/* look for NPAR exc */
static	BOOLEAN		vis_only[MAX_STACK];		/* look for visibles */
static	BOOLEAN		body_ok[MAX_STACK];		/* look for body par */
static	BOOLEAN		suppress_scope;			/* suppress scoping  */
static	BOOLEAN		suppress_visible;		/* suppress visible  */
static	int		scope_top;			/* scope stack top   */
static	struct { OBJECT f1, f2; } symtab[MAX_TAB];	/* the hash table    */
#if DEBUG_ON
static	int		sym_spread[MAX_TAB] = { 0 };	/* hash table spread */
static	int		sym_count = 0;			/* symbol count      */
#endif


/*****************************************************************************/
/*                                                                           */
/*  #define hash(str, len, val)                                              */
/*                                                                           */
/*  Set val to the hash value of string str, which has length len.           */
/*  The hash function is just the character sum mod MAX_TAB.                 */
/*  This definition assumes that working variables rlen and x exist.         */
/*                                                                           */
/*****************************************************************************/

#define hash(str, len, val)						\
{ rlen = len;								\
  x    = str;								\
  val  = *x++;								\
  while( --rlen )  val += *x++;						\
  val %= MAX_TAB;							\
}


/*@::InitSym(), PushScope(), PopScope(), SuppressVisible(), etc.@*************/
/*                                                                           */
/*  InitSym()                                                                */
/*                                                                           */
/*  Initialize the symbol table to empty.                                    */
/*                                                                           */
/*****************************************************************************/

void InitSym(void)
{ int i;
  scope_top = 0;
  suppress_scope = FALSE;
  suppress_visible = FALSE;
  for( i = 0;  i < MAX_TAB;  i++ )
    symtab[i].f1 = symtab[i].f2 = (OBJECT) &symtab[i];
} /* end InitSym */


/*****************************************************************************/
/*                                                                           */
/*  PushScope(x, npars, vis)                                                 */
/*  PopScope()                                                               */
/*                                                                           */
/*  Add or remove an OBJECT x (which must be in the symbol table) to or from */
/*  the scope stack.  If npars is TRUE, only the named parameters of x are   */
/*  added to scope.  If vis is TRUE, only visible locals and parameters are  */
/*  added.                                                                   */
/*                                                                           */
/*****************************************************************************/

void PushScope(OBJECT x, BOOLEAN npars, BOOLEAN vis)
{ debug3(DST, DD, "[ PushScope(%s, %s, %s)", SymName(x), bool(npars), bool(vis));
  assert( suppress_scope == FALSE, "PushScope: suppress_scope!" );
  if( scope_top >= MAX_STACK )
  {
#if DEBUG_ON
    int i;
    for( i = 0; i < scope_top; i++ )
      Error(29, 1, "  scope[%2d] = %s", WARN, &fpos(x), i, SymName(scope[i]));
#endif
    Error(29, 2, "scope depth limit exceeded", INTERN, &fpos(x));
  }
  scope[scope_top]      = x;
  npars_only[scope_top] = npars;
  vis_only[scope_top]   = vis;
  body_ok[scope_top]    = FALSE;
  scope_top++;
} /* end PushScope */

void PopScope(void)
{ debug0(DST, DD, "] PopScope()");
  assert( scope_top > 0, "PopScope: tried to pop empty scope stack");
  assert( suppress_scope == FALSE, "PopScope: suppress_scope!" );
  scope_top--;
} /* end PopScope */


/*****************************************************************************/
/*                                                                           */
/*  SuppressVisible()                                                        */
/*  UnSuppressVisible()                                                      */
/*                                                                           */
/*  Make all children of any symbol acceptable, not just the exported ones.  */
/*                                                                           */
/*****************************************************************************/

void SuppressVisible(void)
{ debug0(DST, DD, "[ SuppressVisible()");
  suppress_visible = TRUE;
} /* end SuppressVisible */

void UnSuppressVisible(void)
{ debug0(DST, DD, "] UnSuppressVisible()");
  suppress_visible = FALSE;
} /* end UnSuppressVisible */


/*@::SuppressScope(), UnSuppressScope(), SwitchScope(), UnswitchScope()@******/
/*                                                                           */
/*  SuppressScope()                                                          */
/*  UnSuppressScope()                                                        */
/*                                                                           */
/*  Suppress all scopes (so that all calls to SearchSym fail); and undo it.  */
/*                                                                           */
/*****************************************************************************/


void SuppressScope(void)
{ debug0(DST, DD, "[ SuppressScope()");
  suppress_scope = TRUE;
} /* end SuppressScope */

void UnSuppressScope(void)
{ debug0(DST, DD, "] UnSuppressScope()");
  suppress_scope = FALSE;
} /* end UnSuppressScope */


/*****************************************************************************/
/*                                                                           */
/*  SwitchScope(sym)                                                         */
/*  UnSwitchScope(sym)                                                       */
/*                                                                           */
/*  Switch to the scope of sym (if nilobj, StartSym); and switch back again. */
/*                                                                           */
/*****************************************************************************/

void SwitchScope(OBJECT sym)
{ int i;
  OBJECT new_scopes[MAX_STACK];
  if( sym == nilobj )  PushScope(StartSym, FALSE, FALSE);
  else
  { i = 0;
    while( sym != StartSym )
    { new_scopes[i++] = enclosing(sym);
      sym = enclosing(sym);
    }
    while( i > 0 )  PushScope(new_scopes[--i], FALSE, FALSE);
  }
}

void UnSwitchScope(OBJECT sym)
{ if( sym == nilobj )  PopScope();
  else
  { while( sym != StartSym )
    { PopScope();
      sym = enclosing(sym);
    }
  }
}


/*****************************************************************************/
/*                                                                           */
/*  BodyParAllowed()                                                         */
/*  BodyParNotAllowed()                                                      */
/*                                                                           */
/*  Allow or disallow invocations of the body parameter of the current tos.  */
/*                                                                           */
/*****************************************************************************/

void BodyParAllowed(void)
{ debug0(DST, DD, "BodyParAllowed()");
  body_ok[scope_top-1] = TRUE;
} /* end BodyParAllowed */

void BodyParNotAllowed(void)
{ debug0(DST, DD, "BodyParNotAllowed()");
  body_ok[scope_top-1] = FALSE;
} /* end BodyParNotAllowed */


/*****************************************************************************/
/*                                                                           */
/*  DebugScope(void)                                                         */
/*                                                                           */
/*  Debug print of current scope stack                                       */
/*                                                                           */
/*****************************************************************************/

void DebugScope(void)
{ int i;
  if( suppress_scope )
  {
    debug0(DST, D, "suppressed");
  }
  else for( i = 0;  i < scope_top;  i++ )
  { debug6(DST, D, "%s %s%s%s%s%s",
      i == scope_top - 1 ? "->" : "  ",
      SymName(scope[i]),
      npars_only[i] ? " npars_only" : "",
      vis_only[i]   ? " vis_only"   : "",
      body_ok[i]    ? " body_ok"    : "",
      i == scope_top - 1 && suppress_visible ? " suppress_visible" : "");
  }
} /* end DebugScope */


/*@::ScopeSnapshot()@*********************************************************/
/*                                                                           */
/*  OBJECT GetScopeSnapshot()                                                */
/*  LoadScopeSnapshot(ss)                                                    */
/*  ClearScopeSnapshot(ss)                                                   */
/*                                                                           */
/*  A scope snapshot is a complete record of the state of the scope stack    */
/*  at some moment.  These routines allow you to take a scope snapshot,      */
/*  then subsequently load it (i.e. make it the current scope), then         */
/*  subsequently clear it (i.e. return to whatever was before the Load).     */
/*                                                                           */
/*****************************************************************************/

OBJECT GetScopeSnapshot()
{ OBJECT ss, x;  int i;
  New(ss, ACAT);
  for( i = scope_top-1;  scope[i] != StartSym;  i-- )
  {
    New(x, SCOPE_SNAPSHOT);
    Link(ss, x);
    Link(x, scope[i]);
    ss_npars_only(x) = npars_only[i];
    ss_vis_only(x) = vis_only[i];
    ss_body_ok(x) = body_ok[i];
  }
  ss_suppress(ss) = suppress_visible;
  return ss;
} /* end GetScopeSnapshot */


void LoadScopeSnapshot(OBJECT ss)
{ OBJECT link, x, sym;  BOOLEAN tmp;
  assert( type(ss) == ACAT, "LoadScopeSnapshot: type(ss)!" );
  PushScope(StartSym, FALSE, FALSE);
  for( link = LastDown(ss);  link != ss;  link = PrevDown(link) )
  { Child(x, link);
    assert( type(x) == SCOPE_SNAPSHOT, "LoadScopeSnapshot: type(x)!" );
    Child(sym, Down(x));
    PushScope(sym, ss_npars_only(x), ss_vis_only(x));
    body_ok[scope_top-1] = ss_body_ok(x);
  }
  tmp = suppress_visible;
  suppress_visible = ss_suppress(ss);
  ss_suppress(ss) = tmp;
  debug0(DST, D, "after LoadScopeSnapshot, scope is:")
  ifdebug(DST, D, DebugScope());
} /* end LoadScopeSnapshot */


void ClearScopeSnapshot(OBJECT ss)
{
  while( scope[scope_top-1] != StartSym )
    scope_top--;
  scope_top--;
  suppress_visible = ss_suppress(ss);
} /* end ClearScopeSnapshot */


/*@::InsertSym()@*************************************************************/
/*                                                                           */
/*  OBJECT InsertSym(str, xtype, xfpos, xprecedence, indefinite, xrecursive, */
/*                                         xpredefined, xenclosing, xbody)   */
/*                                                                           */
/*  Insert a new symbol into the table.  Its string value is str.            */
/*  Initialise the symbol as the parameters indicate.                        */
/*  Return a pointer to the new symbol.                                      */
/*  If str is not a valid symbol name, InsertSym prints an error             */
/*  message and does not insert the symbol.                                  */
/*                                                                           */
/*****************************************************************************/

OBJECT InsertSym(FULL_CHAR *str, unsigned char xtype, FILE_POS *xfpos,
unsigned char xprecedence, BOOLEAN xindefinite, BOOLEAN xrecursive,
unsigned xpredefined, OBJECT xenclosing, OBJECT xbody)
{ register int sum, rlen;
  register unsigned char *x;
  OBJECT p, q, s, tmp, link, entry, plink;  int len;

  debug3(DST, DD, "InsertSym( %s, %s, in %s )",
	Image(xtype), str, SymName(xenclosing));
  if( !LexLegalName(str) )
    Error(29, 3, "invalid symbol name %s", WARN, xfpos, str);

  New(s, xtype);
  FposCopy(fpos(s), *xfpos);
  has_body(s)          = FALSE;
  filter(s)            = nilobj;
  use_invocation(s)    = nilobj;
  imports(s)           = nilobj;
  imports_encl(s)      = FALSE;
  right_assoc(s)       = TRUE;
  precedence(s)        = xprecedence;
  indefinite(s)        = xindefinite;
  recursive(s)         = xrecursive;
  predefined(s)        = xpredefined;
  enclosing(s)         = xenclosing;
  sym_body(s)          = xbody;
  base_uses(s)         = nilobj;
  uses(s)              = nilobj;
  marker(s)            = nilobj;
  cross_sym(s)         = nilobj;
  is_extern_target(s)  = FALSE;
  uses_extern_target(s)= FALSE;
  visible(s)           = FALSE;
  uses_galley(s)       = FALSE;
  horiz_galley(s)      = ROWM;
  has_compulsory(s)    = 0;
  is_compulsory(s)     = FALSE;

  uses_count(s)  = 0;
  dirty(s)       = FALSE;
  if( enclosing(s) != nilobj && type(enclosing(s)) == NPAR )
    dirty(s) = dirty(enclosing(s)) = TRUE;

  has_par(s)     = FALSE;
  has_lpar(s)    = FALSE;
  has_rpar(s)    = FALSE;
  if( is_par(type(s)) )  has_par(enclosing(s))  = TRUE;
  if( type(s) == LPAR )  has_lpar(enclosing(s)) = TRUE;
  if( type(s) == RPAR )  has_rpar(enclosing(s)) = TRUE;

  /* assign a code letter between a and z to any NPAR symbol */
  if( type(s) == NPAR )
  { if( LastDown(enclosing(s)) != enclosing(s) )
    { Child(tmp, LastDown(enclosing(s)));
      if( type(tmp) == NPAR )
      { if( npar_code(tmp) == 'z' || npar_code(tmp) == ' ' )
	  npar_code(s) = ' ';
	else
	  npar_code(s) = npar_code(tmp)+1;
      }
      else
	npar_code(s) = 'a';
    }
    else npar_code(s) = 'a';
  }

  has_target(s)  = FALSE;
  force_target(s) = FALSE;
  if( !StringEqual(str, KW_TARGET) ) is_target(s) = FALSE;
  else
  { is_target(s) = has_target(enclosing(s)) = TRUE;

    /* if @Target is found after @Key, take note of external target */
    if( has_key(enclosing(s)) && xbody != nilobj && is_cross(type(xbody)) )
    { if( LastDown(xbody) != Down(xbody) )
      { OBJECT sym;
	Child(sym, Down(xbody));
	if( type(sym) == CLOSURE )
	{ is_extern_target(actual(sym)) = TRUE;
	  uses_extern_target(actual(sym)) = TRUE;
	}
      }
    }
  }

  has_tag(s) = is_tag(s) = FALSE;
  has_key(s) = is_key(s) = FALSE;
  has_optimize(s) = is_optimize(s) = FALSE;
  has_merge(s) = is_merge(s) = FALSE;
  has_enclose(s) = is_enclose(s) = FALSE;
  if( enclosing(s) != nilobj && type(enclosing(s)) == LOCAL )
  {
    if( StringEqual(str, KW_TAG) )
      is_tag(s) = has_tag(enclosing(s)) = dirty(enclosing(s)) = TRUE;

    if( StringEqual(str, KW_OPTIMIZE) )
      is_optimize(s) = has_optimize(enclosing(s)) = TRUE;

    if( StringEqual(str, KW_KEY) )
    { is_key(s) = has_key(enclosing(s)) = dirty(enclosing(s)) = TRUE;

      /* if @Key is found after @Target, take note of external target */
      for( link=Down(enclosing(s));  link!=enclosing(s);  link=NextDown(link) )
      { Child(p, link);
	if( is_target(p) && sym_body(p)!=nilobj && is_cross(type(sym_body(p))) )
	{ OBJECT sym;
	  Child(sym, Down(sym_body(p)));
	  if( type(sym) == CLOSURE )
	  { is_extern_target(actual(sym)) = TRUE;
	    uses_extern_target(actual(sym)) = TRUE;
	  }
	}
      }
    } 

    if( StringEqual(str, KW_MERGE) )
      is_merge(s) = has_merge(enclosing(s)) = TRUE;

    if( StringEqual(str, KW_ENCLOSE) )
      is_enclose(s) = has_enclose(enclosing(s)) = TRUE;
  }

  if( StringEqual(str, KW_FILTER) )
  { if( type(s) != LOCAL || enclosing(s) == StartSym )
      Error(29, 4, "%s must be a local definition", WARN, &fpos(s), str);
    else if( !has_rpar(enclosing(s)) )
      Error(29, 14, "%s must lie within a symbol with a right parameter",
	WARN, &fpos(s), KW_FILTER);
    else
    { filter(enclosing(s)) = s;
      precedence(enclosing(s)) = FILTER_PREC;
    }
  }

  if( type(s) == RPAR && has_body(enclosing(s)) &&
    (is_tag(s) || is_key(s) || is_optimize(s)) )
    Error(29, 5, "a body parameter may not be named %s", WARN, &fpos(s), str);

  if( type(s) == RPAR && has_target(enclosing(s)) &&
    (is_tag(s) || is_key(s) || is_optimize(s)) )
    Error(29, 6, "the right parameter of a galley may not be called %s",
      WARN, &fpos(s), str);

  len = StringLength(str);
  hash(str, len, sum);

  ifdebug(DST, DD, sym_spread[sum]++;  sym_count++);
  entry = (OBJECT) &symtab[sum];
  for( plink = Down(entry);  plink != entry;  plink = NextDown(plink) )
  { Child(p, plink);
    if( length(p) == len && StringEqual(str, string(p)) )
    { for( link = Down(p);  link != p;  link = NextDown(link) )
      {	Child(q, link);
	if( enclosing(s) == enclosing(q) )
	{ Error(29, 7, "symbol %s previously defined at%s",
	    WARN, &fpos(s), str, EchoFilePos(&fpos(q)) );
	  if( AltErrorFormat )
	  {
	    Error(29, 13, "symbol %s previously defined here",
	      WARN, &fpos(q), str);
	  }
	  break;
	}
      }
      goto wrapup;
    }
  }

  /* need a new OBJECT as well as s */
  NewWord(p, WORD, len, xfpos);
  length(p) = len;
  StringCopy(string(p), str);
  Link(entry, p);

 wrapup:
  Link(p, s);
  if( enclosing(s) != nilobj ) Link(enclosing(s), s);
  debug2(DST, DD, "InsertSym Link(%s, %s) and returning.",
		SymName(enclosing(s)), SymName(s));
  return s;
} /* end InsertSym */


/*****************************************************************************/
/*                                                                           */
/*  InsertAlternativeName(str, s, xfpos)                                     */
/*                                                                           */
/*  Insert an alternative name for symbol s.                                 */
/*                                                                           */
/*****************************************************************************/

void InsertAlternativeName(FULL_CHAR *str, OBJECT s, FILE_POS *xfpos)
{ register int sum, rlen;
  register unsigned char *x;
  int len;
  OBJECT entry, link, plink, p, q;
  debug3(DST, DD, "InsertAlternativeName(%s, %s, %s)",
    str, SymName(s), EchoFilePos(xfpos));

  len = StringLength(str);
  hash(str, len, sum);

  ifdebug(DST, DD, sym_spread[sum]++;  sym_count++);
  entry = (OBJECT) &symtab[sum];
  for( plink = Down(entry);  plink != entry;  plink = NextDown(plink) )
  { Child(p, plink);
    if( length(p) == len && StringEqual(str, string(p)) )
    { for( link = Down(p);  link != p;  link = NextDown(link) )
      {	Child(q, link);
	if( enclosing(s) == enclosing(q) )
	{ Error(29, 12, "symbol name %s previously defined at%s",
	    WARN, &fpos(s), str, EchoFilePos(&fpos(q)) );
	  break;
	}
      }
      goto wrapup;
    }
  }

  /* need a new OBJECT as well as s */
  NewWord(p, WORD, len, xfpos);
  length(p) = len;
  StringCopy(string(p), str);
  Link(entry, p);

 wrapup:
  Link(p, s);
  /* not for copies if( enclosing(s) != nilobj ) Link(enclosing(s), s); */
  debug0(DST, DD, "InsertAlternativeName returning.");
} /* end InsertAlternativeName */


/*@::SearchSym(), SymName()@**************************************************/
/*                                                                           */
/*  OBJECT SearchSym(str, len)                                               */
/*                                                                           */
/*  Search the symbol table for str, with length len, and return an          */
/*  OBJECT referencing the entry if found.  Otherwise return nilobj.         */
/*                                                                           */
/*****************************************************************************/

OBJECT SearchSym(FULL_CHAR *str, int len)
{ register int rlen, sum;
  register FULL_CHAR *x, *y;
  OBJECT p, q, link, plink, entry;
  int s;

  debug2(DST, DDD, "SearchSym( %c..., %d )", str[0], len);

  hash(str, len, sum);
  rlen = len;
  entry = (OBJECT) &symtab[sum];
  for( plink = Down(entry);  plink != entry;  plink = NextDown(plink) )
  { Child(p, plink);
    if( rlen == length(p) )
    { x = str;  y = string(p);
      do; while( *x++ == *y++ && --rlen );
      if( rlen == 0 )
      {
	debug1(DST, DDD, "  found %s", string(p));
	s = scope_top;
	do
	{ s--;
	  for( link = Down(p);  link != p;  link = NextDown(link) )
	  { Child(q, link);
	    { debugcond4(DST, DDD, enclosing(q) == scope[s],
	       "  !npars_only[s] = %s, !vis_only[s] = %s, body_ok[s] = %s, !ss = %s",
	       bool(!npars_only[s]), bool(!vis_only[s]), bool(body_ok[s]),
	       bool(!suppress_scope));
	    }
	    if( enclosing(q) == scope[s]
	      && (!npars_only[s] || type(q) == NPAR)
	      && (!vis_only[s] || visible(q) || suppress_visible )
	      && (body_ok[s] || type(q)!=RPAR || !has_body(enclosing(q))
		  || suppress_visible )
	      && (!suppress_scope || StringEqual(string(p), KW_INCLUDE) ||
				     StringEqual(string(p), KW_SYSINCLUDE))
	    )
	    {	debug3(DST, DD, "SearchSym returning %s %s%%%s",
		  Image(type(q)), SymName(q), SymName(enclosing(q)));
		return q;
	    }
	  }
	} while( scope[s] != StartSym );
      }
    }
    rlen = len;
  }
  debug0(DST, DDD, "SearchSym returning <nilobj>");
  return nilobj;
} /* end SearchSym */


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *SymName(s)                                                    */
/*                                                                           */
/*  Return the string value of the name of symbol s.                         */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *SymName(OBJECT s)
{ OBJECT p;
  if( s == nilobj )  return AsciiToFull("<nilobj>");
  Parent(p, Up(s));
  assert( is_word(type(p)), "SymName: !is_word(type(p))!" );
  return string(p);
} /* end SymName */
	

/*@::FullSymName(), ChildSym()@***********************************************/
/*                                                                           */
/*  FULL_CHAR *FullSymName(x, str)                                           */
/*                                                                           */
/*  Return the path name of symbol x. with str separating each entry.        */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *FullSymName(OBJECT x, FULL_CHAR *str)
{ OBJECT stack[20];  int i;
  static FULL_CHAR buff[MAX_BUFF], *sname;
  if( x == nilobj )  return AsciiToFull("<nilobj>");
  assert( enclosing(x) != nilobj, "FullSymName: enclosing(x) == nilobj!" );
  for( i = 0;  enclosing(x) != nilobj && i < 20;  i++ )
  { stack[i] = x;
    x = enclosing(x);
  }
  StringCopy(buff, STR_EMPTY);
  for( i--;  i > 0;  i-- )
  { sname = SymName(stack[i]);
    if( StringLength(sname)+StringLength(str)+StringLength(buff) >= MAX_BUFF )
      Error(29, 8, "full name of symbol is too long", FATAL, &fpos(x));
    StringCat(buff, sname);
    StringCat(buff, str);
  }
  sname = SymName(stack[0]);
  if( StringLength(sname) + StringLength(buff) >= MAX_BUFF )
    Error(29, 9, "full name of symbol is too long", FATAL, &fpos(x));
  StringCat(buff, sname);
  return buff;
} /* end FullSymName */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT ChildSym(s, typ)                                                  */
/*                                                                           */
/*  Find the child of symbol s of type typ, either LPAR or RPAR.             */
/*                                                                           */
/*****************************************************************************/

OBJECT ChildSym(OBJECT s, unsigned typ)
{ OBJECT link, y;
  for( link = Down(s);  link != s;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == typ && enclosing(y) == s )  return y;
  }
  Error(29, 10, "symbol %s has missing %s", FATAL, &fpos(s),
    SymName(s), Image(typ));
  return nilobj;
} /* end ChildSym */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT ChildSymWithCode(s, code)                                         */
/*                                                                           */
/*  Find the child of symbol s with the given npar code, else nil.           */
/*                                                                           */
/*****************************************************************************/

OBJECT ChildSymWithCode(OBJECT s, unsigned char code)
{ OBJECT link, y;
  for( link = Down(actual(s));  link != actual(s);  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == NPAR && enclosing(y) == actual(s) && npar_code(y) == code )
      return y;
  }
  Error(29, 11, "symbol %s has erroneous code %c (database out of date?)",
    FATAL, &fpos(s), SymName(actual(s)), (char) code);
  return nilobj;
} /* end ChildSym */


/*@::CheckSymSpread(), DeleteSymBody()@***************************************/
/*                                                                           */
/*  CheckSymSpread()                                                         */
/*                                                                           */
/*  Check the spread of symbols through the hash table.                      */
/*                                                                           */
/*****************************************************************************/
#if DEBUG_ON

void CheckSymSpread(void)
{ int i, j, sum, usum;  OBJECT entry, plink;
  debug2(DST, DD, "Symbol table spread (table size = %d, symbols = %d):",
    MAX_TAB, sym_count);
  usum = sum = 0;
  for( i = 0;  i < MAX_TAB;  i++ )
  { fprintf(stderr, "%4d: ", i);
    for( j = 1;  j <= sym_spread[i];  j++ )
    { fprintf(stderr, ".");
      sum += j;
    }
    entry = (OBJECT) &symtab[i];
    for( plink=Down(entry), j=1;  plink != entry;  plink=NextDown(plink), j++ )
    { fprintf(stderr, "+");
      usum += j;
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "average length counting duplicate names = %.1f\n",
	(float) sum / sym_count);
  fprintf(stderr, "average length not counting duplicate names = %.1f\n",
	(float) usum / sym_count);
} /* end CheckSymSpread */


/*****************************************************************************/
/*                                                                           */
/*  static DeleteSymBody(s)                                                  */
/*                                                                           */
/*  Delete the body of symbol s.                                             */
/*                                                                           */
/*****************************************************************************/

static void DeleteSymBody(OBJECT s)
{ OBJECT t;
  debug1(DST, DDD, "DeleteSymBody( %s )", SymName(s));
  switch( type(s) )
  {
    case MACRO:	while( sym_body(s) != nilobj )
		{ t = sym_body(s);
		  sym_body(s) = Delete(sym_body(s), PARENT);
		  Dispose(t);
		}
		break;
	
    case LPAR:
    case NPAR:
    case RPAR:
    case LOCAL:	if( sym_body(s) != nilobj ) DisposeObject(sym_body(s));
		break;

    default:	assert1(FALSE, "DeleteSymBody:", Image(type(s)));
		break;
  }
  debug0(DST, DDD, "DeleteSymBody returning.");
} /* end DeleteSymBody */


/*@::DeleteEverySym()@********************************************************/
/*                                                                           */
/*  DeleteEverySym()                                                         */
/*                                                                           */
/*  Delete every symbol in the symbol table.                                 */
/*  Note that we first delete all bodies, then the symbols themselves.       */
/*  This is so that the closures within the bodies have well-defined         */
/*  actual() pointers, even while the symbol table is being disposed.        */
/*  If this is not done, debug output during the disposal gets confused.     */
/*                                                                           */
/*****************************************************************************/

void DeleteEverySym(void)
{ int i, j, load, cost;  OBJECT p, plink, link, x, entry;
  debug0(DST, DD, "DeleteEverySym()");

  /* dispose the bodies of all symbols */
  for( i = 0;  i < MAX_TAB;  i++ )
  { entry = (OBJECT) &symtab[i];
    for( plink = Down(entry);  plink != entry;  plink = NextDown(plink) )
    { Child(p, plink);
      for( link = Down(p);  link != p;  link = NextDown(link) )
      {	Child(x, link);  DeleteSymBody(x);
	/* *** will not work now
	while( base_uses(x) != nilobj )
	{ tmp = base_uses(x);  base_uses(x) = next(tmp);
	  PutMem(tmp, USES_SIZE);
	}
	while( uses(x) != nilobj )
	{ tmp = uses(x);  uses(x) = next(tmp);
	  PutMem(tmp, USES_SIZE);
	}
	*** */
      }
    }
  }

  /* dispose the symbol name strings, gather statistics, and print them */
  load = cost = 0;
  for( i = 0;  i < MAX_TAB;  i++ )
  { j = 1; entry = (OBJECT) &symtab[i];
    while( Down(entry) != entry )
    { load += 1;  cost += j;  j += 1;
      DisposeChild(Down(entry));
    }
  }
  if( load > 0 )
  { debug4(DST, DD, "size = %d, items = %d (%d%%), probes = %.1f",
      MAX_TAB, load, (100*load)/MAX_TAB, (float) cost/load);
  }
  else
  { debug1(DST, DD, "table size = %d, no entries in table", MAX_TAB);
  }
  debug0(DST, DD, "DeleteEverySym returning.");
} /* end DeleteEverySym */
#endif
