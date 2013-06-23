/*@z22.c:Galley Service:Interpose()@******************************************/
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
/*  FILE:         z22.c                                                      */
/*  MODULE:       Galley Service                                             */
/*  EXTERNS:      Interpose(), FlushInners(), ExpandRecursives(),            */
/*                Promote(), KillGalley(), FreeGalley(),                     */
/*                SetTarget(), CheckComponentOrder()                         */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  Interpose(z, typ, x, y)                                                  */
/*                                                                           */
/*  Insert a new typ object above z.  Its sizes are to be taken from x       */
/*  (column) and y (row).                                                    */
/*                                                                           */
/*****************************************************************************/

void Interpose(OBJECT z, int typ, OBJECT x, OBJECT y)
{ OBJECT encl;
  New(encl, typ);
  FposCopy(fpos(encl), fpos(y));
  ReplaceNode(encl, z);  Link(encl, z);
  back(encl, COLM) = back(x, COLM);
  fwd(encl, COLM) = fwd(x, COLM);
  back(encl, ROWM) = back(y, ROWM);
  fwd(encl, ROWM) = fwd(y, ROWM);
  underline(encl) = underline(z);
} /* end Interpose */


/*@::FlushInners()@***********************************************************/
/*                                                                           */
/*  FlushInners(inners, hd)                                                  */
/*                                                                           */
/*  Flush each galley on the list inners.  These have become flushable       */
/*  by being promoted off the top of galley hd; if hd is the root galley,    */
/*  identifiable by having PrintSym as target, do not flush inners at all.   */
/*                                                                           */
/*****************************************************************************/

void FlushInners(OBJECT inners, OBJECT hd)
{ OBJECT y, z, tmp, dest_index;

  ifdebug(DGF, D,
    OBJECT link;
    fprintf(stderr, "dgf: [ FlushInners(");
    for( link = Down(inners);  link != inners;  link = NextDown(link) )
    {
      Child(y, link);
      fprintf(stderr, " %s", Image(type(y)));
      switch( type(y) )
      {

        case DEAD:
      
	  break;


        case RECEIVING:
        case UNATTACHED:
      
	  if( Down(y) != y )	/* bug fix (was assert before) */
	  { assert( Down(y) != y, "FlushInners: UNATTACHED!");
	    Child(z, Down(y));
	    fprintf(stderr, " %s", SymName(actual(z)));
	  }
	  break;


        case PRECEDES:
      
	  break;


        case GALL_PREC:

	  break;


        default:
      
	  break;
      }
    }
    fprintf(stderr, ")\n");
  )

  /* check for root galley case */
  if( hd != nilobj )
  { assert( Up(hd) != hd, "FlushInners: Up(hd)!" );
    Parent(dest_index, Up(hd));
    if( actual(actual(dest_index)) == PrintSym )
    { DisposeObject(inners);
      debug0(DGF, D, "] FlushInners returning (PrintSym)");
      return;
    }
  }

  while( Down(inners) != inners )
  { Child(y, Down(inners));
    DeleteLink(Down(inners));
    switch( type(y) )
    {

      case DEAD:
      
	break;


      case RECEIVING:
      case UNATTACHED:
      
	if( Down(y) != y )	/* bug fix (was assert before) */
	{ assert( Down(y) != y, "FlushInners: UNATTACHED!");
	  Child(z, Down(y));
	  debug1(DGF,D,"  possibly calling FlushGalley %s from FlushInners (a)",
	    SymName(actual(z)));
	  if( whereto(z)==nilobj || !uses_extern_target(whereto(z)) ) /* &&& */
	    FlushGalley(z);
	}
	break;


      case PRECEDES:
      
	Child(tmp, Down(y));
	if( Up(tmp) != LastUp(tmp) )
	{ Parent(tmp, LastUp(tmp));
	  assert(type(tmp)==FOLLOWS, "FlushInners: FOLLOWS!");
	  if( blocked(tmp) )
	  { blocked(tmp) = FALSE;
	    Parent(z, Up(tmp));
	    debug0(DGF, D, "  calling FlushGalley from FlushInners (b)");
	    if( whereto(z)==nilobj || !uses_extern_target(whereto(z)) )/* &&& */
	      FlushGalley(z);
	  }
	}
	break;


      case GALL_PREC:

	/* someone else is looking after this now */
	break;


      default:
      
	assert1(FALSE, "FlushInners:", Image(type(y)));
	break;
    }
  }
  Dispose(inners);
  debug0(DGF, D, "] FlushInners returning");
} /* end FlushInners */


/*@::ExpandRecursives()@******************************************************/
/*                                                                           */
/*  ExpandRecursives(recs)                                                   */
/*                                                                           */
/*  Expand each of the recursive definite objects in the list recs.          */
/*                                                                           */
/*****************************************************************************/

void ExpandRecursives(OBJECT recs)
{ CONSTRAINT non_c, hc, vc;
  OBJECT target_index, target, z, n1, inners, newrecs, hd, tmp, env, why;
  debug0(DCR, DDD, "ExpandRecursives(recs)");
  SetConstraint(non_c, MAX_FULL_LENGTH, MAX_FULL_LENGTH, MAX_FULL_LENGTH);
  n1 = nilobj;
  assert(recs != nilobj, "ExpandRecursives: recs == nilobj!");
  while( Down(recs) != recs )
  { Child(target_index, Down(recs));  DeleteLink( Down(recs) );
    assert( type(target_index) == RECURSIVE, "ExpandRecursives: index!" );
    target = actual(target_index);
    debug2(DCR, DDD, "  expanding %s %s", Image(type(target_index)),
      EchoObject(target));

    /* expand body of target, convert to galley, and check size */
    New(hd, HEAD);  actual(hd) = actual(target);  must_expand(hd) = TRUE;
    force_gall(hd) = FALSE;
    enclose_obj(hd) = limiter(hd) = headers(hd) = dead_headers(hd) = nilobj;
    opt_components(hd) = opt_constraints(hd) = nilobj;
    gall_dir(hd) = horiz_galley(actual(target));
    whereto(hd) = ready_galls(hd) = nilobj;
    foll_or_prec(hd) = GALL_FOLL;
    sized(hd) = FALSE;
    tmp =  CopyObject(target, &fpos(target));  env = DetachEnv(tmp);
    Link(hd, tmp);  Link(target_index, hd);
    SizeGalley(hd, env, external_ver(target),
      gall_dir(hd) == ROWM ? threaded(target) : FALSE, FALSE, FALSE,
      &save_style(target), &non_c, nilobj, &n1, &newrecs, &inners, nilobj);
    debug0(DCR, DDD, "    as galley:");
    ifdebug(DCR, DDD, DebugObject(hd));
    debug1(DGS, DD, "[ ExpandRecursives calling Constrained(%s, COLM)",
      EchoObject(target));
    Constrained(target, &hc, COLM, &why);
    debug2(DGS, DD, "] ExpandRecursives Constrained(%s, COLM) = %s",
      EchoObject(target), EchoConstraint(&hc));
    debug3(DCR, DDD, "    horizontal size: (%s, %s); constraint: %s",
      EchoLength(back(hd, COLM)), EchoLength(fwd(hd, COLM)), EchoConstraint(&hc));
    if( !FitsConstraint(back(hd, COLM), fwd(hd, COLM), hc) )
    { DisposeChild(Up(hd));
      if( inners != nilobj ) DisposeObject(inners);
      if( newrecs != nilobj ) DisposeObject(newrecs);
      DeleteNode(target_index);
      debug0(DCR, DDD, "    rejecting (too wide)");
      continue;
    }
    if( !external_ver(target) )
    { Constrained(target, &vc, ROWM, &why);
      debug2(DSC, DD, "Constrained( %s, ROWM ) = %s",
	EchoObject(target), EchoConstraint(&vc));
      Child(z, LastDown(hd));
      debug3(DCR, DDD, "    vsize: (%s, %s); constraint: %s",
	EchoLength(back(z, ROWM)), EchoLength(fwd(z, ROWM)), EchoConstraint(&vc));
      if( !FitsConstraint(back(z, ROWM), fwd(z, ROWM), vc) )
      {	DisposeChild(Up(hd));
	if( inners != nilobj ) DisposeObject(inners);
	if( newrecs != nilobj ) DisposeObject(newrecs);
	DeleteNode(target_index);
	debug0(DCR, DDD, "    rejecting (too high)");
	continue;
      }
    }

    /* object fits; adjust sizes and promote */
    debug0(DSA, D, "calling AdjustSize from ExpandRecursives (a)");
    AdjustSize(target, back(hd, COLM), fwd(hd, COLM), COLM);
    if( !external_ver(target) )
    { debug0(DSA, D, "calling AdjustSize from ExpandRecursives (b)");
      AdjustSize(target, back(z, ROWM), fwd(z, ROWM), ROWM);
      Interpose(target, VCAT, z, z);
    }
    debug0(DGS, D, "calling Promote(hd, hd) from ExpandRecursives");
    Promote(hd, hd, target_index, TRUE);  DeleteNode(hd);
    DeleteNode(target_index);
    if( inners != nilobj )
    {
      debug0(DGF, D, "  calling FlushInners from ExpandRecursives");
      FlushInners(inners, nilobj);
    }
    if( newrecs != nilobj )  MergeNode(recs, newrecs);
  } /* end while */
  Dispose(recs);
  debug0(DCR, DDD, "ExpandRecursives returning.");
} /* end ExpandRecursives */

/*@::FindSplitInGalley()@*****************************************************/
/*                                                                           */
/*  static OBJECT FindSplitInGalley(hd)                                      */
/*                                                                           */
/*  Search simply joined galley hd for a SPLIT object, which must be there.  */
/*                                                                           */
/*****************************************************************************/

static OBJECT FindSplitInGalley(OBJECT hd)
{ OBJECT link, y;
  debug0(DGF, D, "FindSplitInGalley(hd)");
  for( link = Down(hd);  link != hd;  link = NextDown(link) )
  { Child(y, link);
    if( is_definite(type(y)) )  break;
  }
  if( link == hd )
  { debug0(DGF, D, "FindSplitInGalley failing, no definite component; hd =");
    ifdebug(DGF, D, DebugObject(hd));
    Error(22, 1, "FindSplit: missing galley component", INTERN, &fpos(hd));
  }
  while( type(y) != SPLIT )  switch( type(y) )
  {
    case VCAT:
    case ONE_ROW:
    case WIDE:
    case HIGH:
    case HSHIFT:
    case VSHIFT:
    case VCONTRACT:
    case VLIMITED:
    case VEXPAND:

      Child(y, Down(y));
      break;


    case BEGIN_HEADER:
    case SET_HEADER:

      Child(y, LastDown(y));
      break;


    case CLOSURE:
    case NULL_CLOS:
    case END_HEADER:
    case CLEAR_HEADER:
    case PAGE_LABEL:
    case HCAT:
    case WORD:
    case QWORD:
    case ACAT:
    case ROW_THR:
    case COL_THR:
    case ONE_COL:
    case SCALE:
    case KERN_SHRINK:
    case HSCALE:
    case VSCALE:
    case HCOVER:
    case VCOVER:
    case HCONTRACT:
    case HLIMITED:
    case HEXPAND:
    case START_HVSPAN:
    case START_HSPAN:
    case START_VSPAN:
    case HSPAN:
    case VSPAN:
    case ROTATE:
    case BACKGROUND:
    case INCGRAPHIC:
    case SINCGRAPHIC:
    case PLAIN_GRAPHIC:
    case GRAPHIC:
    case LINK_SOURCE:
    case LINK_DEST:

      debug0(DGF, D, "FindSplitInGalley(hd) failing, hd =");
      ifdebug(DGF, D, DebugObject(hd));
      Error(22, 2, "FindSplitInGalley failed", INTERN, &fpos(y),Image(type(y)));
      break;


    default:
    
      assert1(FALSE, "FindSplitInGalley:", Image(type(y)));
      break;

  }
  debug0(DGF, D, "FindSplitInGalley returning.");
  return y;
} /* end FindSplitInGalley */


/*****************************************************************************/
/*                                                                           */
/*  DisposeHeaders(OBJECT hd)                                                */
/*                                                                           */
/*  Dispose the headers of hd.                                               */
/*                                                                           */
/*****************************************************************************/

static void DisposeHeaders(OBJECT hd)
{ if( headers(hd) != nilobj )
  { assert(type(headers(hd)) == ACAT || type(headers(hd)) == VCAT,
      "DisposeHeaders: type(headers(hd))!");
    while( Down(headers(hd)) != headers(hd) )
    { DisposeChild(Down(headers(hd)));
    }
    headers(hd) = nilobj;
  }
} /* end DisposeHeaders */


/*@::HandleHeader()@**********************************************************/
/*                                                                           */
/*  HandleHeader(hd, link, header)                                           */
/*                                                                           */
/*  Given galley hd containing header object header, this routine handles    */
/*  the header, i.e. it updates headers(hd) to reflect the effect of the     */
/*  header, then deletes the header and its following gap object.            */
/*                                                                           */
/*  Link is the link from hd to the header (it may actually link to a        */
/*  split object which then links to the header, but no matter).             */
/*                                                                           */
/*  Actually, we no longer allow a SPLIT object above a header, because      */
/*  that means there is a COL_THR or ROW_THR above it and when we dispose    */
/*  the header, Dispose is not able to dispose the appropriate child of the  */
/*  COL_THR or ROW_THR.  So Manifest() must not insert a SPLIT above a       */
/*  header, and we check for that.                                           */
/*                                                                           */
/*****************************************************************************/

void HandleHeader(OBJECT hd, OBJECT header)
{ OBJECT g, z, gaplink;
  debug3(DGS, D, "[ HandleHeader(%s, %s); headers = %s; galley:",
    SymName(actual(hd)), EchoObject(header), EchoObject(headers(hd)));
  ifdebug(DGS, D, DebugGalley(hd, nilobj, 2));
  assert(is_header(type(header)), "HandleHeader: type(header)!");
  assert(Up(header) == LastUp(header) && Up(header) != header,
    "HandleHeader: header parents!");
  switch( type(header) )
  {

    case CLEAR_HEADER:

      /* clear out old headers, if any */
      DisposeHeaders(hd);
      break;


    case SET_HEADER:

      /* clear out old headers (not safe to dispose them yet), if any */
      DisposeHeaders(hd);
      /* NB NO BREAK! */


    case BEGIN_HEADER:

      /* make new headers if required */
      if( headers(hd) == nilobj )
	New(headers(hd), gall_dir(hd) == ROWM ? VCAT : ACAT);

      /* construct a gap object from the left parameter */
      New(g, GAP_OBJ);
      underline(g) = FALSE;
      MoveLink(Down(header), g, PARENT);
      Child(z, Down(g));
      GapCopy(gap(g), line_gap(save_style(header)));
      mark(gap(g)) = join(gap(g)) = FALSE;

      /* move header and gap into headers() */
      MoveLink(LastDown(header), headers(hd), PARENT);
      Link(headers(hd), g);
      debug2(DOB, D, "HandleHeader moving %s header %s",
	SymName(actual(hd)), Image(type(header)));
      break;


    case END_HEADER:

      if( headers(hd) == nilobj )
	Error(22, 11, "no header for %s to remove", WARN, &fpos(hd),
	  KW_END_HEADER);
      else
      {
	/* dispose last gap */
	assert(LastDown(headers(hd))!=headers(hd), "Promote/END_HEADER!");
	Child(g, LastDown(headers(hd)));
	assert(type(g) == GAP_OBJ, "HandleHeader: END_HEADER/gap!");
	DisposeChild(LastDown(headers(hd)));

	/* dispose last header object */
	assert(LastDown(headers(hd))!=headers(hd), "Promote/END_HEADER!");
	DisposeChild(LastDown(headers(hd)));
	if( Down(headers(hd)) == headers(hd) )
	{ DisposeObject(headers(hd));
	  headers(hd) = nilobj;
	}
      }
      break;
	
  }

  /* dispose header object and following gap object */
  /* *** old code
  y = MakeWord(WORD, STR_EMPTY, &fpos(header));
  back(y, COLM) = fwd(y, COLM) = back(y, ROWM) = fwd(y, ROWM) = 0;
  ReplaceNode(y, header);
  *** */

  /* dispose header object (must take care to disentangle safely) */
  gaplink = NextDown(Up(header));
  assert(type(gaplink) == LINK, "HandleHeader: type(gaplink)!");
  if( type(header) == CLEAR_HEADER || type(header) == END_HEADER )
  {
    /* first disentangle child properly */
    assert(Down(header) != header && Down(header) == LastDown(header), "HH!");
    DisposeChild(Down(header));
  }
  DisposeChild(Up(header));
  DisposeChild(gaplink);

  debug2(DGS, D, "] HandleHeader returning; headers(%s) now %s; galley:",
    SymName(actual(hd)), EchoObject(headers(hd)));
  ifdebug(DGS, D, DebugGalley(hd, nilobj, 2));
} /* end HandleHeader */


/*@::Promote()@***************************************************************/
/*                                                                           */
/*  Promote(hd, stop_link, dest_index, join_after)                           */
/*                                                                           */
/*  Promote components of galley hd into its destination (dest), up to but   */
/*  not including the one linked to hd by link stop_link, which always       */
/*  follows a component.  No size adjustments are made, except that when     */
/*  two col_thr nodes are merged, a COLM adjustment is made to the result.   */
/*                                                                           */
/*  If the galley is ending here, Promote inserts a gap at the end of it.    */
/*  Whether to make this a joining gap or not is a tricky question which     */
/*  Promote answers by referring to join_after.                              */
/*                                                                           */
/*  Any BEGIN_HEADER, END_HEADER, SET_HEADER, or CLEAR_HEADER objects in     */
/*  the promoted components are diverted to headers(hd), if the target       */
/*  is internal.                                                             */
/*                                                                           */
/*****************************************************************************/

void Promote(OBJECT hd, OBJECT stop_link, OBJECT dest_index, BOOLEAN join_after)
{
  /* these two variables refer to the root galley only */
  static BOOLEAN first = TRUE;		/* TRUE if first component unwritten */
  static OBJECT page_label=nilobj;	/* current page label object         */

  OBJECT dest, link, y, z, tmp1, tmp2, why, top_y;  FULL_CHAR *label_string;
  FULL_LENGTH aback, afwd;
  int dim;
  debug1(DGS, D, "[ Promote(%s, stop_link):", SymName(actual(hd)));
  ifdebug(DGS, D, DebugGalley(hd, stop_link, 2));

  assert( type(hd) == HEAD, "Promote: hd!" );
  assert( type(stop_link) == LINK || stop_link == hd, "Promote: stop_link!" );
  assert( stop_link != Down(hd), "Promote: stop_link == Down(hd)!" );
  type(dest_index) = RECEIVING;
  dest = actual(dest_index);

  /* insert final gap if galley is ending */
  if( stop_link != hd )
  { Child(y, stop_link);
    if( type(y) != GAP_OBJ )
    { ifdebug(DGS, D, DebugGalley(hd, stop_link, 2));
    }
    assert( type(y) == GAP_OBJ, "Promote: missing GAP_OBJ!" );
    stop_link = NextDown(stop_link);
  }
  else
  { New(y, GAP_OBJ);
    FposCopy(fpos(y), fpos(hd));
    hspace(y) = 0;  vspace(y) = 1;
    /* SetGap(gap(y), FALSE, FALSE, seen_nojoin(hd), FIXED_UNIT, NO_MODE, 0); */
    /* SetGap(gap(y), FALSE, FALSE, threaded(dest), FIXED_UNIT, NO_MODE, 0); */
    /* SetGap(gap(y), FALSE, FALSE, TRUE, FIXED_UNIT, NO_MODE, 0); */
    /* ClearGap(gap(y)); */
    SetGap(gap(y), FALSE, FALSE, join_after, FIXED_UNIT, NO_MODE, 0);
    Link(stop_link, y);
  }

  /* if optimizing, add to dummy paragraph containing components and gaps */
  if( opt_components(hd) != nilobj )
  { OBJECT last, tmp;

    debug1(DOG, DD, "Promote(%s) optimizing:", SymName(actual(hd)));
    if( LastDown(opt_components(hd))!=opt_components(hd) && !opt_gazumped(hd) )
    {
      Child(last, LastDown(opt_components(hd)));
    }
    else last = nilobj;
    for( link = Down(hd);  link != stop_link;  link = NextDown(link) )
    { Child(y, link);
      if( type(y) == GAP_OBJ )
      {
	if( last == nilobj )
	{
	  /* do nothing, gap cannot separate definite objects */
	  debug1(DOG, DD, "  skipping initial GAP_OBJ %s", EchoGap(&gap(y)));
	}
	else if( type(last) == GAP_OBJ )
	{
	  /* previous gap must have preceded an indefinite, so overwrite it */
	  FposCopy(fpos(last), fpos(y));
	  debug2(DOG, DD, "  overwriting GAP_OBJ %s with %s",
	    EchoGap(&gap(last)), EchoGap(&gap(y)));
	  GapCopy(gap(last), gap(y));
	  if( Down(last) != last )  DisposeChild(Down(last));
	  if( Down(y) != y )
	  { Child(tmp, Down(y));
	    tmp = CopyObject(tmp, no_fpos);
	    Link(last, tmp);
	  }
	  join(gap(last)) = TRUE;  /* irrelevant but improves debug output */
	}
	else
	{
	  /* previous was definite, so this gap must be stored */
	  opt_gazumped(hd) = FALSE;
	  New(last, GAP_OBJ);
	  FposCopy(fpos(last), fpos(y));
	  GapCopy(gap(last), gap(y));
	  join(gap(last)) = TRUE;  /* irrelevant but improves debug output */
	  hspace(last) = 1;
	  vspace(last) = 0;
	  Link(opt_components(hd), last);
	  debug1(DOG, DD, "  adding GAP_OBJ %s", EchoGap(&gap(last)));
	}
      }
      else if( is_word(type(y)) )
      {
	/* definite, must be stored */
	opt_gazumped(hd) = FALSE;
	last = MakeWord(type(y), string(y), &fpos(y));
	back(last, COLM) = back(y, gall_dir(hd));
	fwd(last, COLM) = fwd(y, gall_dir(hd));
	word_font(last) = word_font(y);
	word_colour(last) = word_colour(y);
	word_outline(last) = word_outline(y);
	word_language(last) = word_language(y);
	word_hyph(last) = word_hyph(y);
	Link(opt_components(hd), last);
	debug2(DOG, DD, "  adding %s \"%s\"", Image(type(last)), string(last));
      }
      else if( is_indefinite(type(y)) )
      {
	/* indefinite, always skip these */
      }
      else if( is_definite(type(y)) )
      {
	/* definite other than WORD, add it */
	opt_gazumped(hd) = FALSE;
	last = MakeWord(QWORD, AsciiToFull("w"), &fpos(y));
	back(last, COLM) = back(y, gall_dir(hd));
	fwd(last, COLM) = fwd(y, gall_dir(hd));
	Link(opt_components(hd), last);
	debug1(DOG, DD, "  adding word for %s", EchoObject(y));
      }
    }
    debug1(DOG, DD, "Promote(%s) end optimizing", SymName(actual(hd)));
  }

  /* error if promoting a seen_nojoin galley into a threaded destination */
  if( seen_nojoin(hd) && gall_dir(hd) == ROWM && threaded(dest) )
    Error(22, 3, "galley %s must have a single column mark",
      FATAL, &fpos(hd), SymName(actual(hd)));

  /* make nojoin status clear by adding an extra gap at start if needed */
  if( gall_dir(hd) == ROWM && !external_ver(dest) && seen_nojoin(hd) &&
      join(gap(y)) )
  { OBJECT prnt, extra_null, extra_gap;

    /* add nojoin gap at start */
    Parent(prnt, Up(dest));  /* can't be threaded */
    assert( type(prnt) == VCAT, "Promote: nojoin case, can't find VCAT" );
    New(extra_null, NULL_CLOS);
    back(extra_null, COLM) = fwd(extra_null, COLM) = 0;
    back(extra_null, ROWM) = fwd(extra_null, ROWM) = 0;
    New(extra_gap, GAP_OBJ);
    hspace(extra_gap) = vspace(extra_gap) = 0;
    SetGap(gap(extra_gap), FALSE, FALSE, FALSE, FIXED_UNIT, EDGE_MODE, 0);
    Link(Down(prnt), extra_gap);
    Link(Down(prnt), extra_null);
    debug0(DGS, DD, "  Promote adding extra nojoin gap");
    /* join(gap(y)) = FALSE; */
  }


  /* if promoting out of root galley, do special things */
  if( actual(dest) == PrintSym )
  { CONSTRAINT c;
    link = hd;
    while( NextDown(link) != stop_link )
    { Child(y, NextDown(link));
      debug2(DGS, DD, "root promote %s %s", Image(type(y)),
	is_definite(type(y)) ? STR_EMPTY : EchoObject(y));
      if( type(y) == SPLIT )  Child(y, DownDim(y, ROWM));
      switch( type(y) )
      {

	case SCALE_IND:
	case COVER_IND:
	case PRECEDES:
      
	  DisposeChild(NextDown(link));
	  break;
	

	case UNATTACHED:
      
	  assert( Down(y) != y, "FlushRootGalley: UNATTACHED!" );
	  Child(z, Down(y));
	  assert( type(z) == HEAD, "FlushRootGalley: unattached HEAD!" );
	  if( sized(z) )
	  {
	    /* galley is part flushed, leave it here */
	    link = NextDown(link);
	  }
	  /* ??? else if( foll_or_prec(z) == GALL_PREC ) */
	  else if( foll_or_prec(z) == GALL_PREC ||
		   foll_or_prec(z) == GALL_FOLL_OR_PREC )
	  {
	    /* galley is preceding or foll_or_prec, send to CrossSequence */
	    OBJECT t;
	    /* type(y) = GALL_PREC; */
	    type(y) = foll_or_prec(z);
	    pinpoint(y) = nilobj;
	    Child(t, Down(z));
	    /* actual(y) = CrossMake(whereto(z), t, GALL_PREC); */
	    actual(y) = CrossMake(whereto(z), t, type(y));
	    DisposeChild(Down(y));
	    CrossSequence(actual(y));
	    DisposeChild(NextDown(link));
	  }
	  else
	  {
	    /* galley was never attached, print message and kill it */
	    Error(22, 4, "galley %s deleted (never attached)",
	      WARN, &fpos(z), SymName(actual(z)));
	    debug1(DGF, D, "never-attached galley %s:", EchoFilePos(&fpos(z)));
	    ifdebug(DGF, D, DebugObject(z));
	    KillGalley(z, FALSE);
	    /* ***
	    link = NextDown(link);
	    *** */
	  }
	  break;


	case EXPAND_IND:
      
	  /* expand @HExpand or @VExpand to occupy everything possible */
	  dim = type(actual(y)) == HEXPAND ? COLM : ROWM;
	  Constrained(actual(y), &c, dim, &why);
	  if( constrained(c) )
	  { FULL_LENGTH b = back(actual(y), dim);
	    FULL_LENGTH f = fwd(actual(y), dim);
	    EnlargeToConstraint(&b, &f, &c);
	    debug1(DSA, D, "Promote %s AdjustSize", Image(type(actual(y))));
	    AdjustSize(actual(y), b, f, dim);
	  }
	  DisposeChild(NextDown(link));
	  break;


	case PAGE_LABEL_IND:

	  if( page_label != nilobj )
	  { DisposeObject(page_label);
	    page_label = nilobj;
	  }
	  Child(z, Down(y));
	  assert( type(z) == PAGE_LABEL, "Promote: type(z) != PAGE_LABEL!" );
	  assert( Down(z) != z, "Promote: PAGE_LABEL Down(z) == z!" );
	  Child(page_label, Down(z));
	  DeleteLink(Up(page_label));
	  DisposeChild(NextDown(link));
	  break;


	case CROSS_PREC:
	case CROSS_FOLL:
	case CROSS_FOLL_OR_PREC:
	case CROSS_TARG:
	      
	  debug2(DGS, DD, "root promote %s %s", Image(type(y)), EchoObject(y));
	  /* NB NO BREAK */


	case GALL_PREC:
	case GALL_FOLL:
	case GALL_FOLL_OR_PREC:
	case GALL_TARG:

	  CrossSequence(actual(y));
	  DisposeChild(NextDown(link));
	  break;


	case BEGIN_HEADER:
	case END_HEADER:
	case SET_HEADER:
	case CLEAR_HEADER:

	  Error(22, 10, "%s symbol ignored (out of place)", WARN, &fpos(y),
	    Image(type(y)));
	  DisposeChild(NextDown(link));
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
	case ROW_THR:

	case CLOSURE:
	case NULL_CLOS:
	case PAGE_LABEL:
	case CROSS:
	case FORCE_CROSS:

	  /* print this component */
	  debug0(DGS, DD, "root promote definite or indefinite");
	  if( !is_indefinite(type(y)) && size(y, ROWM) != 0 )
	  {
	    /* fix horizontally; work out which fonts needed */
	    SetLengthDim(COLM);
	    FixAndPrintObject(y, back(y, COLM), back(y, COLM), fwd(y, COLM),
	      COLM, FALSE, 0, 0, &aback, &afwd);

	    /* print prefatory or page separating material, including fonts */
	    label_string = page_label != nilobj && is_word(type(page_label)) ?
	      string(page_label) : AsciiToFull("?");
	    debug1(DGS, DD, "root promote definite; label_string = %s",
	      label_string);
	    debug1(DCR, DD, "label_string = %s", label_string);
	    if( first )
	    { BackEnd->PrintBeforeFirstPage(size(hd, COLM), size(y, ROWM),
		label_string);
	      first = FALSE;
	    }
	    else
	      BackEnd->PrintBetweenPages(size(hd, COLM), size(y, ROWM),
		label_string);
	    if( page_label != nilobj )
	    { DisposeObject(page_label);
	      page_label = nilobj;
	    }

	    /* fix and print vertically */
	    debug1(DGF,D, "  Promote calling FixAndPrint %s", Image(type(y)));
	    debug3(DGP,D, "  Promote calling FixAndPrint %s %s,%s", dimen(ROWM),
	      EchoLength(back(y,ROWM)), EchoLength(fwd(y, ROWM)));
	    SetLengthDim(ROWM);
	    FixAndPrintObject(y, back(y,ROWM), back(y, ROWM), fwd(y, ROWM),
	      ROWM, FALSE, size(y, ROWM), 0, &aback, &afwd);

	  }
	  DisposeChild(NextDown(link));

	  /* scavenge any filter files now not needed */
	  FilterScavenge(FALSE);
	  break;


	case GAP_OBJ:

	  DisposeChild(NextDown(link));
	  break;


	default:
      
	  assert1(FALSE, "Promote:", Image(type(y)));
	  break;
	
      }
    }
    debug0(DGS, DD, "Promote returning (root galley).");
    return;
  }

  /* prepare the promotion */
  if( external_ver(dest) && gall_dir(hd) == ROWM )
  { if( threaded(dest) )
    { Parent(tmp1, UpDim(dest, COLM));
      assert( type(tmp1) == COL_THR, "Promote: tmp1 not COL_THR!" );
      y = FindSplitInGalley(hd);
      assert( type(y) == SPLIT, "Promote: FindSplitInGalley!" );
      Child(tmp2, DownDim(y, COLM));
      assert( type(tmp2) == COL_THR, "Promote: tmp2 not COL_THR!" );
      if( tmp1 != tmp2 )
      { FULL_LENGTH b = find_max(back(tmp1, COLM), back(tmp2, COLM));
	FULL_LENGTH f = find_max(fwd(tmp1, COLM),  fwd(tmp2, COLM));
	debug0(DSA, D, "calling AdjustSize(tmp1) from Promote (node merging)");
	AdjustSize(tmp1, b, f, COLM);
	debug0(DSA, D, "calling AdjustSize(tmp2) from Promote (node merging)");
	AdjustSize(tmp2, b, f, COLM);
	MergeNode(tmp1, tmp2);
      }
    }
    link = Up(dest_index);
  }
  else if( external_hor(dest) && gall_dir(hd) == COLM )
  { link = Up(dest_index);
  }
  else
  {
    dim = gall_dir(hd);
    for( link = hd;  NextDown(link) != stop_link;  )
    { Child(y, NextDown(link));
      debug1(DGS, DD, "ordinary promote examining %s", EchoObject(y));
      top_y = y;
      if( type(y) == SPLIT )
	Child(y, DownDim(y, dim));
      if( is_header(type(y)) )
      {
	assert(top_y == y, "Promote: header under SPLIT!");
	HandleHeader(hd, y);
      }
      else if( is_index(type(y)) )
	MoveLink(NextDown(link), Up(dest_index), PARENT);
      else link = NextDown(link);
    }
    assert( Down(hd) != stop_link, "Promote: Down(hd) == stop_link!" );
    assert( UpDim(dest, ROWM) == UpDim(dest, COLM), "Promote: dims!" );
    link = Up(dest);
  }
  
  /* promote components */
  TransferLinks(Down(hd), stop_link, link);

  debug0(DGS, D, "] Promote returning; galley:");
  ifdebug(DGS, D, DebugGalley(hd, nilobj, 2));
} /* end Promote */


/*@::MakeDead(), KillGalley()@************************************************/
/*                                                                           */
/*  static MakeDead(y)                                                       */
/*                                                                           */
/*  Convert object y into a DEAD object and remove it to the dead store.     */
/*                                                                           */
/*****************************************************************************/

static void MakeDead(OBJECT y)
{ static int	dead_count = 0;		/* number of DEAD objects seen       */
  static OBJECT	dead_store = nilobj;	/* where DEAD objects are kept       */

  debug1(DGS, DDD, "MakeDead( %s )", Image(type(y)));
  if( dead_store == nilobj )  New(dead_store, ACAT);
  type(y) = DEAD;
  MoveLink(Up(y), dead_store, PARENT);
  if( dead_count >= 150 )
  { DisposeChild(Down(dead_store));
  }
  else dead_count++;
  debug1(DGS, DDD, "MakeDead returning (dead_count = %d).", dead_count);
} /* end MakeDead */


/*****************************************************************************/
/*                                                                           */
/*  KillGalley(hd, optimize)                                                 */
/*                                                                           */
/*  Kill galley hd, which may be sized or unsized.  The index of hd must     */
/*  be UNATTACHED; it is moved out of its present location to a secret spot. */
/*                                                                           */
/*  If hd is to be optimized, generate all the stuff for the cross           */
/*  reference database.  However, don't do this if optimize is FALSE, for    */
/*  in that case hd is defective in some way and not optimizable.            */
/*                                                                           */
/*****************************************************************************/

void KillGalley(OBJECT hd, BOOLEAN optimize)
{ OBJECT prnt, link, y, z;
  debug2(DGF, D, "[ KillGalley(Galley %s into %s)",
	SymName(actual(hd)), SymName(whereto(hd)));
  assert( type(hd) == HEAD && Up(hd) != hd, "KillGalley: precondition!" );
  Parent(prnt, Up(hd));
  assert( type(prnt) == UNATTACHED, "KillGalley: UNATTACHED precondition!" );
  assert( Up(prnt) != prnt, "KillGalley: prnt!" );

  /* delete any ready_galls that might be hanging about */
  if( ready_galls(hd) != nilobj )
  { DisposeObject(ready_galls(hd));
    ready_galls(hd) = nilobj;
  }

  /* delete every remaining component */
  for( link = hd; NextDown(link) != hd; )
  { Child(y, NextDown(link));
    switch( type(y) )
    {
      case RECEIVING:	while( Down(y) != y )
			{ Child(z, Down(y));
			  DetachGalley(z);
			}
			DeleteNode(y);
			break;
		
      case RECEPTIVE:	assert( Down(y) == y, "KillGalley: RECEPTIVE!" );
			DeleteNode(y);
			break;

      case UNATTACHED:	assert( Down(y) != y, "KillGalley: UNATTACHED!" );
			Child(z, Down(y));  KillGalley(z, FALSE);
			break;

      case HEAD:	assert(FALSE, "KillGalley: head");
			break;

      default:		DisposeChild(NextDown(link));
			break;
    }
  }

  /* perform optimization calculations if required */
  if( optimize && opt_components(hd) != nilobj )
    CalculateOptimize(hd);

  /* move index into dead_store */
  MakeDead(prnt);
  debug0(DGF, D, "] KillGalley returning.");
} /* end KillGalley */


/*@::FreeGalley()@************************************************************/
/*                                                                           */
/*  FreeGalley(hd, stop_link, inners, relocate_link, sym)                    */
/*                                                                           */
/*  Free galley hd up to but not including stop_link.  *Inners is well-      */
/*  defined, either nilobj or an ACAT of galleys to be flushed.              */
/*                                                                           */
/*  Relocate_link defines what to do with any galley attached to one of the  */
/*  freed targets.  If it is non-nilobj, galley hd is searched onwards from  */
/*  it to see if a target can be found there.  If so, the galley is          */
/*  relocated to just before that point.  If not, or if relocate_link is     */
/*  nilobj, the galley is freed and added to *inners for flushing.  If the   */
/*  whereto() of such galley is sym, it is freed, not relocated, because the */
/*  cause of this call to FreeGalley is also targeted to sym, and it will    */
/*  consume all possible targets of sym.                                     */
/*                                                                           */
/*****************************************************************************/

void FreeGalley(OBJECT hd, OBJECT stop_link, OBJECT *inners,
OBJECT relocate_link, OBJECT sym)
{ OBJECT link, y, z, zlink, srch, index;
  assert( type(hd) == HEAD && sized(hd), "FreeGalley: pre!");
  assert( Up(hd) != hd, "FreeGalley: Up(hd)!" );
  assert( *inners == nilobj || type(*inners) == ACAT, "FreeGalley: ACAT!" );
  debug3(DGA, D, "[ FreeGalley(Galley %s into %s); rl %s nilobj",
    SymName(actual(hd)), SymName(whereto(hd)),
    relocate_link == nilobj ? "==" : "!=");

  /* close targets and move or flush any inner galleys */
  for( link = Down(hd);  link != stop_link;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == RECEIVING && actual(actual(y)) == InputSym )
      Error(22, 5, "forcing galley after input point", WARN, &fpos(actual(y)));
    else if( type(y) == RECEIVING )
    {
      /* either relocate or free each galley */
      for( zlink = Down(y);  zlink != y; )
      {	Child(z, zlink);
	zlink = NextDown(zlink);
	assert( type(z) == HEAD, "FreeGalley/RECEIVING: type(z) != HEAD!" );
	debug1(DGA, D, "FreeGalley examining galley %s", SymName(actual(z)));
	if( relocate_link != nilobj && whereto(z) != sym &&
	    (srch = SearchGalley(relocate_link, whereto(z), TRUE,
	    FALSE, TRUE, FALSE)) != nilobj )
	{
	  if( opt_components(z) != nilobj )  GazumpOptimize(z, actual(y));
	  debug2(DGA, D, "  FreeGalley relocating %s to just before %s",
	    SymName(actual(z)), SymName(whereto(z)));
	  DetachGalley(z);
	  Parent(index, Up(z));
	  MoveLink(Up(index), Up(srch), PARENT);  /* just before new dest */
	}
	else
	{ debug1(DGA, D, "  FreeGalley freeing galley %s", SymName(actual(z)));
	  FreeGalley(z, z, inners, nilobj, sym);
	  if( *inners == nilobj )  New(*inners, ACAT);
	  Link(*inners, y);
	}
      }
      non_blocking(y) = TRUE;
    }
    else if( type(y) == RECEPTIVE )
    { non_blocking(y) = TRUE;
    }
  }
  debug0(DGA, D, "] FreeGalley returning.");
} /* end FreeGalley */


/*@::SetTarget()@*************************************************************/
/*                                                                           */
/*  SetTarget(hd)                                                            */
/*                                                                           */
/*  Search for the target of unsized galley hd, and set the following:       */
/*                                                                           */
/*     whereto(hd)          The symbol which is this galley's target         */
/*     foll_or_prec(hd)     GALL_FOLL, GALL_PREC, GALL_FOLL_OR_PREC          */
/*     force_gall(hd)       TRUE is this is a forcing galley                 */
/*                                                                           */
/*****************************************************************************/

void SetTarget(OBJECT hd)
{ OBJECT x, y, link, cr, lpar, rpar, env;
  BOOLEAN copied;
  debug1(DGS, DD, "SetTarget(%s)", SymName(actual(hd)));
  assert( type(hd) == HEAD, "SetTarget: type(hd) != HEAD!" );
  Child(x, Down(hd));
  assert( type(x) == CLOSURE, "SetTarget: type(x) != CLOSURE!" );
  assert( has_target(actual(x)), "SetTarget: x has no target!" );

  /* search the parameters of x for @Target */
  cr = nilobj;
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == PAR && is_target(actual(y)) )
    { assert( Down(y) != y, "SetTarget: Down(PAR)!" );
      Child(cr, Down(y));
      break;
    }
  }

  /* search the children of actual(x) for a default value of @Target */
  if( cr == nilobj )
  for( link = Down(actual(x));  link != actual(x);  link = NextDown(link) )
  { Child(y, link);
    if( is_target(y) )
    { cr = sym_body(y);
      break;
    }
  }
  assert(cr != nilobj, "SetTarget:  cr == nilobj!");
  
  /* if cr is not a cross-reference, expand it until it is */
  copied = FALSE;
  if( !is_cross(type(cr)) )
  { OBJECT nbt[2], nft[2], ntarget, ncrs, nenclose;
    nbt[COLM] = nft[COLM] = nbt[ROWM] = nft[ROWM] = nilobj;
    ntarget = ncrs = nenclose = nilobj;
    cr = CopyObject(cr, &fpos(x));
    copied = TRUE;
    env = GetEnv(x);
    cr = Manifest(cr, env, &InitialStyle, nbt, nft, &ntarget, &ncrs,
           FALSE, FALSE, &nenclose, TRUE);
  }

  /* check that cr is now a cross-reference object */
  debug1(DGS, DD, "SetTarget examining %s", EchoObject(cr));
  debug1(DGS, DD, "  type(cr) = %s", Image( (int) type(cr)) );
  if( !is_cross(type(cr)) )
    Error(22, 6, "target of %s is not a cross reference",
      FATAL, &fpos(cr), SymName(actual(x)));

  /* determine which symbol is the target of this galley */
  Child(lpar, Down(cr));
  if( type(lpar) != CLOSURE )
    Error(22, 7, "left parameter of %s is not a symbol",
      FATAL, &fpos(lpar), KW_CROSS);
  whereto(hd) = actual(lpar);

  /* determine the direction from the right parameter */
  Child(rpar, NextDown(Down(cr)));
  if( !is_word(type(rpar)) )
  {
    Error(22, 8, "replacing %s%s? by %s%s%s", WARN, &fpos(rpar),
      SymName(actual(lpar)), KW_CROSS,
      SymName(actual(lpar)), KW_CROSS, KW_FOLLOWING);
    foll_or_prec(hd) = GALL_FOLL;
  }
  else if( StringEqual(string(rpar), KW_PRECEDING) )
     foll_or_prec(hd) = GALL_PREC;
  else if( StringEqual(string(rpar), KW_FOLLOWING) )
     foll_or_prec(hd) = GALL_FOLL;
  else if( StringEqual(string(rpar), KW_FOLL_OR_PREC) )
     foll_or_prec(hd) = GALL_FOLL_OR_PREC;
  else
  {
    Error(22, 9, "replacing %s%s%s by %s%s%s",
      WARN, &fpos(rpar), SymName(actual(lpar)), KW_CROSS,
      string(rpar), SymName(actual(lpar)), KW_CROSS, KW_FOLLOWING);
    foll_or_prec(hd) = GALL_FOLL;
  }

  /* determine whether this is a forcing galley */
  force_gall(hd) = force_target(actual(hd)) || type(cr) == FORCE_CROSS;

  if( copied )  DisposeObject(cr);
} /* end SetTarget */


/*@::CheckComponentOrder()@***************************************************/
/*                                                                           */
/*  int CheckComponentOrder(preceder, follower)                              */
/*                                                                           */
/*  Check the ordering relation between components preceder and follower,    */
/*  and return its current status:                                           */
/*                                                                           */
/*      CLEAR     follower definitely follows preceder, and always will;     */
/*      PROMOTE   follower is not prevented from following preceder;         */
/*      CLOSE     follower must move down its galley to follow preceder;     */
/*      BLOCK     follower cannot be guaranteed to follow preceder.          */
/*                                                                           */
/*****************************************************************************/

int CheckComponentOrder(OBJECT preceder, OBJECT follower)
{ OBJECT prec_galley, foll_galley, z;  int res;
  debug2(DGS, DD, "CheckComponentOrder( %s, %s )",
    EchoObject(preceder), EchoObject(follower));
  Parent(prec_galley, Up(preceder));
  Parent(foll_galley, Up(follower));
  if( prec_galley == foll_galley )
  { res = CLOSE;
    for( z = Up(follower);  z != foll_galley;  z = pred(z, CHILD) )
    if( z == Up(preceder) )
    { res = CLEAR;
      break;
    }
  }
  else
  { res = PROMOTE;
    while( Up(prec_galley) != prec_galley )
    { Parent(z, Up(prec_galley));	/* index of galley */
      Parent(prec_galley, Up(z));	/* enclosing galley */
      if( prec_galley == foll_galley )
      {	res = BLOCK;
	break;
      }
    }
  }
  debug1(DGS, DD, "CheckComponentOrder returning %s", Image(res));
  return res;
} /* end CheckComponentOrder */
