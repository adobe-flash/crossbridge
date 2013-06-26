/*@z18.c:Galley Transfer:Declarations@****************************************/
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
/*  FILE:         z18.c                                                      */
/*  MODULE:       Galley Transfer                                            */
/*  EXTERNS:      TransferInit(), TransferBegin(), TransferComponent(),      */
/*                TransferEnd(), TransferClose()                             */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

#define	MAX_DEPTH  	30			/* max depth of galleys      */
static OBJECT		root_galley = nilobj;	/* the root galley           */
static OBJECT		targets[MAX_DEPTH];	/* currently open \Inputs    */
static CONSTRAINT	constraints[MAX_DEPTH];	/* their COLM constraints    */
static int		itop;			/* stack top	             */
static CONSTRAINT	initial_constraint;	/* initial COLM constraint   */
       STYLE		InitialStyle;		/* initial style             */
       OBJECT		InitialEnvironment;	/* initial environment	     */

#if DEBUG_ON
static void debug_targets(void)
{ int i;  OBJECT tmp;
  for( i = 0;  i <= itop;  i++ )
  { if( targets[i] == nilobj || Down(targets[i]) == targets[i] )  tmp = nilobj;
    else Child(tmp, Down(targets[i]));
    debug3(DGT, D, "  target[%d] %s = %s", i,
      EchoConstraint(&constraints[i]), EchoObject(tmp));
  }
} /* end debug_targets */
#endif


/*@::TransferInit()@**********************************************************/
/*                                                                           */
/*  TransferInit(InitEnv)                                                    */
/*                                                                           */
/*  Initialise this module.  The initial environment is InitEnv.             */
/*                                                                           */
/*****************************************************************************/

void TransferInit(OBJECT InitEnv)
{ OBJECT dest, x, y, recs, inners, nothing, dest_index, up_hd, why;
  debug1(DGT, D, "[ TransferInit( %s )", EchoObject(InitEnv));
  SetConstraint(initial_constraint,
    MAX_FULL_LENGTH-1, MAX_FULL_LENGTH-1, MAX_FULL_LENGTH-1);

  /* set initial environment and style */
  InitialEnvironment = InitEnv;
  SetGap(line_gap(InitialStyle), FALSE,FALSE,FALSE,FIXED_UNIT,MARK_MODE,18*PT);
  vadjust(InitialStyle)         = FALSE;
  hadjust(InitialStyle)         = FALSE;
  padjust(InitialStyle)         = FALSE;
  space_style(InitialStyle)     = SPACE_LOUT;
  SetGap(space_gap(InitialStyle), FALSE,FALSE,TRUE,FIXED_UNIT,EDGE_MODE,1*EM);
  hyph_style(InitialStyle)      = HYPH_UNDEF;
  fill_style(InitialStyle)      = FILL_UNDEF;
  display_style(InitialStyle)   = DISPLAY_UNDEF;
  small_caps(InitialStyle)      = SMALL_CAPS_OFF;
  font(InitialStyle)            = 0;			/* i.e. undefined    */
  colour(InitialStyle)          = 0;			/* i.e. undefined    */
  outline(InitialStyle)         = FALSE;		/* i.e. not outlined */
  language(InitialStyle)        = 0;			/* i.e. undefined    */
  yunit(InitialStyle)           = 0;			/* i.e. zero         */
  zunit(InitialStyle)           = 0;			/* i.e. zero         */
  nobreakfirst(InitialStyle)	= FALSE;
  nobreaklast(InitialStyle)	= FALSE;

  /* construct destination for root galley */
  New(up_hd, HEAD);
  force_gall(up_hd) = FALSE;
  actual(up_hd) = enclose_obj(up_hd) = limiter(up_hd) = nilobj;
  headers(up_hd) = dead_headers(up_hd) = nilobj;
  opt_components(up_hd) = opt_constraints(up_hd) = nilobj;
  gall_dir(up_hd) = ROWM;
  New(dest_index, RECEIVING);
  New(dest, CLOSURE);  actual(dest) = PrintSym;
  actual(dest_index) = dest;
  debug2(DGT, D, "TransferInit setting external_ver(%s %s) = TRUE",
    Image(type(dest)), SymName(actual(dest)));
  external_ver(dest) = TRUE;
  external_hor(dest) = FALSE;
  threaded(dest) = FALSE;
  blocked(dest_index) = FALSE;
  Link(up_hd, dest_index);

  /* construct root galley */
  New(root_galley, HEAD);
  force_gall(root_galley) = FALSE;
  enclose_obj(root_galley) = limiter(root_galley) = nilobj;
  headers(root_galley) = dead_headers(root_galley) = nilobj;
  opt_components(root_galley) = opt_constraints(root_galley) = nilobj;
  gall_dir(root_galley) = ROWM;
  FposCopy(fpos(root_galley), *no_fpos);
  actual(root_galley) = whereto(root_galley) = nilobj;
  ready_galls(root_galley) = nilobj;
  must_expand(root_galley) = sized(root_galley) =FALSE;
  foll_or_prec(root_galley) = GALL_FOLL;
  New(x, CLOSURE);  actual(x) = InputSym;
  Link(root_galley, x);
  SizeGalley(root_galley, InitEnv, TRUE, FALSE, FALSE, FALSE, &InitialStyle,
    &initial_constraint, nilobj, &nothing, &recs, &inners, nilobj);
  assert( recs   == nilobj , "TransferInit: recs   != nilobj!" );
  assert( inners == nilobj , "TransferInit: inners != nilobj!" );
  Link(dest_index, root_galley);

  /* initialise target and constraint stacks */
  Child(y, Down(root_galley));
  assert( type(y) == RECEPTIVE && type(actual(y)) == CLOSURE &&
	actual(actual(y)) == InputSym, "TransferInit: initial galley!" );
  assert( external_ver(actual(y)), "TransferInit: input sym not external!" );
  blocked(y) = TRUE;
  itop = 0;
  New(targets[itop], ACAT);
  Link(targets[itop], y);
  Constrained(actual(y), &constraints[itop], COLM, &why);
  debug2(DSC, DD, "Constrained( %s, COLM ) = %s",
	EchoObject(y), EchoConstraint(&constraints[itop]));

  debug0(DGT, D, "] TransferInit returning.");
  ifdebug(DGT, DD, debug_targets());
} /* end TransferInit */


/*@::TransferBegin()@*********************************************************/
/*                                                                           */
/*  OBJECT TransferBegin(x)                                                  */
/*                                                                           */
/*  Commence the transfer of a new galley whose header is invocation x.      */
/*                                                                           */
/*****************************************************************************/

OBJECT TransferBegin(OBJECT x)
{ OBJECT xsym, index, y, link, env, new_env, hold_env, res, hd, target, why;
  CONSTRAINT c;
  debug1(DGT, D, "[ [ TransferBegin( %s )", EchoObject(x));
  ifdebug(DGT, DD, debug_targets());
  assert( type(x) == CLOSURE, "TransferBegin: non-CLOSURE!" );

  /* add an automatically generated @Tag parameter to x if required */
  if( has_tag(actual(x)) )  CrossAddTag(x);

  /* construct new (inner) env chain */
  if( Down(targets[itop]) == targets[itop] )
    Error(18, 1, "cannot attach galley %s", FATAL,&fpos(x),SymName(actual(x)));
  Child(target, Down(targets[itop]));
  xsym = actual(x);
  env = GetEnv(actual(target));
  debug1(DGT, DD, "  current env chain: %s", EchoObject(env));
  if( has_body(xsym) )
  {
    /* prepare a copy of x for inclusion in environment */
    y = CopyObject(x, no_fpos);

    /* attach its environment */
    AttachEnv(env, y);

    /* now the new environment is y catenated with the old one */
    debug0(DCR, DDD, "calling SetEnv from TransferBegin (a)");
    new_env = SetEnv(y, nilobj);
  }
  else new_env = env;
  New(hold_env, ACAT);  Link(hold_env, new_env);
  debug1(DGT, DD, "  new env chain: %s", EchoObject(new_env));

  /* convert x into an unsized galley called hd */
  New(index, UNATTACHED);
  pinpoint(index) = nilobj;
  New(hd, HEAD);
  FposCopy(fpos(hd), fpos(x));
  actual(hd) = xsym;
  limiter(hd) = opt_components(hd) = opt_constraints(hd) = nilobj;
  gall_dir(hd) = ROWM;
  ready_galls(hd) = nilobj;
  must_expand(hd) = TRUE;
  sized(hd) = FALSE;
  Link(index, hd);
  Link(hd, x);
  AttachEnv(env, x);
  SetTarget(hd);
  enclose_obj(hd) = (has_enclose(actual(hd)) ? BuildEnclose(hd) : nilobj);
  headers(hd) = dead_headers(hd) = nilobj;

  /* search for destination for hd and release it */
  Link(Up(target), index);
  debug0(DGF,D, "");
  debug1(DGF,D, "  calling FlushGalley(%s) from TransferBegin, root_galley =",
    SymName(actual(hd)));
  ifdebug(DGF, D, DebugGalley(root_galley, nilobj, 4));
  if( whereto(hd) == nilobj || !uses_extern_target(whereto(hd)) ) /* &&& */
    FlushGalley(hd);

  /* if failed to flush, undo everything and exit */
  Parent(index, Up(hd));
  if( type(index) == UNATTACHED && !sized(hd) )
  { DeleteNode(index);
    DisposeObject(hold_env);
    if( LastDown(x) != x )
    { Child(env, LastDown(x));
      if( type(env) == ENV )  DisposeChild(LastDown(x));
    }
    debug1(DGT,D, "] TransferBegin returning failed, x: %s", EchoObject(x));
    return x;
  }

  if( has_rpar(actual(hd)) )
  {
    /* set up new target to be inner \InputSym, or nilobj if none */
    if( ++itop >= MAX_DEPTH )
      Error(18, 2, "galley nested too deeply (max is %d)",
	FATAL, &fpos(x), MAX_DEPTH);
    New(targets[itop], ACAT);  target = nilobj;
    for( link = Down(hd);  link != hd;  link = NextDown(link) )
    { Child(y, link);
      if( type(y) == RECEPTIVE && actual(actual(y)) == InputSym )
      {
	Constrained(actual(y), &constraints[itop], COLM, &why);
	if( FitsConstraint(0, 0, constraints[itop]) )
	{ Link(targets[itop], y);  target = y;
	  debug2(DSC, DD, "Constrained( %s, COLM ) = %s",
	    EchoObject(y), EchoConstraint(&constraints[itop]));
	  env = DetachEnv(actual(y));
	  AttachEnv(new_env, actual(y));
	}
	else
	{ Error(18, 3, "galley %s deleted (insufficient width at target)",
	    WARN, &fpos(hd), SymName(actual(hd)));
	}
	break;
      }
    }

    /* return a token appropriate to the new target */
    if( target == nilobj || external_ver(actual(target)) )
      res = NewToken(GSTUB_EXT, no_fpos, 0, 0, precedence(xsym), nilobj);
    else
    { Constrained(actual(target), &c, ROWM, &why);
      if( constrained(c) )
	Error(18, 4, "right parameter of %s is vertically constrained",
	  FATAL, &fpos(target), SymName(xsym));
      else res = NewToken(GSTUB_INT, no_fpos, 0, 0, precedence(xsym), nilobj);
    }
    debug1(DGT, D, "] TransferBegin returning %s", Image(type(res)));
  }
  else
  {
    res = NewToken(GSTUB_NONE, no_fpos, 0, 0, precedence(xsym), nilobj);
    debug1(DGT, D, "] TransferBegin returning %s", Image(type(res)));
  }

  DisposeObject(hold_env);
  ifdebug(DGT, DD, debug_targets());
  return res;
} /* end TransferBegin */

/*@::TransferComponent()@*****************************************************/
/*                                                                           */
/*  TransferComponent(x)                                                     */
/*                                                                           */
/*  Transfer component x of a galley.                                        */
/*                                                                           */
/*****************************************************************************/

void TransferComponent(OBJECT x)
{ OBJECT y, env, start_search, recs, inners, nothing, hd, dest, dest_index;
  debug1(DGT, D, "[ TransferComponent( %s )", EchoObject(x));
  ifdebug(DGT, DD, debug_targets());

  /* if no dest_index, discard x and exit */
  if( Down(targets[itop]) == targets[itop] )
  { DisposeObject(x);
    debug0(DGT, D, "] TransferComponent returning (no target).");
    return;
  }
  Child(dest_index, Down(targets[itop]));
  assert( external_ver(actual(dest_index)), "TransferComponent: internal!" );

  /* make the component into a galley */
  New(hd, HEAD);
  force_gall(hd) = FALSE;
  enclose_obj(hd) = limiter(hd) = headers(hd) = dead_headers(hd) = nilobj;
  opt_components(hd) = opt_constraints(hd) = nilobj;
  gall_dir(hd) = ROWM;
  FposCopy(fpos(hd), fpos(x));
  actual(hd) = whereto(hd) = ready_galls(hd) = nilobj;
  foll_or_prec(hd) = GALL_FOLL;
  must_expand(hd) = sized(hd) = FALSE;
  Link(hd, x);
  dest = actual(dest_index);
  env = GetEnv(dest);
  debug1(DGT, DD, "  current env chain: %s", EchoObject(env));
  SizeGalley(hd, env, TRUE, threaded(dest), FALSE, TRUE, &save_style(dest),
	&constraints[itop], nilobj, &nothing, &recs, &inners, nilobj);
  if( recs != nilobj )  ExpandRecursives(recs);
  debug3(DSA, D, "after SizeGalley, hd width is (%s,%s), constraint was %s",
    EchoLength(back(hd, COLM)), EchoLength(fwd(hd, COLM)),
    EchoConstraint(&constraints[itop]));

  /* promote the components, remembering where old spot was */
  start_search = PrevDown(Up(dest_index));
  debug1(DSA, D, "  calling AdjustSize from TransferComponent %s",
    EchoFilePos(&fpos(hd)));
  ifdebug(DSA, D,
    Child(y, Down(hd));
    while( type(y) == VCAT )  Child(y, Down(y));
    debug2(DSA, D, "  first component is %s at %s",
      Image(type(y)), EchoFilePos(&fpos(y)));
    if( NextDown(Down(hd)) != hd && NextDown(NextDown(Down(hd))) != hd )
    { Child(y, NextDown(NextDown(Down(hd))));
      debug2(DSA, D, "  second component is %s at %s",
        Image(type(y)), EchoFilePos(&fpos(y)));
    }
  );
  AdjustSize(dest, back(hd, COLM), fwd(hd, COLM), COLM);
  debug0(DGS, D, "calling Promote(hd, hd) from TransferComponent");
  Promote(hd, hd, dest_index, FALSE);
  DeleteNode(hd);

  /* flush any widowed galleys attached to \Input */
  if( Down(dest_index) != dest_index )
  { OBJECT tinners, index;
    New(tinners, ACAT);
    while( Down(dest_index) != dest_index )
    { Child(y, Down(dest_index));
      assert( type(y) == HEAD, "TransferComponent: input child!" );
      if( opt_components(y) != nilobj )
      { DisposeObject(opt_components(y));
	opt_components(y) = nilobj;
	debug1(DOG, D, "TransferComponent de-optimizing %s (@Input case)",
	  SymName(actual(y)));
      }
      DetachGalley(y);
      Parent(index, Up(y));
      MoveLink(Up(index), NextDown(start_search), PARENT);
      Link(tinners, index);
    }
    debug0(DGF, D, "  calling FlushInners() from TransferComponent (a)");
    FlushInners(tinners, nilobj);
  }

  /* flush any galleys inside hd */
  if( inners != nilobj )
  {
    debug0(DGF, D, "  calling FlushInners() from TransferComponent (b)");
    FlushInners(inners, nilobj);
  }

  /* flush parent galley, if needed */
  if( blocked(dest_index) )
  { blocked(dest_index) = FALSE;
    Parent(y, Up(dest_index));
    debug0(DGF, D, "  calling FlushGalley from TransferComponent");
    FlushGalley(y);
  }
  
  debug0(DGT, D, "] TransferComponent returning.");
  ifdebug(DGT, DD, debug_targets());
} /* end TransferComponent */


/*@::TransferEnd()@***********************************************************/
/*                                                                           */
/*  TransferEnd(x)                                                           */
/*                                                                           */
/*  End the transfer of a galley.                                            */
/*                                                                           */
/*****************************************************************************/

void TransferEnd(OBJECT x)
{ OBJECT recs, inners, nothing, z, env, dest, hd, dest_index, y, start_search;
  debug1(DGT, D, "[ TransferEnd( %s )", EchoObject(x));
  ifdebug(DGT, DD, debug_targets());

  /* if no dest_index, discard x and exit */
  if( Down(targets[itop]) == targets[itop] )
  { DisposeObject(x);  DisposeObject(targets[itop--]);
    debug0(DGT, D, "] TransferEnd returning: no dest_index");
    return;
  }
  Child(dest_index, Down(targets[itop]));

  /* make the component into a galley */
  New(hd, HEAD);  FposCopy(fpos(hd), fpos(x));
  force_gall(hd) = FALSE;
  enclose_obj(hd) = limiter(hd) = headers(hd) = dead_headers(hd) = nilobj;
  opt_components(hd) = opt_constraints(hd) = nilobj;
  gall_dir(hd) = ROWM;
  actual(hd) = whereto(hd) = ready_galls(hd) = nilobj;
  foll_or_prec(hd) = GALL_FOLL;
  must_expand(hd) = sized(hd) = FALSE;
  Link(hd, x);  dest = actual(dest_index);  env = GetEnv(dest);
  debug1(DGT, DD, "  current env chain: %s", EchoObject(env));
  SizeGalley(hd, env, external_ver(dest), threaded(dest), FALSE, TRUE,
    &save_style(dest), &constraints[itop], nilobj, &nothing, &recs, &inners,
    nilobj);
  if( recs != nilobj )  ExpandRecursives(recs);
  debug3(DSA, D, "after SizeGalley, hd width is (%s,%s), constraint was %s",
    EchoLength(back(hd, COLM)), EchoLength(fwd(hd, COLM)),
    EchoConstraint(&constraints[itop]));

  /* promote the components, remembering where old spot was */
  start_search = PrevDown(Up(dest_index));
  debug0(DSA, D, "calling AdjustSize from TransferEnd (a)");
  AdjustSize(dest, back(hd, COLM), fwd(hd, COLM), COLM);
  if( !external_ver(dest) )
  { Child(z, LastDown(hd));
    debug0(DSA, D, "calling AdjustSize from TransferEnd (b)");
    AdjustSize(dest, back(z, ROWM), fwd(z, ROWM), ROWM);
    Interpose(dest, VCAT, hd, z);
  }
  debug0(DGS, D, "calling Promote(hd, hd) from TransferEnd");
  Promote(hd, hd, dest_index, TRUE);  DeleteNode(hd);

  /* flush any widowed galleys attached to \Input */
  if( Down(dest_index) != dest_index )
  { OBJECT tinners, index;
    New(tinners, ACAT);
    while( Down(dest_index) != dest_index )
    { Child(y, Down(dest_index));
      assert( type(y) == HEAD, "TransferEnd: input child!" );
      if( opt_components(y) != nilobj )
      { DisposeObject(opt_components(y));
	opt_components(y) = nilobj;
	debug1(DOG, D, "TransferEnd de-optimizing %s (@Input case)",
	  SymName(actual(y)));
      }
      DetachGalley(y);
      Parent(index, Up(y));
      MoveLink(Up(index), NextDown(start_search), PARENT);
      Link(tinners, index);
    }
    debug0(DGF, D, "  calling FlushInners() from TransferEnd (a)");
    FlushInners(tinners, nilobj);
  }

  /* flush any galleys inside hd */
  if( inners != nilobj )
  {
    debug0(DGF, D, "  calling FlushInners() from TransferEnd (b)");
    FlushInners(inners, nilobj);
  }

  /* close dest_index, and flush parent galley if needed */
  if( blocked(dest_index) )
  { Parent(y, Up(dest_index));
    DeleteNode(dest_index);
    debug0(DGF, D, "  calling FlushGalley from TransferEnd");
    FlushGalley(y);
  }
  else DeleteNode(dest_index);
  
  /* pop target stack and exit */
  DisposeObject(targets[itop--]);
  debug0(DGT, D, "] ] TransferEnd returning.");
  ifdebug(DGT, DD, debug_targets());
} /* end TransferEnd */

/*@::TransferClose()@*********************************************************/
/*                                                                           */
/*  TransferClose()                                                          */
/*                                                                           */
/*  Close this module.                                                       */
/*                                                                           */
/*****************************************************************************/

void TransferClose(void)
{ OBJECT inners;
  debug0(DGT, D, "[ TransferClose()");
  ifdebug(DGT, DD, debug_targets());
  debug0(DGA, D, "  calling FreeGalley from TransferClose");
  if( LastDown(root_galley) != root_galley )
  { inners = nilobj;
    FreeGalley(root_galley, root_galley, &inners, nilobj, nilobj);
    if( inners != nilobj )
    {
      debug0(DGF, D, "  calling FlushInners() from TransferClose");
      FlushInners(inners, nilobj);
    }
    debug0(DGF, D, "  calling FlushGalley from TransferClose");
    FlushGalley(root_galley);
  }
  debug0(DGT, D, "] TransferClose returning.");
}
