/*@z14.c:Fill Service:Declarations@*******************************************/
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
/*  FILE:         z14.c                                                      */
/*  MODULE:       Fill Service                                               */
/*  EXTERNS:      FillObject()                                               */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define TOO_TIGHT_BAD	1048576	/* 2^21; badness of a too tight line         */
#define TOO_LOOSE_BAD	  65536	/* 2^16; the max badness of a too loose line */
#define	TIGHT_BAD	   4096	/* 2^12; the max badness of a tight line     */
#define	LOOSE_BAD	   4096	/* 2^12; the max badness of a loose line     */
#define	HYPH_BAD	    128	/* 2^ 7; threshold for calling hyphenation   */
#define	HYPH_BAD_INCR	     16	/* 2 ^4: the badness of one hyphen           */
#define	WIDOW_BAD_INCR	    128	/* 2 ^7: the badness of one widow word       */
#define SQRT_TOO_LOOSE	    512	/* 2^ 9; sqrt(TOO_LOOSE_BAD) (used to be)    */
#define	SQRT_TIGHT_BAD	    128	/* 2^ 7; sqrt(TIGHT_BAD) (used to be)        */
#define	SQRT_LOOSE_BAD	    128	/* 2^ 7; sqrt(LOOSE_BAD) (used to be)        */
#define	SQRT_TOO_TIGHT	   8192	/* 2^13; sqrt(TOO_TIGHT_BAD) (used to be)    */
#define MAX_EXPAND	1
#define MAX_SHRINK	4


typedef struct {
  OBJECT	llink;		/* link to gap before left end of interval   */
  OBJECT	rlink;		/* link to gap after right end of interval   */
  OBJECT	cwid;		/* link to current line width in multi case  */
  int		nat_width;	/* natural width of interval                 */
  int		space_width;	/* natural width of spaces in the interval   */
  int		badness;	/* badness of this interval		     */
  unsigned char	class;		/* badness class of this interval	     */
  unsigned char	tab_count;	/* number of gaps with tab mode in interval  */
  int		tab_pos;	/* if tab_count > 0, this holds the position */
				/*  of the left edge of the object following */
				/*  the rightmost tab gap in the interval    */
  int		width_to_tab;	/* if tab_count > 0, the interval width up   */
				/*  to but not including the rightmost tab   */
} INTERVAL;


/*****************************************************************************/
/*                                                                           */
/*  Badness classes                                                          */
/*                                                                           */
/*****************************************************************************/

#define TOO_LOOSE	  0	/* interval is too loose		     */
#define LOOSE		  1	/* interval is loose but not too loose	     */
#define TIGHT		  2	/* interval is tight but not too tight	     */
#define TOO_TIGHT	  3	/* interval is too tight 		     */
#define TAB_OVERLAP	  4	/* interval has a tab and left part overlaps */
#define AT_END		  5	/* interval ends at right end of paragraph   */
#define UNBREAKABLE_LEFT  6	/* interval has an unbreakable gap at left   */
#define UNBREAKABLE_RIGHT 7	/* interval has an unbreakable gap at right  */
#define EMPTY_INTERVAL	  8	/* interval is empty                         */

/*@::SetIntervalBadness()@****************************************************/
/*                                                                           */
/*  SetIntervalBadness(I)                                                    */
/*                                                                           */
/*  Private, calculates the badness and badness class of a non-empty         */
/*  interval.  Does not take into account any unbreakable gap at either end. */
/*                                                                           */
/*****************************************************************************/

#define SetIntervalBadness(I, max_width, etc_width)			\
{ OBJECT g; int badness;						\
  int col_width;							\
									\
  /* initialize to  saved badness of left-adjoining interval, if any */	\
  /* and set width of column                                         */	\
  if( I.llink == x )							\
  { col_width = (I.cwid!=nilobj) ? bfc(constraint(I.cwid)) : max_width;	\
    I.badness = 0;							\
  }									\
  else									\
  { col_width = (I.cwid!=nilobj) ? bfc(constraint(I.cwid)) : etc_width;	\
    Child(g, I.llink);							\
    I.badness = save_badness(g);					\
  }									\
									\
  /* penalize widow lines, of the form [ <object> &1rt ... ] */		\
  if( I.tab_count > 0 )							\
  { OBJECT glink = NextDown(NextDown(I.llink));				\
    assert( type(glink) == LINK, "SIB: glink!");			\
    Child(g, glink);							\
    if( type(g) == GAP_OBJ && mode(gap(g)) == TAB_MODE &&		\
	units(gap(g)) == AVAIL_UNIT && width(gap(g)) == 1*FR )		\
      I.badness += WIDOW_BAD_INCR;					\
  }									\
									\
  if( col_width <= 0 )							\
  { if( I.nat_width == 0 )						\
    { I.class = TOO_LOOSE;						\
      I.badness += 0;							\
    }									\
    else								\
    { I.class = TIGHT;							\
      I.badness += TOO_TIGHT_BAD;					\
    }									\
  }									\
  else if( I.tab_count > 0 && I.width_to_tab > I.tab_pos )		\
  { I.class = TAB_OVERLAP;						\
    I.badness += TOO_TIGHT_BAD;						\
  }									\
  else if( MAX_EXPAND*(col_width-I.nat_width) > 2*I.space_width )	\
  { I.class = I.tab_count > 0 ? LOOSE : TOO_LOOSE;			\
    badness = (SQRT_TOO_LOOSE*(col_width - I.nat_width)) / col_width;	\
    I.badness += badness * badness;					\
  }									\
  else if( I.nat_width <= col_width )					\
  { I.class = LOOSE;							\
    badness = (SQRT_LOOSE_BAD*(col_width - I.nat_width)) / col_width;	\
    I.badness += badness * badness;					\
  }									\
  else if( BackEnd->fractional_spacing_avail && allow_shrink &&		\
    MAX_SHRINK*(I.nat_width-col_width) <= I.space_width )		\
  { I.class = TIGHT;							\
    badness = (SQRT_TIGHT_BAD*(col_width - I.nat_width)) / col_width;	\
    I.badness += badness * badness;					\
  }									\
  else									\
  { I.class = TOO_TIGHT;						\
    /***								\
    badness = (SQRT_TOO_TIGHT*(col_width-I.nat_width)) / col_width;	\
    I.badness += badness * badness;					\
    ***/								\
    I.badness += TOO_TIGHT_BAD;						\
  }									\
  assert( I.badness >= 0, "SetIntervalBadness: badness < 0!" );		\
} /* end macro SetIntervalBadness */


/*@::MoveRightToGap()@********************************************************/
/*                                                                           */
/*  MoveRightToGap(I, x, rlink, right, max_width, etc_width, hyph_word)      */
/*                                                                           */
/*  Private.  Shared by IntervalInit and IntervalShiftRightEnd, for moving   */
/*  to the next gap to the right, setting save_space(newg), checking for     */
/*  hyphenation case, and setting the interval badness.                      */
/*                                                                           */
/*****************************************************************************/

#define MoveRightToGap(I,x,rlink,right,max_width,etc_width,hyph_word)	\
{ OBJECT newg, foll, tmp;						\
  BOOLEAN jn, unbreakable_at_right = FALSE;				\
  debug0(DOF, DDD, "MoveRightToGap(I, x, rlink, right, -, -, -)");	\
									\
  /* search onwards to find newg, the next true breakpoint */		\
  Child(tmp, rlink);							\
  debug2(DOF, DDD, "NextDefiniteWithGap(%s, %s)", EchoObject(x),	\
    EchoObject(tmp));							\
  NextDefiniteWithGap(x, rlink, foll, newg, jn);			\
									\
  /* set right link and calculate badness of the new interval */	\
  if( rlink != x )							\
  { 									\
    assert( Up(newg) == LastUp(newg), "MoveRightToGap: newg!" );	\
    /* set save_space(newg) now so that it is OK to forget right */	\
    debug0(DOF, DDD, "  MoveRightToGap setting save_space(newg)");	\
    if( I.cwid != nilobj )  etc_width = bfc(constraint(I.cwid));	\
    if( mode(gap(newg)) == TAB_MODE )					\
    { save_space(newg) = ActualGap(0, back(foll,COLM), fwd(foll,COLM),	\
	  &gap(newg), etc_width, 0) - back(foll, COLM);			\
    }									\
    else								\
    { save_space(newg) = ActualGap(fwd(right, COLM), back(foll, COLM),	\
	  fwd(foll,COLM), &gap(newg), etc_width,			\
	  I.nat_width - fwd(right,COLM))				\
	  - back(foll, COLM) - fwd(right, COLM);			\
    }									\
									\
    ifdebug(DOF, DDD,							\
      if( Down(newg) != newg )						\
      { OBJECT tmp;							\
	Child(tmp, Down(newg));						\
	debug5(DOF, DDD, "newg %s: %s %s, gap = %s, save_space = %s",	\
	Image(type(newg)), Image(type(tmp)), EchoObject(tmp),		\
	EchoGap(&gap(newg)), EchoLength(save_space(newg)));		\
      }									\
      else debug3(DOF, DDD, "newg %s: gap = %s, save_space = %s",	\
	Image(type(newg)), EchoGap(&gap(newg)),				\
	EchoLength(save_space(newg)));					\
    )									\
									\
    /* sort out ending with hyphenation and/or being unbreakable */	\
    /* NB ADD_HYPH is possible after a restart                   */	\
    if( mode(gap(newg)) == HYPH_MODE || mode(gap(newg)) == ADD_HYPH )	\
    { if( hyph_allowed )						\
      {									\
	/* hyphenation is allowed, so add hyph_word to nat_width */	\
	if( is_word(type(right)) && 					\
	 !(string(right)[StringLength(string(right))-1] == CH_HYPHEN) )	\
        {								\
	  /* make sure hyph_word exists and is of the right font */	\
	  debug0(DOF, DDD, "  MoveRightToGap checking hyph_word");	\
	  if( hyph_word == nilobj )					\
	  { hyph_word = MakeWord(WORD, STR_HYPHEN, &fpos(x));		\
	    word_font(hyph_word) = 0;					\
	    word_colour(hyph_word) = colour(save_style(x));		\
	    word_outline(hyph_word) = outline(save_style(x));		\
	    word_language(hyph_word) = language(save_style(x));		\
	    word_hyph(hyph_word) = hyph_style(save_style(x))==HYPH_ON;	\
	  }								\
	  if( word_font(hyph_word) != font(save_style(x)) )		\
	  { word_font(hyph_word) = font(save_style(x));			\
	    FposCopy(fpos(hyph_word), fpos(x));				\
	    FontWordSize(hyph_word);					\
	  }								\
									\
	  mode(gap(newg)) = ADD_HYPH;					\
	  I.nat_width += size(hyph_word, COLM);				\
	  debug0(DOF, DDD, "   adding hyph_word from nat_width");	\
        }								\
      }									\
      else								\
      {									\
	/* hyphenation is not allowed, so this gap is unbreakable */	\
	unbreakable_at_right = TRUE;					\
      }									\
    }									\
    else if( nobreak(gap(newg)) )					\
      unbreakable_at_right = TRUE;					\
									\
    I.rlink = Up(newg);							\
    debug2(DOF, DDD, "  MoveRightToGap setting I.rlink to %s %s",	\
      Image(type(newg)), EchoObject(newg));				\
  }									\
  else I.rlink = x;							\
  SetIntervalBadness(I, max_width, etc_width);				\
  if( unbreakable_at_right )  I.class = UNBREAKABLE_RIGHT;		\
  else if( I.class == TIGHT && mode(gap(newg)) == TAB_MODE )		\
    I.class = TOO_TIGHT, I.badness = TOO_TIGHT_BAD;			\
  debug0(DOF, DDD, "MoveRightToGap returning.");				\
}

/*@::IntervalInit(), IntervalShiftRightEnd()@*********************************/
/*                                                                           */
/*  IntervalInit(I, x, max_width, etc_width, hyph_word)                      */
/*                                                                           */
/*  Set I to the first interval of x.                                        */
/*                                                                           */
/*****************************************************************************/

#define IntervalInit(I, x, max_width, etc_width, hyph_word)		\
{ OBJECT rlink, right; BOOLEAN jn;					\
  debug0(DOF, DDD, "IntervalInit(I, x, -, -, hyph_word)");		\
  I.llink = x;								\
									\
  FirstDefinite(x, rlink, right, jn);					\
  if( rlink == x )  I.class = AT_END, I.rlink = x;			\
  else									\
  { 									\
    /* have first definite object, so set interval width etc. */	\
    if( multi != nilobj )						\
    { Child(I.cwid, Down(multi));					\
    }									\
    else I.cwid = nilobj;						\
    I.nat_width = size(right, COLM);					\
    I.space_width = 0;							\
    I.tab_count = 0;							\
									\
    /* move to gap, check hyphenation there etc. */			\
    MoveRightToGap(I,x,rlink,right,max_width,etc_width,hyph_word); 	\
  }									\
  debug0(DOF, DDD, "IntervalInit returning.");				\
} /* end macro IntervalInit */


/*****************************************************************************/
/*                                                                           */
/*  IntervalShiftRightEnd(I, x, hyph_word, max_width, etc_width)             */
/*                                                                           */
/*  Shift the right end of interval I one place to the right.                */
/*                                                                           */
/*****************************************************************************/

#define IntervalShiftRightEnd(I, x, hyph_word, max_width, etc_width) 	\
{ OBJECT rlink, g, right;						\
  assert( I.class != AT_END, "IntervalShiftRightEnd: AT_END!" );	\
  rlink = I.rlink;							\
  if( rlink == x ) I.class = AT_END;					\
  else									\
  {									\
    /* I is optimal here so save its badness and left endpoint */	\
    Child(g, rlink);							\
    assert( type(g) == GAP_OBJ, "IntervalShiftRightEnd: type(g)!" );	\
    save_badness(g) = I.badness;					\
    save_prev(g) = I.llink;						\
    save_cwid(g) = I.cwid;						\
									\
    /* if hyphenation case, must take away width of hyph_word */	\
    /* and increase the badness to discourage breaks at this point */	\
    if( mode(gap(g)) == ADD_HYPH )					\
    { I.nat_width -= size(hyph_word,COLM);				\
      save_badness(g) += HYPH_BAD_INCR;					\
      debug0(DOF, DDD, "   subtracting hyph_word from nat_width");	\
    }									\
									\
    /* find definite object which must lie just to the right of g */	\
    NextDefinite(x, rlink, right);					\
    assert( rlink != x, "IntervalShiftRightEnd: rlink == x!" );		\
									\
    /* modify I to reflect the addition of g and right */		\
    if( mode(gap(g)) == TAB_MODE )					\
    { I.tab_count++;							\
      I.tab_pos = save_space(g);					\
      I.width_to_tab = I.nat_width;					\
      I.nat_width = save_space(g) + size(right, COLM);			\
      I.space_width = 0;						\
    }									\
    else								\
    { I.nat_width += save_space(g) + size(right, COLM);			\
      I.space_width += save_space(g);					\
    }									\
									\
    /* now shift one step to the right */				\
    MoveRightToGap(I, x, rlink, right, max_width, etc_width,hyph_word);	\
  }									\
} /* end macro IntervalShiftRightEnd */


/*@::IntervalShiftLeftEnd(), IntervalBadness()@*******************************/
/*                                                                           */
/*  IntervalShiftLeftEnd(I, x, max_width, etc_width)                         */
/*                                                                           */
/*  Shift the left end of interval I one place to the right.                 */
/*                                                                           */
/*****************************************************************************/

#define IntervalShiftLeftEnd(I, x, max_width, etc_width)		\
{ OBJECT llink, left, lgap, y;  BOOLEAN jn;				\
  debug1(DOF, DDD, "IntervalShiftLeftEnd(%s)", IntervalPrint(I, x));	\
  assert( I.class != AT_END, "IntervalShiftLeftEnd: AT_END!" );		\
									\
  /* find left, the leftmost definite object of I */			\
  llink = I.llink;							\
  NextDefinite(x, llink, left);						\
  assert( llink != x, "IntervalShiftLeftEnd: llink == x!" );		\
									\
  /* find lgap, the first true breakpoint following left */		\
  NextDefiniteWithGap(x, llink, y, lgap, jn);				\
  assert( llink != x, "IntervalShiftLeftEnd: llink == x!" );		\
									\
  /* calculate width and badness of interval minus left and lgap */	\
  if( mode(gap(lgap)) == TAB_MODE )					\
  { assert( I.tab_count > 0 || Up(lgap) == I.rlink,			\
			"IntervalShiftLeftEnd: tab_count <= 0!" );	\
    I.tab_count--;							\
    if( I.tab_count == 0 )  I.nat_width -= save_space(lgap);		\
  }									\
  else /* take from nat_width, or if tab, from width_to_tab */		\
  { if( I.tab_count == 0 )						\
    { I.nat_width -= save_space(lgap) + size(left, COLM);		\
      I.space_width -= save_space(lgap);				\
    }									\
    else if( I.tab_count == 1 )						\
    { I.width_to_tab -= save_space(lgap) + size(left, COLM);		\
    }									\
    /* else no changes since tabs hide them */				\
  }									\
  I.llink = Up(lgap);							\
  if( I.llink == I.rlink )						\
  { I.class = EMPTY_INTERVAL;						\
    I.badness = TOO_TIGHT_BAD + 1;					\
  }									\
  else									\
  {									\
    if( save_cwid(lgap) != nilobj )					\
    { OBJECT tlink;							\
      tlink = NextDown(Up(save_cwid(lgap)));				\
      if( type(tlink) == ACAT )  I.cwid = save_cwid(lgap);		\
      else Child(I.cwid, tlink);					\
    }									\
    SetIntervalBadness(I, max_width, etc_width);			\
    if( nobreak(gap(lgap)) || ( !hyph_allowed &&			\
	(mode(gap(lgap))==HYPH_MODE || mode(gap(lgap))==ADD_HYPH) ) )	\
      I.class = UNBREAKABLE_LEFT;					\
  }									\
  debug1(DOF, DDD, "IShiftLeftEnd returning %s", IntervalPrint(I, x));	\
} /* end macro IntervalShiftLeftEnd */


/*****************************************************************************/
/*                                                                           */
/*  IntervalBadness(I)                                                       */
/*                                                                           */
/*  Return the badness of interval I.                                        */
/*                                                                           */
/*****************************************************************************/

#define IntervalBadness(I)	(I.badness)


/*@IntervalClass(), IntervalPrint()@******************************************/
/*                                                                           */
/*  IntervalClass(I)                                                         */
/*                                                                           */
/*  Return the badness class of interval I.                                  */
/*                                                                           */
/*****************************************************************************/

#define IntervalClass(I)	(I.class)


#if DEBUG_ON
/*****************************************************************************/
/*                                                                           */
/*  IntervalPrint(I, x)                                                      */
/*                                                                           */
/*  Return string image of the contents of interval I of ACAT x.             */
/*                                                                           */
/*****************************************************************************/

static FULL_CHAR *IntervalPrint(INTERVAL I, OBJECT x)
{ static char *class_name[] =
    { "TOO_LOOSE", "LOOSE", "TIGHT", "TOO_TIGHT", "TAB_OVERLAP", "AT_END",
      "UNBREAKABLE_LEFT", "UNBREAKABLE_RIGHT" };
  OBJECT link, y, g, z; int i;
  static FULL_CHAR res[300];
  if( I.llink == I.rlink )  return AsciiToFull("[]");
  StringCopy(res, AsciiToFull(""));
  if( I.cwid != nilobj )
  { StringCat(res, AsciiToFull("!"));
    StringCat(res, EchoLength(bfc(constraint(I.cwid))));
    StringCat(res, AsciiToFull("!"));
  }
  StringCat(res, AsciiToFull("["));
  g = nilobj;
  for( link = NextDown(I.llink);  link != I.rlink;  link = NextDown(link) )
  { assert(link != x, "IntervalPrint: link == x!");
    Child(y, link);
    debug2(DOF, DDD, "IntervalPrint at %s %s", Image(type(y)), EchoObject(y));
    assert(y != x, "IntervalPrint: y == x!");
    if( type(y) == GAP_OBJ )
    { g = y;
      if( Down(g) != g )
      {	Child(z, Down(g));
	StringCat(res, STR_SPACE);
	StringCat(res, EchoCatOp(ACAT, mark(gap(g)), join(gap(g)))),
	StringCat(res, is_word(type(z)) ? string(z) : Image(type(z)));
	StringCat(res, STR_SPACE);
      }
      else for( i = 1;  i <= hspace(g) + vspace(g); i++ )
	     StringCat(res, STR_SPACE);
    }
    else if( is_word(type(y)) )
	StringCat(res, string(y)[0] == '\0' ? AsciiToFull("{}") : string(y));
    else StringCat(res, Image(type(y)));
  }
  StringCat(res, AsciiToFull("] n"));
  StringCat(res, EchoLength(I.nat_width));
  StringCat(res, AsciiToFull(", "));
  StringCat(res, EchoLength(I.space_width));
  StringCat(res, AsciiToFull(" ("));
  StringCat(res, AsciiToFull(class_name[I.class]));
  StringCat(res, AsciiToFull(" "));
  StringCat(res, StringInt(I.badness));
  StringCat(res, AsciiToFull(")"));
  if( I.tab_count > 0 )
  { StringCat(res, AsciiToFull(" <"));
    StringCat(res, StringInt(I.tab_count));
    StringCat(res, STR_SPACE);
    StringCat(res, EchoLength(I.width_to_tab));
    StringCat(res, AsciiToFull(":"));
    StringCat(res, EchoLength(I.tab_pos));
    StringCat(res, AsciiToFull(">"));
  }
  return res;
} /* end IntervalPrint */
#endif


/*@::FillObject()@************************************************************/
/*                                                                           */
/*  FillObject(x, c, multi, can_hyphenate, allow_shrink, extend_unbreakable, */
/*                                              hyph_used)                   */
/*                                                                           */
/*  Break ACAT x into lines using optimal breakpoints.  Set hyph_used to     */
/*  TRUE if any hyphenation was done.                                        */
/*                                                                           */
/*    multi               If multi is not nilobj, ignore c and use the       */
/*                        sequence of constraints within multi for the       */
/*                        successive lines.                                  */
/*                                                                           */
/*    can_hyphenate       TRUE if hyphenation is permitted during this fill. */
/*                                                                           */
/*    allow_shrink        TRUE if gaps may be shrunk as well as expanded.    */
/*                                                                           */
/*    extend_unbreakable  TRUE if nobreak(gap()) fields are to be set so as  */
/*                        to prevent gaps hidden under overstruck objects    */
/*                        from becoming break points.                        */
/*                                                                           */
/*****************************************************************************/

OBJECT FillObject(OBJECT x, CONSTRAINT *c, OBJECT multi, BOOLEAN can_hyphenate,
  BOOLEAN allow_shrink, BOOLEAN extend_unbreakable, BOOLEAN *hyph_used)
{ INTERVAL I, BestI;  OBJECT res, gp, tmp, z, y, link, ylink, prev, next;
  int max_width, etc_width, outdent_margin, f;  BOOLEAN jn;  unsigned typ;
  static OBJECT hyph_word = nilobj;
  BOOLEAN hyph_allowed;	    /* TRUE when hyphenation of words is permitted  */
  assert( type(x) == ACAT, "FillObject: type(x) != ACAT!" );

  debug4(DOF, D, "FillObject(x, %s, can_hyph = %s, %s); %s",
    EchoConstraint(c), bool(can_hyphenate),
    multi == nilobj ? "nomulti" : "multi", EchoStyle(&save_style(x)));
  ifdebug(DOF, DD, DebugObject(x); fprintf(stderr, "\n\n") );

  *hyph_used = FALSE;

  if( multi == nilobj )
  {
    /* set max_width (width of 1st line), etc_width (width of later lines) */
    max_width = find_min(fc(*c), bfc(*c));
    if( display_style(save_style(x)) == DISPLAY_OUTDENT ||
        display_style(save_style(x)) == DISPLAY_ORAGGED )
    { outdent_margin = 2 * FontSize(font(save_style(x)), x);
      etc_width = max_width - outdent_margin;
    }
    else etc_width = max_width;
    assert( size(x, COLM) > max_width, "FillObject: initial size!" );

    /* if column width is ridiculously small, exit with error message */
    if( max_width <= 2 * FontSize(font(save_style(x)), x) )
    {
      Error(14, 6, "paragraph deleted (assigned width %s is too narrow)",
         WARN, &fpos(x), EchoLength(max_width));
      res = MakeWord(WORD, STR_EMPTY, &fpos(x));
      word_font(res) = font(save_style(x));
      word_colour(res) = colour(save_style(x));
      word_outline(res) = outline(save_style(x));
      word_language(res) = language(save_style(x));
      word_hyph(res) = hyph_style(save_style(x)) == HYPH_ON;
      back(res, COLM) = fwd(res, COLM) = 0;
      ReplaceNode(res, x);
      DisposeObject(x);
      return res;
    }
  }
  else max_width = etc_width = 0; /* not used really */

  /* add &1rt {} to end of paragraph */
  New(gp, GAP_OBJ);  hspace(gp) = 1;  vspace(gp) = 0;
  SetGap(gap(gp), FALSE, FALSE, TRUE, AVAIL_UNIT, TAB_MODE, 1*FR);
  tmp = MakeWord(WORD, STR_GAP_RJUSTIFY, &fpos(x));
  Link(gp, tmp);  Link(x, gp);
  tmp = MakeWord(WORD, STR_EMPTY, &fpos(x));
  back(tmp, COLM) = fwd(tmp, COLM) = back(tmp, ROWM) = fwd(tmp, ROWM) = 0;
  word_font(tmp) = 0;
  word_colour(tmp) = 0;
  word_outline(tmp) = 0;
  word_language(tmp) = 0;
  word_hyph(tmp) = 0;
  underline(tmp) = UNDER_OFF;
  Link(x, tmp);

  /* if extend_unbreakable, run through x and set every gap in the     */
  /* shadow of a previous gap to be unbreakable                        */
  if( extend_unbreakable )
  { int f, max_f;  OBJECT g;
    FirstDefinite(x, link, y, jn);
    assert( link != x, "FillObject/extend_unbreakable:  link == x!" );
    f = max_f = size(y, COLM);  prev = y;
    NextDefiniteWithGap(x, link, y, g, jn);
    while( link != x )
    {
      /* add unbreakableness if gap is overshadowed by a previous one */
      f += MinGap(fwd(prev, COLM), back(y, COLM), fwd(y, COLM), &gap(g))
	     - fwd(prev, COLM) + back(y, COLM);
      if( f < max_f )
      { if( units(gap(g)) == FIXED_UNIT )
	  nobreak(gap(g)) = TRUE;
      }
      else
      { max_f = f;
      }

      /* on to next component and gap */
      prev = y;
      NextDefiniteWithGap(x, link, y, g, jn);
    }
  }

  /* initially we can hyphenate if hyphenation is on, but not first pass */
  if( hyph_style(save_style(x)) == HYPH_UNDEF )
    Error(14, 7, "hyphen or nohyphen option missing", FATAL, &fpos(x));
  hyph_allowed = FALSE;

  /* initialize I to first interval, BestI to best ending here, and run */
  RESTART:
  IntervalInit(I, x, max_width, etc_width, hyph_word);  BestI = I;
  while( IntervalClass(I) != AT_END )
  {
    debug0(DOF, D, "loop:");
    debug1(DOF, D, "       %s", IntervalPrint(I, x));
    switch( IntervalClass(I) )
    {

      case TOO_LOOSE:
      case EMPTY_INTERVAL:
      
	/* too loose, so save best and shift right end */
	if( IntervalClass(I) == EMPTY_INTERVAL ||
	    IntervalBadness(BestI) <= IntervalBadness(I) )
	      I = BestI;
	debug1(DOF, D, "BestI: %s\n", IntervalPrint(I, x));
	/* NB no break */


      case UNBREAKABLE_RIGHT:

	IntervalShiftRightEnd(I, x, hyph_word, max_width, etc_width);
	BestI = I;
	break;


      case LOOSE:
      case TIGHT:
      case TOO_TIGHT:
      
	/* reasonable, so check best and shift left end */
	if( IntervalBadness(I) < IntervalBadness(BestI) )  BestI = I;
	/* NB no break */


      case UNBREAKABLE_LEFT:
      case TAB_OVERLAP:
      
	/* too tight, or unbreakable gap at left end, so shift left end */
	IntervalShiftLeftEnd(I, x, max_width, etc_width);
	break;


      /* ***
      case EMPTY_INTERVAL:

	PrevDefinite(x, I.llink, y);
	if( can_hyphenate )
	{ x = Hyphenate(x);
	  can_hyphenate = FALSE;
	  hyph_allowed = TRUE;
	  *hyph_used = TRUE;
	}
	else CorrectOversize(x, I.llink,
	  (I.cwid!=nilobj) ? bfc(constraint(I.cwid)) : etc_width);
	goto RESTART;
      *** */


      default:
      
	assert(FALSE, "FillObject: IntervalClass(I)");
	break;

    }
  }

  /* do end processing */
  ifdebug(DOF, DD,
    debug0(DOF, DD, "final result:");
    debug1(DOF, DD, "%s", IntervalPrint(BestI, x));
    while( BestI.llink != x )
    { BestI.rlink = BestI.llink;
      Child(gp, BestI.rlink);
      BestI.llink = save_prev(gp);
      debug1(DOF, DD, "%s", IntervalPrint(BestI, x));
    }
  );

  if( can_hyphenate && IntervalBadness(BestI) > HYPH_BAD )
  {
    /* the result is bad enough to justify the cost of a second attempt,    */
    /* with hyphenation turned on this time                                 */
    x = Hyphenate(x);
    can_hyphenate = FALSE;
    hyph_allowed = TRUE;
    *hyph_used = TRUE;
    goto RESTART;
  }
  else if( I.llink == x )
  { /* The result has only one line.  Since the line did not fit initially, */
    /* this must mean either that a large word was discarded or else that   */
    /* the line was only slightly tight                                     */
    if( multi == nilobj )
    { res = x;
      back(res, COLM) = 0;  fwd(res, COLM) = max_width;
    }
    else
    { New(res, VCAT);
      adjust_cat(res) = FALSE;
      ReplaceNode(res, x);
      Link(res, x);
    }
  }
  else
  { OBJECT lgap, llink;
    New(res, VCAT);
    adjust_cat(res) = FALSE;
    back(res, COLM) = 0;  fwd(res, COLM) = max_width;
    ReplaceNode(res, x);
    llink = I.llink;

    /* break the lines of x */
    while( llink != x )
    { New(y, ACAT);
      adjust_cat(y) = adjust_cat(x);
      FposCopy(fpos(y), fpos(x));
      StyleCopy(save_style(y), save_style(x));
      if( Down(res) != res &&
		(display_style(save_style(y)) == DISPLAY_ADJUST ||
		 display_style(save_style(y)) == DISPLAY_OUTDENT) )
	 display_style(save_style(y)) = DO_ADJUST;
      back(y, COLM) = 0;
      fwd(y, COLM) = max_width;

      /* if outdented paragraphs, add 2.0f @Wide & to front of new line */
      if( display_style(save_style(x)) == DISPLAY_OUTDENT ||
          display_style(save_style(x)) == DISPLAY_ORAGGED )
      {
	OBJECT t1, t2, z;
	t1 = MakeWord(WORD, STR_EMPTY, &fpos(x));
	back(t1, COLM) = fwd(t1, COLM) = back(t1, ROWM) = fwd(t1, ROWM) = 0;
	word_font(t1) = 0;
	word_colour(t1) = 0;
	word_outline(t1) = 0;
	word_language(t1) = 0;
	word_hyph(t1) = 0;
	underline(t1) = UNDER_OFF;
	New(t2, WIDE);
	SetConstraint(constraint(t2), MAX_FULL_LENGTH, outdent_margin,
	  MAX_FULL_LENGTH);
	back(t2, COLM) = 0;  fwd(t2, COLM) = outdent_margin;
	underline(t2) = UNDER_OFF;
	Link(t2, t1);
	Link(y, t2);
	New(z, GAP_OBJ);
	hspace(z) = vspace(z) = 0;
	SetGap(gap(z), TRUE, FALSE, TRUE, FIXED_UNIT, EDGE_MODE, 0);
	Link(y, z);
      }

      /* move the line to below y */
      TransferLinks(NextDown(llink), x, y);

      /* add hyphen to end of previous line, if lgap is ADD_HYPH */
      Child(lgap, llink);
      if( mode(gap(lgap)) == ADD_HYPH )
      { OBJECT z;  BOOLEAN under;

	/* work out whether the hyphen needs to be underlined */
	Child(z, LastDown(x));
	under = underline(z);

	/* add zero-width gap object */
        New(z, GAP_OBJ);
	debug0(DOF, DD, "   adding hyphen\n");
	hspace(z) = vspace(z) = 0;
	underline(z) = under;
	SetGap(gap(z), TRUE, FALSE, TRUE, FIXED_UNIT, EDGE_MODE, 0);
	Link(x, z);

	/* add hyphen */
	z = MakeWord(WORD, STR_HYPHEN, &fpos(y));
	word_font(z) = font(save_style(x));
	word_colour(z) = colour(save_style(x));
	word_outline(z) = outline(save_style(x));
	word_language(z) = language(save_style(x));
	word_hyph(z) = hyph_style(save_style(x)) == HYPH_ON;
	underline(z) = under;
	FontWordSize(z);
	Link(x, z);
      }

      /* attach y to res, recycle lgap for gap separating the two lines */
      Link(NextDown(res), y);
      MoveLink(llink, NextDown(res), PARENT);
      hspace(lgap) = 0;
      vspace(lgap) = 1;
      GapCopy(gap(lgap), line_gap(save_style(x)));
      if( Down(lgap) != lgap )  DisposeChild(Down(lgap));

      /* move on to previous line */
      llink = save_prev(lgap);
    }

    /* attach first line, x, to res */
    Link(NextDown(res), x);
    back(x, COLM) = 0;
    fwd(x, COLM) = max_width;
    if( display_style(save_style(x)) == DISPLAY_ADJUST ||
	display_style(save_style(x)) == DISPLAY_OUTDENT )
	  display_style(save_style(x)) = DO_ADJUST;

    /* if last line contains only the {} from final &1rt {}, delete the line */
    /* and the preceding gap                                                 */
    Child(y, LastDown(res));
    if( Down(y) == LastDown(y) )
    { DisposeChild(LastDown(res));
      assert( Down(res) != LastDown(res), "almost empty paragraph!" );
      DisposeChild(LastDown(res));
    }

    /* else delete the final &1rt {} from the last line, to help clines */
    else
    { Child(z, LastDown(y));
      assert( type(z)==WORD && string(z)[0]=='\0', "FillObject: last word!" );
      DisposeChild(LastDown(y));
      Child(z, LastDown(y));
      assert( type(z) == GAP_OBJ, "FillObject: last gap_obj!" );
      DisposeChild(LastDown(y));
    }

    /* set unbreakable bit of first and last inter-line gaps, if required */
    if( nobreakfirst(save_style(x)) && Down(res) != LastDown(res) )
    { Child(gp, NextDown(Down(res)));
      assert( type(gp) == GAP_OBJ, "FillObject: type(gp) != GAP_OBJ (a)!" );
      nobreak(gap(gp)) = TRUE;
    }
    if( nobreaklast(save_style(x)) && Down(res) != LastDown(res) )
    { Child(gp, PrevDown(LastDown(res)));
      assert( type(gp) == GAP_OBJ, "FillObject: type(gp) != GAP_OBJ (b)!" );
      nobreak(gap(gp)) = TRUE;
    }


    /* recalculate the width of the last line, since it may now be smaller */
    assert( LastDown(res) != res, "FillObject: empty paragraph!" );
    Child(y, LastDown(res));
    FirstDefinite(y, link, z, jn);
    assert( link != y, "FillObject: last line is empty!" );
    f = back(z, COLM);  prev = z;
    NextDefiniteWithGap(y, link, z, gp, jn);
    while( link != y )
    {
      f += MinGap(fwd(prev, COLM), back(z, COLM), fwd(z, COLM), &gap(gp));
      prev = z;
      NextDefiniteWithGap(y, link, z, gp, jn);
    }
    fwd(y, COLM) = find_min(MAX_FULL_LENGTH, f + fwd(prev, COLM));

    /* make last line DO_ADJUST if it is oversize */
    if( size(y, COLM) > max_width )  display_style(save_style(y)) = DO_ADJUST;
  }

  /* rejoin unused hyphenated gaps so that kerning will work across them */
  if( *hyph_used && type(res) == VCAT )
  { for( link = Down(res);  link != res;  link = NextDown(link) )
    { Child(y, link);
      if( type(y) == ACAT )
      { for( ylink = Down(y);  ylink != y;  ylink = NextDown(ylink) )
        { Child(gp, ylink);
	  if( type(gp) == GAP_OBJ && width(gap(gp)) == 0 &&
	      mode(gap(gp)) == ADD_HYPH )
	  {
	    /* possible candidate for joining, look into what's on each side */
	    Child(prev, PrevDown(ylink));
	    Child(next, NextDown(ylink));
	    if( is_word(type(prev)) && is_word(type(next)) &&
	        word_font(prev) == word_font(next) &&
	        word_colour(prev) == word_colour(next) &&
	        word_outline(prev) == word_outline(next) &&
	        word_language(prev) == word_language(next) &&
	        underline(prev) == underline(next) )
	    { 
	      debug2(DOF, D, "joining %s with %s", EchoObject(prev),
		EchoObject(next));
	      typ = type(prev) == QWORD || type(next) == QWORD ? QWORD : WORD;
	      tmp = MakeWordTwo(typ, string(prev), string(next), &fpos(prev));
	      word_font(tmp) = word_font(prev);
	      word_colour(tmp) = word_colour(prev);
	      word_outline(tmp) = word_outline(prev);
	      word_language(tmp) = word_language(prev);
	      word_hyph(tmp) = word_hyph(prev);
	      FontWordSize(tmp);
	      underline(tmp) = underline(prev);
	      MoveLink(ylink, tmp, CHILD);
	      DisposeChild(Up(prev));
	      DisposeChild(Up(next));
	    }
	  }
        }
      }
    }
  }

  debug0(DOF, D, "FillObject exiting");
  return res;
} /* end FillObject */
