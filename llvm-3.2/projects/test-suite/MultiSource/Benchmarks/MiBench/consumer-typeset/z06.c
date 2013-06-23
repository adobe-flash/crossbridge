/*@z06.c:Parser:PushObj(), PushToken(), etc.@*********************************/
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
/*  FILE:         z06.c                                                      */
/*  MODULE:       Parser                                                     */
/*  EXTERNS:      InitParser(), Parse()                                      */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define	LEFT_ASSOC	0
#define	RIGHT_ASSOC	1

#define	PREV_OP		0		/* means an operator was previous    */
#define	PREV_OBJ	1		/* prev was object not ending in RBR */
#define	PREV_RBR	2		/* prev was object ending in RBR     */

static	OBJECT		cross_name;	/* name of the cr database   */


#define	MAX_STACK	100			/* size of parser stacks     */
static	OBJECT		obj_stack[MAX_STACK];	/* stack of objects          */
static	int		otop = -1;		/* top of obj_stack          */
static	OBJECT		tok_stack[MAX_STACK];	/* stack of tokens           */
static	int		ttop = -1;		/* top of tok_stack          */
static	int		unknown_count = 0;	/* no. of unknown symbols    */
#if DEBUG_ON
static	BOOLEAN		debug_now = FALSE;	/* TRUE when want to debug   */
#endif


/*****************************************************************************/
/*                                                                           */
/*  OBJECT OptimizeCase(x)                                                   */
/*                                                                           */
/*  Optimize the @Case expression x, which is known to be of the form        */
/*  "@BackEnd @Case ...", by evaluating it immediately if its choices        */
/*  are all literal words or "else".                                         */
/*                                                                           */
/*****************************************************************************/

static void check_yield(OBJECT y, OBJECT *res_yield, BOOLEAN *all_literals)
{ OBJECT s1, link, z;
  Child(s1, Down(y));
  debug1(DOP, DD, "  checkyield(%s)", EchoObject(y));
  if( is_word(type(s1)) )
  { if( StringEqual(string(s1), BackEnd->name) ||
	StringEqual(string(s1),STR_ELSE) )
      if( *res_yield == nilobj )  *res_yield = y;
  }
  else if( type(s1) == ACAT )
  { for( link = Down(s1);  link != s1;  link = NextDown(link) )
    { Child(z, link);
      if( type(z) == GAP_OBJ )  continue;
      if( is_word(type(z)) )
      { if( StringEqual(string(z), BackEnd->name) ||
	    StringEqual(string(s1), STR_ELSE))
	  if( *res_yield == nilobj )  *res_yield = y;
      }
      else
      { *all_literals = FALSE;
	*res_yield = nilobj;
	break;
      }
    }
  }
  else
  { *all_literals = FALSE;
    *res_yield = nilobj;
  }
  debug2(DOP, DD, "  checkyield returning (%s, %s)", EchoObject(*res_yield),
    bool(*all_literals));
}

OBJECT OptimizeCase(OBJECT x)
{ OBJECT link, s2, y, res_yield, res;  BOOLEAN all_literals;  
  debug1(DOP, DD, "OptimizeCase(%s)", EchoObject(x));
  assert( type(x) == CASE, "OptimizeCase:  type(x) != CASE!" );

  Child(s2, LastDown(x));
  all_literals = TRUE;  res_yield = nilobj;
  if( type(s2) == YIELD )
  { check_yield(s2, &res_yield, &all_literals);
  }
  else if( type(s2) == ACAT )
  { for( link = Down(s2);  link != s2 && all_literals;  link = NextDown(link) )
    {
      Child(y, link);
      debug2(DOP, DD, "  OptimizeCase examining %s %s", Image(type(y)),
	EchoObject(y));
      if( type(y) == GAP_OBJ )  continue;
      if( type(y) == YIELD )
      { check_yield(y, &res_yield, &all_literals);
      }
      else
      { all_literals = FALSE;
	res_yield = nilobj;
      }
    }
  }
  else
  { all_literals = FALSE;
    res_yield = nilobj;
  }

  if( all_literals && res_yield != nilobj )
  { Child(res, LastDown(res_yield));
    DeleteLink(Up(res));
    DisposeObject(x);
  }
  else
  { res = x;
  }

  debug1(DOP, DD, "OptimizeCase returning %s", EchoObject(res));
  return res;
} /* end OptimizeCase */


/*****************************************************************************/
/*                                                                           */
/*  HuntCommandOptions(x)                                                    */
/*                                                                           */
/*  See if any of the command-line options apply to closure x.  If so,       */
/*  change x to reflect the overriding command line option.                  */
/*                                                                           */
/*****************************************************************************/

static void HuntCommandOptions(OBJECT x)
{ OBJECT colink, coname, coval, opt, y, link, sym;  BOOLEAN found;
  debug1(DOP, DD, "HuntCommandOptions(%s)", SymName(actual(x)));
  sym = actual(x);
  for( colink = Down(CommandOptions);  colink != CommandOptions;
    colink = NextDown(NextDown(colink)) )
  {
    Child(coname, colink);
    Child(coval, NextDown(colink));
    debug2(DOP, DD, "  hunting \"%s\" with value \"%s\"", string(coname),
      EchoObject(coval));

    /* set found to TRUE iff coname is the name of an option of x */
    found = FALSE;
    for( link = Down(sym);  link != sym;  link = NextDown(link) )
    { Child(opt, link);
      if( type(opt) == NPAR && StringEqual(SymName(opt), string(coname)) )
      { found = TRUE;
	debug2(DOP, DD, "  %s is an option of %s", string(coname),SymName(sym));
	break;
      }
    }

    if( found )
    {
      /* see whether this option is already set within x */
      found = FALSE;
      for( link = Down(x);  link != x;  link = NextDown(link) )
      { Child(y, link);
	if( type(y) == PAR && actual(y) == opt )
	{ found = TRUE;
	  debug2(DOP, DD, "  %s is set in %s", string(coname), SymName(sym));
	  break;
	}
      }

      if( found )
      {
	/* option exists already in x: replace it with oval */
	DisposeChild(Down(y));
	Link(y, coval);
	debug2(DOP, DD, "  replacing %s value with %s; x =", string(coname),
	  EchoObject(coval));
	ifdebug(DOP, DD, DebugObject(x));
      }
      else
      {
	/* option applies to x but has not yet been set in x */
	New(y, PAR);
	Link(x, y);
	actual(y) = opt;
	Link(y, coval);
	debug2(DOP, DD, "  inserting %s with value %s; x =", string(coname),
	  EchoObject(coval));
	ifdebug(DOP, DD, DebugObject(x));
      }
    }
  }
  debug1(DOP, DD, "HuntCommandOptions(%s) returning", SymName(sym));
} /* end HuntCommandOptions */


/*****************************************************************************/
/*                                                                           */
/*  PushObj(x)                                                               */
/*  PushToken(t)                                                             */
/*  OBJECT PopObj()                                                          */
/*  OBJECT PopToken()                                                        */
/*  OBJECT TokenTop                                                          */
/*  OBJECT ObjTop                                                            */
/*                                                                           */
/*  Push and pop from the object and token stacks; examine top item.         */
/*                                                                           */
/*****************************************************************************/

#define PushObj(x)							\
{ zz_hold = x;								\
  if( ++otop < MAX_STACK ) obj_stack[otop] = zz_hold;			\
  else Error(6, 1, "expression is too deeply nested",			\
    FATAL, &fpos(obj_stack[otop-1]));					\
}

#define PushToken(t)							\
{ if( ++ttop < MAX_STACK ) tok_stack[ttop] = t;				\
  else Error(6, 2, "expression is too deeply nested",			\
	 FATAL, &fpos(tok_stack[ttop-1]));				\
}

#define PopObj()	obj_stack[otop--]
#define PopToken()	tok_stack[ttop--]
#define	TokenTop	tok_stack[ttop]
#define	ObjTop		obj_stack[otop]


/*@::DebugStacks(), InsertSpace()@********************************************/
/*                                                                           */
/*  DebugStacks()                                                            */
/*                                                                           */
/*  Print debug output of the stacks state                                   */
/*                                                                           */
/*****************************************************************************/

#if DEBUG_ON
static void DebugStacks(int initial_ttop, int obj_prev)
{ int i;
  debug3(ANY, D, "  obj_prev: %s; otop: %d; ttop: %d",
    obj_prev == PREV_OP ? "PREV_OP" : obj_prev == PREV_OBJ ? "PREV_OBJ" :
    obj_prev == PREV_RBR ? "PREV_RBR" : "???", otop, ttop);
  for( i = 0;  i <= otop; i++ )
    debug3(ANY, D, "  obj[%d] = (%s) %s", i,
       Image(type(obj_stack[i])), EchoObject(obj_stack[i]));
  for( i = 0;  i <= ttop;  i++ )
  { if( i == initial_ttop+1 ) debug0(DOP, DD, "  $");
    debug3(ANY, D, "  tok[%d] = %s (precedence %d)", i,
      type(tok_stack[i]) == CLOSURE ?
	SymName(actual(tok_stack[i])) : Image(type(tok_stack[i])),
      precedence(tok_stack[i]));
  }
} /* end DebugStacks */
#endif


/*****************************************************************************/
/*                                                                           */
/*  InsertSpace(t)                                                           */
/*                                                                           */
/*  Add any missing catenation operator in front of token t.                 */
/*                                                                           */
/*****************************************************************************/

#define InsertSpace(t)							\
if( obj_prev )								\
{ int typ, prec;							\
  if( hspace(t) + vspace(t) > 0 )					\
    typ = TSPACE, prec = ACAT_PREC;					\
  else if( type(t) == LBR || obj_prev == PREV_RBR )			\
    typ = TJUXTA, prec = ACAT_PREC;					\
  else									\
    typ = TJUXTA, prec = JUXTA_PREC;					\
  debugcond1(DOP, DD, debug_now, "[ InsertSpace(%s)", Image(typ));	\
  while( obj_prev && precedence(TokenTop) >= prec )			\
    obj_prev = Reduce();						\
  if( obj_prev )							\
  { New(tmp, typ);  precedence(tmp) = prec;				\
    vspace(tmp) = vspace(t);  hspace(tmp) = hspace(t);			\
    mark(gap(tmp)) = FALSE;  join(gap(tmp)) = TRUE;			\
    FposCopy(fpos(tmp), fpos(t));					\
    PushToken(tmp);							\
  }									\
  debugcond0(DOP, DD, debug_now, "] end InsertSpace()");		\
} /* end InsertSpace */


/*@::Shift(), ShiftObj()@*****************************************************/
/*                                                                           */
/*  static Shift(t, prec, rassoc, leftpar, rightpar)                         */
/*  static ShiftObj(t)                                                       */
/*                                                                           */
/*  Shift token or object t onto the stacks; it has the attributes shown.    */
/*                                                                           */
/*****************************************************************************/

#define Shift(t, prec, rassoc, leftpar, rightpar)			\
{ debugcond5(DOP, DD, debug_now, "[ Shift(%s, %d, %s, %s, %s)",		\
    Image(type(t)), prec, rassoc ? "rightassoc" : "leftassoc",		\
      leftpar ? "lpar" : "nolpar", rightpar ? "rpar" : "norpar");	\
  if( leftpar )								\
  { for(;;)								\
    { if( !obj_prev )							\
      {	PushObj( MakeWord(WORD, STR_EMPTY, &fpos(t)) );			\
	obj_prev = PREV_OBJ;						\
      }									\
      else if( precedence(TokenTop) >= prec + rassoc )			\
      { obj_prev = Reduce();						\
	if( ttop == initial_ttop )					\
	{ *token = t;							\
	  debugcond0(DOP, DD, debug_now,				\
	    "] ] end Shift() and Parse(); stacks are:");		\
	  ifdebugcond(DOP, DD, debug_now,				\
	    DebugStacks(initial_ttop, obj_prev));			\
	  return PopObj();						\
	}								\
      }									\
      else break;							\
    }									\
  }									\
  else InsertSpace(t);							\
  PushToken(t);								\
  if( rightpar )  obj_prev = FALSE;					\
  else									\
  { obj_prev = Reduce(); 						\
    if( ttop == initial_ttop )						\
    { *token = nilobj;							\
      debugcond0(DOP, DD, debug_now,					\
	"] ] end Shift and Parse; stacks are:");			\
      ifdebugcond(DOP, DD, debug_now,					\
	DebugStacks(initial_ttop, obj_prev));				\
      return PopObj();							\
    }									\
  }									\
  debugcond0(DOP, DD, debug_now, "] end Shift()");			\
} /* end Shift */


#define ShiftObj(t, new_obj_prev)					\
{ debugcond1(DOP, DD, debug_now, "[ ShiftObj(%s)", Image(type(t)));	\
  InsertSpace(t);							\
  PushObj(t);								\
  obj_prev = new_obj_prev;						\
  debugcond0(DOP, DD, debug_now, "] end ShiftObj()");			\
}

/*@::Reduce()@****************************************************************/
/*                                                                           */
/*  static Reduce()                                                          */
/*                                                                           */
/*  Perform a single reduction of the stacks.                                */
/*                                                                           */
/*****************************************************************************/

static BOOLEAN Reduce(void)
{ OBJECT p1, p2, p3, s1, s2, tmp;
  OBJECT op;  int obj_prev;
  debugcond0(DOP, DD, debug_now, "[ Reduce()");
  /* ifdebugcond(DOP, DD, debug_now, DebugStacks(0, TRUE)); */

  op = PopToken();
  obj_prev = PREV_OBJ;
  switch( type(op) )
  {

    case GSTUB_INT:
    case GSTUB_EXT:
    
	debug0(DGT, D, "calling TransferEnd( PopObj() ) from Reduce()");
	TransferEnd( PopObj() );
	New(p1, NULL_CLOS);
	PushObj(p1);
	Dispose(op);
	break;


    case GSTUB_NONE:

	New(p1, NULL_CLOS);
	PushObj(p1);
	Dispose(op);
	break;


    case NULL_CLOS:
    case PAGE_LABEL:
    case BEGIN_HEADER:
    case END_HEADER:
    case SET_HEADER:
    case CLEAR_HEADER:
    case ONE_COL:
    case ONE_ROW:
    case WIDE:
    case HIGH:
    case HSHIFT:
    case VSHIFT:
    case HSCALE:
    case VSCALE:
    case HCOVER:
    case VCOVER:
    case SCALE:
    case KERN_SHRINK:
    case HCONTRACT:
    case VCONTRACT:
    case HLIMITED:
    case VLIMITED:
    case HEXPAND:
    case VEXPAND:
    case START_HVSPAN:
    case START_HSPAN:
    case START_VSPAN:
    case HSPAN:
    case VSPAN:
    case PADJUST:
    case HADJUST:
    case VADJUST:
    case ROTATE:
    case BACKGROUND:
    case YIELD:
    case BACKEND:
    case XCHAR:
    case FONT:
    case SPACE:
    case YUNIT:
    case ZUNIT:
    case BREAK:
    case UNDERLINE:
    case COLOUR:
    case OUTLINE:
    case LANGUAGE:
    case CURR_LANG:
    case CURR_FAMILY:
    case CURR_FACE:
    case CURR_YUNIT:
    case CURR_ZUNIT:
    case COMMON:
    case RUMP:
    case MELD:
    case INSERT:
    case ONE_OF:
    case NEXT:
    case PLUS:
    case MINUS:
    case TAGGED:
    case INCGRAPHIC:
    case SINCGRAPHIC:
    case PLAIN_GRAPHIC:
    case GRAPHIC:
    case LINK_SOURCE:
    case LINK_DEST:
    case OPEN:
    case RAW_VERBATIM:
    case VERBATIM:

	if( has_rpar(actual(op)) )
	{ s2 = PopObj();
	  Link(op, s2);
	}
	if( has_lpar(actual(op)) )
	{ s1 = PopObj();
	  Link(Down(op), s1);
	}
	PushObj(op);
	break;


    case CASE:

	if( has_rpar(actual(op)) )
	{ s2 = PopObj();
	  Link(op, s2);
	}
	if( has_lpar(actual(op)) )
	{ s1 = PopObj();
	  Link(Down(op), s1);
	  if( type(s1) == BACKEND )
	  { op = OptimizeCase(op);
	  }
	}
	PushObj(op);
	break;


    case CROSS:
    case FORCE_CROSS:

	s2 = PopObj();
	Link(op, s2);
	s1 = PopObj();
	Link(Down(op), s1);
	if( type(s1) != CLOSURE )
	  Error(6, 3, "left parameter of %s is not a symbol (or not visible)",
	    WARN, &fpos(s1), Image(type(op)));
	PushObj(op);
	break;


    case CLOSURE:
    
	if( has_rpar(actual(op)) )
	{ New(s2, PAR);
	  tmp = PopObj();
	  Link(s2, tmp);
	  FposCopy(fpos(s2), fpos(tmp));
	  actual(s2) = ChildSym(actual(op), RPAR);
	  Link(op, s2);
	}
	if( has_lpar(actual(op)) )
	{ New(s1, PAR);
	  tmp = PopObj();
	  Link(s1, tmp);
	  FposCopy(fpos(s1), fpos(tmp));
	  actual(s1) = ChildSym(actual(op), LPAR);
	  Link(Down(op), s1);
	}
	PushObj(op);
	break;


    case LBR:
    
	Error(6, 4, "unmatched %s (inserted %s)", WARN, &fpos(op),
	  KW_LBR, KW_RBR);
	Dispose(op);
	obj_prev = PREV_RBR;
	break;


    case BEGIN:
    
	assert1(FALSE, "Reduce: unmatched", KW_BEGIN);
	break;


    case RBR:
    
	if( type(TokenTop) == LBR )
	{ /* *** FposCopy(fpos(ObjTop), fpos(TokenTop)); *** */
	  Dispose( PopToken() );
	}
	else if( type(TokenTop) == BEGIN )
	{ if( file_num(fpos(TokenTop)) > 0 )
	    Error(6, 5, "unmatched %s; inserted %s at%s (after %s)",
	      WARN, &fpos(op), KW_RBR, KW_LBR,
	      EchoFilePos(&fpos(TokenTop)), KW_BEGIN);
	  else
	    Error(6, 6, "unmatched %s not enclosed in anything",
	      FATAL, &fpos(op), KW_RBR);
	}
	else 
	{ assert1(FALSE, "Reduce: unmatched", KW_RBR);
	}
	Dispose(op);
	obj_prev = PREV_RBR;
	break;


    case END:
    
	if( type(TokenTop) != BEGIN )
	{ assert1(FALSE, "Reduce: unmatched", KW_END);
	}
	else
	{ if( actual(op) != actual(TokenTop) )
	  {
	    if( actual(op) == StartSym )
	      Error(6, 7, "%s %s appended at end of file to match %s at%s",
		WARN, &fpos(op), KW_END, SymName(actual(TokenTop)),
		KW_BEGIN, EchoFilePos(&fpos(TokenTop)) );
	    else if( actual(op) == nilobj )
	      Error(6, 8, "%s replaced by %s %s to match %s at%s",
	        WARN, &fpos(op), KW_END, KW_END,
		actual(TokenTop) == nilobj ? AsciiToFull("??") :
		  SymName(actual(TokenTop)),
		KW_BEGIN, EchoFilePos(&fpos(TokenTop)) );
	    else
	      Error(6, 9, "%s %s replaced by %s %s to match %s at%s",
	        WARN, &fpos(op), KW_END, SymName(actual(op)),
		KW_END, SymName(actual(TokenTop)),
		KW_BEGIN, EchoFilePos(&fpos(TokenTop)) );
	  }
	  Dispose( PopToken() );
	}
	Dispose(op);
	obj_prev = PREV_RBR;
	break;


    case GAP_OBJ:

	p1 = PopObj();
	Link(op, p1);
	PushObj(op);
	obj_prev = PREV_OP;
	break;


    case VCAT:
    case HCAT:
    case ACAT:
    
	p3 = PopObj();  p2 = PopObj();  p1 = PopObj();
	if( type(p1) == type(op) )
	{ Dispose(op);
	}
	else
	{ Link(op, p1);
	  p1 = op;
	}
	Link(p1, p2);
	Link(p1, p3);
	PushObj(p1);
	break;


    case TSPACE:
    case TJUXTA:

	p2 = PopObj();  p1 = PopObj();
	if( type(p1) != ACAT )
	{ New(tmp, ACAT);
	  Link(tmp, p1);
	  FposCopy(fpos(tmp), fpos(p1));
	  p1 = tmp;
	}
	type(op) = GAP_OBJ;
	Link(p1, op);
	Link(p1, p2);
	PushObj(p1);
	break;


    default:
    
	assert1(FALSE, "Reduce:", Image(type(op)));
	break;

  } /* end switch */
  debugcond1(DOP, DD, debug_now, "] end Reduce(), returning %s",
    obj_prev == PREV_OP ? "PREV_OP" : obj_prev == PREV_OBJ ? "PREV_OBJ" :
    obj_prev == PREV_RBR ? "PREV_RBR" : "???");
  return obj_prev;
} /* end Reduce */


/*@::SetScope(), InitParser()@************************************************/
/*                                                                           */
/*  SetScope(env, count, vis_only)                                           */
/*                                                                           */
/*  Push scopes required to parse object whose environment is env.           */
/*  Add to *count the number of scope pushes made.                           */
/*                                                                           */
/*  If vis_only is true, we only want visible things of the top-level        */
/*  element of env to be visible in this scope.                              */
/*                                                                           */
/*****************************************************************************/

void SetScope(OBJECT env, int *count, BOOLEAN vis_only)
{ OBJECT link, y, yenv;  BOOLEAN visible_only;
  debugcond2(DOP,DD, debug_now, "[ SetScope(%s, %d)", EchoObject(env), *count);
  assert( env != nilobj && type(env) == ENV, "SetScope: type(env) != ENV!" );
  if( Down(env) != env )
  { Child(y, Down(env));
    assert( LastDown(y) != y, "SetScope: LastDown(y)!" );
    link = LastDown(env) != Down(env) ? LastDown(env) : LastDown(y);
    Child(yenv, link);
    assert( type(yenv) == ENV, "SetScope: type(yenv) != ENV!" );
    SetScope(yenv, count, FALSE);
    visible_only = vis_only || (use_invocation(actual(y)) != nilobj);
    /* i.e. from @Use clause */
    PushScope(actual(y), FALSE, visible_only);  (*count)++;
    /*** this following was a bright idea that did not work owing to
	 allowing body parameters at times they definitely shouldn't be
    BodyParAllowed();
    ***/
  }
  debugcond1(DOP, DD, debug_now, "] SetScope returning, count = %d", *count);
} /* end SetScope */


/*****************************************************************************/
/*                                                                           */
/*  InitParser()                                                             */
/*                                                                           */
/*  Initialise the parser to contain just GstubExt.                          */
/*  Remember cross_db, the name of the cross reference database, for Parse.  */
/*                                                                           */
/*****************************************************************************/

void InitParser(FULL_CHAR *cross_db)
{ if( StringLength(cross_db) >= MAX_WORD )
    Error(6, 10, "cross reference database file name %s is too long",
      FATAL, no_fpos, cross_db);
  cross_name = MakeWord(WORD, cross_db, no_fpos);
  PushToken( NewToken(GSTUB_EXT, no_fpos, 0, 0, DEFAULT_PREC, StartSym) );
} /* end InitParser */


/*@::ParseEnvClosure()@*******************************************************/
/*                                                                           */
/*  static OBJECT ParseEnvClosure(t, encl)                                   */
/*                                                                           */
/*  Parse an object which is a closure with environment.  Consume the        */
/*  concluding @LClos.                                                       */
/*                                                                           */
/*****************************************************************************/

static OBJECT ParseEnvClosure(OBJECT t, OBJECT encl)
{ OBJECT env, res, y;  int count, i;
  debugcond0(DOP, DDD, debug_now, "ParseEnvClosure(t, encl)");
  assert( type(t) == ENV, "ParseEnvClosure: type(t) != ENV!" );
  env = t;  t = LexGetToken();
  while( type(t) != CLOS )  switch( type(t) )
  {
    case LBR:	count = 0;
		SetScope(env, &count, FALSE);
		y = Parse(&t, encl, FALSE, FALSE);
		if( type(y) != CLOSURE )
		{
		  debug1(DIO, D, "  Parse() returning %s:", Image(type(y)));
		  ifdebug(DIO, D, DebugObject(y));
		  Error(6, 11, "syntax error in cross reference database",
		    FATAL, &fpos(y));
		}
		for( i = 1;  i <= count;  i++ )  PopScope();
		AttachEnv(env, y);
		debug0(DCR, DDD, "  calling SetEnv from ParseEnvClosure (a)");
		env = SetEnv(y, nilobj);
		t = LexGetToken();
		break;

    case ENV:	y = ParseEnvClosure(t, encl);
		debug0(DCR, DDD, "  calling SetEnv from ParseEnvClosure (b)");
		env = SetEnv(y, env);
		t = LexGetToken();
		break;

    default:	Error(6, 12, "error in cross reference database",
		  FATAL, &fpos(t));
		break;
  }
  Dispose(t);
  if( Down(env) == env || Down(env) != LastDown(env) )
    Error(6, 13, "error in cross reference database", FATAL, &fpos(env));
  Child(res, Down(env));
  DeleteNode(env);
  debugcond1(DOP, DDD, debug_now, "ParseEnvClosure ret. %s", EchoObject(res));
  assert( type(res) == CLOSURE, "ParseEnvClosure: type(res) != CLOSURE!" );
  return res;
} /* end ParseEnvClosure */


/*@::Parse()@*****************************************************************/
/*                                                                           */
/*  OBJECT Parse(token, encl, defs_allowed, transfer_allowed)                */
/*                                                                           */
/*  Parse input tokens, beginning with *token, looking for an object of the  */
/*  form { ... } or @Begin ... @End <sym>, and return the object.            */
/*  The parent definition is encl, and scope has been set appropriately.     */
/*  Parse reads up to and including the last token of the object             */
/*  (the right brace or <sym>), and returns nilobj in *token.                */
/*                                                                           */
/*  If defs_allowed == TRUE, there may be local definitions in the object.   */
/*  In this case, encl is guaranteed to be the enclosing definition.         */
/*                                                                           */
/*  If transfer_allowed == TRUE, the parser may transfer components to the   */
/*  galley handler as they are read.                                         */
/*                                                                           */
/*  Note: the lexical analyser returns "@End \Input" at end of input, so the */
/*  parser does not have to handle end of input separately.                  */
/*                                                                           */
/*****************************************************************************/

OBJECT Parse(OBJECT *token, OBJECT encl,
BOOLEAN defs_allowed, BOOLEAN transfer_allowed)
{ OBJECT t, x, tmp, xsym, env, y, link, res, imps, xlink;
  int i, offset, lnum, initial_ttop = ttop;
  int obj_prev, scope_count, compulsory_count;  BOOLEAN revealed;

  debugcond4(DOP, DD, debug_now, "[ Parse(%s, %s, %s, %s)", EchoToken(*token),
      SymName(encl), bool(defs_allowed), bool(transfer_allowed));
  assert( type(*token) == LBR || type(*token) == BEGIN, "Parse: *token!" );

  obj_prev = PREV_OP;
  Shift(*token, precedence(*token), 0, FALSE, TRUE);
  t = LexGetToken();
  if( defs_allowed )
  { ReadDefinitions(&t, encl, LOCAL);

    /* if error in definitions, stop now */
    if( ErrorSeen() )
      Error(6, 14, "exiting now", FATAL, &fpos(t));

    if( encl == StartSym )
    {
      /* read @Use, @Database, and @Prepend commands and defs and construct env */
      New(env, ENV);
      for(;;)
      {
	if( type(t) == WORD && (
	    StringEqual(string(t), KW_DEF)     ||
	    /* StringEqual(string(t), KW_FONTDEF) || */
	    StringEqual(string(t), KW_LANGDEF) ||
	    StringEqual(string(t), KW_MACRO)   ||
	    StringEqual(string(t), KW_IMPORT)  ||
	    StringEqual(string(t), KW_EXTEND)  ||
	    StringEqual(string(t), KW_EXPORT)  ) )
	{
	  ReadDefinitions(&t, encl, LOCAL);

          /* if error in definitions, stop now */
          if( ErrorSeen() )
	    Error(6, 39, "exiting now", FATAL, &fpos(t));

	}
	else if( type(t) == USE )
	{
	  OBJECT crs, res_env;  STYLE style;
	  Dispose(t);  t = LexGetToken();
	  if( type(t) != LBR )
	    Error(6, 15, "%s expected after %s", FATAL, &fpos(t),KW_LBR,KW_USE);
	  debug0(DOP, DD, "  Parse() calling Parse for @Use clause");
	  y = Parse(&t, encl, FALSE, FALSE);
	  if( is_cross(type(y)) )
	  { OBJECT z;
	    Child(z, Down(y));
	    if( type(z) == CLOSURE )
	    { crs = nilobj;
	      y = CrossExpand(y, env, &style, &crs, &res_env);
	      if( crs != nilobj )
	      { Error(6, 16, "%s or %s tag not allowed here",
		  FATAL, &fpos(y), KW_PRECEDING, KW_FOLLOWING);
	      }
	      HuntCommandOptions(y);
	      AttachEnv(res_env, y);
	      debug0(DCR, DDD, "  calling SetEnv from Parse (a)");
	      env = SetEnv(y, env);
	    }
	    else Error(6, 17, "invalid parameter of %s", FATAL, &fpos(y), KW_USE);
	  }
	  else if( type(y) == CLOSURE )
	  { if( use_invocation(actual(y)) != nilobj )
	      Error(6, 18, "symbol %s occurs in two %s clauses",
		FATAL, &fpos(y), SymName(actual(y)), KW_USE);
	    use_invocation(actual(y)) = y;
	    HuntCommandOptions(y);
	    AttachEnv(env, y);
	    debug0(DCR, DDD, "  calling SetEnv from Parse (b)");
	    env = SetEnv(y, nilobj);
	  }
	  else Error(6, 19, "invalid parameter of %s", FATAL, &fpos(y), KW_USE);
	  PushScope(actual(y), FALSE, TRUE);
	  t = LexGetToken();
        }
	else if( type(t) == PREPEND || type(t) == SYS_PREPEND )
	{ ReadPrependDef(type(t), encl);
	  Dispose(t);
	  t = LexGetToken();
	}
	else if( type(t) == DATABASE || type(t) == SYS_DATABASE )
	{ ReadDatabaseDef(type(t), encl);
	  Dispose(t);
	  t = LexGetToken();
	}
	else break;
      }

      /* transition point from defs to content; turn on debugging now */
#if DEBUG_ON
      debug_now = TRUE;
#endif
      debugcond4(DOP, DD, debug_now, "[ Parse (first) (%s, %s, %s, %s)",
	EchoToken(*token), SymName(encl), bool(defs_allowed),
	bool(transfer_allowed));

      /* load cross-references from previous run, open new cross refs */
      if( AllowCrossDb )
      {
	  NewCrossDb = DbCreate(MakeWord(WORD, string(cross_name), no_fpos));
	  OldCrossDb = DbLoad(cross_name, SOURCE_PATH, FALSE, nilobj,
	    InMemoryDbIndexes);
      }
      else OldCrossDb = NewCrossDb = nilobj;

      /* tidy up and possibly print symbol table */
      FlattenUses();
      ifdebug(DST, DD, DebugObject(StartSym));

      TransferInit(env);
      debug0(DMA, D, "at end of definitions:");
      ifdebug(DMA, D, DebugMemory());
    }
  }

  for(;;)
  { 
    debugcond0(DOP, DD, debug_now, "");
    ifdebugcond(DOP, DD, debug_now, DebugStacks(0, obj_prev));
    debugcond0(DOP, DD, debug_now, "");
    debugcond2(DOP, DD, debug_now, ">> %s (precedence %d)", EchoToken(t), precedence(t));

    switch( type(t) )
    {

      case WORD:
      
	if( string(t)[0] == CH_SYMSTART &&
	  (obj_prev != PREV_OBJ || vspace(t) + hspace(t) > 0) )
	{
	  Error(6, 20, "symbol %s unknown or misspelt",
	    WARN, &fpos(t), string(t));
	  if( ++unknown_count > 25 )
	  {
	    Error(6, 21, "too many errors (%s lines missing or out of order?)",
	      FATAL, &fpos(t), KW_SYSINCLUDE);
	  }
	}
	ShiftObj(t, PREV_OBJ);
	t = LexGetToken();
	break;


      case QWORD:
      
	ShiftObj(t, PREV_OBJ);
	t = LexGetToken();
	break;


      case VCAT:
      case HCAT:
      case ACAT:
      
	/* clean up left context */
	Shift(t, precedence(t), LEFT_ASSOC, TRUE, TRUE);

	/* invoke transfer subroutines if appropriate */
	/* *** if( type(t) == VCAT && !has_join(actual(t)) *** */
	if( transfer_allowed && type(t) == VCAT && !has_join(actual(t))
		&& type(tok_stack[ttop-2]) == GSTUB_EXT )
	{
	  debug0(DGT, DD, "  calling TransferComponent from Parse:");
	  ifdebug(DGT, DD, DebugStacks(0, obj_prev));
	  TransferComponent( PopObj() );
	  New(tmp, NULL_CLOS);
	  FposCopy( fpos(tmp), fpos(t) );
	  PushObj(tmp);
	}

	/* push GAP_OBJ token, to cope with 3 parameters */
	New(x, GAP_OBJ);
	mark(gap(x)) = has_mark(actual(t));
	join(gap(x)) = has_join(actual(t));
	hspace(x) = hspace(t);
	vspace(x) = vspace(t);
	precedence(x) = GAP_PREC;
	FposCopy( fpos(x), fpos(t) );
	Shift(x, GAP_PREC, LEFT_ASSOC, FALSE, TRUE);

	/* if op is followed by space, insert {} */
	t = LexGetToken();
	if( hspace(t) + vspace(t) > 0 )
	{ ShiftObj(MakeWord(WORD, STR_EMPTY, &fpos(x)), PREV_OBJ);
	}
	break;


      case CROSS:
      case FORCE_CROSS:
      case NULL_CLOS:
      case PAGE_LABEL:
      case BEGIN_HEADER:
      case END_HEADER:
      case SET_HEADER:
      case CLEAR_HEADER:
      case ONE_COL:
      case ONE_ROW:
      case WIDE:
      case HIGH:
      case HSHIFT:
      case VSHIFT:
      case HSCALE:
      case VSCALE:
      case HCOVER:
      case VCOVER:
      case SCALE:
      case KERN_SHRINK:
      case HCONTRACT:
      case VCONTRACT:
      case HLIMITED:
      case VLIMITED:
      case HEXPAND:
      case VEXPAND:
      case START_HVSPAN:
      case START_HSPAN:
      case START_VSPAN:
      case HSPAN:
      case VSPAN:
      case PADJUST:
      case HADJUST:
      case VADJUST:
      case ROTATE:
      case BACKGROUND:
      case CASE:
      case YIELD:
      case BACKEND:
      case XCHAR:
      case FONT:
      case SPACE:
      case YUNIT:
      case ZUNIT:
      case BREAK:
      case UNDERLINE:
      case COLOUR:
      case OUTLINE:
      case LANGUAGE:
      case CURR_LANG:
      case CURR_FAMILY:
      case CURR_FACE:
      case CURR_YUNIT:
      case CURR_ZUNIT:
      case COMMON:
      case RUMP:
      case MELD:
      case INSERT:
      case ONE_OF:
      case NEXT:
      case TAGGED:
      case INCGRAPHIC:
      case SINCGRAPHIC:
      case PLAIN_GRAPHIC:
      case GRAPHIC:
      case LINK_SOURCE:
      case LINK_DEST:

	/* clean up left context of t (these ops are all right associative) */
	Shift(t, precedence(t), RIGHT_ASSOC,
	  has_lpar(actual(t)), has_rpar(actual(t)));
	t = LexGetToken();
	break;


      case VERBATIM:
      case RAW_VERBATIM:

	/* clean up left context of t */
	x = t;
	Shift(t, precedence(t), RIGHT_ASSOC,
	  has_lpar(actual(t)), has_rpar(actual(t)));
	
	/* check for opening brace or begin following, and shift it onto the stacks */
	t = LexGetToken();
	if( type(t) != BEGIN && type(t) != LBR )
	  Error(6, 40, "right parameter of %s or %s must be enclosed in braces",
	    FATAL, &fpos(x), KW_VERBATIM, KW_RAWVERBATIM);
        actual(t) = type(x) == VERBATIM ? VerbatimSym : RawVerbatimSym;
	Shift(t, LBR_PREC, 0, FALSE, TRUE);

	/* read right parameter and add it to the stacks, and reduce */
	y = LexScanVerbatim( (FILE *) NULL, type(t) == BEGIN, &fpos(t),
	  type(x) == RAW_VERBATIM);
	ShiftObj(y, PREV_OBJ);

	/* carry on, hopefully to the corresponding right brace or @End @Verbatim */
	t = LexGetToken();
	break;


      case PLUS:
      case MINUS:

	/* clean up left context of t (these ops are all left associative) */
	Shift(t, precedence(t), LEFT_ASSOC,
	  has_lpar(actual(t)), has_rpar(actual(t)));
	t = LexGetToken();
	break;


      case UNEXPECTED_EOF:

	Error(6, 22, "unexpected end of input", FATAL, &fpos(t));
	break;


      case BEGIN:
      
	if( actual(t) == nilobj )
	{ Error(6, 23, "%s replaced by %s", WARN, &fpos(t), KW_BEGIN, KW_LBR);
	  type(t) = LBR;
	}
	/* NB NO BREAK! */


      case LBR:
      
	Shift(t, LBR_PREC, 0, FALSE, TRUE);
	t = LexGetToken();
	break;


      case END:
      
	if( actual(t) == nilobj )  /* haven't sought following symbol yet */
	{ x = LexGetToken();
	  if( type(x) == CLOSURE )
	  { actual(t) = actual(x);
	    Dispose(x);
	    x = nilobj;
	  }
	  else if( type(x) == VERBATIM )
	  { actual(t) = VerbatimSym;
	    Dispose(x);
	    x = nilobj;
	  }
	  else if( type(x) == RAW_VERBATIM )
	  { actual(t) = RawVerbatimSym;
	    Dispose(x);
	    x = nilobj;
	  }
	  else if( type(x) == WORD && string(x)[0] == CH_SYMSTART )
	  { Error(6, 24, "unknown or misspelt symbol %s after %s deleted",
	      WARN, &fpos(x), string(x), KW_END);
	    actual(t) = nilobj;
	    Dispose(x);
	    x = nilobj;
	  }
	  else
	  { Error(6, 25, "symbol expected after %s", WARN, &fpos(x), KW_END);
	    actual(t) = nilobj;
	  }
	}
	else x = nilobj;
	Shift(t, precedence(t), 0, TRUE, FALSE);
	t = (x != nilobj) ? x : LexGetToken();
	break;


      case RBR:
      
	Shift(t, precedence(t), 0, TRUE, FALSE);
	t = LexGetToken();
	break;
				

      case USE:
      case NOT_REVEALED:
      case PREPEND:
      case SYS_PREPEND:
      case DATABASE:
      case SYS_DATABASE:
      
	Error(6, 26, "%s symbol out of place",
	  FATAL, &fpos(t), SymName(actual(t)));
	break;


      case ENV:
      
	/* only occurs in cross reference databases */
	res = ParseEnvClosure(t, encl);
	ShiftObj(res, PREV_OBJ);
	t = LexGetToken();
	break;


      case ENVA:
      
	/* only occurs in cross reference databases */
	offset = LexNextTokenPos() -StringLength(KW_ENVA)-StringLength(KW_LBR)-1;
	Dispose(t); t = LexGetToken();
	tmp = Parse(&t, encl, FALSE, FALSE);
	env = SetEnv(tmp, nilobj);
	ShiftObj(env, PREV_OBJ);
	t = LexGetToken();
	EnvReadInsert(file_num(fpos(t)), offset, env);
	break;


      case ENVB:
      
	/* only occurs in cross reference databases */
	offset = LexNextTokenPos() -StringLength(KW_ENVB)-StringLength(KW_LBR)-1;
	Dispose(t); t = LexGetToken();
	env = Parse(&t, encl, FALSE, FALSE);
	t = LexGetToken();
	res = Parse(&t, encl, FALSE, FALSE);
	env = SetEnv(res, env);
	ShiftObj(env, PREV_OBJ);
	t = LexGetToken();
	EnvReadInsert(file_num(fpos(t)), offset, env);
	break;


      case ENVC:
      
	/* only occurs in cross reference databases */
	Dispose(t); t = LexGetToken();
	New(res, ENV);
	ShiftObj(res, PREV_OBJ);
	break;


      case ENVD:
      
	/* only occurs in cross reference databases */
	Dispose(t); t = LexGetToken();
	if( type(t) != QWORD ||
	  sscanf((char *) string(t), "%d %d", &offset, &lnum) != 2 )
	  Error(6, 37, "error in cross reference database", FATAL, &fpos(t));
	if( !EnvReadRetrieve(file_num(fpos(t)), offset, &env) )
	{ LexPush(file_num(fpos(t)), offset, DATABASE_FILE, lnum, TRUE);
	  Dispose(t);  t = LexGetToken();
	  env = Parse(&t, encl, FALSE, FALSE);
	  LexPop();
	}
	else
	{ Dispose(t);
	}
	ShiftObj(env, PREV_OBJ);
	t = LexGetToken();
	break;


      case CENV:
      
	/* only occurs in cross reference databases */
	Dispose(t); t = LexGetToken();
	env = Parse(&t, encl, FALSE, FALSE);
	scope_count = 0;
	SetScope(env, &scope_count, FALSE);
	t = LexGetToken();
	res = Parse(&t, encl, FALSE, FALSE);
	for( i = 0;  i < scope_count;  i++ ) PopScope();
	AttachEnv(env, res);
	ShiftObj(res, PREV_OBJ);
	t = LexGetToken();
	break;


      case LUSE:

	/* only occurs in cross-reference databases */
	/* copy invocation from use_invocation(xsym), don't read it */
	Dispose(t);  t = LexGetToken();
	if( type(t) != CLOSURE )
	  Error(6, 27, "symbol expected following %s", FATAL,&fpos(t),KW_LUSE);
	xsym = actual(t);
	if( use_invocation(xsym) == nilobj )
	  Error(6, 28, "%s clause(s) changed from previous run",
	    FATAL, &fpos(t), KW_USE);
	x = CopyObject(use_invocation(xsym), no_fpos);
	for( link = LastDown(x);  link != x;  link = PrevDown(link) )
	{ Child(y, link);
	  if( type(y) == ENV )
	  { DeleteLink(link);
	    break;
	  }
	}
	ShiftObj(x, PREV_OBJ);
	t = LexGetToken();
	break;


      case LVIS:
      
	/* only occurs in cross-reference databases */
	SuppressVisible();
	Dispose(t);  t = LexGetToken();
	UnSuppressVisible();
	if( type(t) != CLOSURE )
	  Error(6, 29, "symbol expected following %s", FATAL,&fpos(t),KW_LVIS);
	/* NB NO BREAK! */


      case CLOSURE:
      
	x = t;  xsym = actual(x);

	/* look ahead one token, which could be an NPAR */
	/* or could be @NotRevealed */
	PushScope(xsym, TRUE, FALSE);
	t = LexGetToken();
	if( type(t) == NOT_REVEALED )
	{ Dispose(t);
	  t = LexGetToken();
	  revealed = FALSE;
	}
	else revealed = TRUE;
	PopScope();

	/* if x starts a cross-reference, make it a CLOSURE */
	if( is_cross(type(t)) )
	{ ShiftObj(x, PREV_OBJ);
	  break;
	}

	/* clean up left context of x */
	Shift(x,precedence(x),right_assoc(xsym),has_lpar(xsym),has_rpar(xsym));

	/* update uses relation if required */
	if( encl != StartSym && encl != nilobj )
	{ if( has_target(xsym) )
	  { uses_galley(encl) = TRUE;
	    dirty(encl) = (dirty(encl) || dirty(xsym));
	  }
	  else if( revealed )  InsertUses(encl, xsym);
	}

	/* read named parameters */
	compulsory_count = 0;
	while( (type(t) == CLOSURE && enclosing(actual(t)) == xsym
				       && type(actual(t)) == NPAR)
	  || (type(t) == LBR && precedence(t) != LBR_PREC) )
	{	
	  OBJECT new_par;

	  /* check syntax and attach the named parameter to x */
	  if( type(t) == CLOSURE )
	  {
	    new_par = t;
	    t = LexGetToken();
	    if( type(t) != LBR )
	    { Error(6, 30, "%s must follow named parameter %s",
	        WARN, &fpos(new_par), KW_LBR, SymName(actual(new_par)));
	      Dispose(new_par);
	      break;
	    }
	  }
	  else
	  {
	    /* compressed form of named parameter */
	    new_par = NewToken(CLOSURE, &fpos(t), vspace(t), hspace(t),
	      NO_PREC, ChildSymWithCode(x, precedence(t)));
	    precedence(t) = LBR_PREC;
	  }

	  /* add import list of the named parameter to current scope */
	  scope_count = 0;
	  imps = imports(actual(new_par));
	  if( imps != nilobj )
	  { for( link = Down(imps);  link != imps;  link = NextDown(link) )
	    { Child(y, link);
	      PushScope(actual(y), FALSE, TRUE);
	      scope_count++;
	    }
	  }

	  /* read the body of the named parameter */
	  PushScope(actual(new_par), FALSE, FALSE);
	  tmp = Parse(&t, encl, FALSE, FALSE);
	  PopScope();
	  type(new_par) = PAR;
	  Link(new_par, tmp);

	  /* pop the scopes pushed for the import list */
	  for( i = 0;  i < scope_count;  i++ )
	    PopScope();

	  /* check that new_par has not already occurred, then link it to x */
	  for( link = Down(x);  link != x;  link = NextDown(link) )
	  { Child(y, link);
	    assert( type(y) == PAR, "Parse: type(y) != PAR!" );
	    if( actual(new_par) == actual(y) )
	    { Error(6, 31, "named parameter %s of %s appears twice", WARN,
		&fpos(new_par), SymName(actual(new_par)), SymName(actual(x)));
	      DisposeObject(new_par);
	      new_par = nilobj;
	      break;
	    }
	  }
	  if( new_par != nilobj )
	  {
	    /* keep track of the number of compulsory named parameters */
	    if( is_compulsory(actual(new_par)) )
	      compulsory_count++;

	    Link(x, new_par);
	  }

	  /* get next token, possibly another NPAR */
	  PushScope(xsym, TRUE, FALSE);	 /* allow NPARs only */
	  if( t == nilobj )  t = LexGetToken();
	  PopScope();

	} /* end while */

	/* report absence of compulsory parameters */
	debug4(DOP, D, "%s %s %d : %d", EchoFilePos(&fpos(x)),
	  SymName(xsym), compulsory_count, has_compulsory(xsym));
	if( compulsory_count < has_compulsory(xsym) )
	{
	  for( xlink = Down(xsym);  xlink != xsym;  xlink = NextDown(xlink) )
	  { Child(tmp, xlink);
	    if( type(tmp) == NPAR && is_compulsory(tmp) )
	    { for( link = Down(x);  link != x;  link = NextDown(link) )
	      { Child(y, link);
		if( type(y) == PAR && actual(y) == tmp )
		  break;
	      }
	      if( link == x )
	      {
		Error(6, 38, "compulsory option %s missing from %s",
		  WARN, &fpos(x), SymName(tmp), SymName(xsym));
	      }
	    }
	  }
	}

	/* record symbol name in BEGIN following, if any */
	if( type(t) == BEGIN )
	{ if( !has_rpar(xsym) )
	    Error(6, 32, "%s out of place here (%s has no right parameter)",
	      WARN, &fpos(x), KW_BEGIN, SymName(xsym));
	  else actual(t) = xsym;
	}

	/* if x can be transferred, do so */
	if( transfer_allowed && has_target(xsym) &&
	    !has_key(xsym) && filter(xsym) == nilobj )
	{   
	  if( !has_rpar(xsym) || uses_count(ChildSym(xsym, RPAR)) <= 1 )
	  {
	    debug1(DGT, D, "examining transfer of %s", SymName(xsym));
	    ifdebug(DGT, D, DebugStacks(initial_ttop, obj_prev));
	    i = has_rpar(xsym) ? ttop -1 : ttop;
	    while( is_cat_op(type(tok_stack[i])) )   i--;
	    if( (type(tok_stack[i])==LBR || type(tok_stack[i])==BEGIN)
		  && type(tok_stack[i-1]) == GSTUB_EXT )
	    {
	      /* at this point it is likely that x is transferable */
	      if( has_rpar(xsym) )
	      { New(tmp, CLOSURE);
		actual(tmp) = InputSym;
		FposCopy( fpos(tmp), fpos(t) );
		ShiftObj(tmp, PREV_OBJ);
		obj_prev = Reduce();
	      }
	      x = PopObj();
	      x = TransferBegin(x);
	      if( type(x) == CLOSURE )	/* failure: unReduce */
	      {	if( has_rpar(xsym) )
		{ Child(tmp, LastDown(x));
		  assert(type(tmp)==PAR && type(actual(tmp))==RPAR,
				"Parse: cannot undo rpar" );
		  DisposeChild(LastDown(x));
		  if( has_lpar(xsym) )
		  { Child(tmp, Down(x));
		    assert(type(tmp)==PAR && type(actual(tmp))==LPAR,
				"Parse: cannot undo lpar" );
		    Child(tmp, Down(tmp));
		    PushObj(tmp);
		    DeleteLink(Up(tmp));
		    DisposeChild(Down(x));
		  }
		  PushToken(x);  obj_prev = PREV_OP;
		}
		else
		{ PushObj(x);
		  obj_prev = PREV_OBJ;
		}
	      }
	      else /* success */
	      { obj_prev = PREV_OP;
	        Shift(x, NO_PREC, 0, FALSE, has_rpar(xsym));
	      }
	    }
	  }
	} /* end if has_target */

	if( filter(xsym) != nilobj )
	{
	  if( type(t) == BEGIN || type(t) == LBR )
	  {
	    /* create filter object and copy parameter into temp file */
	    tmp = FilterCreate((BOOLEAN) (type(t) == BEGIN), xsym, &fpos(t));

	    /* push filter object onto stacks and keep going */
	    Shift(t, precedence(t), 0, FALSE, TRUE);
	    ShiftObj(tmp, PREV_OBJ);
	    t = LexGetToken();
	  }
	  else Error(6, 33, "right parameter of %s must be enclosed in braces",
		 FATAL, &fpos(x), SymName(xsym));
	}

	else if( has_body(xsym) )
	{ if( type(t) == BEGIN || type(t) == LBR )
	  { PushScope(xsym, FALSE, TRUE);
	    PushScope(ChildSym(xsym, RPAR), FALSE, FALSE);
	    PushObj( Parse(&t, encl, FALSE, TRUE) );
	    obj_prev = Reduce();
	    PopScope();
	    PopScope();
	    if( t == nilobj )  t = LexGetToken();
	  }
	  else
	  { Error(6, 34, "body parameter of %s must be enclosed in braces",
	      WARN, &fpos(t), SymName(xsym));
	  }
	}
	break;


      case OPEN:

	x = t;  xsym = nilobj;
	Shift(t, precedence(t), RIGHT_ASSOC, TRUE, TRUE);
	if( type(ObjTop) == CLOSURE )  xsym = actual(ObjTop);
	else if( is_cross(type(ObjTop)) && Down(ObjTop) != ObjTop )
	{ Child(tmp, Down(ObjTop));
	  if( type(tmp) == CLOSURE )  xsym = actual(tmp);
	}
	t = LexGetToken();

	if( xsym == nilobj )
	  Error(6, 35, "invalid left parameter of %s", WARN, &fpos(x), KW_OPEN);
	else if( type(t) != BEGIN && type(t) != LBR )
	  Error(6, 36, "right parameter of %s must be enclosed in braces",
	    WARN, &fpos(t), KW_OPEN);
	else
	{ PushScope(xsym, FALSE, TRUE);
	  tmp = Parse(&t, encl, FALSE, FALSE);
	  ShiftObj(tmp, PREV_RBR);
	  PopScope();
	  if( t == nilobj )  t = LexGetToken();
	  obj_prev = Reduce();
	}
	break;


      default:
      
	assert1(FALSE, "Parse:", Image(type(t)));
	break;

    } /* end switch */
  } /* end for */

} /* end Parse */
