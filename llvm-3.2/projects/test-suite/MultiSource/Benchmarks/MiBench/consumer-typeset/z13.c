/*@z13.c:Object Breaking:BreakJoinedGroup()@**********************************/
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
/*  FILE:         z13.c                                                      */
/*  MODULE:       Object Breaking                                            */
/*  EXTERNS:      BreakObject()                                              */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define	broken(x)	back(x, ROWM)	/* OK since no vertical sizes yet    */

#if DEBUG_ON
static int debug_depth = 1;
static int debug_depth_max = 5;
#endif


/*****************************************************************************/
/*                                                                           */
/*  static BreakJoinedGroup(start, stop, m, c, res_back, res_fwd)            */
/*                                                                           */
/*  Break joined group of components of a VCAT, beginning from Child(start)  */
/*  inclusive and ending at Child(stop) inclusive.  Break component m first  */
/*  because it is the widest.                                                */
/*                                                                           */
/*****************************************************************************/

static void BreakJoinedGroup(OBJECT start, OBJECT stop, OBJECT m,
CONSTRAINT *c, FULL_LENGTH *res_back, FULL_LENGTH *res_fwd)
{ OBJECT y, link;  FULL_LENGTH b, f, sb, sf;  CONSTRAINT yc;
  debug1(DOB, DD, "[ BreakJoinedGroup(start, stop, m, %s, -, -)",
    EchoConstraint(c));

  /* work out a suitable constraint to apply to each component */
  sb = sf = 0;
  for( link = start;  link != NextDown(stop);  link = NextDown(link) )
  { Child(y, link);
    if( !is_definite(type(y)) )  continue;
    sb = find_max(sb, back(y, COLM));
    sf = find_max(sf, fwd(y, COLM));
  }
  if( sb <= bc(*c) )
  {
    /* make sure the constraint will accept objects with size (sb, 0) */
    b = sb;
    f = 0;
  }
  else
  {
    /* sb is too wide anyway, so don't worry about it */
    b = 0;
    f = 0;
  }
  SetConstraint(yc, find_min(bc(*c), bfc(*c)-f), bfc(*c), find_min(fc(*c), bfc(*c)-b));

  /* apply this constraint to each component in turn, m first */
  if( m != nilobj )
  {
    debug1(DOB, DD, "  +++BreakJoinedGroup calling first child, yc = %s",
      EchoConstraint(&yc));
    m = BreakObject(m, &yc);
    b = back(m, COLM);
    f = fwd(m, COLM);
    SetConstraint(yc, find_min(bc(yc), bfc(yc)-f), bfc(yc), find_min(fc(yc), bfc(yc)-b));
  }
  else b = f = 0;
  for( link = start;  link != NextDown(stop);  link = NextDown(link) )
  { Child(y, link);
    if( !is_definite(type(y)) || y == m )  continue;
    debug1(DOB, DD, "  +++BreakJoinedGroup calling child, yc = %s",
      EchoConstraint(&yc));
    y = BreakObject(y, &yc);
    b = find_max(b, back(y, COLM));
    f = find_max(f, fwd(y, COLM));
    SetConstraint(yc, find_min(bc(yc), bfc(yc)-f), bfc(yc), find_min(fc(yc), bfc(yc)-b));
  }
  if( !FitsConstraint(b, f, *c) )
  { debug3(DOB, DD, "  in BreakJoinedGroup: !FitsConstraint(%s, %s, %s)",
      EchoLength(b), EchoLength(f), EchoConstraint(c));
    Error(13, 1, "failed to break column to fit into its available space",
      WARN, m != nilobj ? &fpos(m) : (y != nilobj ? &fpos(y) : no_fpos));
  }
  *res_back = b;  *res_fwd = f;
  debug2(DOB, DD,"] BreakJoinedGroup returning (%s, %s)",
	EchoLength(b), EchoLength(f));
} /* end BreakJoinedGroup */


/*@::BreakVcat()@*************************************************************/
/*                                                                           */
/*  static OBJECT BreakVcat(x, c)                                            */
/*                                                                           */
/*  Break a VCAT to satisfy constraint c.  This is tedious because every     */
/*  group of components between //  ...  // must be broken separately.       */
/*                                                                           */
/*****************************************************************************/

static OBJECT BreakVcat(OBJECT x, CONSTRAINT *c)
{ OBJECT y, link, start_group, m;  FULL_LENGTH b, f, dble_fwd;  CONSTRAINT tc;
  BOOLEAN dble_found;
  debug1(DOB, DD, "[ BreakVcat(x, %s)", EchoConstraint(c));
  assert(Down(x) != x, "BreakVcat: Down(x) == x!" );
  SetConstraint(tc, MAX_FULL_LENGTH, find_min(bfc(*c), fc(*c)), MAX_FULL_LENGTH);
  
  dble_found = FALSE;  dble_fwd = 0;  start_group = nilobj;
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( is_index(type(y)) )  continue;
    if( type(y) == GAP_OBJ )
    { assert( start_group != nilobj, "BreakVcat: start_group == nilobj!" );
      if( !join(gap(y)) )
      {
	/* finish off and break this group */
	if( !FitsConstraint(b, f, tc) )
	  BreakJoinedGroup(start_group, link, m, &tc, &b, &f);
	dble_found = TRUE;
	dble_fwd = find_max(dble_fwd, b + f);
	start_group = nilobj;
	debug1(DOB, DD, "  end group, dble_fwd: %s", EchoLength(dble_fwd));
      }
    }
    else if( start_group == nilobj )
    {	
      /* start new group */
      b = back(y, COLM);  f = fwd(y, COLM);
      start_group = link;  m = y;
      debug2(DOB, DD, "  starting group (b = %s, f = %s):",
	EchoLength(b), EchoLength(f));
      ifdebug(DOB, DD, DebugObject(y));
    }
    else
    {
      /* continue with current group */
      b = find_max(b, back(y, COLM));  f = find_max(f, fwd(y, COLM));
      if( fwd(y, COLM) > fwd(m, COLM) )  m = y;
      debug3(DOB, DD, "  in group%s (b = %s, f = %s):",
	m == y ? " (new max)" : "",
	EchoLength(b), EchoLength(f));
      ifdebug(DOB, DD, DebugObject(y));
    }
  }
  assert( start_group != nilobj, "BreakVcat: start_group == nilobj (2)!" );

  if( dble_found )
  {	
    /* finish off and break this last group, and set sizes of x */
    if( !FitsConstraint(b, f, tc) )
      BreakJoinedGroup(start_group, LastDown(x), m, &tc, &b, &f);
    dble_fwd = find_max(dble_fwd, b + f);
    debug1(DOB, DD, "  ending last group, dble_fwd: %s",EchoLength(dble_fwd));
    back(x, COLM) = 0;  fwd(x, COLM) = find_min(MAX_FULL_LENGTH, dble_fwd);
  }
  else
  {
    /* finish off and break this last and only group, and set sizes of x */
    debug2(DOB, DD, "  BreakVcat ending last and only group (%s, %s)",
	EchoLength(b), EchoLength(f));
    BreakJoinedGroup(start_group, LastDown(x), m, c, &b, &f);
    back(x, COLM) = b;  fwd(x, COLM) = f;
  }

  debug0(DOB, DD, "] BreakVcat returning x:");
  ifdebug(DOB, DD, DebugObject(x));
  debug2(DOB, DD, "  (size is %s, %s)",
	EchoLength(back(x, COLM)), EchoLength(fwd(x, COLM)));
  return x;
} /* end BreakVcat */


/*@::BreakTable()@************************************************************/
/*                                                                           */
/*  static OBJECT BreakTable(x, c)                                           */
/*                                                                           */
/*  Break table (HCAT) x to satisfy constraint c.                            */
/*                                                                           */
/*  Outline of algorithm:                                                    */
/*                                                                           */
/*     bcount = number of components to left of mark;                        */
/*     fcount = no. of components on and right of mark;                      */
/*     bwidth = what back(x) would be if all components had size (0, 0);     */
/*     fwidth = what fwd(x) would be if all components had size (0, 0);      */
/*     Set all components of x to Unbroken (broken(y) holds this flag);      */
/*     while( an Unbroken component of x exists )                            */
/*     {   my = the Unbroken component of x of minimum width;                */
/*         mc = desirable constraint for my (see below);                     */
/*         BreakObject(my, &mc);                                             */
/*         Set my to Broken and update bcount, fcount, bwidth, fwidth        */
/*            to reflect the actual size of my, now broken;                  */
/*     }                                                                     */
/*                                                                           */
/*  The constraint mc is chosen in an attempt to ensure that:                */
/*                                                                           */
/*     a)  Any sufficiently narrow components will not break;                */
/*     b)  All broken components will have the same bfc(mc), if possible;    */
/*     c)  All available space is used.                                      */
/*                                                                           */
/*****************************************************************************/

static OBJECT BreakTable(OBJECT x, CONSTRAINT *c)
{ FULL_LENGTH bwidth, fwidth;	/* running back(x) and fwd(x)		     */
  int    bcount, fcount;	/* running no. of components		     */
  OBJECT mlink, my;		/* minimum-width unbroken component	     */
  BOOLEAN ratm;			/* TRUE when my has a mark to its right      */
  int    mside;			/* side of the mark my is on: BACK, ON, FWD  */
  FULL_LENGTH msize;		/* size of my (minimal among unbroken)	     */
  CONSTRAINT mc;		/* desirable constraint for my		     */
  OBJECT pg, prec_def;		/* preceding definite object of my           */
  OBJECT sg, succ_def;		/* succeeding definite object of my          */
  FULL_LENGTH pd_extra,sd_extra;/* space availiable for free each side of my */
  FULL_LENGTH av_colsize;	/* the size of each unbroken component       */
				/* if they are all assigned equal width      */
  FULL_LENGTH fwd_max, back_max;/* maximum space available forward of or     */
				/* back of the mark, when columns are even   */
  FULL_LENGTH col_size;		/* the column size actually used in breaking */
  FULL_LENGTH prev_col_size;	/* previous column size (try to keep equal)  */
  FULL_LENGTH beffect, feffect;	/* the amount bwidth, fwidth must increase   */
				/* when my is broken			     */
  OBJECT link, y, prev, g;  FULL_LENGTH tmp, tmp2;

  debug1(DOB, DD, "[ BreakTable( x, %s )", EchoConstraint(c));

  /* Initialise csize, bcount, fcount, bwidth, fwidth and broken(y) */
  bcount = fcount = 0;  bwidth = fwidth = 0;  prev = nilobj;
  prev_col_size = 0;
  Child(y, Down(x));
  assert( type(y) != GAP_OBJ, "BreakTable: GAP_OBJ!" );
  assert( !is_index(type(y)), "BreakTable: index!" );
  broken(y) = is_indefinite(type(y));
  if( !broken(y) )  prev = y, fcount = 1;

  for( link = NextDown(Down(x));  link != x;  link = NextDown(NextDown(link)) )
  {
    /* find the next gap g and following child y */
    Child(g, link);
    assert( type(g) == GAP_OBJ, "BreakTable: GAP_OBJ!" );
    assert( NextDown(link) != x, "BreakTable: GAP_OBJ is last!" );
    Child(y, NextDown(link));

    assert( type(y) != GAP_OBJ, "BreakTable: GAP_OBJ!" );
    assert( !is_index(type(y)), "BreakTable: index!" );
    broken(y) = is_indefinite(type(y));
    if( !broken(y) )
    { if( prev == nilobj )  fcount = 1;
      else if( mark(gap(g)) )
      {	bcount += fcount;
	bwidth += fwidth + MinGap(0, 0, 0, &gap(g));
	fcount  = 1;  fwidth = 0;
      }
      else
      {	fwidth += MinGap(0, 0, 0, &gap(g));
	fcount += 1;
      }
      prev = y;
    }
  }

  /* if column gaps alone are too wide, kill them all */
  if( !FitsConstraint(bwidth, fwidth, *c) )
  {
    debug2(DOB, DD, "column gaps alone too wide: bwidth: %s; fwidth: %s",
       EchoLength(bwidth), EchoLength(fwidth));
    Error(13, 2, "reducing column gaps to 0i (object is too wide)",
      WARN, &fpos(x));
    for( link = Down(x);  link != x;  link = NextDown(link) )
    { Child(g, link);
      if( type(g) == GAP_OBJ )
      {	SetGap(gap(g), nobreak(gap(g)), mark(gap(g)), join(gap(g)),
	  FIXED_UNIT, EDGE_MODE, 0);
      }
    }
    bwidth = fwidth = 0;
  }

  /* break each column, from smallest to largest */
  while( bcount + fcount > 0 && FitsConstraint(bwidth, fwidth, *c) )
  {
    debug2(DOB, DD, "bcount: %d;  bwidth: %s", bcount, EchoLength(bwidth));
    debug2(DOB, DD, "fcount: %d;  fwidth: %s", fcount, EchoLength(fwidth));

    /* find a minimal-width unbroken component my */
    my = nilobj;  msize = size(x, COLM);       /* an upper bound for size(y) */
    for( link = Down(x);  ;  link = NextDown(link) )
    { Child(y, link);
      assert( type(y) != GAP_OBJ, "BreakTable: type(y) == GAP_OBJ!" );
      if( !broken(y) && (size(y, COLM) < msize || my == nilobj) )
      {	msize = size(y, COLM);
	my = y;  mlink = link;
	ratm = FALSE;
      }

      /* next gap */
      link = NextDown(link);
      if( link == x )  break;
      Child(g, link);
      assert( type(g) == GAP_OBJ, "BreakTable: type(g) != GAP_OBJ!" );
      if( mark(gap(g)) )  ratm = TRUE;
    }

    /* find neighbouring definite objects and resulting pd_extra and sd_extra */
    SetNeighbours(mlink, ratm, &pg, &prec_def, &sg, &succ_def, &mside);
    debug2(DOB, DD, "my (%s): %s", Image(mside), EchoObject(my));
    pd_extra = pg == nilobj ? 0 :
      ExtraGap(broken(prec_def) ? fwd(prec_def,COLM) : 0, 0, &gap(pg), BACK);
    sd_extra = sg == nilobj ? 0 :
      ExtraGap(0, broken(succ_def) ? back(succ_def,COLM) : 0, &gap(sg), FWD);
    debug2(DOB, DD, "pd_extra:   %s;  sd_extra:      %s",
		EchoLength(pd_extra), EchoLength(sd_extra) );

    /* calculate desirable constraints for my */
    av_colsize = (bfc(*c) - bwidth - fwidth) / (bcount + fcount);
    debug1(DOB, DD, "av_colsize = %s", EchoLength(av_colsize));
    debug1(DOB, DD, "prev_col_size = %s", EchoLength(prev_col_size));
    switch( mside )
    {

      case BACK:
      
	back_max = find_min(bc(*c), bwidth + av_colsize * bcount);
	col_size = (back_max - bwidth) / bcount;
	if( col_size > prev_col_size && col_size - prev_col_size < PT )
	  col_size = prev_col_size;
	SetConstraint(mc,
	  find_min(MAX_FULL_LENGTH, col_size + pd_extra),
	  find_min(MAX_FULL_LENGTH, col_size + pd_extra + sd_extra),
	  find_min(MAX_FULL_LENGTH, col_size + sd_extra));
	break;


      case ON:
      
	fwd_max = find_min(fc(*c), fwidth + av_colsize * fcount);
	col_size = (fwd_max - fwidth) / fcount;
	if( col_size > prev_col_size && col_size - prev_col_size < PT )
	  col_size = prev_col_size;
	SetConstraint(mc,
	  find_min(MAX_FULL_LENGTH, pd_extra + back(my, COLM)),
	  find_min(MAX_FULL_LENGTH, pd_extra + back(my, COLM) + col_size + sd_extra),
	  find_min(MAX_FULL_LENGTH, col_size + sd_extra));
	break;


      case FWD:
      
	fwd_max = find_min(fc(*c), fwidth + av_colsize * fcount);
	col_size = (fwd_max - fwidth) / fcount;
	if( col_size > prev_col_size && col_size - prev_col_size < PT )
	  col_size = prev_col_size;
	SetConstraint(mc,
	  find_min(MAX_FULL_LENGTH, col_size + pd_extra),
	  find_min(MAX_FULL_LENGTH, col_size + pd_extra + sd_extra),
	  find_min(MAX_FULL_LENGTH, col_size + sd_extra));
	break;


      default:
      
	assert(FALSE, "BreakTable: mside");
	break;
    }
    debug1(DOB, DD, "col_size = %s", EchoLength(col_size));
    prev_col_size = col_size;

    /* now break my according to these constraints, and accept it */
    debug2(DOB, DD, "  calling BreakObject(%s, %s)", EchoObject(my),
      EchoConstraint(&mc));
    my = BreakObject(my, &mc);  broken(my) = TRUE;

    /* calculate the effect of accepting my on bwidth and fwidth */
    if( pg != nilobj )
    { tmp = broken(prec_def) ? fwd(prec_def, COLM) : 0;
      beffect = MinGap(tmp, back(my, COLM), fwd(my, COLM), &gap(pg)) -
	        MinGap(tmp, 0,             0,            &gap(pg));
    }
    else beffect = back(my, COLM);

    if( sg != nilobj )
    { tmp = broken(succ_def) ? back(succ_def, COLM) : 0;
      tmp2 = broken(succ_def) ? fwd(succ_def, COLM) : 0;
      feffect = MinGap(fwd(my, COLM), tmp, tmp2, &gap(sg)) -
	        MinGap(0,            tmp, tmp2, &gap(sg));
    }
    else feffect = fwd(my, COLM);

    switch( mside )
    {
	case BACK:	bwidth += beffect + feffect;
			bcount--;
			break;
	
	case ON:	bwidth += beffect;  fwidth += feffect;
			fcount--;
			break;

	case FWD:	fwidth += beffect + feffect;
			fcount--;
			break;
	
	default:	assert(FALSE, "BreakTable: mside");
			break;
    }

  } /* end while */

  back(x, COLM) = bwidth;
  fwd(x, COLM) = fwidth;

  debug2(DOB, DD,  "] BreakTable returning %s,%s; x =",
    EchoLength(bwidth), EchoLength(fwidth));
  ifdebug(DOB, DD, DebugObject(x));
  return x;
} /* end BreakTable */


/*@::BreakObject()@***********************************************************/
/*                                                                           */
/*  OBJECT BreakObject(x, c)                                                 */
/*                                                                           */
/*  Break lines of object x so that it satisfies constraint c.               */
/*                                                                           */
/*****************************************************************************/

OBJECT BreakObject(OBJECT x, CONSTRAINT *c)
{ OBJECT link, y;  CONSTRAINT yc;  FULL_LENGTH f;  BOOLEAN junk;
  debugcond4(DOB, D, debug_depth++ < debug_depth_max,
    "%*s[ BreakObject(%s %d)", (debug_depth-1)*2, " ", Image(type(x)), (int) x);
  debug4(DOB, DD,  "[ BreakObject(%s (%s,%s),  %s), x =",
    Image(type(x)), EchoLength(back(x, COLM)), EchoLength(fwd(x, COLM)),
    EchoConstraint(c));
  ifdebug(DOB, DD, DebugObject(x));

  /* if constraint is negative (should really be never), replace with empty */
  if( !(bc(*c)>=0 && bfc(*c)>=0 && fc(*c)>=0) )
  {
    Error(13, 11, "replacing with empty object: negative size constraint %s,%s,%s",
      WARN, &fpos(x), EchoLength(bc(*c)), EchoLength(bfc(*c)), EchoLength(fc(*c)));
    y = MakeWord(WORD, STR_EMPTY, &fpos(x));
    back(y, COLM) = fwd(y, COLM) = 0;
    ReplaceNode(y, x);
    DisposeObject(x);
    x = y;
    debugcond6(DOB, D, --debug_depth < debug_depth_max,
      "%*s] BreakObject(%s %d) (neg!) = (%s, %s)", debug_depth*2, " ",
      Image(type(x)), (int) x, EchoLength(back(x, COLM)),
      EchoLength(fwd(x, COLM)));
    debug0(DOB, DD, "] BreakObject returning (negative constraint).");
    return x;
  }

  /* if no breaking required, return immediately */
  if( FitsConstraint(back(x, COLM), fwd(x, COLM), *c) )
  { debug0(DOB, DD, "] BreakObject returning (fits).");
    debugcond6(DOB, D, --debug_depth < debug_depth_max,
      "%*s] BreakObject(%s %d) (fits) = (%s, %s)", debug_depth*2, " ",
      Image(type(x)), (int) x, EchoLength(back(x, COLM)),
      EchoLength(fwd(x, COLM)));
    return x;
  }

  switch( type(x) )
  {

    case ROTATE:
    
      if( BackEnd->scale_avail && InsertScale(x, c) )
      {
	Parent(x, Up(x));
	Error(13, 3, "%s object scaled horizontally by factor %.2f (too wide)",
	  WARN, &fpos(x), KW_ROTATE, (float) bc(constraint(x)) / SF );
      }
      else
      { Error(13, 4, "%s deleted (too wide; cannot break %s)",
	  WARN, &fpos(x), KW_ROTATE, KW_ROTATE);
        y = MakeWord(WORD, STR_EMPTY, &fpos(x));
        back(y, COLM) = fwd(y, COLM) = 0;
        ReplaceNode(y, x);
        DisposeObject(x);
        x = y;
      }
      break;


    case SCALE:

      InvScaleConstraint(&yc, bc(constraint(x)), c);
      Child(y, Down(x));
      y = BreakObject(y, &yc);
      back(x, COLM) = (back(y, COLM) * bc(constraint(x))) / SF;
      fwd(x, COLM) =  (fwd(y, COLM)  * bc(constraint(x))) / SF;
      break;


    case KERN_SHRINK:

      /* not really accurate, but there you go */
      Child(y, LastDown(x));
      y = BreakObject(y, c);
      back(x, COLM) = back(y, COLM);
      fwd(x, COLM) = fwd(y, COLM);
      break;


    case WORD:
    case QWORD:
    
      if( word_hyph(x) )
      {
	/* create an ACAT with the same size as x */
	New(y, ACAT);
	FposCopy(fpos(y), fpos(x));
	back(y, COLM) = back(x, COLM);
	fwd(y, COLM) = fwd(x, COLM);
	back(y, ROWM) = back(x, ROWM);
	fwd(y, ROWM) = fwd(x, ROWM);

	/* set ACAT's save_style; have to invent a line_gap, unfortunately */
	SetGap(line_gap(save_style(y)), FALSE, FALSE, FALSE, FIXED_UNIT,
	  MARK_MODE, 1.1 * FontSize(word_font(x), x));
	SetGap(space_gap(save_style(y)), FALSE, FALSE, TRUE, FIXED_UNIT,
	  EDGE_MODE, 0);
	hyph_style(save_style(y)) = HYPH_ON;
	fill_style(save_style(y)) = FILL_ON;
	display_style(save_style(y)) = DISPLAY_LEFT;
	small_caps(save_style(y)) = FALSE;
	font(save_style(y)) = word_font(x);
	colour(save_style(y)) = word_colour(x);
	outline(save_style(y)) = word_outline(x);
	language(save_style(y)) = word_language(x);
	debug3(DOF, DD, "  in BreakObject y %s %s %s",
	  EchoStyle(&save_style(y)), Image(type(y)), EchoObject(y));

	/* enclose x in the ACAT and try breaking (i.e. filling) it */
	ReplaceNode(y, x);
	Link(y, x);
	x = y;
	debug3(DOF, DD, "  in BreakObject x %s %s %s",
	  EchoStyle(&save_style(x)), Image(type(x)), EchoObject(x));
	x = BreakObject(x, c);
      }
      else if( BackEnd->scale_avail && InsertScale(x, c) )
      { OBJECT tmp;
	tmp = x;
	Parent(x, Up(x));
	Error(13, 5, "word %s scaled horizontally by factor %.2f (too wide)",
	  WARN, &fpos(x), string(tmp), (float) bc(constraint(x)) / SF);
      }
      else
      { Error(13, 6, "word %s deleted (too wide)", WARN, &fpos(x), string(x));
        y = MakeWord(WORD, STR_EMPTY, &fpos(x));
        back(y, COLM) = fwd(y, COLM) = 0;
        ReplaceNode(y, x);
        DisposeObject(x);
        x = y;
      }
      break;


    case WIDE:
    
      MinConstraint(&constraint(x), c);
      Child(y, Down(x));
      y = BreakObject(y, &constraint(x));
      back(x, COLM) = back(y, COLM);
      fwd(x, COLM) = fwd(y, COLM);
      EnlargeToConstraint(&back(x, COLM), &fwd(x, COLM), &constraint(x));
      break;


    case INCGRAPHIC:
    case SINCGRAPHIC:

      if( BackEnd->scale_avail && InsertScale(x, c) )
      {
	Parent(x, Up(x));
	Error(13, 7, "%s scaled horizontally by factor %.2f (too wide)",
	  WARN, &fpos(x),
	  type(x) == INCGRAPHIC ? KW_INCGRAPHIC : KW_SINCGRAPHIC,
	  (float) bc(constraint(x)) / SF);
      }
      else
      { Error(13, 8, "%s deleted (too wide)", WARN, &fpos(x),
	  type(x) == INCGRAPHIC ? KW_INCGRAPHIC : KW_SINCGRAPHIC);
        y = MakeWord(WORD, STR_EMPTY, &fpos(x));
        back(y, COLM) = fwd(y, COLM) = 0;
        ReplaceNode(y, x);
        DisposeObject(x);
        x = y;
      }
      break;


    case HIGH:
    case VSCALE:
    case VCOVER:
    case VSHIFT:
    case HCONTRACT: 
    case VCONTRACT:
    case HLIMITED: 
    case VLIMITED:
    case HEXPAND: 
    case VEXPAND:
    case ONE_COL:
    case ONE_ROW:
    case HSPANNER:
    
      assert( Down(x) == LastDown(x), "BreakObject: downs!" );
      Child(y, Down(x));
      y = BreakObject(y, c);
      back(x, COLM) = back(y, COLM);
      fwd(x, COLM) = fwd(y, COLM);
      break;


    case BACKGROUND:

      Child(y, Down(x));
      y = BreakObject(y, c);
      Child(y, LastDown(x));
      y = BreakObject(y, c);
      back(x, COLM) = back(y, COLM);
      fwd(x, COLM) = fwd(y, COLM);
      break;


    case START_HVSPAN:
    case START_HSPAN:
    case START_VSPAN:
    case HSPAN:
    case VSPAN:

      /* these all have size zero except the last one, so if we get to  */
      /* this point we must be at the last column and need to break it. */
      /* this is done just by setting its size to zero, unless it is    */
      /* the last column in which case it claims everything that is     */
      /* going; the real break is deferred to the first ROWM touch,     */
      /* when we know that all contributing columns have been broken    */
      /* unless the child is not a spanner, in which case it's @OneCol  */
      Child(y, Down(x));
      if( type(y) != HSPANNER )
      {
        y = BreakObject(y, c);
        back(x, COLM) = back(y, COLM);
        fwd(x, COLM) = fwd(y, COLM);
      }
      else
      {
        back(x, COLM) = 0;
        fwd(x, COLM) = find_min(bfc(*c), fc(*c));
      }
      break;


    case HSHIFT:

      Child(y, Down(x));
      f = FindShift(x, y, COLM);
      SetConstraint(yc,
	find_min(bc(*c), bfc(*c)) - f, bfc(*c), find_min(fc(*c), bfc(*c)) + f);
      BreakObject(y, &yc);
      f = FindShift(x, y, COLM);
      back(x, COLM) = find_min(MAX_FULL_LENGTH, find_max(0, back(y, COLM) + f));
      fwd(x, COLM)  = find_min(MAX_FULL_LENGTH, find_max(0, fwd(y, COLM)  - f));
      break;


    case END_HEADER:
    case CLEAR_HEADER:

      /* these have size zero anyway, so not likely to reach this point */
      break;


    case BEGIN_HEADER:
    case SET_HEADER:
    
      Child(y, LastDown(x));
      y = BreakObject(y, c);
      back(x, COLM) = back(y, COLM);
      fwd(x, COLM) = fwd(y, COLM);
      debug3(DOB, D, "BreakObject(%s, COLM) = (%s, %s)", Image(type(x)),
	EchoLength(back(x, COLM)), EchoLength(fwd(x, COLM)));
      break;


    case PLAIN_GRAPHIC:
    case GRAPHIC:
    case LINK_SOURCE:
    case LINK_DEST:
    
      Child(y, LastDown(x));
      y = BreakObject(y, c);
      back(x, COLM) = back(y, COLM);
      fwd(x, COLM) = fwd(y, COLM);
      break;


    case SPLIT:
    
      Child(y, DownDim(x, COLM));
      y = BreakObject(y, c);
      back(x, COLM) = back(y, COLM);
      fwd(x, COLM) = fwd(y, COLM);
      break;


    case ACAT:
    
      if( back(x, COLM) > 0 )
      { int sz;  OBJECT rpos;
	/* shift the column mark of x to the left edge */
	sz = size(x, COLM);
	fwd(x, COLM) = find_min(MAX_FULL_LENGTH, sz);
	back(x, COLM) = 0;
	rpos = x;
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ Child(y, link);
	  if( type(y) == GAP_OBJ && mark(gap(y)) )
	  { mark(gap(y)) = FALSE;
	    rpos = y;
	  }
	}
	if( FitsConstraint(back(x, COLM), fwd(x, COLM), *c) )
	{ Error(13, 9, "column mark of unbroken paragraph moved left",
	    WARN, &fpos(rpos));
	  break;
	}
	Error(13, 10, "column mark of paragraph moved left before breaking",
	  WARN, &fpos(rpos));
	ifdebug(DOB, DD, DebugObject(x));
      }
      x = FillObject(x, c, nilobj, TRUE, TRUE, FALSE, &junk);
      break;


    case HCAT:
    
      x = BreakTable(x, c);
      break;


    case COL_THR:
    
      BreakJoinedGroup(Down(x), LastDown(x), nilobj, c,
	&back(x,COLM), &fwd(x,COLM));
      break;


    case VCAT:
    
      x = BreakVcat(x, c);
      break;
			

    default:
    
      assert1(FALSE, "BreakObject:", Image(type(x)));
      break;

  }
  assert( back(x, COLM) >= 0, "BreakObject: back(x, COLM) < 0!" );
  assert( fwd(x, COLM) >= 0, "BreakObject: fwd(x, COLM) < 0!" );
  debugcond6(DOB, D, --debug_depth < debug_depth_max,
    "%*s] BreakObject(%s %d) = (%s, %s)", debug_depth*2, " ", Image(type(x)),
    (int) x, EchoLength(back(x, COLM)), EchoLength(fwd(x, COLM)));
  debug2(DOB, DD,  "] BreakObject returning %s,%s, x =",
    EchoLength(back(x, COLM)), EchoLength(fwd(x, COLM)));
  ifdebug(DOB, DD,  DebugObject(x));
  return x;
} /* end BreakObject */
