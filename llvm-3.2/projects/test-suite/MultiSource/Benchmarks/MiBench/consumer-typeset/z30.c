/*@z30.c:Symbol uses:InsertUses()@********************************************/
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
/*  FILE:         z30.c                                                      */
/*  MODULE:       Symbol Uses                                                */
/*  EXTERNS:      InsertUses(), FlattenUses(), SearchUses(),                 */
/*                FirstExternTarget(), NextExternTarget()                    */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  InsertUses(x, y)                                                         */
/*                                                                           */
/*  Record the fact that symbol x uses symbol y, by linking them.            */
/*  Increment count of the number of times y is used, if y is a parameter.   */
/*                                                                           */
/*****************************************************************************/

void InsertUses(OBJECT x, OBJECT y)
{ OBJECT tmp;
  debug2(DSU, D, "InsertUses( %s, %s )", SymName(x), SymName(y));
  if( type(x) == LOCAL && type(y) == LOCAL && !predefined(y) )
  { GetMem(tmp, USES_SIZE, no_fpos);  item(tmp) = y;
    if( base_uses(x) == nilobj )  next(tmp) = tmp;
    else next(tmp) = next(base_uses(x)), next(base_uses(x)) = tmp;
    base_uses(x) = tmp;
  }
  if( is_par(type(y)) )
  {
    uses_count(y) += (enclosing(y) == x ? 1 : 2);
    if( dirty(y) || uses_count(y) > 1 )  dirty(enclosing(y)) = TRUE;
  }
  else if( sym_body(y) == nilobj || dirty(y) )  dirty(x) = TRUE;
  debug5(DSU, D, "InsertUses returning ( %s %s; %s %s, count = %d )",
    SymName(x), (dirty(x) ? "dirty" : "clean"),
    SymName(y), (dirty(y) ? "dirty" : "clean"), uses_count(y));
} /* end InsertUses */


/*@::GatherUses(), GatherAllUses(), FlattenUses()@****************************/
/*                                                                           */
/*  static GatherUses(x, sym)                                                */
/*  static GatherAllUses(x)                                                  */
/*                                                                           */
/*  GatherUses adds all the unmarked descendants of x to the uses relation   */
/*  of sym;  GatherAllUses applies gather_uses to all descendants of x.      */
/*                                                                           */
/*****************************************************************************/

static void GatherUses(OBJECT x, OBJECT sym)
{ OBJECT link, y, tmp;
  if( base_uses(x) != nilobj )
  { link = next(base_uses(x));
    do
    { y = item(link);
      if( marker(y) != sym )
      {	if( y != sym )
	{ marker(y) = sym;
	  GetMem(tmp, USES_SIZE, no_fpos);  item(tmp) = y;
	  if( uses(sym) == nilobj )  next(tmp) = tmp;
	  else next(tmp) = next(uses(sym)), next(uses(sym)) = tmp;
	  uses(sym) = tmp;
	  if( indefinite(y) )  indefinite(sym) = TRUE;
	  if( uses_extern_target(y) )  uses_extern_target(sym) = TRUE;
	  GatherUses(y, sym);
	}
	else recursive(sym) = TRUE;
      }
      link = next(link);
    } while( link != next(base_uses(x)) );
  }
} /* end GatherUses */


static void GatherAllUses(OBJECT x)
{ OBJECT link, y;
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == LOCAL )  GatherUses(y, y);
    GatherAllUses(y);
  }
} /* end GatherAllUses */


/*****************************************************************************/
/*                                                                           */
/*  FlattenUses()                                                            */
/*                                                                           */
/*  Traverse the directed graph assembled by InsertUses, finding its         */
/*  transitive closure and storing this explicitly in uses(x) for all x.     */
/*                                                                           */
/*****************************************************************************/

void FlattenUses(void)
{ GatherAllUses(StartSym);
} /* end FlattenUses */


/*@::SearchUses(), FirstExternTarget(), NextExternTarget()@*******************/
/*                                                                           */
/*  BOOLEAN SearchUses(x, y)                                                 */
/*                                                                           */
/*  Discover whether symbol x uses symbol y by searching the uses list of x. */
/*                                                                           */
/*****************************************************************************/

BOOLEAN SearchUses(OBJECT x, OBJECT y)
{ OBJECT p;
  debug3(DSU, DD, "SearchUses(%s, %s) uses: %d", SymName(x),SymName(y),uses(x));
  if( x == y )  return TRUE;
  if( uses(x) != nilobj )
  { p = next(uses(x));
    do
    { debug1(DSU, DDD, "  checking %s", SymName(item(p)));
      if( item(p) == y )  return TRUE;
      p = next(p);
    } while( p != next(uses(x)) );
  }
  return FALSE;
} /* end SearchUses */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT FirstExternTarget(sym, cont)                                      */
/*  OBJECT NextExternTarget(sym, cont)                                       */
/*                                                                           */
/*  Together these two procedures return all symbols which are both used by  */
/*  sym and a target for at least one external galley.  Return nilobj at end.*/
/*                                                                           */
/*****************************************************************************/

OBJECT FirstExternTarget(OBJECT sym, OBJECT *cont)
{ OBJECT res;
  debug1(DSU, D, "FirstExternTarget( %s )", SymName(sym));
  res = nilobj;  *cont = nilobj;
  if( is_extern_target(sym) )  res = sym;
  else if( uses(sym) != nilobj )
  { *cont = next(uses(sym));
    do
    { if( is_extern_target(item(*cont)) )
      {	res = item(*cont);
	break;
      }
      *cont = next(*cont);
    } while( *cont != next(uses(sym)) );
  }
  debug1(DSU, D, "FirstExternTarget returning %s", SymName(res));
  return res;
} /* end FirstExternTarget */

OBJECT NextExternTarget(OBJECT sym, OBJECT *cont)
{ OBJECT res;
  debug1(DSU, D, "NextExternTarget( %s )", SymName(sym));
  res = nilobj;
  if( *cont != nilobj )
  { *cont = next(*cont);
    while( *cont != next(uses(sym)) )
    { if( is_extern_target(item(*cont)) )
      {	res = item(*cont);
	break;
      }
      *cont = next(*cont);
    }
  }
  debug1(DSU, D, "NextExternTarget returning %s", SymName(res));
  return res;
} /* end NextExternTarget */
