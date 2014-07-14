/*@z19.c:Galley Attaching:DetachGalley()@*************************************/
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
/*  FILE:         z19.c                                                      */
/*  MODULE:       Galley Attaching                                           */
/*  EXTERNS:      SearchGalley(), AttachGalley(), DetachGalley()             */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  OBJECT InterposeScale(y, scale_factor, dim)                              */
/*                                                                           */
/*  Interpose a @Scale symbol above y with the given scale factor.           */
/*                                                                           */
/*****************************************************************************/

static OBJECT InterposeScale(OBJECT y, int scale_factor, int dim)
{ OBJECT res;
  New(res, SCALE);
  FposCopy(fpos(res), fpos(y));
  if( dim == COLM )
  { bc(constraint(res)) = scale_factor;
    fc(constraint(res)) = 1 * SF;
  }
  else
  { bc(constraint(res)) = 1 * SF;
    fc(constraint(res)) = scale_factor;
  }
  back(res, dim) = (back(y, dim) * scale_factor) / SF;
  fwd(res, dim)  = (fwd(y, dim) * scale_factor) / SF;
  back(res, 1-dim) = back(y, 1-dim);
  fwd(res, 1-dim)  = fwd(y, 1-dim);
  ReplaceNode(res, y);
  Link(res, y);
  return res;
} /* end InterposeScale */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT InterposeWideOrHigh(y, dim)                                       */
/*                                                                           */
/*  Interpose a @Wide or @High symbol above y with the same size as y, with  */
/*  a value which prevents any further increase in the size of y.            */
/*                                                                           */
/*****************************************************************************/

static OBJECT InterposeWideOrHigh(OBJECT y, int dim)
{ OBJECT res;
  New(res, dim == COLM ? WIDE : HIGH);
  FposCopy(fpos(res), fpos(y));
  back(res, dim) = back(y, dim);
  fwd(res, dim)  = fwd(y, dim);
  back(res, 1-dim) = back(y, 1-dim);
  fwd(res, 1-dim)  = fwd(y, 1-dim);
  SetConstraint(constraint(res), MAX_FULL_LENGTH, size(res, dim), MAX_FULL_LENGTH);
  ReplaceNode(res, y);
  Link(res, y);
  return res;
} /* end InterposeWideOrHigh */


/*****************************************************************************/
/*                                                                           */
/*  DetachGalley(hd)                                                         */
/*                                                                           */
/*  Detach galley hd from its target.                                        */
/*                                                                           */
/*****************************************************************************/

void DetachGalley(OBJECT hd)
{ OBJECT prnt, index;
  assert( type(hd) == HEAD && Up(hd) != hd, "DetachGalley: precondition!" );
  debug1(DGA, D, "DetachGalley( %s )", SymName(actual(hd)));
  Parent(prnt, Up(hd));
  assert( Up(prnt) != prnt, "DetachGalley: parent!" );
  New(index, UNATTACHED);
  pinpoint(index) = nilobj;
  MoveLink(Up(hd), index, PARENT);
  Link(NextDown(Up(prnt)), index);
  debug0(DGA, D, "DetachGalley returning.");
} /* end DetachGalley */


/*@::SearchGalley()@**********************************************************/
/*                                                                           */
/*  OBJECT SearchGalley(start, sym, forwards, subgalleys, closures, input)   */
/*                                                                           */
/*  Search a galley and its sub-galleys for a target which uses sym.  The    */
/*  meanings of the flags are as follows:                                    */
/*                                                                           */
/*    forwards     If TRUE, search forwards from just after start, else      */
/*                 search backwards from just before start                   */
/*    subgalleys   If TRUE, search down into sub-galleys of this galley      */
/*    closures     If TRUE, closures in this galley are acceptable results   */
/*    input        If TRUE, InputSym is an acceptable result                 */
/*                                                                           */
/*****************************************************************************/

OBJECT SearchGalley(OBJECT start, OBJECT sym, BOOLEAN forwards,
BOOLEAN subgalleys, BOOLEAN closures, BOOLEAN input)
{ OBJECT y, res, z, zlink, link;
  debug5(DGA, DD, "[ SearchGalley(start, %s, %s, %s, %s, %s)", SymName(sym),
	forwards ? "fwd" : "back", subgalleys ? "subgalleys" : "nosubgalleys",
	closures ? "closures" : "noclosures", input ? "input" : "noinput");
  assert( type(start) == LINK || type(start) == HEAD, "SearchGalley: start!" );

  link = forwards ? NextDown(start) : PrevDown(start);
  res = nilobj;
  while( res == nilobj && type(link) != HEAD )
  { Child(y, link);
    switch( type(y) )
    {
      case UNATTACHED:
      case RECEIVING:
	
        debug1(DGA, DD, "  examining %s", EchoIndex(y));
	if( subgalleys )
	for( zlink = Down(y); zlink!=y && res==nilobj; zlink=NextDown(zlink) )
	{ Child(z, zlink);
	  res = SearchGalley(z, sym, TRUE, TRUE, TRUE, input);
	}
	if( res == nilobj && input && type(y) == RECEIVING &&
	    actual(actual(y)) == InputSym )
	  res = y;
	break;


      case RECEPTIVE:
	
        debug1(DGA, DD, "  examining %s", EchoIndex(y));
	if( closures && type(actual(y)) == CLOSURE
		     && SearchUses(actual(actual(y)), sym) )  res = y;
	else if( input && actual(actual(y)) == InputSym )  res = y;
	break;


      default:
	
	break;

    }
    link = forwards ? NextDown(link) : PrevDown(link);
  }
  debug1(DGA, DD, "] SearchGalley returning %s", EchoIndex(res));
  return res;
} /* end SearchGalley */


/*@@**************************************************************************/
/*                                                                           */
/*  int AttachGalley(hd, inners, suspend_pt)                                 */
/*                                                                           */
/*  Attach galley hd, which may be unsized, to a destination.  This involves */
/*  searching for a destination forward or back from the attachment point of */
/*  hd and promoting up to and including the first definite component of hd. */
/*                                                                           */
/*  Although AttachGalley never flushes any galleys, it may identify some    */
/*  galleys which should be flushed, even if the attach is itself not        */
/*  successful.  These are returned in *inners, or nilobj if none.           */
/*                                                                           */
/*  The integer returned by AttachGalley indicates what happened to hd:      */
/*                                                                           */
/*    ATTACH_KILLED     The galley was sized to begin with but no target     */
/*                      for it could be found.  The galley has been killed   */
/*                      and that's the end of it.                            */
/*                                                                           */
/*    ATTACH_INPUT      When searching for a target for the galley we came   */
/*                      upon InputSym, suggesting that the target might be   */
/*                      still to be read.  So the galley has been linked to  */
/*                      that InputSym and must now wait.                     */
/*                                                                           */
/*    ATTACH_NOTARGET   The galley is unsized and no target could be found   */
/*                      for it.  This is fine, it just means that we can't   */
/*                      flush the galley now and we must try again later.    */
/*                                                                           */
/*    ATTACH_SUSPEND    The galley is sized and a target was found for it,   */
/*                      but the first component of the galley proved to be   */
/*                      indefinite so could not be promoted.  The galley     */
/*                      remains unattached but is moved to just before its   */
/*                      target so that it can find it easily later when its  */
/*                      first component becomes definite and it is flushed.  */
/*                                                                           */
/*    ATTACH_NULL       The galley is sized and a target was found for it,   */
/*                      but the body of the galley proved to be null (i.e.   */
/*                      there were no definite components to be flushed).    */
/*                      This is to be treated just like the normal case      */
/*                      following, except that the target is replaced by     */
/*                      @Null rather than by its body.                       */
/*                                                                           */
/*    ATTACH_ACCEPT     The galley is sized and a target was found for it,   */
/*                      and one component of the galley has been promoted.   */
/*                                                                           */
/*****************************************************************************/

int AttachGalley(OBJECT hd, OBJECT *inners, OBJECT *suspend_pt)
{ OBJECT hd_index;		/* the index of hd in the enclosing galley   */
  OBJECT hd_inners;		/* inner galleys of hd, if unsized           */
  OBJECT dest;			/* the target @Galley hd empties into        */
  OBJECT dest_index;		/* the index of dest                         */
  OBJECT target;		/* the target indefinite containing dest     */
  OBJECT target_index;		/* the index of target                       */
  OBJECT target_galley;		/* the body of target, made into a galley    */
  OBJECT tg_inners;		/* inner galleys of target_galley            */
  BOOLEAN need_precedes;	/* true if destination lies before galley    */
  OBJECT recs;			/* list of recursive definite objects        */
  OBJECT link, y;		/* for scanning through the components of hd */
  CONSTRAINT c;			/* temporary variable holding a constraint   */
  OBJECT env, n1, tmp, zlink, z, sym;	/* placeholders and temporaries	     */
  BOOLEAN was_sized;		/* true if sized(hd) initially               */
  int dim;			/* the galley direction                      */
  FULL_LENGTH perp_back, perp_fwd;
  OBJECT why, junk;

  debug2(DGA, D, "[ AttachGalley(Galley %s into %s)",
	SymName(actual(hd)), SymName(whereto(hd)));
  ifdebug(DGA, DD, DebugGalley(hd, nilobj, 4));
  assert( Up(hd) != hd, "AttachGalley: no index!" );
  Parent(hd_index, Up(hd));
  assert( type(hd_index) == UNATTACHED, "AttachGalley: not UNATTACHED!" );
  hd_inners = tg_inners = nilobj;
  was_sized = sized(hd);
  dim = gall_dir(hd);

  for(;;)
  {
    /*************************************************************************/
    /*                                                                       */
    /*  Search for a destination for hd.  If hd is unsized, search for       */
    /*  inner galleys preceding it first of all, then for receptive objects  */
    /*  following it, possibly in inner galleys.  If no luck, exit.          */
    /*  If hd is sized, search only for receptive objects in the current     */
    /*  galley below the current spot, and fail if cannot find any.          */
    /*                                                                       */
    /*************************************************************************/

    sym = whereto(hd);
    if( sized(hd) )
    {
      /* sized galley case: search on from current spot */
      target_index = SearchGalley(Up(hd_index), sym, TRUE, FALSE, TRUE, TRUE);
      if( target_index == nilobj )
      {	
	/* search failed to find any new target, so kill the galley */
	for( link = Down(hd); link != hd; link = NextDown(link) )
	{ Child(y, link);
	  if( type(y) == SPLIT )  Child(y, DownDim(y, dim));
	  if( is_definite(type(y)) )  break;
	}
	if( link != hd )
	  Error(19, 1, "galley %s deleted from here (no target)",
	    WARN, &fpos(y), SymName(actual(hd)));
	if( hd_inners != nilobj )  DisposeObject(hd_inners), hd_inners=nilobj;
	if( tg_inners != nilobj )  DisposeObject(tg_inners), tg_inners=nilobj;
	KillGalley(hd, FALSE);
	*inners = nilobj;
	debug0(DGA, D, "] AttachGalley returning ATTACH_KILLED");
	return ATTACH_KILLED;
      }
      else if( actual(actual(target_index)) == InputSym )
      {
	/* search found input object, so suspend on that */
	DeleteNode(hd_index);
	Link(target_index, hd);
	*inners = nilobj;
	debug0(DGA, D, "] AttachGalley returning ATTACH_INPUT");
	return ATTACH_INPUT;
      }

    }
    else /* unsized galley, either backwards or normal */
    {
      if( foll_or_prec(hd) == GALL_PREC )
      {	target_index= SearchGalley(Up(hd_index), sym, FALSE, TRUE,TRUE,FALSE);
	need_precedes = FALSE;
      }
      else
      {	target_index = SearchGalley(Up(hd_index), sym, FALSE,TRUE,FALSE,FALSE);
	need_precedes = (target_index != nilobj);
	if( target_index == nilobj )
	  target_index = SearchGalley(Up(hd_index), sym, TRUE,TRUE,TRUE,FALSE);
      }

      /* if no luck, exit without error */
      if( target_index == nilobj )
      {	*inners = nilobj;
	debug0(DGA, D, "] AttachGalley returning ATTACH_NOTARGET");
	return ATTACH_NOTARGET;
      }
    }
    assert( type(target_index) == RECEPTIVE, "AttachGalley: target_index!" );
    target = actual(target_index);
    assert( type(target) == CLOSURE, "AttachGalley: target!" );

    /* set target_galley to the expanded value of target */
    debug1(DYY, D, "[ EnterErrorBlock(FALSE) (expanding target %s)",
      SymName(actual(target)));
    EnterErrorBlock(FALSE);
    New(target_galley, HEAD);
    force_gall(target_galley) = FALSE;
    enclose_obj(target_galley) = limiter(target_galley) = nilobj;
    headers(target_galley) = dead_headers(target_galley) = nilobj;
    opt_components(target_galley) = opt_constraints(target_galley) = nilobj;
    gall_dir(target_galley) = external_hor(target) ? COLM : ROWM;
    FposCopy(fpos(target_galley), fpos(target));
    actual(target_galley) = actual(target);
    whereto(target_galley) = ready_galls(target_galley) = nilobj;
    foll_or_prec(target_galley) = GALL_FOLL;
    must_expand(target_galley) = FALSE;
    sized(target_galley) = FALSE;

    /* get perpendicular constraint (none if horizontal galley) */
    if( dim == ROWM )
    {
      Constrained(target, &c, 1-dim, &junk);
      if( !constrained(c) )
        Error(19, 2, "receptive symbol %s has unconstrained width",
	  FATAL, &fpos(target), SymName(actual(target)));
      debug2(DSC, DD, "Constrained( %s, 1-dim ) = %s",
	EchoObject(target), EchoConstraint(&c));
      if( !FitsConstraint(0, 0, c) )
      { debug0(DGA, D, "  reject: target_galley horizontal constraint is -1");
	y = nilobj;
        goto REJECT;
      }
    }
    else /* actually unused */
      SetConstraint(c, MAX_FULL_LENGTH, MAX_FULL_LENGTH, MAX_FULL_LENGTH);

    debug1(DGA, DDD, "  expanding %s", EchoObject(target));
    tmp = CopyObject(target, no_fpos);
    Link(target_galley, tmp);
    env = DetachEnv(tmp);
    debug4(DGM, D, "  external_ver(%s) = %s, external_hor(%s) = %s",
      SymName(actual(target)), bool(external_ver(target)),
      SymName(actual(target)), bool(external_hor(target)));
    SizeGalley(target_galley, env,
	external_ver(target) || external_hor(target),
	threaded(target), non_blocking(target_index),
	trigger_externs(target_index), &save_style(target),
	&c, whereto(hd), &dest_index, &recs, &tg_inners,
	enclose_obj(hd) != nilobj ? CopyObject(enclose_obj(hd), no_fpos):nilobj);
    debug1(DGA, DD, "  SizeGalley tg_inners: %s", DebugInnersNames(tg_inners));
    if( recs != nilobj )  ExpandRecursives(recs);
    dest = actual(dest_index);
    if( underline(dest) == UNDER_UNDEF )  underline(dest) = UNDER_OFF;

    /* verify that hd satisfies any horizontal constraint on dest */
    if( dim == ROWM )
    {
      debug1(DGA, DDD, "  checking hor fit of hd in %s",SymName(actual(dest)));
      Constrained(dest, &c, 1-dim, &junk);
      debug3(DSC, DD, "Constrained( %s, %s ) = %s",
	EchoObject(dest), dimen(1-dim), EchoConstraint(&c));
      assert( constrained(c), "AttachGalley: dest unconstrained!" );
      if( !FitsConstraint(0, 0, c) )
      { debug0(DGA, D, "  reject: hd horizontal constraint is -1");
	y = nilobj;
        goto REJECT;
      }
    }

    /* manifest and size the galley if not done yet */
    if( !sized(hd) )
    {
      debug2(DYY, D, "[ EnterErrorBlock(TRUE) (sizing galley %s into %s)",
	SymName(actual(hd)), SymName(whereto(hd)));
      EnterErrorBlock(TRUE);
      n1 = nilobj;
      Child(y, Down(hd));
      env = DetachEnv(y);
      /*** threaded() only defined in ROWM case
      SizeGalley(hd, env, TRUE, threaded(dest), non_blocking(target_index),
	TRUE, &save_style(dest), &c, nilobj, &n1, &recs, &hd_inners);
      *** */
      SizeGalley(hd, env, TRUE, dim == ROWM ? threaded(dest) : FALSE,
	non_blocking(target_index), TRUE, &save_style(dest), &c, nilobj,
	&n1, &recs, &hd_inners, nilobj);
      debug1(DGA,DD,"  SizeGalley hd_inners: %s", DebugInnersNames(hd_inners));
      if( recs != nilobj )  ExpandRecursives(recs);
      if( need_precedes )		/* need an ordering constraint */
      {	OBJECT index1, index2;
        New(index1, PRECEDES);
	New(index2, FOLLOWS);
	blocked(index2) = FALSE;
	tmp = MakeWord(WORD, STR_EMPTY, no_fpos);
	Link(index1, tmp);  Link(index2, tmp);
	Link(Up(hd_index), index1);
	Link(Down(hd), index2);
	debug0(DGA, D, "  inserting PRECEDES and FOLLOWS");
      }
      LeaveErrorBlock(TRUE);
      debug0(DYY, D, "] LeaveErrorBlock(TRUE) (finished sizing galley)");
    }

    if( dim == ROWM )
    { if( !FitsConstraint(back(hd, 1-dim), fwd(hd, 1-dim), c) )
      { debug3(DGA, D, "  reject: hd %s,%s does not fit target_galley %s",
	  EchoLength(back(hd, 1-dim)), EchoLength(fwd(hd, 1-dim)),
	  EchoConstraint(&c));
        Error(19, 3, "too little horizontal space for galley %s at %s",
	  WARN, &fpos(hd), SymName(actual(hd)), SymName(actual(dest)));
        goto REJECT;
      }
    }

    /* check status of first component of hd */
    debug0(DGA, DDD, "  now ready to attach; hd =");
    ifdebug(DGA, DDD, DebugObject(hd));
    for( link = Down(hd);  link != hd;  link = NextDown(link) )
    {
      Child(y, link);
      debug1(DGA, DDD, "  examining %s", EchoIndex(y));
      if( type(y) == SPLIT )  Child(y, DownDim(y, dim));
      switch( type(y) )
      {

	case EXPAND_IND:
	case SCALE_IND:
	case COVER_IND:
	case GALL_PREC:
	case GALL_FOLL:
	case GALL_FOLL_OR_PREC:
	case GALL_TARG:
	case CROSS_PREC:
	case CROSS_FOLL:
	case CROSS_FOLL_OR_PREC:
	case CROSS_TARG:
	case PAGE_LABEL_IND:
	    
	  break;


	case PRECEDES:
	case UNATTACHED:
	    
	  if( was_sized )
	  { /* SizeGalley was not called, so hd_inners was not set by it */
	    if( hd_inners == nilobj )  New(hd_inners, ACAT);
	    Link(hd_inners, y);
	  }
	  break;


	case RECEPTIVE:

	  goto SUSPEND;


	case RECEIVING:
	    
	  goto SUSPEND;


	case FOLLOWS:
	    
	  Child(tmp, Down(y));
	  if( Up(tmp) == LastUp(tmp) )
	  { link = pred(link, CHILD);
	    debug0(DGA, DD, "  disposing FOLLOWS");
	    DisposeChild(NextDown(link));
	    break;
	  }
	  Parent(tmp, Up(tmp));
	  assert(type(tmp) == PRECEDES, "Attach: PRECEDES!");
	  switch( CheckComponentOrder(tmp, target_index) )
	  {
	    case CLEAR:		DeleteNode(tmp);
				link = pred(link, CHILD);
				DisposeChild(NextDown(link));
				break;

	    case PROMOTE:	break;

	    case BLOCK:		debug0(DGA, DD, "CheckContraint: BLOCK");
				goto SUSPEND;

	    case CLOSE:		debug0(DGA, D, "  reject: CheckContraint");
				goto REJECT;
	  }
	  break;


	case GAP_OBJ:

	  underline(y) = underline(dest);
	  if( !join(gap(y)) )  seen_nojoin(hd) = TRUE;
	  break;


	case BEGIN_HEADER:
	case END_HEADER:
	case SET_HEADER:
	case CLEAR_HEADER:

	  /* do nothing until actually promoted out of here */
	  underline(y) = underline(dest);
	  break;


	case CLOSURE:
	case CROSS:
	case FORCE_CROSS:
	case NULL_CLOS:
	case PAGE_LABEL:

	  underline(y) = underline(dest);
	  break;


	case WORD:
	case QWORD:
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
	case ROTATE:
	case BACKGROUND:
	case SCALE:
	case KERN_SHRINK:
	case INCGRAPHIC:
	case SINCGRAPHIC:
	case PLAIN_GRAPHIC:
	case GRAPHIC:
	case LINK_SOURCE:
	case LINK_DEST:
	case ACAT:
	case HCAT:
	case VCAT:
	case ROW_THR:
	case COL_THR:
	    

	  underline(y) = underline(dest);
	  if( dim == ROWM )
	  {
	    /* make sure y is not joined to a target below (vertical only) */
	    for( zlink = NextDown(link); zlink != hd; zlink = NextDown(zlink) )
	    { Child(z, zlink);
	      switch( type(z) )
	      {
	        case RECEPTIVE:
		
		  if( non_blocking(z) )
		  { zlink = PrevDown(zlink);
		    DeleteNode(z);
		  }
		  else
		  { y = z;
		    goto SUSPEND;
		  }
		  break;


	        case RECEIVING:
		
		  if( non_blocking(z) )
		  { zlink = PrevDown(zlink);
		    while( Down(z) != z )
		    { Child(tmp, Down(y));
		      if( opt_components(tmp) != nilobj )
		      { DisposeObject(opt_components(tmp));
		        opt_components(tmp) = nilobj;
		        debug3(DOG, D, "AttachGalley(%s) de-optimizing %s %s",
			  SymName(actual(hd)), SymName(actual(tmp)), "(join)");
		      }
		      DetachGalley(tmp);
		      KillGalley(tmp, FALSE);
		    }
		    DeleteNode(z);
		  }
		  else
		  { y = z;
		    goto SUSPEND;
		  }
		  break;


	        case GAP_OBJ:
		
		  if( !join(gap(z)) )  zlink = PrevDown(hd);
		  break;


	        default:	break;
	      }
	    }

	    /* if HCAT, try vertical hyphenation (vertical galleys only) */
	    if( type(y) == HCAT )  VerticalHyphenate(y);
	  }


	  /* check availability of parallel space for the first component */
	  why = nilobj;
	  Constrained(dest, &c, dim, &why);
	  debug3(DGF, DD, "  dest parallel Constrained(%s, %s) = %s",
	    EchoObject(dest), dimen(dim), EchoConstraint(&c));
	  if( !FitsConstraint(back(y, dim), fwd(y, dim), c) )
	  { BOOLEAN scaled;

	    /* if forcing galley doesn't fit, try scaling first component */
	    scaled = FALSE;
	    if( force_gall(hd) && size(y, dim) > 0 )
	    { int scale_factor;
	      scale_factor = ScaleToConstraint(back(y,dim), fwd(y,dim), &c);
	      if( scale_factor > 0.5 * SF )
	      {	char num1[20], num2[20];
		sprintf(num1, "%.1fc", (float) size(y, dim) / CM);
		sprintf(num2, "%.1fc", (float) bfc(c) / CM);
		if( dim == ROWM )
		  Error(19, 4, "%s object too high for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		else
		  Error(19, 5, "%s object too wide for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		y = InterposeScale(y, scale_factor, dim);
		scaled = TRUE;
	      }
	    }

	    /* otherwise we must reject, and warn the user */
	    if( !scaled )
	    { char num1[20], num2[20];
	      debug3(DGA, D, "  reject: vsize %s,%s in %s; y=",
		EchoLength(back(y, dim)), EchoLength(fwd(y, dim)),
		EchoConstraint(&c));
	      ifdebug(DGA, D, DebugObject(y));
	      if( size(y, dim) > 0 )
	      { sprintf(num1, "%.1fc", (float) size(y, dim) / CM);
	        sprintf(num2, "%.1fc", (float) bfc(c) / CM);
	        if( dim == ROWM )
		  Error(19, 12, "%s object too high for %s space; will try elsewhere",
		    WARN, &fpos(y), num1, num2);
	        else
		  Error(19, 13, "%s object too wide for %s space; will try elsewhere",
		    WARN, &fpos(y), num1, num2);
	      }
	      goto REJECT;
	    }

	  }

	  /* check availability of perpendicular space for first component */
	  if( dim == ROWM )
	  { perp_back = back(hd, 1-dim);  perp_fwd = fwd(hd, 1-dim);
	  }
	  else
	  { perp_back = back(y, 1-dim);  perp_fwd = fwd(y, 1-dim);
	  }
	  Constrained(dest, &c, 1-dim, &junk);
	  debug3(DGF, DD, "  dest perpendicular Constrained(%s, %s) = %s",
	    EchoObject(dest), dimen(1-dim), EchoConstraint(&c));
	  if( !FitsConstraint(perp_back, perp_fwd, c) )
	  { BOOLEAN scaled;

	    /* if forcing galley doesn't fit, try scaling first component */
	    scaled = FALSE;
	    if( force_gall(hd) && perp_back + perp_fwd > 0 )
	    { int scale_factor;
	      scale_factor = ScaleToConstraint(perp_back, perp_fwd, &c);
	      if( scale_factor > 0.5 * SF )
	      {	char num1[20], num2[20];
		sprintf(num1, "%.1fc", (float) (perp_back + perp_fwd) / CM);
		sprintf(num2, "%.1fc", (float) bfc(c) / CM);
		if( 1-dim == ROWM )
		  Error(19, 6, "%s object too high for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		else
		  Error(19, 7, "%s object too wide for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		y = InterposeScale(y, scale_factor, 1-dim);
		scaled = TRUE;
	      }
	    }

	    /* otherwise we must reject, and warn the user */
	    if( !scaled )
	    {
	      debug3(DGA, D, "  reject: vsize %s,%s in %s; y=",
		EchoLength(perp_back), EchoLength(perp_fwd),
		EchoConstraint(&c));
	      ifdebug(DGA, D, DebugObject(y));
	      goto REJECT;
	    }

	  }

	  /* dest seems OK, so perform its size adjustments */
	  debug0(DSA, D, "calling AdjustSize from AttachGalley (a)");
	  AdjustSize(dest, back(y, dim), fwd(y, dim), dim);
	  debug0(DSA, D, "calling AdjustSize from AttachGalley (b)");
	  AdjustSize(dest, perp_back, perp_fwd, 1-dim);


	  /* now check parallel space for target_galley in target */
	  Constrained(target, &c, dim, &why);
	  debug3(DGF, DD, "  target parallel Constrained(%s, %s) = %s",
	    EchoObject(target), dimen(dim), EchoConstraint(&c));
	  Child(z, LastDown(target_galley));  /* works in all cases? */
	  assert( !is_index(type(z)), "AttachGalley: is_index(z)!" );
	  assert( back(z, dim)>=0 && fwd(z, dim)>=0, "AttachGalley: z size!" );
	  if( !FitsConstraint(back(z, dim), fwd(z, dim), c) )
	  { BOOLEAN scaled;

	    debug2(DGA, D, "  why     = %d %s", (int) why, EchoObject(why));
	    debug2(DGA, D, "  limiter = %d %s", (int) limiter(hd),
	      EchoObject(limiter(hd)));

	    /* if forcing galley doesn't fit, try scaling z */
	    scaled = FALSE;
	    if( force_gall(hd) && size(z, dim) > 0 && limiter(hd) != why )
	    { int scale_factor;
	      scale_factor = ScaleToConstraint(back(z,dim), fwd(z,dim), &c);
	      if( scale_factor > 0.5 * SF )
	      {	char num1[20], num2[20];
		sprintf(num1, "%.1fc", (float) size(z, dim) / CM);
		sprintf(num2, "%.1fc", (float) bfc(c) / CM);
		if( dim == ROWM )
		  Error(19, 8, "%s object too high for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		else
		  Error(19, 9, "%s object too wide for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		z = InterposeWideOrHigh(z, dim);
		z = InterposeScale(z, scale_factor, dim);
		scaled = TRUE;
	      }
	    }

	    if( !scaled )
	    { char num1[20], num2[20];
	      limiter(hd) = why;
	      debug3(DGA, D, "  set limiter(%s) = %d %s", SymName(actual(hd)),
		(int) limiter(hd), EchoObject(limiter(hd)));
	      debug3(DGA, D, "  reject: size was %s,%s in %s; y =",
		EchoLength(back(z, dim)), EchoLength(fwd(z, dim)),
		EchoConstraint(&c));
	      ifdebug(DGA, D, DebugObject(y));
	      if( size(z, dim) > 0 )
	      { sprintf(num1, "%.1fc", (float) size(z, dim) / CM);
	        sprintf(num2, "%.1fc", (float) bfc(c) / CM);
	        if( dim == ROWM )
		  Error(19, 14, "%s object too high for %s space; will try elsewhere",
		    WARN, &fpos(y), num1, num2);
	        else
		  Error(19, 15, "%s object too wide for %s space; will try elsewhere",
		    WARN, &fpos(y), num1, num2);
	      }
	      goto REJECT;
	    }
	  }
	  limiter(hd) = why;
	  debug3(DGA, D, "  set limiter(%s) = %d %s", SymName(actual(hd)),
	    (int) limiter(hd), EchoObject(limiter(hd)));

	  /* now check perpendicular space for target_galley in target */
	  Constrained(target, &c, 1-dim, &junk);
	  debug3(DGF, DD, "  target perpendicular Constrained(%s, %s) = %s",
	    EchoObject(target), dimen(1-dim), EchoConstraint(&c));
	  Child(z, LastDown(target_galley));  /* works in all cases? */
	  assert( !is_index(type(z)), "AttachGalley: is_index(z)!" );
	  assert( back(z, 1-dim)>=0 && fwd(z, 1-dim)>=0,
	    "AttachGalley: z size (perpendicular)!" );
	  if( !FitsConstraint(back(z, 1-dim), fwd(z, 1-dim), c) )
	  { BOOLEAN scaled;

	    /* if forcing galley doesn't fit, try scaling z */
	    scaled = FALSE;
	    if( force_gall(hd) && size(z, 1-dim) > 0 )
	    { int scale_factor;
	      scale_factor = ScaleToConstraint(back(z,1-dim), fwd(z,1-dim), &c);
	      if( scale_factor > 0.5 * SF )
	      {	char num1[20], num2[20];
		sprintf(num1, "%.1fc", (float) size(z, 1-dim) / CM);
		sprintf(num2, "%.1fc", (float) bfc(c) / CM);
		if( 1-dim == ROWM )
		  Error(19, 10, "%s object too high for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		else
		  Error(19, 11, "%s object too wide for %s space; %s inserted",
		    WARN, &fpos(y), num1, num2, KW_SCALE);
		z = InterposeWideOrHigh(z, 1-dim);
		z = InterposeScale(z, scale_factor, 1-dim);
		scaled = TRUE;
	      }
	    }

	    if( !scaled )
	    {
	      debug3(DGA, D, "  reject: size was %s,%s in %s; y =",
		EchoLength(back(z, 1-dim)), EchoLength(fwd(z, 1-dim)),
		EchoConstraint(&c));
	      ifdebug(DGA, D, DebugObject(y));
	      goto REJECT;
	    }
	  }

	  /* target seems OK, so adjust sizes and accept */
	  if( external_hor(target) )
	  {
	    /* don't adjust any sizes, none to adjust */
	    debug0(DSA, D, "not calling AdjustSize from AttachGalley (c)");
	  }
	  else if( external_ver(target) )
	  {
	    /* adjust perp size only, to galley size */
	    debug0(DSA, D, "calling AdjustSize from AttachGalley (d)");
	    AdjustSize(target, back(target_galley, 1-dim),
	      fwd(target_galley, 1-dim), 1-dim);
	  }
	  else
	  {
	    /* adjust both directions, using z (last component) */
	    Child(z, LastDown(target_galley));
	    debug0(DSA, D, "AttachGalley AdjustSize using z =");
	    ifdebug(DSA, D, DebugObject(z));
	    debug0(DSA, D, "calling AdjustSize from AttachGalley (e)");
	    AdjustSize(target, back(z, dim), fwd(z, dim), dim);
	    debug0(DSA, D, "calling AdjustSize from AttachGalley (f)");
	    AdjustSize(target, back(z, 1-dim), fwd(z, 1-dim), 1-dim);
	  }

	  goto ACCEPT;


	default:
	    
	  assert1(FALSE, "AttachGalley:", Image(type(y)));
	  break;

      } /* end switch */
    } /* end for */

    /* null galley: promote whole galley without expanding the target */
    debug0(DGA, D, "  null galley");
    if( tg_inners != nilobj )  DisposeObject(tg_inners), tg_inners = nilobj;
    DisposeObject(target_galley);
    LeaveErrorBlock(FALSE);
    debug0(DYY, D, "] LeaveErrorBlock(FALSE) (null galley)");

    /* kill off any null objects within the galley, then transfer it */
    /* don't use Promote() since it does extra unwanted things here  */
    for( link = Down(hd);  link != hd;  link = NextDown(link) )
    { Child(y, link);
      switch( type(y) )
      {

	case GAP_OBJ:
	case CLOSURE:
	case CROSS:
	case FORCE_CROSS:
	case NULL_CLOS:
	case PAGE_LABEL:
	
	  link = PrevDown(link);
	  debug1(DGA, D, "  null galley, disposing %s", Image(type(y)));
	  DisposeChild(NextDown(link));
	  break;

	
	default:
	
	  break;
      }
    }
    TransferLinks(NextDown(hd), hd, Up(target_index));

    /* attach hd temporarily to target_index */
    MoveLink(Up(hd), target_index, PARENT);
    assert( type(hd_index) == UNATTACHED, "AttachGalley: type(hd_index)!" );
    DeleteNode(hd_index);

    /* return; only hd_inners needs to be flushed now */
    *inners = hd_inners;
    debug0(DGA, D, "] AttachGalley returning ATTACH_NULL");
    return ATTACH_NULL;


    REJECT:
	
      /* reject first component */
      /* debug1(DGA, D, "  reject %s", EchoObject(y)); */
      debug0(DGA, D, "  reject first component");
      LeaveErrorBlock(TRUE);
      debug0(DYY, D, "] LeaveErrorBlock(TRUE) (REJECT)");
      if( tg_inners != nilobj )  DisposeObject(tg_inners), tg_inners = nilobj;
      DisposeObject(target_galley);
      if( foll_or_prec(hd) == GALL_PREC && !sized(hd) )
      {
	/* move to just before the failed target */
	MoveLink(Up(hd_index), Up(target_index), PARENT);
      }
      else
      {
	/* move to just after the failed target */
	MoveLink(Up(hd_index), NextDown(Up(target_index)), PARENT);
      }
      continue;


    SUSPEND:
	
      /* suspend at first component */
      debug1(DGA, D, "  suspend %s", EchoIndex(y));
      blocked(y) = TRUE;
      LeaveErrorBlock(FALSE);
      debug0(DYY, D, "] LeaveErrorBlock(FALSE) (SUSPEND)");
      if( tg_inners != nilobj )  DisposeObject(tg_inners), tg_inners = nilobj;
      DisposeObject(target_galley);
      MoveLink(Up(hd_index), Up(target_index), PARENT);
      if( was_sized )
      { /* nothing new to flush if suspending and already sized */
	if( hd_inners != nilobj )  DisposeObject(hd_inners), hd_inners=nilobj;
	*inners = nilobj;
      }
      else
      { /* flush newly discovered inners if not sized before */
	*inners = hd_inners;
      }
      debug0(DGA, D, "] AttachGalley returning ATTACH_SUSPEND");
      *suspend_pt = y;
      return ATTACH_SUSPEND;


    ACCEPT:
	
      /* accept first component; now committed to the attach */
      debug3(DGA, D, "  accept %s %s %s", Image(type(y)), EchoObject(y),
	EchoFilePos(&fpos(y)));
      LeaveErrorBlock(TRUE);
      debug0(DYY, D, "] LeaveErrorBlock(TRUE) (ACCEPT)");

      /* attach hd to dest */
      MoveLink(Up(hd), dest_index, PARENT);
      assert( type(hd_index) == UNATTACHED, "AttachGalley: type(hd_index)!" );
      DeleteNode(hd_index);

      /* move first component of hd into dest */
      /* nb Interpose must be done after all AdjustSize calls */
      if( dim == ROWM && !external_ver(dest) )
	Interpose(dest, VCAT, hd, y);
      else if( dim == COLM && !external_hor(dest) )
      { Interpose(dest, ACAT, y, y);
	Parent(junk, Up(dest));
	assert( type(junk) == ACAT, "AttachGalley: type(junk) != ACAT!" );
	StyleCopy(save_style(junk), save_style(dest));
	adjust_cat(junk) = padjust(save_style(junk));
      }
      debug1(DGS, D, "calling Promote(hd, %s) from AttachGalley/ACCEPT",
	link == hd ? "hd" : "NextDown(link)");
      Promote(hd, link == hd ? hd : NextDown(link), dest_index, TRUE);

      /* move target_galley into target */
      /* nb Interpose must be done after all AdjustSize calls */
      if( !(external_ver(target) || external_hor(target)) )
      {	Child(z, LastDown(target_galley));
	Interpose(target, VCAT, z, z);
      }
      debug0(DGS, D, "calling Promote(target_galley) from AttachGalley/ACCEPT");
      Promote(target_galley, target_galley, target_index, TRUE);
      DeleteNode(target_galley);
      assert(Down(target_index)==target_index, "AttachGalley: target_ind");
      if( blocked(target_index) )  blocked(dest_index) = TRUE;
      DeleteNode(target_index);

      /* return; both tg_inners and hd_inners need to be flushed now;        */
      /* if was_sized, hd_inners contains the inners of the first component; */
      /* otherwise it contains the inners of all components, from SizeGalley */
      if( tg_inners == nilobj ) *inners = hd_inners;
      else if( hd_inners == nilobj ) *inners = tg_inners;
      else
      {	TransferLinks(Down(hd_inners), hd_inners, tg_inners);
	DeleteNode(hd_inners);
	*inners = tg_inners;
      }
      debug0(DGA, D, "] AttachGalley returning ATTACH_ACCEPT");
      ifdebug(DGA, D,
	if( dim == COLM && !external_hor(dest) )
	{ OBJECT z;
	  Parent(z, Up(dest));
	  debug2(DGA, D, "  COLM dest_encl on exit = %s %s",
	    Image(type(z)), EchoObject(z));
	}
      )
      return ATTACH_ACCEPT;

  } /* end for */
} /* end AttachGalley */
