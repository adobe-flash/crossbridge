/*@z23.c:Galley Printer:ScaleFactor()@****************************************/
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
/*  FILE:         z23.c                                                      */
/*  MODULE:       Galley Printer                                             */
/*  EXTERNS:      FixAndPrintObject()                                        */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define	NO_SUPPRESS	FALSE
#define	SUPPRESS	TRUE
#define word_equal(x, str)  (is_word(type(x)) && StringEqual(string(x), str))


/*****************************************************************************/
/*                                                                           */
/*  static float ScaleFactor(avail_size, inner_size)                         */
/*                                                                           */
/*  Return the scale factor for this scaling, or 0 if impossible.            */
/*                                                                           */
/*****************************************************************************/

static float ScaleFactor(FULL_LENGTH avail_size, FULL_LENGTH inner_size)
{ float scale_factor;
  scale_factor = avail_size <= 0 ? 0 :
		 inner_size <= 0 ? 0 : (float) avail_size / inner_size;
  return scale_factor;
}


/*@::FindAdjustIncrement()@***************************************************/
/*                                                                           */
/*  static FULL_LENGTH FindAdjustIncrement(x, frame_size, dim)               */
/*                                                                           */
/*  Find the amount by which to increase the width of the subobjects of      */
/*  concatenation object x so that it is adjusted to fill size frame_size.   */
/*                                                                           */
/*****************************************************************************/

static FULL_LENGTH FindAdjustIncrement(OBJECT x, FULL_LENGTH frame_size,int dim)
{ OBJECT y, link, prev, g;
  int adjustable_gaps;  BOOLEAN jn;
  FULL_LENGTH inc, mk, actual_size;

  debug2(DGP, DD, "FindAdjustIncrement(x, %s, %s)",
	EchoLength(frame_size), dimen(dim));
  FirstDefinite(x, link, prev, jn);
  if( link != x )
  { adjustable_gaps = 0;
    mk = back(prev, dim);
    NextDefiniteWithGap(x, link, y, g, jn);
    while( link != x )
    { if ( mode(gap(g)) == TAB_MODE || units(gap(g)) == AVAIL_UNIT
				    || units(gap(g)) == FRAME_UNIT )
      {	debug0(DGP, DD, "FindAdjustIncrement returning 0 (tab gap)");
	return 0;
      }
      mk += ActualGap(fwd(prev, dim), back(y, dim), fwd(y, dim), &gap(g),
		frame_size, mk);
      prev = y;
      adjustable_gaps++;
      NextDefiniteWithGap(x, link, y, g, jn);
    }
    actual_size = mk + fwd(prev, dim);
    debug2(DGP, DD, "  actual_size = %s, adjustable_gaps = %d",
	EchoLength(actual_size), adjustable_gaps);
    inc = adjustable_gaps==0 ? 0 : (frame_size - actual_size) / adjustable_gaps;
  }
  else inc = 0;
  debug1(DGP, DD, "FindAdjustIncrement returning %s", EchoLength(inc));
  return inc;
} /* end FindAdjustIncrement */


/*@::FixAndPrintObject()@*****************************************************/
/*                                                                           */
/*  OBJECT FixAndPrintObject(x, xmk, xb, xf, dim, suppress, pg, count,       */
/*           actual_back, actual_fwd)                                        */
/*                                                                           */
/*  Fix the absolute position of object x in dimension dim, in such a way    */
/*  that the principal mark of x has coordinate xmk, and x has actual size   */
/*  (xb, xf), where usually xb >= back(x, dim) and xf >= fwd(x, dim).        */
/*                                                                           */
/*  Actually, in the case where x includes an object lying on a thread       */
/*  leading outside x, the final size of x may be different.  Because        */
/*  of this, the procedure sets *actual_back and *actual_fwd to the actual   */
/*  size of x upon return.  The caller assumes that x will exactly occupy    */
/*  this space (actual_back, actual_fwd).                                    */
/*                                                                           */
/*  The suppress parameter is true if a temporary suppression of adjustment  */
/*  in this direction is in effect (because a neighbouring adjustment has    */
/*  already been done).  This is for @HAdjust and @VAdjust, not @PAdjust.    */
/*                                                                           */
/*  If dim == COLM, the coordinate information is merely stored; but if      */
/*  dim == ROWM, it is used to generate PostScript for printing x.           */
/*                                                                           */
/*  Parameter pg records the height of the current page.  This is used       */
/*  to correct for the fact that Lout places its origin at the top left,     */
/*  while PostScript places its origin at the bottom left.  This correction  */
/*  cannot be made by transforming user space.                               */
/*                                                                           */
/*  x is child number count of its parent (used by COL_THR and ROW_THR only) */
/*                                                                           */
/*  FixAndPrintObject ordinarily returns the object passed to it; however    */
/*  it occasionally replaces that object with another, and then it is the    */
/*  replacement object that is returned.                                     */
/*                                                                           */
/*****************************************************************************/

OBJECT FixAndPrintObject(OBJECT x, FULL_LENGTH xmk, FULL_LENGTH xb,
  FULL_LENGTH xf, int dim, BOOLEAN suppress, FULL_LENGTH pg, int count,
  FULL_LENGTH *actual_back, FULL_LENGTH *actual_fwd)
{ OBJECT y, link, prev, g, z, face, thr, res, uplink;
  /* OBJECT fixed_thr, tmp; */
  FULL_LENGTH mk, ymk, frame_size, back_edge, yb, yf, inc, f;
  FULL_LENGTH aback, afwd;
  int i; float scale_factor;  BOOLEAN jn;
  debug8(DGP, DD, "[ FixAndPrintObject(%s %s%s, %s, %s,%s, %s, %s, pg, count)",
    Image(type(x)),
    ((type(x) == WORD || type(x) == QWORD) ? string(x) : STR_EMPTY),
    EchoFilePos(&fpos(x)),
    EchoLength(xmk), EchoLength(xb), EchoLength(xf),dimen(dim),
    (suppress == SUPPRESS ? "suppress" : "no_suppress"));
  debug2(DGP, DD, "  size(x) = %s,%s;  x =",
    EchoLength(back(x, dim)), EchoLength(fwd(x, dim)));
  ifdebug(DGP, DD, DebugObject(x));
  res = x;

  /*** start and stop debugging
  if( dim == COLM && is_word(type(x)) &&
      StringEqual(string(x), AsciiToFull("STARTBUG")) )
    dbg[DGP].on[DD] = dbg[DGP].on[D] = TRUE;
  if( dim == COLM && is_word(type(x)) &&
      StringEqual(string(x), AsciiToFull("STOPBUG")) )
    dbg[DGP].on[DD] = dbg[DGP].on[D] = FALSE;
  *** */


  switch( type(x) )
  {

    case CLOSURE:
    case NULL_CLOS:
    case PAGE_LABEL:
    case CROSS:
    case FORCE_CROSS:
    
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case START_HVSPAN:
    case START_HSPAN:
    case START_VSPAN:

      CountChild(y, DownDim(x, dim), count);
      if( type(y) == HSPANNER || type(y) == VSPANNER )
      {
        Child(z, Down(y));
	Parent(thr, UpDim(x, dim));
	save_mark(y) = xmk - back(thr, dim) + back(z, dim);

        /* do the fix now if the first column is also the last one */
	debug2(DGP, DD, "  pre-inc spanner_fixed(y) = %d, spanner_count(y) = %d",
	  spanner_fixed(y), spanner_count(y));
	if( ++spanner_fixed(y) == spanner_count(y) )
	{
	  debug6(DGP, DD, "  f+last SPAN: yf = max(%s + %s - %s, %s, %s - %s)",
	    EchoLength(xmk), EchoLength(xf), EchoLength(save_mark(y)),
	    EchoLength(fwd(z, dim)),
	    EchoLength(bfc(constraint(y))), EchoLength(back(z, dim)));
	  yf = find_max(xmk + xf - save_mark(y), fwd(z, dim));
	  yf = find_max(yf, bfc(constraint(y)) - back(z, dim));
          z = FixAndPrintObject(z, save_mark(y), back(z, dim), yf, dim,
		FALSE, pg, 1, &aback, &afwd);
	  spanner_fixed(y) = 0; /* restart for if printed again */
	}
	*actual_back = back(x, dim); *actual_fwd = fwd(x, dim);
      }
      else
      {
	debug6(DGP, DD, "%s alternate FixAndPrintObject(%s, %s, %s, %s, %s, ..)",
	  Image(type(x)), Image(type(y)), EchoLength(xmk), EchoLength(xb),
	  EchoLength(xf), dimen(dim));
        y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	      actual_back, actual_fwd);
      }
      break;


    case HSPAN:
    case VSPAN:

      /* do the fix on the last one */
      if( (dim == COLM) == (type(x) == HSPAN) )
      {
        CountChild(y, DownDim(x, dim), count);
	assert(type(y) == HSPANNER || type(y) == VSPANNER, "FAPO HSPAN/VSPAN!");
	debug2(DGP, DD, "  pre-inc spanner_fixed(y) = %d, spanner_count(y) = %d",
	  spanner_fixed(y), spanner_count(y));
	if( ++spanner_fixed(y) == spanner_count(y) )
	{
          Child(z, Down(y));
	  debug6(DGP, DD, "  last SPAN: yf = max(%s + %s - %s, %s, %s - %s)",
	    EchoLength(xmk), EchoLength(xf), EchoLength(save_mark(y)),
	    EchoLength(fwd(z, dim)),
	    EchoLength(bfc(constraint(y))), EchoLength(back(z, dim)));
	  yf = find_max(xmk + xf - save_mark(y), fwd(z, dim));
	  yf = find_max(yf, bfc(constraint(y)) - back(z, dim));
          z = FixAndPrintObject(z, save_mark(y), back(z, dim), yf, dim,
		FALSE, pg, 1, &aback, &afwd);
	  *actual_back = back(x, dim); *actual_fwd = fwd(x, dim);
	  spanner_fixed(y) = 0; /* restart for if printed again */
	}
      }
      break;


    case WORD:
    case QWORD:
    
      if( dim == COLM )
      {
	/* save horizontal position for PrintWord below */
	word_save_mark(x) = xmk;

	/* if first occurrence of this font on this page, notify font */
	if( string(x)[0] != '\0' )
	{ face = finfo[word_font(x)].original_face;
	  if( font_page(face) < font_curr_page )
	  { debug3(DFT, DD, "FAPO: x = %s, word_font = %d, face = %s",
	      string(x), word_font(x), EchoObject(face));
	    FontPageUsed(face);
	  }
	}
      }
      else
      {
	if( string(x)[0] != '\0' )
	{ BackEnd->PrintWord(x, word_save_mark(x), pg - xmk);
	  /* NB if this word is to be underlined, it will be already enclosed
	     in an ACAT by Manifest, and that ACAT will do the underlining */
	}
      }
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case WIDE:
    case HIGH:
    
      CountChild(y, Down(x), count);
      if( (dim == COLM) == (type(x) == WIDE) )
      { yf = bfc(constraint(x)) - back(y, dim);
        y = FixAndPrintObject(y, xmk, back(y,dim), yf, dim, NO_SUPPRESS, pg,
	      count, &aback, &afwd);
        *actual_back = xb;  *actual_fwd = xf;
      }
      else
      {	y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	      actual_back, actual_fwd);
      }
      break;


    case HSHIFT:
    case VSHIFT:

      CountChild(y, Down(x), count);
      if( (dim == COLM) == (type(x) == HSHIFT) )
      {
	/* work out the size of the shift depending on the units */
	f = FindShift(x, y, dim);
	ymk = xmk - f;
	yb = find_max(0, xb - f);
	yf = find_max(0, xf + f);
	y = FixAndPrintObject(y, ymk, yb, yf, dim, suppress, pg, count,
	      &aback, &afwd);

	/* recalculate the size of x as in MinSize */
	f = FindShift(x, y, dim);
	*actual_back = find_min(MAX_FULL_LENGTH, find_max(0, aback + f));
	*actual_fwd  = find_min(MAX_FULL_LENGTH, find_max(0, afwd  - f));
      }
      else
      {	y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	      actual_back, actual_fwd);
      }
      break;


    case HCONTRACT:
    case VCONTRACT:
    
      CountChild(y, Down(x), count);
      if( (dim == COLM) == (type(x) == HCONTRACT) )
      {	y = FixAndPrintObject(y, xmk, back(y,dim), fwd(y,dim), dim,
	  NO_SUPPRESS, pg, count, &aback, &afwd);
        *actual_back = xb;  *actual_fwd = xf;
      }
      else
      {	y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	      actual_back, actual_fwd);
      }
      break;


    case ONE_COL:
    case ONE_ROW:
    case HLIMITED:
    case VLIMITED:
    case HEXPAND:
    case VEXPAND:
    
      CountChild(y, Down(x), count);
      if( (dim == COLM) == (type(x) == ONE_COL || type(x) == HEXPAND) )
      { y = FixAndPrintObject(y, xmk, xb, xf, dim, NO_SUPPRESS, pg, count,
	      &aback, &afwd);
        *actual_back = xb;  *actual_fwd = xf;
      }
      else
      {	y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	      actual_back, actual_fwd);
      }
      break;


    case VSCALE:

      debug0(DRS, DD, "FixAndPrintObject at VSCALE");
      CountChild(y, Down(x), count);
      if( BackEnd->scale_avail )
      {
	if( dim == COLM )
	  y = FixAndPrintObject(y, xmk, xb, xf, dim, NO_SUPPRESS, pg, count,
		&aback, &afwd);
	else if( (scale_factor = ScaleFactor(xb+xf, size(y, ROWM))) > 0 )
	{ BackEnd->SaveGraphicState(y);
	  BackEnd->CoordTranslate(0,
	    pg - (xmk - xb + (FULL_LENGTH) (back(y, ROWM) * scale_factor)));
	  BackEnd->CoordScale(1.0, scale_factor);
	  y = FixAndPrintObject(y, 0, back(y,ROWM), fwd(y,ROWM), dim,
	    NO_SUPPRESS, 0, count, &aback, &afwd);
	  BackEnd->RestoreGraphicState();
	}
	else if( !is_word(type(y)) || string(y)[0] != '\0' )
	  Error(23, 1, "object deleted (it cannot be scaled vertically)",
	    WARN, &fpos(x));
      }
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case HSCALE:
    
      debug0(DRS, DD, "FixAndPrintObject at HSCALE");
      CountChild(y, Down(x), count);
      if( BackEnd->scale_avail )
      {	if( dim == COLM )
        { save_mark(x) = xmk;
	  bc(constraint(x)) = xb;
	  fc(constraint(x)) = xf;
          if( (scale_factor = ScaleFactor(xb+xf, size(y, COLM))) > 0 )
	    y = FixAndPrintObject(y, 0, back(y, COLM), fwd(y, COLM), dim,
	      NO_SUPPRESS, pg, count, &aback, &afwd);
          else if( !is_word(type(y)) || string(y)[0] != '\0' )
	    Error(23, 2, "object deleted (it cannot be scaled horizontally)",
	      WARN, &fpos(y));
        }
        else if( (scale_factor =
	  ScaleFactor(bc(constraint(x))+fc(constraint(x)),size(y,COLM))) > 0 )
        { BackEnd->SaveGraphicState(y);
	  BackEnd->CoordTranslate(save_mark(x) - bc(constraint(x))
	       + (FULL_LENGTH) (back(y, COLM)*scale_factor), 0);
	  BackEnd->CoordScale(scale_factor, 1.0);
          y = FixAndPrintObject(y, xmk, xb, xf, dim, NO_SUPPRESS, pg, count,
		&aback, &afwd);
	  BackEnd->RestoreGraphicState();
        }
      }
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case SCALE:

      CountChild(y, Down(x), count);
      if( BackEnd->scale_avail )
      {
        if( dim == COLM )
        { assert( bc(constraint(x)) > 0, "FAPO: horizontal scale factor!" );
	  save_mark(x) = xmk;
	  yb = xb * SF / bc(constraint(x));
	  yf = xf * SF / bc(constraint(x));
          y = FixAndPrintObject(y, 0, yb, yf, dim, NO_SUPPRESS, pg, count,
		&aback, &afwd);
        }
        else
        { assert( fc(constraint(x)) > 0, "FAPO: vertical scale factor!" );
	  yb = xb * SF / fc(constraint(x));
	  yf = xf * SF / fc(constraint(x));
	  BackEnd->SaveGraphicState(y);
	  BackEnd->CoordTranslate(save_mark(x), pg - xmk);
	  BackEnd->CoordScale( (float)bc(constraint(x))/SF,
	    (float)fc(constraint(x))/SF);
          y = FixAndPrintObject(y, 0, yb, yf, dim, NO_SUPPRESS, 0, count,
		&aback, &afwd);
	  BackEnd->RestoreGraphicState();
        }
      }
      else if( bc(constraint(x)) == SF && fc(constraint(x)) == SF )
      {
	y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	      &aback, &afwd);
      }
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case KERN_SHRINK:

      CountChild(y, LastDown(x), count);
      if( dim == COLM )
      { y = FixAndPrintObject(y, xmk, back(y,dim), fwd(y,dim), dim,
	  NO_SUPPRESS, pg, count, &aback, &afwd);
	*actual_back = back(x, dim);  *actual_fwd = fwd(x, dim);
      }
      else
      {	y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	      actual_back, actual_fwd);
      }
      break;


    case BACKGROUND:
 
      /* this object has the size of its second child; but its first */
      /* child gets printed too, in the same space                   */
      CountChild(y, Down(x), count);
      y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	&aback, &afwd);
      CountChild(y, LastDown(x), count);
      y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
	&aback, &afwd);
      *actual_back = back(x, dim);  *actual_fwd = fwd(x, dim);
      break;


    case ROTATE:
    
      CountChild(y, Down(x), count);
      if( BackEnd->rotate_avail )
      {
        if( dim == COLM )
        { CONSTRAINT colc, rowc, yc;
          save_mark(x) = xmk;
	  SetConstraint(colc, back(x,COLM), MAX_FULL_LENGTH, fwd(x,COLM));
	  SetConstraint(rowc, back(x,ROWM), MAX_FULL_LENGTH, fwd(x,ROWM));
	  RotateConstraint(&yc, y, sparec(constraint(x)), &colc, &rowc,COLM);
	  y = FixAndPrintObject(y, 0, bc(yc), fc(yc), COLM, NO_SUPPRESS, pg,
		count, &aback, &afwd);
        }
        else
        { CONSTRAINT colc, rowc, yc;
	  BackEnd->SaveGraphicState(y);
	  BackEnd->CoordTranslate(save_mark(x), pg - xmk);
	  BackEnd->CoordRotate(sparec(constraint(x)));
	  SetConstraint(colc, back(x,COLM), MAX_FULL_LENGTH, fwd(x,COLM));
	  SetConstraint(rowc, back(x,ROWM), MAX_FULL_LENGTH, fwd(x,ROWM));
	  RotateConstraint(&yc, y, sparec(constraint(x)), &colc, &rowc, ROWM);
	  y = FixAndPrintObject(y, 0, bc(yc), fc(yc), ROWM, NO_SUPPRESS, 0,
		count, &aback, &afwd);
	  BackEnd->RestoreGraphicState();
        }
      }
      else if( sparec(constraint(x)) == 0 )
	y = FixAndPrintObject(y,xmk,xb,xf,dim,suppress,pg,count,&aback,&afwd);
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case PLAIN_GRAPHIC:

      CountChild(y, LastDown(x), count);
      if( BackEnd->plaingraphic_avail )
      {
	if( dim == COLM )
	{
	  back(x, dim) = xb;		/* NB state change here */
	  fwd(x, dim)  = xf;
	  save_mark(x) = xmk - back(x, dim);
	  debug2(DGP, DD, "PLAIN_GRAPHIC COLM storing size %s, %s",
	    EchoLength(back(x, dim)), EchoLength(fwd(x, dim)));
          y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
		&aback, &afwd);
	}
	else
	{ OBJECT tmp, pre, post;
          Child(tmp, Down(x));
          if( type(tmp) == VCAT )
          { Child(pre, Down(tmp));
            Child(post, LastDown(tmp));
          }
          else pre = tmp, post = nilobj;
	  back(x, dim) = xb;
	  fwd(x, dim)  = xf;
          BackEnd->PrintPlainGraphic(pre, save_mark(x),
	    pg - (xmk - back(x, dim)), x);
          y = FixAndPrintObject(y, xmk, xb, xf, dim, suppress, pg, count,
		&aback, &afwd);
          if( post != nilobj )
	    BackEnd->PrintPlainGraphic(post, save_mark(x),
	      pg - (xmk - back(x, dim)), x);
	}
      }
      else
	y = FixAndPrintObject(y, xmk,xb,xf,dim,suppress,pg,count,&aback,&afwd);
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case GRAPHIC:
    
      CountChild(y, LastDown(x), count);
      if( BackEnd->graphic_avail )
      {
	if( dim == COLM )
	{
	  /* if first occurrence of this font on this page, notify font */
	  if( font(save_style(x)) > 0 )
	  { face = finfo[font(save_style(x))].original_face;
	    if( font_page(face) < font_curr_page )  FontPageUsed(face);
	  }

	  back(x, dim) = xb;  /* NB state change here */
	  fwd(x, dim)  = xf;
	  debug2(DGP, DD, "GRAPHIC COLM storing size %s, %s",
	    EchoLength(back(x, dim)), EchoLength(fwd(x, dim)));
	  save_mark(x) = xmk - back(x, COLM);
          y = FixAndPrintObject(y, xb, xb, xf, dim, NO_SUPPRESS, pg, count,
		&aback, &afwd);
	}
	else
	{ OBJECT tmp, pre, post;
          Child(tmp, Down(x));
          if( type(tmp) == VCAT )
          { Child(pre, Down(tmp));
            Child(post, LastDown(tmp));
          }
          else pre = tmp, post = nilobj;
	  back(x, dim) = xb;
	  fwd(x, dim)  = xf;

	  BackEnd->SaveTranslateDefineSave(x, save_mark(x),
	    pg - (xmk + fwd(x, ROWM)));
          BackEnd->PrintGraphicObject(pre);
          BackEnd->RestoreGraphicState();
          y = FixAndPrintObject(y, xb, xb, xf, dim, NO_SUPPRESS, xb + xf,
		count, &aback, &afwd);
          if( post != nilobj )  BackEnd->PrintGraphicObject(post);
          BackEnd->RestoreGraphicState();
	}
      }
      else
        y = FixAndPrintObject(y, xmk,xb,xf,dim,suppress,pg,count,&aback,&afwd);
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case LINK_SOURCE:
    case LINK_DEST:
    
      CountChild(y, LastDown(x), count);
      if( dim == COLM )
	save_mark(x) = xmk;
      else
      {	Child(z, Down(x));
	if( type(x) == LINK_SOURCE )
	  BackEnd->LinkSource(z, save_mark(x) - back(x, COLM),
	    (pg - xmk) - xf, save_mark(x) + fwd(x, COLM),
	    (pg - xmk) + xb);
	else
	  BackEnd->LinkDest(z, save_mark(x) - back(x, COLM),
	    (pg - xmk) - xf, save_mark(x) + fwd(x, COLM),
	    (pg - xmk) + xb);
      }
      y = FixAndPrintObject(y, xmk, xb, xf, dim, NO_SUPPRESS, pg, count,
	    &aback, &afwd);
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case INCGRAPHIC:
    case SINCGRAPHIC:

      CountChild(y, Down(x), count);
      if( BackEnd->incgraphic_avail )
      {
	if( dim == COLM )
	{ save_mark(x) = xmk;
	  if( incgraphic_ok(x) )
	  { debug2(DGP, DD, "  %s (style %s)",
	      EchoObject(x), EchoStyle(&save_style(x)));
	    face = finfo[font(save_style(x))].original_face;
	    if( font_page(face) < font_curr_page )
	    { debug3(DFT, DD, "FAPO-IG: x = %s, font = %d, face = %s",
		string(x), font(save_style(x)), EchoObject(face));
	      FontPageUsed(face);
	    }
	  }
	}
	else if( incgraphic_ok(x) )
	  BackEnd->PrintGraphicInclude(x, save_mark(x), pg - xmk);
      }
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case SPLIT:
    
      link = DownDim(x, dim);  CountChild(y, link, count);
      y = FixAndPrintObject(y, xmk, find_max(back(y, dim), xb),
	find_max(fwd(y, dim), xf), dim, suppress, pg, count,
	actual_back, actual_fwd);
      break;


    case VCAT:
    case HCAT:

      if( (type(x) == VCAT) == (dim == ROWM) )
      { 
	debug6(DGP, DD, "[ FAPO-CAT %s (%s,%s): xmk %s, xb %s, xf %s",
	    Image(type(x)), EchoLength(back(x, dim)), EchoLength(fwd(x, dim)),
	    EchoLength(xmk), EchoLength(xb), EchoLength(xf));

	FirstDefinite(x, link, prev, jn);
	if( link != x )
	{

	  /*******************************************************************/
	  /*                                                                 */
	  /*  handle the special case of a 0rt gap at the beginning (left    */
	  /*  justify) by converting it to 0ie but increasing fwd(prev) to   */
	  /*  the maximum possible                                           */
	  /*                                                                 */
	  /*******************************************************************/

	  NextDefiniteWithGap(x, link, y, g, jn);
	  if( link != x && mode(gap(g)) == TAB_MODE &&
	      units(gap(g)) == AVAIL_UNIT && width(gap(g)) == 0 )
	  {
	    debug2(DGP, DD, "  FAPO-CAT converting 0rt (back(x, dim) %s, xb %s)",
	      EchoLength(back(x, dim)), EchoLength(xb));
	    /* NB state change here */
	    fwd(prev, dim) += xb - back(x, dim);
	    back(x, dim) = xb;
	    mode(gap(g)) = EDGE_MODE;
	    units(gap(g)) = FIXED_UNIT;
	  }
	  FirstDefinite(x, link, prev, jn);

	  /*******************************************************************/
	  /*                                                                 */
	  /*  Initialize the following variables:                            */
	  /*                                                                 */
	  /*  frame_size   the total width actually available                */
	  /*                                                                 */
	  /*  back_edge    where the first element begins                    */
	  /*                                                                 */
	  /*  inc          the adjust increment, used when adjusting gaps    */
	  /*                                                                 */
	  /*  mk           where the mark of prev is to go                   */
	  /*                                                                 */
	  /*******************************************************************/

	  frame_size = back(x, dim) + xf;
	  back_edge = xmk - back(x, dim);
	  if( adjust_cat(x) && !suppress )
	    inc = FindAdjustIncrement(x, frame_size, dim);
	  else inc = 0;
	  mk = back_edge + back(prev, dim);
	  debug4(DGP, DD, "  FAPO-CAT back_edge %s, mk %s, frame %s, inc %s",
	    EchoLength(back_edge), EchoLength(mk), EchoLength(frame_size),
	    EchoLength(inc));

	  /*******************************************************************/
	  /*                                                                 */
	  /*  Fix each element "prev" in turn along the cat operator         */
	  /*                                                                 */
	  /*******************************************************************/

	  NextDefiniteWithGap(x, link, y, g, jn);
	  while( link != x )
	  {
	    if( mode(gap(g)) == TAB_MODE && units(gap(g)) == AVAIL_UNIT &&
		width(gap(g))==FR )
	    {
	      /* object is followed by 1rt gap, give it full space to print */
	      debug5(DGP,D,"  FAPO (a) calling FAPO(%s, %s, %s, max(%s, %s))",
	        Image(type(prev)), EchoLength(mk), EchoLength(back(prev, dim)),
		EchoLength(fwd(prev, dim)), EchoLength(xmk+xf-mk-size(y,dim)));
	      prev = FixAndPrintObject(prev, mk, back(prev, dim),
		find_max(fwd(prev, dim), xmk+xf-mk - size(y, dim)),
		dim, NO_SUPPRESS, pg, count, &aback, &afwd);
	    }
	    else
	    {
	      debug5(DGP, DD, "  FAPO-CAT (b) calling FAPO(%s, %s, %s, %s+%s)",
	        Image(type(prev)), EchoLength(mk), EchoLength(back(prev, dim)),
		EchoLength(fwd(prev, dim)), EchoLength(inc));
	      prev = FixAndPrintObject(prev, mk, back(prev, dim),
		fwd(prev, dim) + inc, dim, NO_SUPPRESS, pg, count,&aback,&afwd);
	    }
	    mk += ActualGap(afwd, back(y, dim), fwd(y, dim), &gap(g),
		    frame_size, mk - back_edge);
	    prev = y;
	    NextDefiniteWithGap(x, link, y, g, jn);
	  }

	  /*******************************************************************/
	  /*                                                                 */
	  /*  At end, fix last element in conformity with "suppress"         */
	  /*  and set *actual_back and *actual_fwd.                          */
	  /*                                                                 */
	  /*******************************************************************/

	  if( suppress )
	  {
	    debug4(DGP, DD, "  FAPO-CAT (c) calling FAPO(%s, %s, %s, %s)",
	      Image(type(prev)), EchoLength(mk), EchoLength(back(prev, dim)),
	      EchoLength(fwd(prev, dim)));
	    prev = FixAndPrintObject(prev, mk, back(prev, dim), fwd(prev, dim),
	      dim, NO_SUPPRESS, pg, count, &aback, &afwd);
	  }
	  else
	  {
	    debug5(DGP, DD,"  FAPO-CAT (d) calls FAPO(%s, %s, %s, max(%s, %s))",
	      Image(type(prev)), EchoLength(mk), EchoLength(back(prev, dim)),
	      EchoLength(fwd(prev, dim)), EchoLength(xmk + xf - mk));
	    ifdebug(DGP, DD, DebugObject(prev));
	    prev = FixAndPrintObject(prev, mk, back(prev,dim),
	      find_max(fwd(prev, dim), xmk + xf - mk),
	      dim, NO_SUPPRESS, pg, count, &aback, &afwd);
	  }
	  *actual_back = find_max(back(x, dim), xb);
	  *actual_fwd = mk + fwd(prev, dim) - back_edge - *actual_back;
	  debugcond4(DGP, DD, type(x) == HCAT,
	    "HCAT original (%s, %s) to actual (%s, %s)",
	    EchoLength(back(x, dim)), EchoLength(fwd(x, dim)),
	    EchoLength(*actual_back), EchoLength(*actual_fwd));
	}
	else *actual_back = xb, *actual_fwd = xf;
	debug0(DGP, DD, "] FAPO-CAT returning.");
      }
      else
      { OBJECT start_group, zlink, m;  BOOLEAN dble_found;
	FULL_LENGTH b, f, dlen;
	start_group = nilobj;  dble_found = FALSE;  dlen = 0;
	debug0(DGP, DD, "  groups beginning.");
	FirstDefinite(x, link, y, jn);
	if( link != x )
	{
	  /* start first group, with or without join */
	  b = back(y, dim);
	  f = fwd(y, dim);
	  m = y;
	  start_group = link;
	  dble_found = !jn;
	  debug4(DGP, DD, "  starting first group %s (%sdbl_found): b %s, f %s",
	    Image(type(y)), dble_found ? "" : "not ",
	    EchoLength(b), EchoLength(f));
	
	  NextDefiniteWithGap(x, link, y, g, jn);
	  while( link != x )
	  {
	    if( !jn )
	    {
	      /* finish off and fix the group ending just before g */
	      debug2(DGP, DD, "  finishing group: b = %s, f = %s",
		EchoLength(b), EchoLength(f));
	      m = FixAndPrintObject(m, xmk+b, b, xf-b, dim,
		NO_SUPPRESS, pg, count, &aback, &afwd);
	      b = back(m, dim);  f = fwd(m, dim);
	      for( zlink = start_group;  zlink != link;  zlink=NextDown(zlink) )
	      { CountChild(z, zlink, count);
		if( !is_definite(type(z)) || z == m )  continue;
		z = FixAndPrintObject(z, xmk + b, b, xf - b, dim,
		  SUPPRESS, pg, count, &aback, &afwd);
		b = find_max(b, back(z, dim));  f = find_max(f, fwd(z, dim));
	      }
	      dlen = find_max(dlen, b + f);
	      dble_found = TRUE;
	      start_group = nilobj;

	      /* start new group */
	      b = back(y, dim);
	      f = fwd(y, dim);
	      m = y;
	      start_group = link;
	      debug2(DGP, DD, "  starting group: b = %s, f = %s",
		EchoLength(b), EchoLength(f));
	    }
	    else
	    {
	      /* continue with current group */
	      b = find_max(b, back(y, dim));
	      f = find_max(f, fwd(y, dim));
	      if( fwd(y, dim) > fwd(m, dim) )  m = y;
	      debug2(DGP, DD, "  continuing group: b = %s, f = %s",
		EchoLength(b), EchoLength(f));
	    }

	    NextDefiniteWithGap(x, link, y, g, jn);
	  }
	  assert( start_group != nilobj, "FAPO: final start_group!" );

	  if( dble_found || !jn )
	  {
	    /* finish off and fix this last group */
	    debug2(DGP, DD, "  finishing last group: b = %s, f = %s",
	        EchoLength(b), EchoLength(f));
	    m = FixAndPrintObject(m, xmk+b, b, xf - b, dim, NO_SUPPRESS, pg,
		  count, &aback, &afwd);
	    b = back(m, dim);  f = fwd(m, dim);
	    for( zlink = start_group;  zlink != x;  zlink = NextDown(zlink) )
	    { CountChild(z, zlink, count);
	      if( !is_definite(type(z)) || z == m )  continue;
	      z = FixAndPrintObject(z, xmk+b, b, xf - b, dim, SUPPRESS, pg,
		    count, &aback, &afwd);
	      b = find_max(b, back(z, dim));  f = find_max(f, fwd(z, dim));
	    }
	    dlen = find_max(dlen, b + f);
	    *actual_back = 0;  *actual_fwd = dlen;
	  }
	  else
	  {
	    /* finish off and fix this last and only group */
	    debug2(DGP, DD, "  finishing last and only group: b = %s, f = %s",
	      EchoLength(b), EchoLength(f));
	    m = FixAndPrintObject(m, xmk, xb, xf, dim, NO_SUPPRESS, pg, count,
	      &b, &f);
	    for( zlink = start_group;  zlink != x;  zlink = NextDown(zlink) )
	    { CountChild(z, zlink, count);
	      if( !is_definite(type(z)) || z == m )  continue;
	      z = FixAndPrintObject(z, xmk, xb, xf, dim, SUPPRESS, pg, count,
		    &aback, &afwd);
	      b = find_max(b, aback);  f = find_max(f, afwd);
	    }
	    *actual_back = b;  *actual_fwd = f;
	  }
	}
      }
      break;


    case ACAT:

      if( dim == COLM )
      { BOOLEAN will_adjust, adjusting;
	FULL_LENGTH actual_size,
	adjust_indent, frame_size, back_edge, adjust_inc, inc, adjust_sofar;
	int adjustable_gaps, gaps_sofar;
	BOOLEAN underlining; int underline_xstart;
	FONT_NUM underline_font;  COLOUR_NUM underline_colour;
	OBJECT urec, last_bad_gap;
      

	/*********************************************************************/
	/*                                                                   */
	/*  The first step is to calculate the following values:             */
	/*                                                                   */
	/*    last_bad_gap     The rightmost tab gap, or nilobj if none;     */
	/*                                                                   */
	/*    adjustable_gaps  the number of gaps suitable for adjustment;   */
	/*                     i.e. to the right of the right-most tab gap,  */
	/*                     and of non-zero width;                        */
	/*                                                                   */
	/*    actual_size      the actual size of x without adjustment.      */
	/*                                                                   */
	/*  These are needed when adjusting the line.                        */
	/*                                                                   */
	/*********************************************************************/

	FirstDefinite(x, link, y, jn);
	if( link == x )
	{
	  *actual_back = back(x, dim); *actual_fwd = fwd(x, dim);
	  break;  /* no definite children, nothing to print */
	}

	/*** nasty bug finder 
	{ OBJECT ff = y;
	debugcond1(DGP, DD, word_equal(ff, "@ReportLayout"),
	  "FAPO(%s, COLM)", EchoObject(x));
	debugcond1(DGP, DD, word_equal(ff, "@ReportLayout"),
	  "  adjust_cat(x) = %s", bool(adjust_cat(x)));
	}
	***/

	last_bad_gap = nilobj;
	adjustable_gaps = 0;
	back_edge = xmk - xb;
	mk = back_edge + back(y, dim);
	frame_size = xb + xf;
	prev = y;
	NextDefiniteWithGap(x, link, y, g, jn);
	while( link != x )
	{
	  save_actual_gap(g) = ActualGap(fwd(prev, dim), back(y, dim),
		fwd(y, dim), &gap(g), frame_size, mk - back_edge);
	  mk += save_actual_gap(g);
	  if( mode(gap(g)) == TAB_MODE || units(gap(g)) == AVAIL_UNIT
				       || units(gap(g)) == FRAME_UNIT )
	  { last_bad_gap = g;
	    adjustable_gaps = 0;
	  }
	  else if( width(gap(g)) > 0 )  adjustable_gaps++;
	  prev = y;
	  NextDefiniteWithGap(x, link, y, g, jn);
	}
	actual_size = mk + fwd(prev, dim) - back_edge;

	/*********************************************************************/
	/*                                                                   */
	/*  It is possible that the line cannot be displayed in any          */
	/*  reasonable way, because the paragraph breaker was forced to      */
	/*  produce an overfull line.  In this case, actual_size will        */
	/*  exceed frame_size and there will be no adjustable gaps.  The     */
	/*  solution is to horizontally scale the line if possible, or       */
	/*  else to not print it at all.                                     */
	/*                                                                   */
	/*********************************************************************/

	if( actual_size > frame_size && adjustable_gaps == 0 )
	{ 
	  /* can't be fixed by adjustment, so scale the line or delete it */
	  CONSTRAINT c;
	  SetConstraint(c, 0, frame_size, frame_size);
	  fwd(x, dim) = actual_size;
	  debug2(DGP, DD, "  oversize, actual_size = %s, frame_size = %s",
	      EchoLength(actual_size), EchoLength(frame_size));
	  if( BackEnd->scale_avail && InsertScale(x, &c) )
	  {
	    /* the problem has just been fixed, by inserting a @Scale above x */
	    OBJECT prnt;
	    Parent(prnt, Up(x));
	    Child(y, Down(x));
	    if( actual_size - frame_size < 1 * PT )
	    {
	      /* the correction is probably due to roundoff error, and */
	      /* anyway is too small to print an error message about   */
	    }
	    else if( Down(x) == LastDown(x) && is_word(type(y)) )
	    {
	      Error(23, 3, "word %s horizontally scaled by factor %.2f (too wide for %s paragraph)",
		WARN, &fpos(y), string(y), (float) bc(constraint(prnt)) / SF,
		EchoLength(frame_size));
	    }
	    else
	    {
	      Error(23, 4, "%s object horizontally scaled by factor %.2f (too wide for %s paragraph)",
		WARN, &fpos(x), EchoLength(size(x, COLM)),
		(float) bc(constraint(prnt)) / SF, EchoLength(frame_size));
	    }
	    prnt = FixAndPrintObject(prnt, xmk, back(prnt, dim), fwd(prnt, dim), dim,
	      NO_SUPPRESS, pg, count, &aback, &afwd);
	  }
	  else
	  {
	    /* fix the problem by refraining from printing the line */
	    if( size(x, COLM) <= 0 )
	      Error(23, 5, "oversize object has size 0 or less", INTERN, &fpos(x));
	    Child(y, Down(x));
	    if( Down(x) == LastDown(x) && is_word(type(y)) )
	    { Error(23, 6, "word %s deleted (too wide for %s paragraph)",
		WARN, &fpos(y), string(y), EchoLength(frame_size));
	    }
	    else
	    { Error(23, 7, "%s object deleted (too wide for %s paragraph)",
		WARN, &fpos(x), EchoLength(size(x, COLM)), EchoLength(frame_size));
	    }

	    /* delete and dispose every child of x */
	    while( Down(x) != x )
	      DisposeChild(Down(x));
	    y = MakeWord(WORD, STR_EMPTY, &fpos(x));
	    Link(x, y);
	    back(y, COLM) = fwd(y, COLM) = 0;
	    back(y, ROWM) = fwd(y, ROWM) = 0;
	  }
        }
        else
        {

	  /********************************************************************/
	  /*                                                                  */
	  /*  The line may be displayed in one of four ways:  centred, right- */
	  /*  justified, adjusted, or none of the above (i.e. left justified).*/
	  /*  An overfull line is always adjusted; otherwise, the line will   */
	  /*  be centred or right justified if the display style asks for it; */
	  /*  otherwise, the line will be adjusted if adjust_cat(x) == TRUE   */
	  /*  (i.e. there is an enclosing @PAdjust) or if the display style is*/
	  /*  DO_ADJUST (meaning that this line is one of a paragraph set in  */
	  /*  the adjust or outdent break style, other than the last line);   */
	  /*  otherwise, the line is left justified.                          */
	  /*                                                                  */
	  /*  The second step is to decide which of these four cases holds    */
	  /*  for this line, and to record the decision in these variables:   */
	  /*                                                                  */
	  /*    will_adjust      TRUE if the adjusted style applies; in this  */
	  /*                     case, variables adjust_inc and inc will be   */
	  /*                     set to the appropriate adjustment value;     */
	  /*                                                                  */
	  /*    adjust_indent    If centring or right justification applies,  */
	  /*                     the indent to produce this, else zero.       */
	  /*                                                                  */
	  /*  NB adjust_inc may be negative, if the optimal paragraph breaker */
	  /*  has chosen to shrink some gaps.                                 */
	  /*                                                                  */
	  /*  NB we are assigning to adjust_cat here; is this a problem?      */
	  /*                                                                  */
	  /********************************************************************/

	  if( actual_size > frame_size )
	  { 
	    assert( adjustable_gaps > 0, "FAPO: adjustable_gaps!" );
	    adjust_cat(x) = TRUE;
	    adjust_indent = 0;
	  }
	  else switch( display_style(save_style(x)) )
	  {
	    case DO_ADJUST:	adjust_cat(x) = TRUE;
				adjust_indent = 0;
				debug1(DSF, D,  "adjust %s", EchoObject(x));
				break;
	
	    case DISPLAY_CENTRE: adjust_cat(x) = FALSE;
				adjust_indent = (frame_size - actual_size)/2;
				debug1(DGP, DD, "cdisp %s", EchoObject(x));
				break;

	    case DISPLAY_RIGHT:	adjust_cat(x) = FALSE;
				adjust_indent = frame_size - actual_size;
				debug1(DGP, DD, "rdisp %s", EchoObject(x));
				debug1(DSF, D,  "rdisp %s", EchoObject(x));
				break;

	    default:		/* leave adjust_cat(x) as is */
				adjust_indent = 0;
				break;
	  }

	  debug2(DGP, DD, "ACAT %s %s",
	    EchoStyle(&save_style(x)), EchoObject(x));
	  debug2(DGP, DD, "frame_size = %s, actual_size = %s",
	    EchoLength(frame_size), EchoLength(actual_size));

	  if( adjust_cat(x) && adjustable_gaps > 0  )
	  { will_adjust = TRUE;
	    adjust_inc = (frame_size - actual_size) / adjustable_gaps;
	    inc = find_max(adjust_inc, 0);
	    gaps_sofar = 0;	/* number of gaps adjusted so far */
	    adjust_sofar = 0;	/* total width of adjustments so far */
	    debug2(DGP, DD,"will_adjust: adjustable_gaps = %d, adjust_inc = %s",
	      adjustable_gaps, EchoLength(adjust_inc));
	  }
	  else will_adjust = FALSE;


	  /********************************************************************/
	  /*                                                                  */
	  /*  The third and final step is to traverse x, fixing subobjects.   */
	  /*  Variable "adjusting" is true while adjusting is occurring.      */
	  /*                                                                  */
	  /********************************************************************/

	  underlining = FALSE;
	  adjusting = will_adjust && last_bad_gap == nilobj;
	  FirstDefinite(x, link, y, jn);
	  prev = y;
	  mk = xmk - back(x, dim) + back(y, dim) + adjust_indent;
	  NextDefiniteWithGap(x, link, y, g, jn);
	  while( link != x )
	  {
	    /* check for underlining */
	    if( underline(prev) == UNDER_ON )
	    {
	      debug3(DGP, D, "  FAPO/ACAT1 underline() := %s for %s %s",
	        bool(FALSE), Image(type(prev)), EchoObject(prev));
	      if( !underlining )
	      {
	        /* underlining begins here */
	        underlining = TRUE;
	        debug2(DGP, DD, "underlining begins at %s %s",
		  Image(type(prev)), EchoObject(prev));
	        underline_font = is_word(type(prev)) ? word_font(prev) :
		    font(save_style(x));
		underline_colour = is_word(type(prev)) ? word_colour(prev) :
		    colour(save_style(x));
	        underline_xstart = mk - back(prev, dim);
	      }
	      if( underline(g) == UNDER_OFF )
	      {
	        /* underlining ends here */
	        debug2(DGP, D, "underlining ends at %s %s",
		  Image(type(prev)), EchoObject(prev));
	        New(urec, UNDER_REC);
	        back(urec, COLM) = underline_xstart;
	        fwd(urec, COLM) = mk + fwd(prev, dim);
	        back(urec, ROWM) = underline_font;
	        fwd(urec, ROWM) = underline_colour;
	        underlining = FALSE;
	        Link(Up(prev), urec);
	      }
	    }

	    /* fix previous definite now we know it is not the last one  */
	    if( adjusting && width(gap(g)) > 0 )
	    { int tmp;

	      prev = FixAndPrintObject(prev, mk, back(prev, dim),
		fwd(prev, dim) + inc, dim, NO_SUPPRESS, pg, count,&aback,&afwd);
	      gaps_sofar++;
	      tmp = ((frame_size - actual_size) * gaps_sofar) / adjustable_gaps;
	      mk += save_actual_gap(g) + (tmp - adjust_sofar);
	      adjust_sofar = tmp;
	    }
	    else
	    {
	      prev = FixAndPrintObject(prev, mk, back(prev, dim), fwd(prev,dim),
	        dim, NO_SUPPRESS, pg, count, &aback, &afwd);

	      mk += save_actual_gap(g);
	    }
	    prev = y;

	    /* commence adjustment if required */
	    if( !adjusting && will_adjust && g == last_bad_gap )
	      adjusting = TRUE;

	    NextDefiniteWithGap(x, link, y, g, jn);
	  }

	  /* check for underlining */
	  debugcond3(DGP, DD, underline(prev) == UNDER_UNDEF,
	    "  underlining is UNDER_UNDEF in %s: %s %s in para:",
	    EchoFilePos(&fpos(prev)), Image(type(prev)), EchoObject(prev));
	  debugcond1(DGP, DD, underline(prev)==UNDER_UNDEF, "%s",EchoObject(x));
	  assert( underline(prev) == UNDER_OFF || underline(prev) == UNDER_ON,
	    "FixAndPrint: underline(prev)!" );
	  if( underline(prev) == UNDER_ON )
	  {
	    debug3(DGP, D, "  FAPO/ACAT1 underline() := %s for %s %s",
	      bool(FALSE), Image(type(prev)), EchoObject(prev));
	    if( !underlining )
	    {
	      /* underlining begins here */
	      debug2(DGP, DD, "underlining begins at %s %s",
	        Image(type(prev)), EchoObject(prev));
	      underlining = TRUE;
	      underline_font = is_word(type(prev)) ? word_font(prev) :
		    font(save_style(x));
	      underline_colour = is_word(type(prev)) ? word_colour(prev) :
		    colour(save_style(x));
	      underline_xstart = mk - back(prev, dim);
	    }

	    /* underlining must end here */
	    debug2(DGP, DD, "underlining ends at %s %s",
	      Image(type(prev)), EchoObject(prev));
	    New(urec, UNDER_REC);
	    back(urec, COLM) = underline_xstart;
	    fwd(urec, COLM) = mk + fwd(prev, dim);
	    back(urec, ROWM) = underline_font;
	    fwd(urec, ROWM) = underline_colour;
	    underlining = FALSE;
	    Link(Up(prev), urec);
	  }

	  /* fix the last definite subobject, prev, which must exist */
	  prev = FixAndPrintObject(prev, mk, back(prev, dim),
	    frame_size - (mk - xmk) - back(x, dim),
	    dim, NO_SUPPRESS, pg, count, &aback, &afwd);

        }
      }
      else for( link = Down(x);  link != x;  link = NextDown(link) )
      {	Child(y, link);
	if( !is_definite(type(y)) )
	{
	  if( type(y) == UNDER_REC )   /* generate an underline now */
	  { BackEnd->PrintUnderline(back(y, ROWM), fwd(y, ROWM), back(y, COLM),
	      fwd(y, COLM), pg - xmk);
	    link = PrevDown(link);     /* remove all trace of underlining */
	    DisposeChild(Up(y));       /* in case we print this object again */
	  }
	  continue;
	}
	y = FixAndPrintObject(y, xmk, xb, xf, dim, NO_SUPPRESS, pg, count,
	      &aback, &afwd);
      }
      *actual_back = xb;  *actual_fwd = xf;
      break;


    case COL_THR:
    case ROW_THR:

      assert( (type(x) == COL_THR) == (dim == COLM), "FixAndPrintObject: thr!" );
      for( link = Down(x), uplink = Up(x), i = 1;
	link != x && uplink != x && i < count;
	link = NextDown(link), uplink = NextUp(uplink), i++ );
      assert( link != x && uplink != x, "FixAndPrintObject: link or uplink!" );
      CountChild(y, link, count);
      debug7(DGP, DD, "  fapo of %s (%s,%s) child %d %s (%s,%s)",
	Image(type(x)),
	EchoLength(back(x, dim)), EchoLength(fwd(x, dim)),
	i, Image(type(y)), EchoLength(back(y, dim)), EchoLength(fwd(y, dim)));
      MoveLink(uplink, link, CHILD);  DeleteLink(link);
      assert( type(y) != GAP_OBJ, "FAPO: THR!");

      if( thr_state(x) != FINALSIZE )
      {	back(x, dim) = xb;  fwd(x, dim) = xf;
	thr_state(x) = FINALSIZE;
      }

      y = FixAndPrintObject(y, xmk, back(x, dim), fwd(x, dim), dim,
	NO_SUPPRESS, pg, count, &aback, &afwd);
      *actual_back = xb;  *actual_fwd = xf;
      /* if( Up(x) == x )  Dispose(x); */
      break;

      /* convert everyone to FIXED_COL_THR or FIXED_ROW_THR as appropriate */
      /* *** old code
      if( thr_state(x) == FINALSIZE )
	debug1(DGP, D, "thr_state(%d)", (int) x);
      assert(thr_state(x) != FINALSIZE, "FAPO/COL_THR: thr_state(x)!");
      ifdebug(DGP, D,
	link = Down(x);
	uplink = Up(x);
	while( link != x && uplink != x )
	{
	  Parent(tmp, uplink);
	  debug1(DGP, D, "parnt: %s", EchoObject(tmp));
	  Child(tmp, link);
	  debug1(DGP, D, "child: %s", EchoObject(tmp));
	  link = NextDown(link);
	  uplink = NextUp(uplink);
	}
	while( uplink != x )
	{ Parent(tmp, uplink);
	  debug1(DGP, D, "extra parnt: %s", EchoObject(tmp));
	  uplink = NextUp(uplink);
	}
	while( link != x )
	{ Child(tmp, link);
	  debug1(DGP, D, "extra child: %s", EchoObject(tmp));
	  link = NextDown(link);
	}
      )
      i = 1;  res = nilobj;
      while( Down(x) != x && Up(x) != x )
      {
	New(fixed_thr, type(x) == COL_THR ? FIXED_COL_THR : FIXED_ROW_THR);
	MoveLink(Up(x), fixed_thr, CHILD);
	MoveLink(Down(x), fixed_thr, PARENT);
	back(fixed_thr, dim) = xb;
	fwd(fixed_thr, dim) = xf;
	if( count == i )
	  res = fixed_thr;
	i++;
      }
      if( Up(x) != x || Down(x) != x )
      {
	debug2(DGP, D, "links problem at %s %d:", Image(type(x)), (int) x);
	if( Up(x) != x )
	{
	  Parent(tmp, Up(x));
	  debug1(DGP, D, "first parent is %s", EchoObject(tmp));
	}
	if( Down(x) != x )
	{
	  Child(tmp, Down(x));
	  debug1(DGP, D, "first child is %s", EchoObject(tmp));
	}
      }
      assert( Up(x) == x && Down(x) == x, "FAPO/COL_THR: x links!" );
      Dispose(x);
      assert(res != nilobj, "FixAndPrintObject: COL_THR res!");
      x = res;
      *** */
      /* NB NO BREAK! */


    /* ***
    case FIXED_COL_THR:
    case FIXED_ROW_THR:

      assert( (type(x) == FIXED_COL_THR) == (dim == COLM),
	"FixAndPrintObject: fixed_thr!" );
      CountChild(y, Down(x), count);
      y = FixAndPrintObject(y, xmk, back(x, dim), fwd(x, dim), dim,
	NO_SUPPRESS, pg, count, &aback, &afwd);
      *actual_back = back(x, dim);  *actual_fwd = fwd(x, dim);
      break;
      *** */


    case BEGIN_HEADER:
    case END_HEADER:
    case SET_HEADER:
    case CLEAR_HEADER:

      if( dim == COLM )
        Error(23, 8, "%s symbol ignored (out of place)", WARN, &fpos(x),
	  Image(type(x)));
      break;


    default:
    
      assert1(FALSE, "FixAndPrintObject:", Image(type(x)));
      break;


  } /* end switch */
  debug2(DGP, DD, "] FixAndPrintObject returning (actual %s,%s).",
	EchoLength(*actual_back), EchoLength(*actual_fwd));
  return res;
} /* end FixAndPrintObject */
