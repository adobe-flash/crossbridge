/*@z32.c:Counter Service:Next()@**********************************************/
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
/*  FILE:         z32.c                                                      */
/*  MODULE:       Counter Service                                            */
/*  EXTERNS:      Next()                                                     */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

/*****************************************************************************/
/*                                                                           */
/*  OBJECT Next(x, inc, done)                                                */
/*                                                                           */
/*  Return x with its value incremented by inc (if possible).                */
/*  Set *done to TRUE if successful, leave *done unchanged otherwise.        */
/*                                                                           */
/*****************************************************************************/

OBJECT Next(OBJECT x, int inc, BOOLEAN *done)
{ OBJECT y, link;  int l, r, n, len;
  FULL_CHAR buff[MAX_BUFF];
  debug3(DCS, DD, "Next( %s, %d, %s )", EchoObject(x), inc, bool(*done));
  switch( type(x) )
  {
    case WORD:
    case QWORD:
    
      len = StringLength(string(x));
      for( r = len - 1;  r >= 0 && !decimaldigit(string(x)[r]);  r--);
      if( r < 0 ) break;
      for( l = r-1;  l >= 0 && decimaldigit(string(x)[l]);  l-- );
      sscanf( (char *) &string(x)[l+1], "%d", &n);
      string(x)[l+1] = '\0';
      StringCopy(buff, string(x));
      StringCat(buff, StringInt(n+inc));
      StringCat(buff, &string(x)[r+1]);
      if( StringLength(buff) >= MAX_BUFF )
	Error(32, 1, "word %s is too long", FATAL, &fpos(x), buff);
      y = MakeWord(type(x), buff, &fpos(x));
      word_font(y) = word_font(x);
      word_colour(y) = word_colour(x);
      word_outline(y) = word_outline(x);
      word_language(y) = word_language(x);
      word_hyph(y) = word_hyph(x);
      underline(y) = underline(x);
      MergeNode(y, x);  x = y;
      *done = TRUE;
      break;


    case INCGRAPHIC:
    case SINCGRAPHIC:
    case GAP_OBJ:
    case CLOSURE:
    case NULL_CLOS:
    case PAGE_LABEL:
    case CROSS:
    case FORCE_CROSS:
    case START_HVSPAN:
    case START_HSPAN:
    case START_VSPAN:
    case HSPAN:
    case VSPAN:
    case END_HEADER:
    case CLEAR_HEADER:
    
      break;


    case BEGIN_HEADER:
    case SET_HEADER:
    case ONE_COL:
    case ONE_ROW:
    case WIDE:
    case HIGH:
    case HSHIFT:
    case VSHIFT:
    case HCONTRACT:
    case VCONTRACT:
    case HLIMITED:
    case VLIMITED:
    case HEXPAND:
    case VEXPAND:
    case PADJUST:
    case HADJUST:
    case VADJUST:
    case HSCALE:
    case VSCALE:
    case HCOVER:
    case VCOVER:
    case ROTATE:
    case BACKGROUND:
    case SCALE:
    case KERN_SHRINK:
    case SPLIT:
    case PLAIN_GRAPHIC:
    case GRAPHIC:
    case LINK_SOURCE:
    case LINK_DEST:
    
      Child(y, LastDown(x));
      y = Next(y, inc, done);
      break;


    case ACAT:
    
      /* *** seems identical!
      link = LastDown(x);
      while( link != x && !*done )
      {	Child(y, link);
	if( is_index(type(y)) )  continue;
	y = Next(y, inc, done);
	if( !*done )  link = PrevDown(link);
      }
      break;
      *** */


    case COL_THR:
    case ROW_THR:
    case HCAT:
    case VCAT:
    
      link = LastDown(x);
      while( link != x && !*done )
      {	Child(y, link);
	if( is_index(type(y)) )  continue;
	y = Next(y, inc, done);
	if( !*done )  link = PrevDown(link);
      }
      break;


    default:
    
      assert1(FALSE, "Next:", Image(type(x)));
      break;

  } /* end switch */
  debug1(DCS, DD, "Next returning %s", EchoObject(x));
  return x;
} /* end Next */
