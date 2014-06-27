/*@z20.c:Galley Flushing:DebugInnersNames()@**********************************/
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
/*  FILE:         z20.c                                                      */
/*  MODULE:       Galley Flushing                                            */
/*  EXTERNS:      FlushGalley()                                              */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

#if DEBUG_ON
FULL_CHAR *DebugInnersNames(OBJECT inners)
{ static FULL_CHAR buff[MAX_BUFF];
  OBJECT link, y, z;
  StringCopy(buff, STR_EMPTY);
  if( inners != nilobj )
  { for( link = Down(inners);  link != inners;  link = NextDown(link) )
    { Child(y, link);
      if( link != Down(inners) )  StringCat(buff, STR_SPACE);
      switch( type(y) )
      {

        case RECEIVING:
        case UNATTACHED:
      
	  assert( Down(y) != y, "DebugInnersNames: UNATTACHED!");
	  Child(z, Down(y));
          StringCat(buff, SymName(actual(z)));
	  break;


        case PRECEDES:
        case GALL_PREC:
        case DEAD:
      
	  StringCat(buff, Image(type(y)));
	  break;


        default:
      
	  assert1(FALSE, "DebugInnersNames:", Image(type(y)));
	  break;
      }
    }
  }
  return buff;
} /* end DebugInnersNames */
#endif


/*@::ParentFlush(), FlushGalley()@********************************************/
/*                                                                           */
/*  ParentFlush(prnt_flush, dest_index, kill)                                */
/*                                                                           */
/*  Flush the galley which is the parent of dest_index, if likely to flush.  */
/*  If kill is TRUE, delete dest_index.                                      */
/*                                                                           */
/*****************************************************************************/

static void ParentFlush(BOOLEAN prnt_flush, OBJECT dest_index, BOOLEAN kill)
{ OBJECT prnt;
  debug3(DGF, DD, "ParentFlush(%s, %s, %s)",
    bool(prnt_flush), EchoIndex(dest_index), bool(kill));
  if( prnt_flush )
  { Parent(prnt, Up(dest_index));
    if( kill )  DeleteNode(dest_index);
    debug0(DGF, DD, "  calling FlushGalley from ParentFlush");
    FlushGalley(prnt);
  }
  else if( kill )  DeleteNode(dest_index)
  debug0(DGF, DD, "ParentFlush returning.");
} /* end ParentFlush */


/*****************************************************************************/
/*                                                                           */
/*  FlushGalley(hd)                                                          */
/*                                                                           */
/*  Flush galley hd as far as possible.  It could be the root galley.        */
/*                                                                           */
/*****************************************************************************/

void FlushGalley(OBJECT hd)
{ OBJECT dest;			/* the target galley hd empties into         */
  OBJECT dest_index;		/* the index of dest                         */
  OBJECT inners;		/* list of galleys and PRECEDES to flush     */
  OBJECT link, y;		/* for scanning through the components of hd */
  int dim;			/* direction of galley                       */
  CONSTRAINT dest_par_constr;	/* the parallel size constraint on dest      */
  CONSTRAINT dest_perp_constr;	/* the perpendicular size constraint on dest */
  int pb, pf, f;		/* candidate replacement sizes for dest      */

  OBJECT dest_encl;		/* the VCAT or ACAT enclosing dest, if any   */
  int    dest_side;		/* if dest_encl != nilobj, side dest is on   */
  BOOLEAN need_adjust;		/* TRUE as soon as dest_encl needs adjusting */
  FULL_LENGTH dest_back, dest_fwd; /* the current size of dest_encl or dest  */
  FULL_LENGTH frame_size;	/* the total constraint of dest_encl         */
  OBJECT prec_gap;		/* the gap preceding dest if any else nilobj */
  OBJECT prec_def;		/* the component preceding dest, if any      */
  OBJECT succ_gap;		/* the gap following dest if any else nilobj */
  OBJECT succ_def;		/* the component following dest, if any      */
  OBJECT stop_link;		/* most recently seen gap link of hd         */
  FULL_LENGTH stop_back;        /* back(dest_encl) incl all before stop_link */
  FULL_LENGTH stop_fwd;         /* fwd(dest_encl) incl. all before stop_link */
  FULL_LENGTH stop_perp_back;   /* back(dest_encl) in other direction        */
  FULL_LENGTH stop_perp_fwd;    /* fwd(dest_encl) in other direction         */
  BOOLEAN prnt_flush;		/* TRUE when the parent of hd needs a flush  */
  BOOLEAN target_is_internal;   /* TRUE if flushing into an internal target  */
  BOOLEAN headers_seen;		/* TRUE if a header is seen at all           */
  OBJECT zlink, z, tmp, prnt;  int attach_status;  BOOLEAN remove_target;
  OBJECT why;
  FULL_LENGTH perp_back, perp_fwd;  /* current perp size of dest_encl        */

  debug1(DGF, D, "[ FlushGalley %s (hd)", SymName(actual(hd)));
  prnt_flush = FALSE;
  dim = gall_dir(hd);

  RESUME:
  assert( type(hd) == HEAD, "FlushGalley: type(hd) != HEAD!" );
  debug1(DGF, D, "  resuming FlushGalley %s, hd =", SymName(actual(hd)));
  ifdebugcond(DGF, DD, actual(hd) == nilobj, DebugGalley(hd, nilobj, 4));
  assert( Up(hd) != hd, "FlushGalley: resume found no parent to hd!" );


  /*@@************************************************************************/
  /*                                                                         */
  /*  The first step is to examine the parent of galley hd to determine the  */
  /*  status of the galley.  If this is not suitable for flushing, we do     */
  /*  what we can to change the status.  If still no good, return; so if     */
  /*  this code does not return, then the galley is ready to flush into a    */
  /*  destination in the normal way, and the following variables are set:    */
  /*                                                                         */
  /*     dest_index   the parent of the galley and index of its destination  */
  /*     dest         the destination of the galley, a @Galley object        */
  /*                                                                         */
  /***************************************************************************/

  Parent(dest_index, Up(hd));
  switch( type(dest_index) )
  {

    case DEAD:
    
      /* the galley has been killed off while this process was sleeping */
      debug1(DGF, D, "] FlushGalley %s returning (DEAD)", SymName(actual(hd)));
      return;


    case UNATTACHED:
    
      /* the galley is currently not attached to a destination */
      attach_status = AttachGalley(hd, &inners, &y);
      debug1(DGF, DD, "  ex-AttachGalley inners: %s", DebugInnersNames(inners));
      Parent(dest_index, Up(hd));
      switch( attach_status )
      {

	case ATTACH_KILLED:

	  assert(inners==nilobj, "FlushGalley/ATTACH_KILLED: inners!=nilobj!");
	  debug1(DGF, D, "] FlushGalley %s returning (ATTACH_KILLED)",
	    SymName(actual(hd)));
	  debug1(DGF, D, "    prnt_flush = %s", bool(prnt_flush));
	  return;


	case ATTACH_INPUT:

	  ParentFlush(prnt_flush, dest_index, FALSE);
	  assert(inners==nilobj, "FlushGalley/ATTACH_INPUT: inners!=nilobj!");
	  debug1(DGF, D, "] FlushGalley %s returning (ATTACH_INPUT)",
	    SymName(actual(hd)));
	  return;


	case ATTACH_NOTARGET:

	  ParentFlush(prnt_flush, dest_index, FALSE);
	  assert(inners==nilobj, "FlushGalley/ATTACH_NOTARG: inners!=nilobj!");
	  debug1(DGF, D, "] FlushGalley %s returning (ATTACH_NOTARGET)",
	    SymName(actual(hd)));
	  return;


	case ATTACH_SUSPEND:

	  /* AttachGalley only returns inners here if they really need to */
	  /* be flushed; in particular the galley must be unsized before  */
	  if( inners != nilobj )
	  {
	    debug0(DGF, DD, "  calling FlushInners() from FlushGalley (a)");
	    FlushInners(inners, nilobj);
	    goto RESUME;
	  }
	  stop_link = nilobj;
	  goto SUSPEND;	/* nb y will be set by AttachGalley in this case */


	case ATTACH_NULL:

	  /* hd will have been linked to the unexpanded target in this case */
	  remove_target = (actual(actual(dest_index)) == whereto(hd));
          if( force_gall(hd) )
          {
            /* if hd is a forcing galley, close all predecessors */
	    debug3(DGA, D, "  forcing ATTACH_NULL case for %s into %s (%s)",
	      SymName(actual(hd)), SymName(whereto(hd)),
	      remove_target ? "remove_target" : "not remove_target");
	    Parent(prnt, Up(dest_index));
	    if( !non_blocking(dest_index) && remove_target )
	    {
	      /* ***
	      prnt_flush = TRUE;
	      *** */
	      prnt_flush = non_blocking(dest_index) = TRUE;
	    }
	    FreeGalley(prnt, Up(dest_index), &inners, Up(dest_index),
	      whereto(hd));
          }
          else
	  {
	    debug3(DGA, D, "  non-force ATTACH_NULL case for %s into %s (%s)",
	      SymName(actual(hd)), SymName(whereto(hd)),
	      remove_target ? "remove_target" : "not remove_target");
	    if( blocked(dest_index) && remove_target )  prnt_flush = TRUE;
	  }
	  DetachGalley(hd);
	  KillGalley(hd, TRUE);
          if( inners != nilobj )
	  {
	    debug0(DGF, DD, "  calling FlushInners() from FlushGalley (b)");
	    FlushInners(inners, nilobj);
	  }
	  else ParentFlush(prnt_flush, dest_index, remove_target);
	  debug0(DGF, D, "] FlushGalley returning ATTACH_NULL");
	  return;


	case ATTACH_ACCEPT:

          /* if hd is a forcing galley, or actual(dest_index) is   */
	  /* @ForceGalley, then close all predecessors             */
          if( force_gall(hd) || actual(actual(dest_index)) == ForceGalleySym )
          { Parent(prnt, Up(dest_index));
	    debug1(DGA, D, "  forcing ATTACH_ACCEPT case for %s",
	      SymName(actual(hd)));
	    /* debug0(DGA, DD, "  force: prnt ="); */
	    /* ifdebug(DGA, DD, DebugObject(prnt)); */
	    /* debug1(DGA, D,"  calling FreeGalley from FlushGalley(%s)", */
	    /*   SymName(actual(hd))); */
	    if( !non_blocking(dest_index) )  prnt_flush = TRUE; /* bug fix */
	    FreeGalley(prnt, Up(dest_index), &inners, Up(dest_index),
	      whereto(hd));
	    /* debug0(DGA, DD, "  force: after FreeGalley, prnt ="); */
	    /* ifdebug(DGA, DD, DebugObject(prnt)); */
          }
          else prnt_flush = prnt_flush || blocked(dest_index);
          debug1(DGF, DD, "    force: prnt_flush = %s", bool(prnt_flush));
          if( inners != nilobj )
	  {
	    debug0(DGF, DD, "  calling FlushInners() from FlushGalley (c)");
	    FlushInners(inners, nilobj);
	  }
          goto RESUME;


	default:

	  assert(FALSE, "FlushGalley: attach_status");
	  break;

      }
      break;


    case RECEIVING:
    
      if( actual(actual(dest_index)) == InputSym )
      { ParentFlush(prnt_flush, dest_index, FALSE);
	debug1(DGF, D, "] FlushGalley %s retn, input", SymName(actual(hd)));
	return;
      }
      break;


    default:
    
      assert1(FALSE, "FlushGalley: dest_index", Image(type(dest_index)));
      break;
  }
  dest = actual(dest_index);
  if( underline(dest) == UNDER_UNDEF )  underline(dest) = UNDER_OFF;
  target_is_internal =
    (dim==ROWM && !external_ver(dest)) || (dim==COLM && !external_hor(dest));
  headers_seen = FALSE;
  debug1(DGF, DD, "  dest_index: %s", EchoObject(dest_index));


  /*@@************************************************************************/
  /*                                                                         */
  /*  The second step is to examine the components of the galley one by one  */
  /*  to determine if they can be promoted.  Each component has the format   */
  /*                                                                         */
  /*    { <index> } <object>                                                 */
  /*                                                                         */
  /*  and is always followed by a gap object (except the last component).    */
  /*  An index indicates that the following object has some interesting      */
  /*  feature, and it points to that feature inside the object.  There are   */
  /*  two possible actions for each component, in addition to accepting it:  */
  /*                                                                         */
  /*    REJECT:   The component does not fit, so detach the galley           */
  /*    SUSPEND:  The component is incomplete; go to sleep and wait          */
  /*                                                                         */
  /***************************************************************************/

  stop_link = dest_encl = inners = nilobj;
  need_adjust = FALSE;

  /***************************************************************************/
  /*                                                                         */
  /*  Loop invariant                                                         */
  /*                                                                         */
  /*  The children of hd up to but not including Child(link) have been       */
  /*  examined and pronounced to be promotable, if unbreakable gaps are      */
  /*  ignored.  When unbreakable gaps are taken into account, the most       */
  /*  recent gap where a break is possible is at Child(stop_link), or        */
  /*  nowhere if stop_link == nilobj.                                        */
  /*                                                                         */
  /*  Case 1:  target_is_internal == FALSE                                   */
  /*                                                                         */
  /*  If this flag is FALSE, it means that the target of this galley is      */
  /*  external.  Consequently, there is no need to calculate sizes because   */
  /*  there is no constraint on them.  Also, a REJECT action is impossible   */
  /*  so unbreakable gaps are no impediment.  Variable dest_encl is nilobj.  */
  /*                                                                         */
  /*  Case 2:  target_is_internal == TRUE                                    */
  /*                                                                         */
  /*  If this flag is TRUE, it means that the target of this galley is       */
  /*  internal.  Consequently, sizes need to be calculated, and unbreakable  */
  /*  gaps need to be taken into account.  Variable dest_encl may be not     */
  /*  nilobj, in which case the following variables are defined:             */
  /*                                                                         */
  /*    dest_encl        the object enclosing dest (which must exist)        */
  /*    prec_gap         gap object preceding dest (which must exist)        */
  /*    prec_def         first definite object preceding dest (must exist)   */
  /*    dest_back        back(dest_encl) including effect of accepted compts */
  /*    dest_fwd         fwd(dest_encl) including effect of accepted compts  */
  /*    dest_side        BACK or FWD, i.e. which side of the mark dest is on */
  /*    dest_par_constr  the parallel size constraint on dest                */
  /*    dest_perp_constr the perpendicular size constraint on dest           */
  /*    frame_size       size of frame enclosing dest_encl                   */
  /*    perp_back        back(dest_encl) in other direction, incl accepteds  */
  /*    perp_fwd         fwd(dest_encl) in other direction,  incl accepteds  */
  /*                                                                         */
  /*  if dest_encl is nilobj, these variables are not defined.               */
  /*                                                                         */
  /*  If stop_link is non-nilobj, then in the internal case dest_encl must   */
  /*  be non-nilobj, and the following variables are defined:                */
  /*                                                                         */
  /*    stop_back        back(dest_encl) including all before stop_link      */
  /*    stop_fwd         fwd(dest_encl)  including all before stop_link      */
  /*    stop_perp_back   back(dest_encl) in other direction                  */
  /*    stop_perp_fwd    fwd(dest_encl) in other direction                   */
  /*                                                                         */
  /*  need_adjust is true if at least one definite component has been        */
  /*  accepted for promotion and the destination is internal; hence,         */
  /*  dest_encl is defined and its size needs to be adjusted.                */
  /*                                                                         */
  /*  inners is the set of all PRECEDES and UNATTACHED indexes found.        */
  /*                                                                         */
  /***************************************************************************/

  for( link = Down(hd);  link != hd;  link = NextDown(link) )
  {
    Child(y, link);
    if( type(y) == SPLIT )  Child(y, DownDim(y, dim));
    debug2(DGF, DD, "  examining %s %s", Image(type(y)), EchoObject(y));
    switch( type(y) )
    {

      case GAP_OBJ:

	underline(y) = underline(dest);
	prec_gap = y;
	if( target_is_internal )
	{
	  /* *** not necessarily true
	  assert( dest_encl != nilobj, "FlushGalley/GAP_OBJ: dest_encl!" );
	  *** */
	  if( dest_encl != nilobj && !nobreak(gap(prec_gap)) )
	  {
	    stop_link = link;
	    stop_back = dest_back;
	    stop_fwd  = dest_fwd;
	    stop_perp_back = perp_back;
	    stop_perp_fwd = perp_fwd;
	  }
	}
	else stop_link = link;
	if( !join(gap(y)) )  seen_nojoin(hd) = TRUE;
	break;


      case SCALE_IND:
      case COVER_IND:
      case EXPAND_IND:
      case GALL_PREC:
      case GALL_FOLL:
      case GALL_FOLL_OR_PREC:
      case GALL_TARG:
      case CROSS_PREC:
      case CROSS_FOLL:
      case CROSS_FOLL_OR_PREC:
      case CROSS_TARG:
      case PAGE_LABEL_IND:

	underline(y) = underline(dest);
	break;


      case PRECEDES:
      case UNATTACHED:
	  
	if( inners == nilobj )  New(inners, ACAT);
	Link(inners, y);
	break;


      case RECEIVING:
      case RECEPTIVE:
	  
	goto SUSPEND;


      case FOLLOWS:
	  
	Child(tmp, Down(y));
	if( Up(tmp) == LastUp(tmp) )
	{ link = PrevDown(link);
	  DisposeChild(NextDown(link));
	  break;
	}
	Parent(tmp, Up(tmp));
	assert(type(tmp) == PRECEDES, "Flush: PRECEDES!");
	switch( CheckComponentOrder(tmp, dest_index) )
	{
	  case CLEAR:	DeleteNode(tmp);
			link = PrevDown(link);
			DisposeChild(NextDown(link));
			break;

	  case PROMOTE:	break;

	  case BLOCK:	goto SUSPEND;

	  case CLOSE:	if( opt_components(hd) != nilobj )
			{ DisposeObject(opt_components(hd));
			  opt_components(hd) = nilobj;
			  debug2(DOG, D, "FlushGalley(%s) de-optimizing %s",
			    "(CLOSE problem)", SymName(actual(hd)));
			}
			debug1(DGF, DD, "  reject (a) %s", EchoObject(y));
			goto REJECT;
	}
	break;


      case BEGIN_HEADER:
      case END_HEADER:
      case SET_HEADER:
      case CLEAR_HEADER:

	/* do nothing except take note, until actually promoted out of here */
	headers_seen = TRUE;
	break;


      case NULL_CLOS:
      case PAGE_LABEL:
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
      case CLOSURE:
      case CROSS:
      case FORCE_CROSS:

	underline(y) = underline(dest);
	if( dim == ROWM )
	{
	  /* make sure y is not joined to a target below (vertical case only) */
	  for( zlink = NextDown(link); zlink != hd; zlink = NextDown(zlink) )
	  { Child(z, zlink);
	    switch( type(z) )
	    {
	      case RECEPTIVE:
	      case RECEIVING:	y = z;
				goto SUSPEND;

	      case GAP_OBJ:	if( !join(gap(z)) )  zlink = PrevDown(hd);
				break;

	      default:		break;
	    }
	  }

	  /* try vertical hyphenation before anything else */
	  if( type(y) == HCAT )  VerticalHyphenate(y);

	}

	/* check size constraint */
	if( target_is_internal )
	{
	  /* initialise dest_encl etc if not done yet */
	  if( dest_encl == nilobj )
	  { assert( UpDim(dest,1-dim) == UpDim(dest,dim), "FlushG: UpDims!" );
	    /* *** weird old code, trying for UpDim(dest, ROWM)?
	    Parent(dest_encl, NextDown(Up(dest)));
	    *** */
	    Parent(dest_encl, Up(dest));
	    debug4(DGF, DD, "  flush dest = %s %s, dest_encl = %s %s",
	      Image(type(dest)), EchoObject(dest),
	      Image(type(dest_encl)), EchoObject(dest_encl));
	    assert( (dim==ROWM && type(dest_encl)==VCAT) ||
	            (dim==COLM && type(dest_encl)==ACAT),
	      "FlushGalley: dest != VCAT or ACAT!" );
	    SetNeighbours(Up(dest), FALSE, &prec_gap, &prec_def,
	      &succ_gap, &succ_def, &dest_side);
	    assert(prec_gap != nilobj || is_indefinite(type(y)),
	      "FlushGalley: prec_gap == nilobj && !is_indefinite(type(y))!" );
	    assert(succ_gap == nilobj, "FlushGalley: succ_gap != nilobj!" );
	    assert(dest_side == FWD || is_indefinite(type(y)),
	      "FlushGalley: dest_side != FWD || !is_indefinite(type(y))!");
	    dest_back = back(dest_encl, dim);
	    dest_fwd  = fwd(dest_encl, dim);
	    perp_back = back(dest_encl, 1-dim);
	    perp_fwd  = fwd(dest_encl, 1-dim);
	    Constrained(dest_encl, &dest_par_constr, dim, &why);
	    Constrained(dest_encl, &dest_perp_constr, 1-dim, &why);
	    debug1(DGF, DD, "  setting dest_perp_constr = %s",
	      EchoConstraint(&dest_perp_constr));
	    frame_size = constrained(dest_par_constr) ? bfc(dest_par_constr) :0;
	  }

	  if( !is_indefinite(type(y)) )
	  {
	    ifdebugcond(DGF, DD,  mode(gap(prec_gap)) == NO_MODE,
	      DebugGalley(hd, y, 4));

	    /* calculate parallel effect of adding y to dest */
	    f = dest_fwd  + fwd(y, dim) - fwd(prec_def, dim) +
		  ActualGap(fwd(prec_def, dim), back(y, dim),
			fwd(y, dim), &gap(prec_gap), frame_size,
			dest_back + dest_fwd - fwd(prec_def, dim));
	    debug5(DGF, DD, "  f = %s + %s - %s + %s (prec_gap %s)",
	      EchoLength(dest_fwd), EchoLength(fwd(y, dim)),
	      EchoLength(fwd(prec_def, dim)), EchoLength(
		  ActualGap(fwd(prec_def, dim), back(y, dim),
			fwd(y, dim), &gap(prec_gap), frame_size,
			dest_back + dest_fwd - fwd(prec_def, dim))
	      ), EchoGap(&gap(prec_gap)));
	    debug3(DGF, DD, "  b,f: %s,%s;   dest_encl: %s",
			EchoLength(dest_back), EchoLength(f),
			EchoConstraint(&dest_par_constr));

	    /* check new size against parallel constraint */
	    if( (units(gap(prec_gap))==FRAME_UNIT && width(gap(prec_gap)) > FR)
	        || !FitsConstraint(dest_back, f, dest_par_constr)
		|| (opt_components(hd) != nilobj && opt_comps_permitted(hd)<=0)
	      )
	    {
	      if( opt_components(hd) != nilobj )
	      { OBJECT z;

		/* record the size of this just-completed target area for hd */
		New(z, WIDE);
		CopyConstraint(constraint(z), dest_par_constr);
		Link(opt_constraints(hd), z);
		ifdebug(DOG, D,
		  debug2(DOG, D, "FlushGalley(%s) adding constraint %s",
		    SymName(actual(hd)), EchoConstraint(&constraint(z)));
		  if( units(gap(prec_gap))==FRAME_UNIT &&
		      width(gap(prec_gap)) > FR ) 
		  { debug1(DOG, D, "  prec_gap = %s", EchoGap(&gap(prec_gap)));
		  }
		  if( !FitsConstraint(dest_back, f, dest_par_constr) )
		  { debug3(DOG, D, "  !FitsConstraint(%s, %s, %s)",
		      EchoLength(dest_back), EchoLength(f),
		      EchoConstraint(&dest_par_constr));
		  }
		  if( opt_comps_permitted(hd) <= 0 )
		  { debug1(DOG, D, "  opt_comps_permitted = %2d",
		      opt_comps_permitted(hd));
		  }
		  debug4(DOG, D, "prec_gap = %s;  y = %s (%s,%s):",
		    EchoGap(&gap(prec_gap)), Image(type(y)),
		    EchoLength(back(y, dim)), EchoLength(fwd(y, dim)));
		  DebugObject(y);
		)

		/* refresh the number of components permitted into the next target */
		if( opt_counts(hd) != nilobj && Down(opt_counts(hd)) != opt_counts(hd) )
		{ Child(z, Down(opt_counts(hd)));
		  opt_comps_permitted(hd) += comp_count(z) - 1;
		  DisposeChild(Up(z));
		}
		else opt_comps_permitted(hd) = MAX_FILES;  /* a large number */
		debug1(DOG, D, "  REJECT permitted = %2d", opt_comps_permitted(hd));
	      }
	      debug1(DGF, DD, "  reject (b) %s", EchoObject(y));
	      goto REJECT;
	    }

	    /* calculate perpendicular effect of adding y to dest */
	    if( seen_nojoin(hd) )
	    {
	      pb = 0;
	      pf = find_max(perp_fwd,  size(y, 1-dim));
	    }
	    else
	    {
	      pb = find_max(perp_back, back(y, 1-dim));
	      pf = find_max(perp_fwd,  fwd(y,  1-dim));
	    }

	    /* check new size against perpendicular constraint */
	    if( !FitsConstraint(pb, pf, dest_perp_constr) )
	    {
	      if( opt_components(hd) != nilobj )
	      { DisposeObject(opt_components(hd));
		opt_components(hd) = nilobj;
		debug1(DOG, D, "FlushGalley(%s) de-optimizing (perp problem)",
		  SymName(actual(hd)));
	      }
	      if( dim == ROWM )
	      {
		Error(20, 3, "component too wide for available space",
		  WARN, &fpos(y));
		debug6(DGF, DD, "  %s,%s [%s,%s] too wide for %s, y = %s",
		  EchoLength(pb), EchoLength(pf),
		  EchoLength(back(y, 1-dim)), EchoLength(fwd(y, 1-dim)),
		  EchoConstraint(&dest_perp_constr), EchoObject(y));
	      }
	      debug1(DGF, DD, "  reject (c) %s", EchoObject(y));
	      goto REJECT;
	    }

	    /* accept definite component */
	    dest_fwd = f;  prec_def = y;
	    perp_back = pb;  perp_fwd = pf;
	    need_adjust = TRUE;
	    if( opt_components(hd) != nilobj )
	    { opt_comps_permitted(hd)--;
	      debug1(DOG, D, "  ACCEPT permitted = %2d", opt_comps_permitted(hd));
	    }
	  }
	  /* accept indefinite component */
	} /* end if( target_is_internal ) */

	/* accept this component into dest, subject to following nobreaks */
	debug3(DGF, DD, "  t-accept %s %s %s", Image(type(y)), EchoObject(y),
	  EchoFilePos(&fpos(y)));
	prnt_flush = prnt_flush || blocked(dest_index);
	debug1(DGF, DDD, "    prnt_flush = %s", bool(prnt_flush));
	debug1(DGF, DDD, "    inners = %s", DebugInnersNames(inners));
	if( inners != nilobj )
	{ BOOLEAN promotable;  OBJECT tgp;

	  /* We would prefer to promote right now, then give these inners */
	  /* a chance.  However this is not possible unless the following */
	  /* gap (if any) is breakable                                    */

	  if( type(NextDown(link)) == LINK )
	  { Child(tgp, NextDown(link));
	    assert( type(tgp) == GAP_OBJ, "FlushGalley:  tgp!" );
	    promotable = !nobreak(gap(tgp));
	  }
	  else promotable = TRUE;

	  if( promotable )
	  {
	    debug0(DGS, D, "calling Promote(hd, stop_link) from FlushGalley (ACCEPT)");
	    Promote(hd, NextDown(link), dest_index, TRUE);
	    if( need_adjust )
	    { debug0(DSA, D, "  calling AdjustSize from FlushGalley (ACCEPT)");
	      AdjustSize(dest_encl, dest_back, dest_fwd, dim);
	      AdjustSize(dest_encl, perp_back, perp_fwd, 1-dim);
	    }
	    debug0(DGF, DD, "  calling FlushInners() from FlushGalley (d)");
	    FlushInners(inners, hd);
	    goto RESUME;
	  }
	}
	break;


      default:
	  
	assert1(FALSE, "FlushGalley:", Image(type(y)));
	break;

    } /* end switch */
  } /* end for */


  /* EMPTY: */

    /* galley is now completely accepted; clean up and exit */
    debug0(DGF, DD, "  galley empty now");
    if( inners != nilobj )  DisposeObject(inners);
    if( Down(hd) != hd )
    {
      debug0(DGS, D, "calling Promote(hd, hd) from FlushGalley (EMPTY)");
      Promote(hd, hd, dest_index, TRUE);
      if( need_adjust )
      { debug0(DSA, D, "  calling AdjustSize from FlushGalley (EMPTY)");
	AdjustSize(dest_encl, dest_back, dest_fwd, dim);
	AdjustSize(dest_encl, perp_back, perp_fwd, 1-dim);
      }
    }
    if( opt_components(hd) != nilobj )
    { OBJECT z;
      New(z, WIDE);
      if( dest_encl != nilobj )
        CopyConstraint(constraint(z), dest_par_constr);
      else
        SetConstraint(constraint(z),
	  MAX_FULL_LENGTH, MAX_FULL_LENGTH, MAX_FULL_LENGTH);
      Link(opt_constraints(hd), z);
      debug2(DOG, D, "FlushGalley(%s) empty adding constraint %s",
        SymName(actual(hd)), EchoConstraint(&constraint(z)));
    }
    DetachGalley(hd);
    debug0(DGF, DD, "  calling KillGalley from FlushGalley");
    KillGalley(hd, TRUE);
    ParentFlush(prnt_flush, dest_index, TRUE);
    debug1(DGF,D,"] FlushGalley %s returning (emptied).", SymName(actual(hd)));
    return;


  REJECT:
  
    /* reject this component and move to a new dest; at this point, link is */
    /* the link to the rejected component; its child is either y or else it */
    /* is a SPLIT whose child is y                                          */
    debug3(DGF, D, "at REJECT now (stop_link %s); headers(%s) = %s",
      stop_link != nilobj ? "non-nil" : "nil",
      SymName(actual(hd)), EchoObject(headers(hd)));
    assert(actual(dest) != PrintSym, "FlushGalley: reject print!");
    if( inners != nilobj )  DisposeObject(inners);
    if( stop_link != nilobj )
    {
      debug0(DGS, D, "calling Promote(hd, stop_link) from FlushGalley (REJECT)");
      Promote(hd, stop_link, dest_index, TRUE);
      if( need_adjust )
      { debug0(DSA, D, "  calling AdjustSize from FlushGalley (REJECT)");
	AdjustSize(dest_encl, stop_back, stop_fwd, dim);
	AdjustSize(dest_encl, stop_perp_back, stop_perp_fwd, 1-dim);
      }
    }

    /* if headers_seen, handle any headers not already handled by Promote() */
    if( target_is_internal && headers_seen )
    { OBJECT z, zlink, top_z;
      for( zlink = hd;  NextDown(zlink) != link;  )
      {
	Child(z, NextDown(zlink));
	top_z = z;
	debug2(DGF, D, "FlushGalley(%s)/REJECT header-examining %s",
	  SymName(actual(hd)), EchoObject(z));
	if( type(z) == SPLIT )
	  Child(z, DownDim(z, dim));
	if( is_header(type(z)) )
	{
	  assert(top_z == z, "FlushGalley: header under SPLIT!");
	  HandleHeader(hd, z);
	}
	else
	  zlink = NextDown(zlink);
      }
    }

    /* now, if there are headers, dump them into the galley */
    if( headers(hd) != nilobj )
    { int headers_count;

      /* dump new copy of headers into top of galley */
      assert(Down(headers(hd))!=headers(hd), "FlushGalley/REJECT: headers!");
      tmp = Down(hd);
      assert( tmp != hd, "FlushGalley/REJECT: first_link!" );
      headers_count = 0;
      for( link=Down(headers(hd)); link != headers(hd); link=NextDown(link) )
      { Child(y, link);
        debug2(DGS, D, "FlushGalley(%s)/REJECT linking %s",
	  SymName(actual(hd)), EchoObject(y));
	assert(type(y)!=COL_THR && type(y)!=ROW_THR, "FlushGalley/REJECT THR!");
	Link(tmp, y);
	headers_count++;
      }
      assert(headers_count % 2 == 0, "FlushGalley/REJECT: headers_count!");
    }

    /* now detach and resume */
    DetachGalley(hd);
    assert( type(dest_index) == RECEIVING, "FlushGalley/REJECT: dest_index!" );
    prnt_flush = prnt_flush || blocked(dest_index);
    DeleteNode(dest_index);
    goto RESUME;


  SUSPEND:
  
    /* suspend this component */
    debug1(DGF, D, "  suspend %s", EchoIndex(y));
    if( inners != nilobj )  DisposeObject(inners);
    if( stop_link != nilobj )
    {
      debug0(DGS, D, "calling Promote(hd, stop_link) from FlushGalley/SUSPEND");
      Promote(hd, stop_link, dest_index, TRUE);
      if( need_adjust )
      { debug0(DSA, D, "  calling AdjustSize from FlushGalley (SUSPEND)");
	AdjustSize(dest_encl, stop_back, stop_fwd, dim);
	AdjustSize(dest_encl, stop_perp_back, stop_perp_fwd, 1-dim);
      }
    }

    /* check whether external galleys can remove the blockage */
    if( type(y) == RECEPTIVE && ready_galls(hd) != nilobj && AllowCrossDb )
    { OBJECT eg, val, index2, hd2, tag, seq, newsym;
      BOOLEAN found, gall;  FULL_CHAR newtag[MAX_BUFF], newseq[MAX_BUFF];

      /* get first ready galley in from cross reference database */
      Child(eg, Down(ready_galls(hd)));
      SwitchScope(nilobj);
      val = ReadFromFile(eg_fnum(eg), eg_fpos(eg), eg_lnum(eg));
      UnSwitchScope(nilobj);
      if( val == nilobj )
	Error(20, 1, "error in database file %s",
	  FATAL, &fpos(y), FileName(eg_fnum(eg)));
      assert( type(val) == CLOSURE, "AttachG: db CLOSURE!" );
      New(index2, UNATTACHED);
      pinpoint(index2) = nilobj;
      New(hd2, HEAD);
      FposCopy(fpos(hd2), fpos(val));
      actual(hd2) = actual(val);
      limiter(hd2) = nilobj;
      opt_components(hd2) = opt_constraints(hd2) = nilobj;
      gall_dir(hd2) = horiz_galley(actual(val));
      sized(hd2) = FALSE;
      ready_galls(hd2) = nilobj;
      must_expand(hd2) = TRUE;
      Link(index2, hd2);
      Link(hd2, val);
      SetTarget(hd2);
      foll_or_prec(hd2) = GALL_FOLL;
      enclose_obj(hd2) = (has_enclose(actual(hd2)) ? BuildEnclose(hd2) : nilobj);
      headers(hd2) = dead_headers(hd2) = nilobj;
      Link(Up(y), index2);

      /* set up the next ready galley for reading next time */
      Child(tag, Down(eg));  Child(seq, LastDown(eg));
      do /* skip duplicate seq values */
      {	found = DbRetrieveNext(OldCrossDb, &gall, &newsym, newtag, newseq,
		&eg_fnum(eg), &eg_fpos(eg), &eg_lnum(eg), &eg_cont(eg));
	debug2(DGF, DD, "  ext gall  found:   %15s  gall:    %15s",
			bool(gall), bool(found));
	debug2(DGF, DD, "  ext gall  new sym: %15s  old sym: %15s",
			SymName(newsym), SymName(eg_symbol(eg)));
	debug2(DGF, DD, "  ext gall  new tag: %15s  old tag: %15s",
			newtag, string(tag));
	debug2(DGF, DD, "  ext gall  new seq: %15s  old seq: %15s",
			newseq, string(seq));
	if( found )  found = gall && newsym == eg_symbol(eg) &&
			StringEqual(newtag, string(tag));

	/* merge galleys whose seq strings are equal */
	if( found && StringEqual(newseq, string(seq)) )
	{
	  SwitchScope(nilobj);
	  val = ReadFromFile(eg_fnum(eg), eg_fpos(eg), eg_lnum(eg));
	  UnSwitchScope(nilobj);
	  if( val == nilobj )
	    Error(20, 2, "error in database file %s",
	      FATAL, &fpos(y), FileName(eg_fnum(eg)));
	  assert( type(val) == CLOSURE, "AttachG: db CLOSURE!" );
	  if( !has_merge(actual(val)) )  DisposeObject(val);
	  else /* add val to hd2 */
	  { if( type(hd2) != ACAT )
	    { OBJECT tmp = hd2;
	      New(hd2, ACAT);
	      MoveLink(Up(tmp), hd2, CHILD);
	      Link(hd2, tmp);
	    }
	    Link(hd2, val);
	  }
	}

      } while( found && StringEqual(newseq, string(seq)) );
      if( found )
      {	DisposeChild(Up(tag));
	DisposeChild(Up(seq));
	tag = MakeWord(WORD, newtag, no_fpos);
	seq = MakeWord(WORD, newseq, no_fpos);
	Link(eg, tag);  Link(eg, seq);
	debug1(DGF, DD, "  another ext gall: into %s", SymName(newsym));
      }
      else
      {	DisposeChild(Up(eg));
	debug1(DGF, DD, "  last ext gall into ", SymName(eg_symbol(eg)));
	if( Down(ready_galls(hd)) == ready_galls(hd) )
	{ Dispose(ready_galls(hd));
	  ready_galls(hd) = nilobj;
	  debug0(DGF, DD, "  all ext galls exhausted");
	}
      }

      /* flush the ready galley found above, and resume */
      debug2(DGF, DD, "  ext gall FlushGalley (%s into %s)",
	SymName(actual(hd2)), SymName(whereto(hd2)));
      debug0(DGF, DD, "  calling FlushGalley from FlushGalley/SUSPEND");
      if( type(hd2) == ACAT )
	hd2 = ConvertGalleyList(hd2);
      FlushGalley(hd2);
      goto RESUME;
    }
    else if( type(y) == RECEPTIVE && trigger_externs(y) && AllowCrossDb )
    { OBJECT sym, cr, ins, tag, seq, eg, cnt;  BOOLEAN found;
      FULL_CHAR newseq[MAX_BUFF];  FILE_NUM tfnum;  long tfpos, tcont;
      int tlnum;
      debug1(DGF, DD, "  ext gall target %s", SymName(actual(actual(y))));
      for( sym = FirstExternTarget(actual(actual(y)), &cnt);
	     sym != nilobj;  sym = NextExternTarget(actual(actual(y)), &cnt) )
      {
	debug1(DGF, DD, "  ext gall gall_targ %s", SymName(sym));
	cr = GallTargEval(sym, &fpos(actual(y)));
	New(ins, GALL_TARG);
	actual(ins) = cr;
	Link(Up(y), ins);
	Child(tag, LastDown(cr));
	assert( is_word(type(tag)), "FlushGalley: cr is_word(type(tag))!" );
	found = DbRetrieve(OldCrossDb, TRUE, sym, string(tag),
		newseq, &tfnum, &tfpos, &tlnum, &tcont);
	if( found )
	{ if( ready_galls(hd) == nilobj )  New(ready_galls(hd), ACAT);
	  New(eg, EXT_GALL);
	  debug1(DGF, DD, "  ext gall retrieved: into %s", SymName(sym));
	  eg_fnum(eg) = tfnum;
	  eg_fpos(eg) = tfpos;
	  eg_lnum(eg) = tlnum;
	  eg_symbol(eg) = sym;
	  eg_cont(eg) = tcont;
	  tag = MakeWord(WORD, string(tag), no_fpos);
	  Link(eg, tag);
	  seq = MakeWord(WORD, newseq, no_fpos);
	  Link(eg, seq);
	  Link(ready_galls(hd), eg);
	}
      }
      trigger_externs(y) = FALSE;
      if( ready_galls(hd) != nilobj )  goto RESUME;
    } /* end if external galleys */

    /* if non-blocking, delete the index and resume */
    if( type(y) == RECEPTIVE && non_blocking(y) )
    { DeleteNode(y);
      goto RESUME;
    }
    else if( type(y) == RECEIVING && non_blocking(y) )
    {	
      if( Down(y) == y )
      {	DeleteNode(y);
      }
      else
      {	Child(z, Down(y));
	if( opt_components(z) != nilobj )
	  GazumpOptimize(z, actual(y));
	DetachGalley(z);
      }
      goto RESUME;
    }

    /* if all the above fail to remove the blockage, suspend */
    blocked(y) = TRUE;
    ParentFlush(prnt_flush, dest_index, FALSE);
    debug1(DGF,D, "] FlushGalley %s returning (suspend)", SymName(actual(hd)));
    return;

} /* end FlushGalley */
