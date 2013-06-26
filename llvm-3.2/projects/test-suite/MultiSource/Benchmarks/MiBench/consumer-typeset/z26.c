/*@z26.c:Echo Service:BeginString()@******************************************/
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
/*  FILE:         z26.c                                                      */
/*  MODULE:       Echo Service                                               */
/*  EXTERNS:      BeginString(), AppendString(), EndString(),                */
/*                EchoLength(), Image()                                      */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

#if DEBUG_ON
#define	MULTI	  7			/* max no of simultaneous calls      */

static	FULL_CHAR buff[MULTI][2*MAX_BUFF];/* buffers for strings       */
static	int	curr = 1;		/* current buffer in use             */
static	int	bp;			/* next free space in buff[curr]     */
static	BOOLEAN	instring = FALSE;	/* TRUE while making a string        */


/*****************************************************************************/
/*                                                                           */
/*  BeginString()                                                            */
/*                                                                           */
/*  Locate a clear buffer into which debug output may be accumulated.        */
/*                                                                           */
/*****************************************************************************/

void BeginString(void)
{ assert(!instring, "BeginString: currently in string");
  instring = TRUE;  curr = (curr + 1) % MULTI;
  assert( 0 <= curr && curr < MULTI, "BeginString: curr!" );
  StringCopy(buff[curr], "");  bp = 0;
}


/*@::AppendString(), EndString(), EchoLength()@*******************************/
/*                                                                           */
/*  AppendString(str, p1, p2, p3, p4, p5, p6)                                */
/*                                                                           */
/*  Sprintf str to the current buffer, if space is available there.          */
/*                                                                           */
/*****************************************************************************/

void AppendString(FULL_CHAR *str)
{ int len;
  assert(instring, "AppendString: no current string");
  assert( 0 <= curr && curr < MULTI, "BeginString: curr!" );
  if( bp == MAX_BUFF ) return;		/* no space, do nothing */

  len = StringLength(str);
  if( len + bp >= MAX_BUFF )
  { StringCopy( &buff[curr][MAX_BUFF/2], AsciiToFull(" ... <too long>") );
    bp = MAX_BUFF;
  }
  else
  { StringCopy(&buff[curr][bp], str);
    while( buff[curr][bp] != '\0' )  bp++;
    if( bp >= MAX_BUFF )  Error(26, 1, "AppendString abort", INTERN, no_fpos);
  }
} /* end AppendString */


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *EndString()                                                   */
/*                                                                           */
/*  Return the string constructed by previous AppendString operations.       */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *EndString(void)
{ assert(instring, "EndString: no string");
  assert( 0 <= curr && curr < MULTI, "BeginString: curr!" );
  instring = FALSE;
  return buff[curr];
} /* end Endstring */
#endif

/*****************************************************************************/
/*                                                                           */
/*  SetLengthDim(int dim)                                                    */
/*                                                                           */
/*  Set dimension for echoing lengths.                                       */
/*                                                                           */
/*****************************************************************************/

static int length_dim = COLM;

void SetLengthDim(int dim)
{
  length_dim = dim;
}

/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *EchoLength(len)                                               */
/*                                                                           */
/*  Echo a length.                                                           */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *EchoLength(int len)
{ static FULL_CHAR buff[8][20];
  static int i = 0;
  i = (i + 1) % 8;
  if( len == MAX_FULL_LENGTH )
    sprintf( (char *) buff[i], "%s", "INF");
  else
    BackEnd->PrintLength(buff[i], len, ROWM);
  return buff[i];
} /* end EchoLength */


/*@::Image()@*****************************************************************/
/*                                                                           */
/*  FULL_CHAR *Image(c)                                                      */
/*                                                                           */
/*  Returns the string value of type c.                                      */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *Image(unsigned int c)
{ static FULL_CHAR b[20];
  switch(c)
  {

    case LINK:			return  AsciiToFull("link");

    case SPLIT:			return  AsciiToFull("split");
    case HEAD:			return  AsciiToFull("head");
    case PAR:			return  AsciiToFull("par");
    case WORD:			return  AsciiToFull("word");
    case QWORD:			return  AsciiToFull("qword");
    case GAP_OBJ:		return  AsciiToFull("gap_obj");
    case ROW_THR:		return  AsciiToFull("row_thr");
    case COL_THR:		return  AsciiToFull("col_thr");
    case CLOSURE:		return  AsciiToFull("closure");
    case NULL_CLOS:		return  KW_NULL;
    case PAGE_LABEL:		return  KW_PAGE_LABEL;
    case CROSS:			return  KW_CROSS;
    case FORCE_CROSS:		return  KW_FORCE_CROSS;
    case BEGIN_HEADER:		return  KW_BEGIN_HEADER;
    case END_HEADER:		return  KW_END_HEADER;
    case SET_HEADER:		return  KW_SET_HEADER;
    case CLEAR_HEADER:		return  KW_CLEAR_HEADER;
    case ONE_COL:		return  KW_ONE_COL;
    case ONE_ROW:		return  KW_ONE_ROW;
    case WIDE:			return  KW_WIDE;
    case HIGH:			return  KW_HIGH;
    case HSHIFT:		return  KW_HSHIFT;
    case VSHIFT:		return  KW_VSHIFT;
    case HSCALE:		return  KW_HSCALE;
    case VSCALE:		return  KW_VSCALE;
    case HCOVER:		return  KW_HCOVER;
    case VCOVER:		return  KW_VCOVER;
    case HCONTRACT:		return  KW_HCONTRACT;
    case VCONTRACT:		return  KW_VCONTRACT;
    case HLIMITED:		return  KW_HLIMITED;
    case VLIMITED:		return  KW_VLIMITED;
    case HEXPAND:		return  KW_HEXPAND;
    case VEXPAND:		return  KW_VEXPAND;
    case START_HVSPAN:		return  KW_STARTHVSPAN;
    case START_HSPAN:		return  KW_STARTHSPAN;
    case START_VSPAN:		return  KW_STARTVSPAN;
    case HSPAN:			return  KW_HSPAN;
    case VSPAN:			return  KW_VSPAN;
    case HSPANNER:		return  AsciiToFull("hspannner");
    case VSPANNER:		return  AsciiToFull("vspannner");
    case PADJUST:		return  KW_PADJUST;
    case HADJUST:		return  KW_HADJUST;
    case VADJUST:		return  KW_VADJUST;
    case ROTATE:		return  KW_ROTATE;
    case BACKGROUND:		return	KW_BACKGROUND;
    case SCALE:			return  KW_SCALE;
    case KERN_SHRINK:		return  KW_KERN_SHRINK;
    case RAW_VERBATIM:		return  KW_RAWVERBATIM;
    case VERBATIM:		return  KW_VERBATIM;
    case CASE:			return  KW_CASE;
    case YIELD:			return  KW_YIELD;
    case BACKEND:		return  KW_BACKEND;
    case FILTERED:		return  AsciiToFull("filtered");
    case XCHAR:			return  KW_XCHAR;
    case FONT:			return  KW_FONT;
    case SPACE:			return  KW_SPACE;
    case YUNIT:			return  KW_YUNIT;
    case ZUNIT:			return  KW_ZUNIT;
    case BREAK:			return  KW_BREAK;
    case UNDERLINE:		return  KW_UNDERLINE;
    case COLOUR:		return  KW_COLOUR;
    case OUTLINE:		return  KW_OUTLINE;
    case LANGUAGE:		return  KW_LANGUAGE;
    case CURR_LANG:		return  KW_CURR_LANG;
    case CURR_FAMILY:		return  KW_CURR_FAMILY;
    case CURR_FACE:		return  KW_CURR_FACE;
    case CURR_YUNIT:		return  KW_CURR_YUNIT;
    case CURR_ZUNIT:		return  KW_CURR_ZUNIT;
    case COMMON:		return  KW_COMMON;
    case RUMP:			return  KW_RUMP;
    case MELD:			return  KW_MELD;
    case INSERT:		return  KW_INSERT;
    case ONE_OF:		return  KW_ONE_OF;
    case NEXT:			return  KW_NEXT;
    case PLUS:			return  KW_PLUS;
    case MINUS:			return  KW_MINUS;
    case ENV_OBJ:		return  AsciiToFull("env_obj");
    case ENV:			return  KW_ENV;
    case ENVA:			return  KW_ENVA;
    case ENVB:			return  KW_ENVB;
    case ENVC:			return  KW_ENVC;
    case ENVD:			return  KW_ENVD;
    case CENV:			return  KW_CENV;
    case CLOS:			return  KW_CLOS;
    case LVIS:			return  KW_LVIS;
    case LUSE:			return  KW_LUSE;
    case LEO:			return  KW_LEO;
    case OPEN:			return  KW_OPEN;
    case TAGGED:		return  KW_TAGGED;
    case INCGRAPHIC:		return  KW_INCGRAPHIC;
    case SINCGRAPHIC:		return  KW_SINCGRAPHIC;
    case PLAIN_GRAPHIC:		return  KW_PLAINGRAPHIC;
    case GRAPHIC:		return  KW_GRAPHIC;
    case LINK_SOURCE:		return  KW_LINK_SOURCE;
    case LINK_DEST:		return  KW_LINK_DEST;
    case ACAT:			return  AsciiToFull("acat");
    case HCAT:			return  AsciiToFull("hcat");
    case VCAT:			return  AsciiToFull("vcat");

    case TSPACE:		return  AsciiToFull("tspace");
    case TJUXTA:		return  AsciiToFull("tjuxta");
    case LBR:			return  AsciiToFull("lbr");
    case RBR:			return  AsciiToFull("rbr");
    case UNEXPECTED_EOF:	return  AsciiToFull("unexpected_eof");
    case BEGIN:			return  KW_BEGIN;
    case END:			return  KW_END;
    case USE:			return  KW_USE;
    case NOT_REVEALED:		return  KW_NOT_REVEALED;
    case GSTUB_NONE:		return  AsciiToFull("gstub_none");
    case GSTUB_INT:		return  AsciiToFull("gstub_int");
    case GSTUB_EXT:		return  AsciiToFull("gstub_ext");
    case INCLUDE:		return  KW_INCLUDE;
    case SYS_INCLUDE:		return  KW_SYSINCLUDE;
    case PREPEND:		return  KW_PREPEND;
    case SYS_PREPEND:		return  KW_SYSPREPEND;
    case DATABASE:		return  KW_DATABASE;
    case SYS_DATABASE:		return  KW_SYSDATABASE;
    /* case START: 		return  AsciiToFull("start"); unused */

    case DEAD:			return  AsciiToFull("dead");
    case UNATTACHED:		return  AsciiToFull("unattached");
    case RECEPTIVE:		return  AsciiToFull("receptive");
    case RECEIVING:		return  AsciiToFull("receiving");
    case RECURSIVE:		return  AsciiToFull("recursive");
    case PRECEDES:		return  AsciiToFull("precedes");
    case FOLLOWS:		return  AsciiToFull("follows");
    case CROSS_LIT:		return  AsciiToFull("cross_lit");
    case CROSS_FOLL:		return  AsciiToFull("cross_foll");
    case CROSS_FOLL_OR_PREC:	return  AsciiToFull("cross_foll_or_prec");
    case GALL_FOLL:		return  AsciiToFull("gall_foll");
    case GALL_FOLL_OR_PREC:	return  AsciiToFull("gall_foll_or_prec");
    case CROSS_TARG:		return  AsciiToFull("cross_targ");
    case GALL_TARG:		return  AsciiToFull("gall_targ");
    case GALL_PREC:		return  AsciiToFull("gall_prec");
    case CROSS_PREC:		return  AsciiToFull("cross_prec");
    case PAGE_LABEL_IND:	return  AsciiToFull("page_label_ind");
    case SCALE_IND:		return  AsciiToFull("scale_ind");
    case COVER_IND:		return  AsciiToFull("cover_ind");
    case EXPAND_IND:		return  AsciiToFull("expand_ind");
    case THREAD:		return  AsciiToFull("thread");
    case CROSS_SYM:		return  AsciiToFull("cross_sym");
    case CR_ROOT:		return  AsciiToFull("cr_root");
    case MACRO:			return  KW_MACRO;
    case LOCAL:			return  AsciiToFull("local");
    case LPAR:			return  AsciiToFull("lpar");
    case NPAR:			return  AsciiToFull("npar");
    case RPAR:			return  AsciiToFull("rpar");
    case CR_LIST:		return  AsciiToFull("cr_list");
    case EXT_GALL:		return  AsciiToFull("ext_gall");
    case DISPOSED:		return  AsciiToFull("disposed");

    case BACK:			return  AsciiToFull("back");
    case ON:			return  AsciiToFull("on");
    case FWD:			return  AsciiToFull("fwd");

    case PROMOTE:		return  AsciiToFull("promote");
    case CLOSE:			return  AsciiToFull("close");
    case BLOCK:			return  AsciiToFull("block");
    case CLEAR:			return  AsciiToFull("clear");

    case GAP_ABS:		return  AsciiToFull("abs");
    case GAP_INC:		return  AsciiToFull("inc");
    case GAP_DEC:		return  AsciiToFull("dec");

    default:			sprintf( (char *) b, "?? (%d)", c);
				return b;
  } /* end switch */
} /* end Image */
