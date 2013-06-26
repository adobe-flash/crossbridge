/*@z11.c:Style Service:EchoStyle()@*******************************************/
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
/*  FILE:         z11.c                                                      */
/*  MODULE:       Style Service                                              */
/*  EXTERNS:      EchoStyle(), SpaceChange(), BreakChange()                  */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


#if DEBUG_ON
/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *EchoStyle(style)                                              */
/*                                                                           */
/*  Returns a string showing the value of the style.                         */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *EchoStyle(STYLE *style)
{ static FULL_CHAR res[100];
  static char *hyphwords[] = { "hyph_undef", "hyph_off", "hyph_on" };
  static char *fillwords[] = { "fill_undef", "fill_off", "fill_on" };
  static char *spacewords[] = { "lout", "comp", "troff", "tex" };
  static char *displaywords[] = { "undef", "adjust", "outdent", "oragged",
				  "left", "centre", "right", "do" };

  StringCopy(res, AsciiToFull("["));
  StringCat(res, EchoCatOp(VCAT,mark(line_gap(*style)),join(line_gap(*style))));
  StringCat(res, EchoGap(&line_gap(*style)));
  StringCat(res, AsciiToFull(", "));
  StringCat(res, font(*style) == 0 ?
		   AsciiToFull("nofont") : FontFamilyAndFace(font(*style)));
  StringCat(res, AsciiToFull(" ("));
  StringCat(res, AsciiToFull(spacewords[space_style(*style)]));
  StringCat(res, AsciiToFull(" "));
  StringCat(res, EchoGap(&space_gap(*style)));
  StringCat(res, AsciiToFull("), "));
  StringCat(res, AsciiToFull(hyph_style(*style) < 3 ?
		    hyphwords[hyph_style(*style)] : "?"));
  StringCat(res, AsciiToFull(":"));
  StringCat(res, AsciiToFull(fill_style(*style) < 3 ?
		    fillwords[fill_style(*style)] : "?"));
  StringCat(res, AsciiToFull(":"));
  StringCat(res, AsciiToFull(display_style(*style) < 7 ?
		    displaywords[display_style(*style)] : "?"));
  if( small_caps(*style) > 0 ) StringCat(res, AsciiToFull(":smallcaps"));
  if( vadjust(*style) ) StringCat(res, AsciiToFull(":vadjust"));
  if( hadjust(*style) ) StringCat(res, AsciiToFull(":hadjust"));
  if( padjust(*style) ) StringCat(res, AsciiToFull(":padjust"));
  if( yunit(*style) != 0 )
  { StringCat(res, AsciiToFull(":y="));
    StringCat(res, EchoLength(yunit(*style)));
  }
  if( zunit(*style) != 0 )
  { StringCat(res, AsciiToFull(":z="));
    StringCat(res, EchoLength(zunit(*style)));
  }
  if( nobreakfirst(*style) ) StringCat(res, AsciiToFull(":NBF"));
  if( nobreaklast(*style) ) StringCat(res, AsciiToFull(":NBL"));
  StringCat(res, AsciiToFull("]"));
  return res;
} /* end EchoStyle */
#endif


/*@::SpaceChange()@***********************************************************/
/*                                                                           */
/*  SpaceChange(style, x)                                                    */
/*                                                                           */
/*  Change the current break style as indicated by object x.                 */
/*                                                                           */
/*****************************************************************************/

static void changespace(STYLE *style, OBJECT x)
{ GAP res_gap;  unsigned gap_inc;
  assert( is_word(type(x)), "changespace: type(x)!" );
  if( beginsbreakstyle(string(x)[0]) )
  {
    /* should be a new space style option */
    if( StringEqual(string(x), STR_SPACE_LOUT) )
	space_style(*style) = SPACE_LOUT;
    else if( StringEqual(string(x), STR_SPACE_COMPRESS) )
	space_style(*style) = SPACE_COMPRESS;
    else if( StringEqual(string(x), STR_SPACE_SEPARATE) )
	space_style(*style) = SPACE_SEPARATE;
    else if( StringEqual(string(x), STR_SPACE_TROFF) )
	space_style(*style) = SPACE_TROFF;
    else if( StringEqual(string(x), STR_SPACE_TEX) )
	space_style(*style) = SPACE_TEX;
    else Error(11, 1, "unknown option to %s symbol (%s)",
	   WARN, &fpos(x), KW_SPACE, string(x));
  }
  else /* should be a new space gap */
  { GetGap(x, style, &res_gap, &gap_inc);
    if( gap_inc != GAP_ABS && units(res_gap) != units(space_gap(*style)) )
    { Error(11, 2, "spacing %s is not compatible with current spacing",
	WARN, &fpos(x), string(x));
    }
    else
    { units(space_gap(*style)) = units(res_gap);
      mode(space_gap(*style))  = mode(res_gap);
      width(space_gap(*style)) = gap_inc == GAP_ABS ? width(res_gap) :
	     gap_inc == GAP_INC ? width(space_gap(*style)) + width(res_gap) :
	     find_max(width(space_gap(*style)) - width(res_gap), 0);
    }
  }
  debug1(DSS, D, "SpaceChange returning %s", EchoStyle(style));
} /* end SpaceChange */


void SpaceChange(STYLE *style, OBJECT x)
{ OBJECT link, y;
  debug2(DSS, D, "SpaceChange(%s, %s)", EchoStyle(style), EchoObject(x));
  switch( type(x) )
  {
    case NULL_CLOS: break;

    case WORD:
    case QWORD:	if( !StringEqual(string(x), STR_EMPTY) )
		  changespace(style, x);
		break;


    case ACAT:	for( link = Down(x);  link != x;  link = NextDown(link) )
		{ Child(y, link);
		  if( type(y) == GAP_OBJ || type(y) == NULL_CLOS )  continue;
		  else if( is_word(type(y)) )
		  { if( !StringEqual(string(y), STR_EMPTY) )
		      changespace(style, y);
		  }
		  else Error(11, 3, "invalid left parameter of %s",
			 WARN, &fpos(x), KW_SPACE);
		}
		break;


    default:	Error(11, 4, "invalid left parameter of %s",
		  WARN, &fpos(x), KW_SPACE);
		break;
  }
  debug1(DSS, D, "SpaceChange returning %s", EchoStyle(style));
} /* end SpaceChange */


/*@::BreakChange()@***********************************************************/
/*                                                                           */
/*  BreakChange(style, x)                                                    */
/*                                                                           */
/*  Change the current break style as indicated by object x.                 */
/*                                                                           */
/*****************************************************************************/

static void changebreak(STYLE *style, OBJECT x)
{ GAP res_gap;  unsigned gap_inc;
  if( beginsbreakstyle(string(x)[0]) )
  {
    /* should be a new break style option */
    if( StringEqual(string(x), STR_BREAK_HYPHEN) )
	hyph_style(*style) = HYPH_ON;
    else if( StringEqual(string(x), STR_BREAK_NOHYPHEN) )
	hyph_style(*style) = HYPH_OFF;
    else if( StringEqual(string(x), STR_BREAK_ADJUST) )
	fill_style(*style) = FILL_ON, display_style(*style) = DISPLAY_ADJUST;
    else if( StringEqual(string(x), STR_BREAK_OUTDENT) )
	fill_style(*style) = FILL_ON, display_style(*style) = DISPLAY_OUTDENT;
    else if( StringEqual(string(x), STR_BREAK_RAGGED) )
	fill_style(*style) = FILL_ON, display_style(*style) = DISPLAY_LEFT;
    else if( StringEqual(string(x), STR_BREAK_CRAGGED) )
	fill_style(*style) = FILL_ON, display_style(*style) = DISPLAY_CENTRE;
    else if( StringEqual(string(x), STR_BREAK_RRAGGED) )
	fill_style(*style) = FILL_ON, display_style(*style) = DISPLAY_RIGHT;
    else if( StringEqual(string(x), STR_BREAK_ORAGGED) )
	fill_style(*style) = FILL_ON, display_style(*style) = DISPLAY_ORAGGED;
    else if( StringEqual(string(x), STR_BREAK_LINES) )
	fill_style(*style) = FILL_OFF, display_style(*style) = DISPLAY_LEFT;
    else if( StringEqual(string(x), STR_BREAK_CLINES) )
	fill_style(*style) = FILL_OFF, display_style(*style) = DISPLAY_CENTRE;
    else if( StringEqual(string(x), STR_BREAK_RLINES) )
	fill_style(*style) = FILL_OFF, display_style(*style) = DISPLAY_RIGHT;
    else if( StringEqual(string(x), STR_BREAK_NOFIRST) )
	nobreakfirst(*style) = TRUE;
    else if( StringEqual(string(x), STR_BREAK_FIRST) )
	nobreakfirst(*style) = FALSE;
    else if( StringEqual(string(x), STR_BREAK_NOLAST) )
	nobreaklast(*style) = TRUE;
    else if( StringEqual(string(x), STR_BREAK_LAST) )
	nobreaklast(*style) = FALSE;
    else Error(11, 5, "unknown option to %s symbol (%s)",
	   WARN, &fpos(x), KW_BREAK, string(x));
  }
  else /* should be a new inter-line gap */
  { GetGap(x, style, &res_gap, &gap_inc);
    if( gap_inc != GAP_ABS && units(res_gap) != units(line_gap(*style)) )
      Error(11, 6, "line spacing %s is not compatible with current spacing",
        WARN, &fpos(x), string(x));
    else
    { units(line_gap(*style)) = units(res_gap);
      mode(line_gap(*style))  = mode(res_gap);
      width(line_gap(*style)) = gap_inc == GAP_ABS ? width(res_gap) :
	gap_inc == GAP_INC ? width(line_gap(*style)) + width(res_gap) :
	find_max(width(line_gap(*style)) - width(res_gap), 0);
    }
  }
} /* end changebreak */

void BreakChange(STYLE *style, OBJECT x)
{ OBJECT link, y;
  debug3(DSS, D, "BreakChange(%s, %s at %s)", EchoStyle(style),
    EchoObject(x), EchoFilePos(&fpos(x)));
  switch( type(x) )
  {
    case NULL_CLOS: break;

    case WORD:
    case QWORD:	if( !StringEqual(string(x), STR_EMPTY) )
		  changebreak(style, x);
		break;


    case ACAT:	for( link = Down(x);  link != x;  link = NextDown(link) )
		{ Child(y, link);
		  if( type(y) == GAP_OBJ || type(y) == NULL_CLOS )  continue;
		  else if( is_word(type(y)) )
		  { if( !StringEqual(string(y), STR_EMPTY) )
		      changebreak(style, y);
		  }
		  else Error(11, 7, "invalid left parameter of %s",
			 WARN, &fpos(x), KW_BREAK);
		}
		break;


    default:	Error(11, 8, "invalid left parameter of %s",
		  WARN, &fpos(x), KW_BREAK);
		break;
  }
  debug1(DSS, D, "BreakChange returning %s", EchoStyle(style));
} /* end BreakChange */


/*@::YUnitChange(), ZUnitChange()@********************************************/
/*                                                                           */
/*  YUnitChange(style, x)                                                    */
/*                                                                           */
/*  Change the current value of the y unit as indicated by object x.         */
/*                                                                           */
/*****************************************************************************/

void YUnitChange(STYLE *style, OBJECT x)
{ GAP res_gap; unsigned gap_inc;
  GetGap(x, style, &res_gap, &gap_inc);
  if( units(res_gap) != FIXED_UNIT )
    Error(11, 9, "this unit not allowed with %s symbol",
      WARN, &fpos(x), KW_YUNIT);
  else
  { if( gap_inc == GAP_ABS ) yunit(*style) = width(res_gap);
    else if( gap_inc == GAP_INC ) yunit(*style) += width(res_gap);
    else yunit(*style) = find_max(yunit(*style) - width(res_gap), 0);
  }
} /* end YUnitChange */


/*****************************************************************************/
/*                                                                           */
/*  ZUnitChange(style, x)                                                    */
/*                                                                           */
/*  Change the current value of the z unit as indicated by object x.         */
/*                                                                           */
/*****************************************************************************/

void ZUnitChange(STYLE *style, OBJECT x)
{ GAP res_gap; unsigned gap_inc;
  GetGap(x, style, &res_gap, &gap_inc);
  if( units(res_gap) != FIXED_UNIT )
    Error(11, 10, "this unit not allowed with %s symbol",
      WARN, &fpos(x), KW_ZUNIT);
  else
  { if( gap_inc == GAP_ABS ) zunit(*style) = width(res_gap);
    else if( gap_inc == GAP_INC ) zunit(*style) += width(res_gap);
    else zunit(*style) = find_max(zunit(*style) - width(res_gap), 0);
  }
} /* end ZUnitChange */
