/*@z09.c:Closure Expansion:SearchEnv()@***************************************/
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
/*  FILE:         z09.c                                                      */
/*  MODULE:       Closure Expansion                                          */
/*  EXTERNS:      SearchEnv(), SetEnv(), AttachEnv(), GetEnv(),              */
/*                DetachEnv(), ClosureExpand()                               */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  OBJECT SearchEnv(env, sym)                                               */
/*                                                                           */
/*  Search environment env for a symbol such that actual() == sym.           */
/*                                                                           */
/*****************************************************************************/

OBJECT SearchEnv(OBJECT env, OBJECT sym)
{ OBJECT link, y;
  debug2(DCE, DD, "[ SearchEnv(%s, %s)", EchoObject(env), SymName(sym));
  for(;;)
  {
    debug1(DCE, DDD, "  searching env %s", EchoObject(env));
    assert( env != nilobj && type(env) == ENV, "SearchEnv: env!" );
    if( Down(env) == env )
    { debug0(DCE, DD, "] SearchEnv returning <nilobj>");
      return nilobj;
    }
    Child(y, Down(env));
    assert( type(y) == CLOSURE, "SearchEnv: type(y) != CLOSURE!" );
    if( actual(y) == sym )
    { debug1(DCE, DD, "] SearchEnv returning %s", EchoObject(y));
      return y;
    }
    assert( LastDown(y) != y, "SearchEnv: LastDown(y) == y!" );
    link = LastDown(env) != Down(env) ? LastDown(env) : LastDown(y);
    Child(env, link);
  }
} /* end SearchEnv */


/*@::SetEnv(), AttachEnv(), GetEnv(), DetachEnv()@****************************/
/*                                                                           */
/*  OBJECT SetEnv(x, y)                                                      */
/*                                                                           */
/*  Create a new environment containing x and possibly y.                    */
/*                                                                           */
/*****************************************************************************/

OBJECT SetEnv(OBJECT x, OBJECT y)
{ OBJECT res;
  debug1(DCE, DD, "SetEnv( x, %s ), x =", EchoObject(y));
  ifdebug(DCE, DD, DebugObject(x));
  assert( x!=nilobj && type(x)==CLOSURE, "SetEnv: x==nilobj or not CLOSURE!" );
  assert( y==nilobj || type(y)==ENV, "SetEnv: y!=nilobj && type(y) != ENV!" );
  New(res, ENV);  Link(res, x);
  if( y != nilobj )  Link(res, y);
  debug1(DCE, DD, "SetEnv returning %s", EchoObject(res));
  return res;
} /* end SetEnv */


/*****************************************************************************/
/*                                                                           */
/*  AttachEnv(env, x)                                                        */
/*                                                                           */
/*  Attach environment env to CLOSURE x.                                     */
/*                                                                           */
/*****************************************************************************/

void AttachEnv(OBJECT env, OBJECT x)
{ debug2(DCE, DD, "AttachEnv( %s, %s )", EchoObject(env), EchoObject(x));
  assert( env != nilobj && type(env) == ENV, "AttachEnv: type(env) != ENV!" );
  assert( type(x) == CLOSURE || type(x) == ENV_OBJ, "AttachEnv: type(x)!" );
  Link(x, env);
  debug0(DCE, DD, "AttachEnv returning.");
} /* end AttachEnv */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT GetEnv(x)                                                         */
/*                                                                           */
/*  Get from CLOSURE x the environment previously attached.                  */
/*                                                                           */
/*****************************************************************************/

OBJECT GetEnv(OBJECT x)
{ OBJECT env;
  assert( type(x) == CLOSURE, "GetEnv: type(x) != CLOSURE!" );
  assert( LastDown(x) != x, "GetEnv: LastDown(x) == x!" );
  Child(env, LastDown(x));
  assert( type(env) == ENV, "GetEnv: type(env) != ENV!" );
  return env;
} /* end GetEnv */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT DetachEnv(x)                                                      */
/*                                                                           */
/*  Detach from CLOSURE x the environment previously attached.               */
/*                                                                           */
/*****************************************************************************/

OBJECT DetachEnv(OBJECT x)
{ OBJECT env;
  debug1(DCE, DD, "DetachEnv( %s )", EchoObject(x));
  assert( type(x) == CLOSURE, "DetachEnv: type(x) != CLOSURE!" );
  assert( LastDown(x) != x, "DetachEnv: LastDown(x) == x!" );
  Child(env, LastDown(x));
  DeleteLink(LastDown(x));
  assert( type(env) == ENV, "DetachEnv: type(env) != ENV!" );
  debug1(DCE, DD, "DetachEnv resturning %s", EchoObject(env));
  return env;
} /* end DetachEnv */


/*@::ClosureExpand()@*********************************************************/
/*                                                                           */
/*  OBJECT ClosureExpand(x, env, crs_wanted, crs, res_env)                   */
/*                                                                           */
/*  Return expansion of closure x in environment env.                        */
/*  The body comes from the environment of x if x is a parameter, else from  */
/*  the symbol table.  The original x is pushed into the environments.       */
/*  If crs_wanted and x has a tag, a cross-reference is added to crs.        */
/*                                                                           */
/*****************************************************************************/

OBJECT ClosureExpand(OBJECT x, OBJECT env, BOOLEAN crs_wanted,
OBJECT *crs, OBJECT *res_env)
{ OBJECT link, y, res, prnt_env, par, prnt;
  debug3(DCE, D, "[ ClosureExpand( %s, %s, %s, crs, res_env )",
    EchoObject(x), EchoObject(env), bool(crs_wanted));
  assert( type(x) == CLOSURE, "ClosureExpand given non-CLOSURE!");
  assert( predefined(actual(x)) == FALSE, "ClosureExpand given predefined!" );

  /* add tag to x if needed but not provided;  add cross-reference to crs  */
  if( has_tag(actual(x)) )  CrossAddTag(x);
  if( crs_wanted && has_tag(actual(x)) )
  { OBJECT tmp = CopyObject(x, no_fpos);  AttachEnv(env, tmp);
    y = CrossMake(actual(x), tmp, CROSS_TARG);
    New(tmp, CROSS_TARG);  actual(tmp) = y;  Link(tmp, y);
    if( *crs == nilobj )  New(*crs, CR_LIST);   Link(*crs, tmp);
  }

  /* case x is a parameter */
  res = *res_env = nilobj;
  if( is_par(type(actual(x))) )
  { prnt = SearchEnv(env, enclosing(actual(x)));
    if( prnt != nilobj )
    {
      prnt_env = GetEnv(prnt);
      for( link = Down(prnt);  link != prnt;  link = NextDown(link) )
      { Child(par, link);
        if( type(par) == PAR && actual(par) == actual(x) )
        { assert( Down(par) != par, "ExpandCLosure: Down(par)!");
	  Child(res, Down(par));
	  if( dirty(enclosing(actual(par))) || is_enclose(actual(par)) )
	  { debug2(DCE, DD, "copy %s %s", SymName(actual(par)), EchoObject(res));
	    res = CopyObject(res, no_fpos);
	  }
	  else
	  { debug2(DCE, DD, "link %s %s",
	      FullSymName(actual(par), AsciiToFull(".")), EchoObject(res));
	    DeleteLink(Down(par));
	    y = MakeWord(WORD, STR_NOCROSS, &fpos(res));
	    Link(par, y);
	  }
	  ReplaceNode(res, x);
	  if( type(actual(x)) == RPAR && has_body(enclosing(actual(x))) )
	  { debug0(DCR, DDD, "  calling SetEnv from ClosureExpand (a)");
	    *res_env = SetEnv(prnt, nilobj);  DisposeObject(x);
	  }
	  else if( type(actual(x)) == NPAR && imports_encl(actual(x)) )
	  { debug0(DCR, DDD, "  calling SetEnv from ClosureExpand (x)");
	    AttachEnv(env, x);
	    *res_env = SetEnv(x, nilobj);
	  }
	  else
	  { AttachEnv(env, x);
	    debug0(DCR, DDD, "  calling SetEnv from ClosureExpand (b)");
	    *res_env = SetEnv(x, prnt_env);
	  }
	  break;
        }
      }
    }
    else
    {
      /* fail only if there is no default value available */
      if( sym_body(actual(x)) == nilobj )
      {
        debug3(DCE, D, "failing ClosureExpand( %s, crs, %s, %s, res_env )\n",
	  EchoObject(x), bool(crs_wanted), EchoObject(env));
        Error(9, 2, "no value for parameter %s of symbol %s:", WARN, &fpos(x),
	  SymName(actual(x)), SymName(enclosing(actual(x))));
        Error(9, 1, "symbol with import list misused", FATAL, &fpos(x));
      }
    }
  }

  /* case x is a user-defined symbol or default parameter */
  if( res == nilobj )
  { if( sym_body(actual(x)) == nilobj )
      res = MakeWord(WORD,STR_NOCROSS,&fpos(x));
    else res = CopyObject(sym_body(actual(x)), &fpos(x));
    ReplaceNode(res, x);  AttachEnv(env, x);
    debug0(DCR, DDD, "  calling SetEnv from ClosureExpand (c)");
    *res_env = SetEnv(x, nilobj);
  }

  assert( *res_env!=nilobj && type(*res_env)==ENV, "ClosureExpand: *res_env!");
  debug0(DCE, D, "] ClosureExpand returning, res =");
  ifdebug(DCE, D, DebugObject(res));
  debug1(DCE, D, "  environment = %s", EchoObject(*res_env));
  return res;
} /* end ClosureExpand */


/*@::ParameterCheck()@********************************************************/
/*                                                                           */
/*  OBJECT ParameterCheck(x, env)                                            */
/*                                                                           */
/*  Check whether object x (which is an actual parameter that happens to be  */
/*  a CLOSURE) has a value which is a simple word, and if so return a copy   */
/*  of that word, else nilobj.                                               */
/*                                                                           */
/*****************************************************************************/

OBJECT ParameterCheck(OBJECT x, OBJECT env)
{ OBJECT link, y, res, prnt_env, par, prnt;
  debug2(DCE, DD, "ParameterCheck(%s, %s)", EchoObject(x), EchoObject(env));
  assert( type(x) == CLOSURE, "ParameterCheck given non-CLOSURE!");

  /* case x is a parameter */
  prnt = SearchEnv(env, enclosing(actual(x)));
  if( prnt == nilobj )
  { debug0(DCE, DD, "ParameterCheck returning nilobj (prnt fail)");
    return nilobj;
  }
  prnt_env = GetEnv(prnt);
  for( link = Down(prnt);  link != prnt;  link = NextDown(link) )
  { Child(par, link);
    if( type(par) == PAR && actual(par) == actual(x) )
    {	assert( Down(par) != par, "ParameterCheck: Down(par)!");
	Child(y, Down(par));
	res = is_word(type(y)) ? CopyObject(y, no_fpos) : nilobj;
	debug1(DCE, DD, "  ParameterCheck returning %s", EchoObject(res));
	return res;
    }
  }

  /* case x is a default parameter */
  y = sym_body(actual(x));
  if( y == nilobj )
  { res = nilobj;
  }
  else if( is_word(type(y)) )
  { res = CopyObject(y, &fpos(y));
  }
  else if( type(y) == CLOSURE && is_par(type(actual(y))) )
  { res = ParameterCheck(y, prnt_env);
  }
  else
  { res = nilobj;
  }
  debug1(DCE, DD, "ParameterCheck returning %s", EchoObject(res));
  return res;
} /* end ParameterCheck */
