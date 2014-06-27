/*@z08.c:Object Manifest:ReplaceWithSplit()@**********************************/
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
/*  FILE:         z08.c                                                      */
/*  MODULE:       Object Manifest                                            */
/*  EXTERNS:      Manifest()                                                 */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define line_breaker(g)							\
  (vspace(g) > 0 || (units(gap(g)) == FRAME_UNIT && width(gap(g)) > FR))


/*****************************************************************************/
/*                                                                           */
/*  static SetUnderline(x)                                                   */
/*                                                                           */
/*  Set underline() flags in object x to UNDER_ON as appropriate.            */
/*                                                                           */
/*****************************************************************************/

static void SetUnderline(OBJECT x)
{ OBJECT y, link;
  debug2(DOM, DD, "  Manifest underlining %s %s", Image(type(x)),EchoObject(x));
  if( type(x) == ACAT )
  { for( link = Down(x);  link != x;  link = NextDown(link) )
    { Child(y, link);
      SetUnderline(y);
    }
  }
  debug3(DOM, DDD, "  SetUnderline underline() := %s for %s %s",
    "UNDER_ON", Image(type(x)), EchoObject(x));
  underline(x) = UNDER_ON;
} /* end SetUnderline */


/*****************************************************************************/
/*                                                                           */
/*  static ReplaceWithSplit(x, bthr, fthr)                                   */
/*                                                                           */
/*  Replace object x with a SPLIT object, if threads for this object are     */
/*  requested by bthr and/or fthr.                                           */
/*                                                                           */
/*****************************************************************************/

#define ReplaceWithSplit(x, bthr, fthr)					\
   if( bthr[ROWM] || bthr[COLM] || fthr[ROWM] || fthr[COLM] )		\
	x = insert_split(x, bthr, fthr)

static OBJECT insert_split(OBJECT x, OBJECT bthr[2], OBJECT fthr[2])
{ OBJECT res, new_op;  int dim;
  debug1(DOM, DD, "ReplaceWithSplit(%s, -)", EchoObject(x));
  assert( type(x) != SPLIT, "ReplaceWithSplit: type(x) already SPLIT!" );
  New(res, SPLIT);
  FposCopy(fpos(res), fpos(x));
  ReplaceNode(res, x);
  for( dim = COLM;  dim <= ROWM;  dim++ )
  { if( bthr[dim] || fthr[dim] )
    {
      debug0(DGP, D, "  calling New(thread) from Manifest now");
      New(new_op, dim == COLM ? COL_THR : ROW_THR);
      thr_state(new_op) = NOTSIZED;
      fwd(new_op, 1-dim) = 0;	/* will hold max frame_size */
      back(new_op, 1-dim) = 0;	/* will hold max frame_origin */
      FposCopy(fpos(new_op), fpos(x));
      Link(res, new_op);  Link(new_op, x);
      if( bthr[dim] )  Link(bthr[dim], new_op);
      if( fthr[dim] )  Link(fthr[dim], new_op);
    }
    else Link(res, x);
  }

  debug1(DOM, DD, "ReplaceWithSplit returning %s", EchoObject(res));
  return res;
} /* end insert_split */

/*@::ReplaceWithTidy()@*******************************************************/
/*                                                                           */
/*  OBJECT ReplaceWithTidy(x, one_word)                                      */
/*                                                                           */
/*  Replace object x with a tidier version in which juxtapositions are       */
/*  folded.  If this is not possible, return the original object.            */
/*                                                                           */
/*  If one_word is TRUE, the result is to be a single QWORD with inter-      */
/*  word spaces converted to single space characters.  Otherwise an ACAT     */
/*  is the preferred result.                                                 */
/*                                                                           */
/*  *** Meaning changed, now interword spaces are converted to the same      */
/*  number of spaces to assist construction of sorting keys.                 */
/*                                                                           */
/*****************************************************************************/

OBJECT ReplaceWithTidy(OBJECT x, BOOLEAN one_word)
{ static FULL_CHAR	buff[MAX_BUFF];		/* the growing current word */
  static int		buff_len;		/* length of current word   */
  static FILE_POS	buff_pos;		/* filepos of current word  */
  static unsigned	buff_typ;		/* WORD or QWORD of current */
  OBJECT                link, y, tmp, res;	/* temporaries              */
  int i;
  debug2(DOM, DD, "ReplaceWithTidy(%s, %s)", EchoObject(x), bool(one_word));
  switch( type(x) )
  {
    case ACAT:
    
      for( link = Down(x);  link != x;  link = NextDown(link) )
      {	Child(y, link);
	if( type(y) == ACAT )
	{ tmp = Down(y);  TransferLinks(tmp, y, link);
	  DisposeChild(link);  link = PrevDown(tmp);
	}
      }
      res = nilobj;  buff_len = 0;  buff_typ = WORD;
      FposCopy(buff_pos, fpos(x));
      for( link = Down(x); link != x; link = NextDown(link) )
      {	Child(y, link);
	if( is_word(type(y)) )
	{ if( buff_len + StringLength(string(y)) >= MAX_BUFF )
	    Error(8, 1, "word is too long", WARN, &fpos(y));
	  else
	  { if( buff_len == 0 )  FposCopy(buff_pos, fpos(y));
	    StringCopy(&buff[buff_len], string(y));
	    buff_len += StringLength(string(y));
	    if( type(y) == QWORD )  buff_typ = QWORD;
	  }
	}
	else if( type(y) == GAP_OBJ )
	{ if( Down(y) != y || hspace(y) + vspace(y) > 0 )
	  { if( one_word )
	    { if( buff_len + hspace(y) + vspace(y) >= MAX_BUFF )
		Error(8, 2, "word is too long", WARN, &fpos(y));
	      else
	      { for( i = 0;  i < hspace(y) + vspace(y);  i++ )
		{ StringCopy(&buff[buff_len], AsciiToFull(" "));
		  buff_len++;
		}
		buff_typ = QWORD;
	      }
	    }
	    else
	    { tmp = MakeWord(buff_typ, buff, &buff_pos);
	      buff_len = 0;  buff_typ = WORD;
	      if( res == nilobj )
	      { New(res, ACAT);
		FposCopy(fpos(res), fpos(x));
	      }
	      Link(res, tmp);  Link(res, y);
	    }
	  }
	}
	else /* error */
	{ if( res != nilobj )  DisposeObject(res);
	  debug0(DOM, DD, "ReplaceWithTidy returning unchanged");
	  return x;
	}
      }
      tmp = MakeWord(buff_typ, buff, &buff_pos);
      if( res == nilobj )  res = tmp;
      else Link(res, tmp);
      ReplaceNode(res, x);  DisposeObject(x);
      debug1(DOM, DD, "ReplaceWithTidy returning %s", EchoObject(res));
      return res;


    case WORD:
    case QWORD:

      debug1(DOM, DD, "ReplaceWithTidy returning %s", EchoObject(x));
      return x;


    default:

      debug0(DOM, DD, "ReplaceWithTidy returning unchanged");
      return x;
  }
} /* end ReplaceWithTidy */


/*@::GetScaleFactor()@********************************************************/
/*                                                                           */
/*  static float GetScaleFactor(x)                                           */
/*                                                                           */
/*  Find a scale factor in object x and return it as a float, after checks.  */
/*                                                                           */
/*****************************************************************************/

static float GetScaleFactor(OBJECT x)
{ float scale_factor;
  if( !is_word(type(x)) )
  { Error(8, 3, "replacing invalid scale factor by 1.0", WARN, &fpos(x));
    scale_factor = 1.0;
  }
  else if( sscanf( (char *) string(x), "%f", &scale_factor) != 1 )
  { Error(8, 4, "replacing invalid scale factor %s by 1.0",
      WARN, &fpos(x), string(x));
    scale_factor = 1.0;
  }
  else if( scale_factor < 0.01 )
  { Error(8, 5, "replacing undersized scale factor %s by 1.0",
      WARN, &fpos(x), string(x));
    scale_factor = 1.0;
  }
  else if( scale_factor > 100 )
  { Error(8, 6, "replacing oversized scale factor %s by 1.0",
      WARN, &fpos(x), string(x));
    scale_factor = 1.0;
  }
  return scale_factor;
} /* GetScaleFactor */


static OBJECT nbt[2] = { nilobj, nilobj };	/* constant nilobj threads */
static OBJECT nft[2] = { nilobj, nilobj };	/* constant nilobj threads */
static OBJECT ntarget = nilobj;			/* constant nilobj target  */
static OBJECT nenclose = nilobj;		/* constant nilobj enclose  */


/*@::ManifestCat@*************************************************************/
/*                                                                           */
/*  OBJECT ManifestCat(x,env,style,bthr, fthr, target, crs, ok, need_expand, */
/*                     enclose, fcr)                                         */
/*                                                                           */
/*  This procedure implements Manifest (see below) when x is HCAT or VCAT.   */
/*                                                                           */
/*****************************************************************************/

static OBJECT ManifestCat(OBJECT x, OBJECT env, STYLE *style, OBJECT bthr[2],
OBJECT fthr[2], OBJECT *target, OBJECT *crs, BOOLEAN ok, BOOLEAN need_expand,
OBJECT *enclose, BOOLEAN fcr)
{ OBJECT bt[2], ft[2], y, link, gaplink, g, first_bt, last_ft, z;
  int par, perp;
  unsigned res_inc;  BOOLEAN still_backing;
  STYLE new_style;
  debug1(DOM, DD, "[ ManifestCat(%s)", EchoObject(x));
    
  StyleCopy(new_style, *style);
  if( type(x) == HCAT )
  { par = ROWM;
    adjust_cat(x) = hadjust(*style);
    hadjust(new_style) = FALSE;
  }
  else
  { par = COLM;
    adjust_cat(x) = vadjust(*style);
    vadjust(new_style) = FALSE;
  }
  perp = 1 - par;
  link = Down(x);
  gaplink = NextDown(link);
  assert( link!=x && gaplink!=x, "Manifest/VCAT: less than two children!" );
  Child(y, link);  Child(g, gaplink);

  /* set bt and ft threads for y */
  bt[perp] = bthr[perp];
  ft[perp] = fthr[perp];
  if( bthr[par] )  { New(first_bt, THREAD); }
  else first_bt = nilobj;
  bt[par] = first_bt;
  if( join(gap(g)) )  { New(ft[par], THREAD); }
  else ft[par] = nilobj;
  still_backing = first_bt != nilobj;

  /* manifest y and insinuate any cross-references */
  y = Manifest(y, env, &new_style, bt, ft, target, crs, ok, FALSE, enclose, fcr);
  if( type(x) == VCAT && ok && *crs != nilobj )
  { debug1(DCR, DD, "  insinuating %s", EchoObject(*crs));
    TransferLinks(Down(*crs), *crs, link);
    DisposeObject(*crs);
    *crs = nilobj;
  }

  /* manifest the remaining children */
  while( g != nilobj )
  {	
    /* manifest the gap object, store it in gap(g), add perp threads */
    assert( type(g) == GAP_OBJ, "Manifest/VCAT: type(g) != GAP_OBJECT!" );
    assert( Down(g) != g, "Manifest/VCAT: GAP_OBJ has no child!" );
    Child(z, Down(g));
    debug1(DOM, DD, "manifesting gap, style = %s", EchoStyle(style));
    z = Manifest(z, env, &new_style, nbt, nft, &ntarget, crs, FALSE, FALSE, enclose, fcr);
    debug1(DOM, DD, "replacing with tidy, style = %s", EchoStyle(style));
    z = ReplaceWithTidy(z, FALSE);
    debug1(DOM, DD, "calling GetGap, style = %s", EchoStyle(style));
    GetGap(z, style, &gap(g), &res_inc);
    if( bt[perp] )  Link(bt[perp], g);
    if( ft[perp] )  Link(ft[perp], g);

    /* find the next child y, and following gap if any */
    link = NextDown(gaplink);
    assert( link != x, "Manifest/VCAT: GAP_OBJ is last child!" );
    Child(y, link);
    gaplink = NextDown(link);
    if( gaplink == x )  g = nilobj;
    else Child(g, gaplink);

    /* set bt and ft threads for y */
    last_ft = ft[par];
    if( ft[par] ) { New(bt[par], THREAD); }  else bt[par] = nilobj;
    if( g != nilobj )
    { if( join(gap(g)) )  { New(ft[par], THREAD); }  else ft[par] = nilobj;
    }
    else
    {
      if( fthr[par] )  { New(ft[par], THREAD); }  else ft[par] = nilobj;
    }

    /* manifest y and insinuate any cross references */
    y = Manifest(y, env, &new_style, bt, ft, target, crs, ok, FALSE, enclose, fcr);
    if( type(x) == VCAT && ok && *crs != nilobj )
    { debug1(DCR, DD, "  insinuating %s", EchoObject(*crs));
      TransferLinks(Down(*crs), *crs, link);
      DisposeObject(*crs);
      *crs = nilobj;
    }

    if( bt[par] )	/* then thread lists last_ft and bt[par] must merge */
    { OBJECT llink, rlink, lthread, rthread;
      BOOLEAN goes_through;
      assert( Down(bt[par]) != bt[par], "Manifest: bt[par] no children!" );
      assert( last_ft!=nilobj && Down(last_ft)!=last_ft, "Manifest:last_ft!" );

      /* check whether marks run right through y in par direction */
      goes_through = FALSE;
      if( ft[par] )
      {	assert( Down(ft[par]) != ft[par], "Manifest: ft[par] child!" );
	Child(lthread, LastDown(bt[par]));
	Child(rthread, LastDown(ft[par]));
	goes_through = lthread == rthread;
      }

      /* merge the thread lists */
      llink = Down(last_ft);  rlink = Down(bt[par]);
      while( llink != last_ft && rlink != bt[par] )
      {	Child(lthread, llink);
	Child(rthread, rlink);
	assert( lthread != rthread, "Manifest: lthread == rthread!" );
	MergeNode(lthread, rthread);
	llink = NextDown(llink);
	rlink = NextDown(rlink);
      }

      /* attach leftover back threads to first_bt if required */
      if( rlink != bt[par] )
      { 
	if( still_backing )  TransferLinks(rlink, bt[par], first_bt);
      }
      DisposeObject(bt[par]);

      /* attach leftover forward threads to ft[par] if required */
      if( llink != last_ft )
      {
	if( goes_through )  TransferLinks(llink, last_ft, ft[par]);
      }
      DisposeObject(last_ft);

      if( !goes_through )  still_backing = FALSE;

    }
    else still_backing = FALSE;

  } /* end while */

  /* export par threads */
  if( fthr[par] )  MergeNode(fthr[par], ft[par]);
  if( bthr[par] )  MergeNode(bthr[par], first_bt);
  debug0(DOM, DD, "] ManifestCat returning");
  return x;
} /* end ManifestCat */


/*@::ManifestCase@************************************************************/
/*                                                                           */
/*  OBJECT ManifestCase(x,env,style,bthr,fthr, target, crs, ok, need_expand, */
/*                      enclose, fcr)                                        */
/*                                                                           */
/*  This procedure implements Manifest (see below) when x is CASE.           */
/*                                                                           */
/*****************************************************************************/

static OBJECT ManifestCase(OBJECT x, OBJECT env, STYLE *style, OBJECT bthr[2],
OBJECT fthr[2], OBJECT *target,  OBJECT *crs, BOOLEAN ok, BOOLEAN need_expand,
OBJECT *enclose, BOOLEAN fcr)
{ OBJECT y, tag, ylink, yield, ytag, zlink;
  OBJECT res, z, firsttag, firstres;

  /* make sure left parameter (the tag) is in order */
  debug0(DOM, DD, "  manifesting CASE now");
  Child(tag, Down(x));
  debug1(DOM, DD, "  manifesting CASE tag %s now", EchoObject(tag));
  tag = Manifest(tag, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE,
  &nenclose, fcr);
  tag = ReplaceWithTidy(tag, FALSE);

  /* make sure the right parameter is an ACAT */
  Child(y, LastDown(x));
  if( type(y) == YIELD )
  { New(z, ACAT);
    MoveLink(Up(y), z, PARENT);
    Link(x, z);
    y = z;
  }
  if( type(y) != ACAT )
  { Error(8, 7, "%s deleted (right parameter is malformed)",
      WARN, &fpos(y), KW_CASE);
    y = MakeWord(WORD, STR_EMPTY, &fpos(x));
    ReplaceNode(y, x);  DisposeObject(x);
    x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
    return x;
  }

  /* hunt through right parameter for res, the selected case */
  res = nilobj;  firsttag = nilobj;
  for( ylink = Down(y);  ylink != y && res == nilobj;  ylink = NextDown(ylink) )
  { Child(yield, ylink);
    if( type(yield) == GAP_OBJ )  continue;
    if( type(yield) != YIELD )
    { Error(8, 8, "%s expected here", WARN, &fpos(yield), KW_YIELD);
      break;
    }
    Child(ytag, Down(yield));
    ytag = Manifest(ytag, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE,
      &nenclose, fcr);
    ytag = ReplaceWithTidy(ytag, FALSE);
    if( is_word(type(ytag)) )
    { if( firsttag == nilobj )
      {	firsttag = ytag;
	Child(firstres, LastDown(yield));
      }
      if( (is_word(type(tag)) && StringEqual(string(ytag), string(tag))) ||
	     StringEqual(string(ytag), STR_ELSE)  )
      {	Child(res, LastDown(yield));
	break;
      }
    }
    else if( type(ytag) == ACAT )
    { z = ytag;
      for( zlink = Down(z);  zlink != z;  zlink = NextDown(zlink) )
      {	Child(ytag, zlink);
	if( type(ytag) == GAP_OBJ )  continue;
	if( !is_word(type(ytag)) )
	{ Error(8, 9, "error in left parameter of %s",
	    WARN, &fpos(ytag), KW_YIELD);
	  break;
        }
        if( firsttag == nilobj )
        { firsttag = ytag;
	  Child(firstres, LastDown(yield));
        }
        if( (is_word(type(tag)) && StringEqual(string(ytag), string(tag)))
	        || StringEqual(string(ytag), STR_ELSE) )
        { Child(res, LastDown(yield));
	  break;
        }
      }
    }
    else Error(8, 10, "error in left parameter of %s",
	   WARN, &fpos(ytag), KW_YIELD);
  }
  if( res == nilobj )
  { if( firsttag != nilobj )
    { Error(8, 11, "replacing unknown %s option %s by %s",
	WARN, &fpos(tag), KW_CASE, string(tag), string(firsttag));
      res = firstres;
      debug1(DGP, D, "  res = %s", EchoObject(res));
    }
    else
    { Error(8, 12, "%s deleted (choice %s unknown)",
	WARN, &fpos(tag), KW_CASE, string(tag));
      y = MakeWord(WORD, STR_EMPTY, &fpos(x));
      ReplaceNode(y, x);  DisposeObject(x);
      x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      return x;
    }
  }

  /* now manifest the result and replace x with it */
  DeleteLink(Up(res));
  ReplaceNode(res, x);
  DisposeObject(x);
  x = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
  return x;
} /* ManifestCase */


/*@::ManifestTg@**************************************************************/
/*                                                                           */
/*  OBJECT ManifestTg(x,env,style,bthr, fthr, target, crs, ok, need_expand,  */
/*                    enclose, fcr)                                          */
/*                                                                           */
/*  This procedure implements Manifest (see below) when x is TAGGED.         */
/*                                                                           */
/*****************************************************************************/

static OBJECT ManifestTg(OBJECT x, OBJECT env, STYLE *style, OBJECT bthr[2],
OBJECT fthr[2], OBJECT *target, OBJECT *crs, BOOLEAN ok, BOOLEAN need_expand,
OBJECT *enclose, BOOLEAN fcr)
{ OBJECT y, tag, z;

  /* make sure first argument is a cross-reference */
  assert( Down(x) != x && NextDown(Down(x)) != x &&
	NextDown(NextDown(Down(x))) == x, "Manifest TAGGED: children!" );
  Child(y, Down(x));
  if( !is_cross(type(y)) )
  {
    y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, TRUE);
    if( !is_cross(type(y)) )
    { Error(8, 13, "left parameter of %s is not a cross reference",
        WARN, &fpos(y), KW_TAGGED);
      y = MakeWord(WORD, STR_EMPTY, &fpos(x));
      ReplaceNode(y, x);  DisposeObject(x);
      x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      return x;
    }
  }

  /* make sure the arguments of the cross-reference are OK */
  Child(z, Down(y));
  if( type(z) != CLOSURE )
  { Error(8, 14, "left parameter of %s must be a symbol",
      WARN, &fpos(y), KW_TAGGED);
    y = MakeWord(WORD, STR_EMPTY, &fpos(x));
    ReplaceNode(y, x);  DisposeObject(x);
    x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
    return x;
  }
  if( !has_tag(actual(z)) )
  { Error(8, 15, "symbol %s not allowed here (it has no %s)",
      WARN, &fpos(z), SymName(actual(z)), KW_TAG);
    y = MakeWord(WORD, STR_EMPTY, &fpos(x));
    ReplaceNode(y, x);  DisposeObject(x);
    x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
    return x;
  }
  Child(z, NextDown(Down(y)));
  z = Manifest(z, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
  z = ReplaceWithTidy(z, FALSE);
  if( is_word(type(z)) && StringEqual(string(z), KW_PRECEDING) )
    cross_type(y) = CROSS_PREC;
  else if( is_word(type(z)) && StringEqual(string(z), KW_FOLLOWING) )
    cross_type(y) = CROSS_FOLL;
  else if( is_word(type(z)) && StringEqual(string(z), KW_FOLL_OR_PREC) )
    cross_type(y) = CROSS_FOLL_OR_PREC;
  else
  { Error(8, 16, "%s, %s or %s expected in left parameter of %s",
      WARN, &fpos(z), KW_PRECEDING, KW_FOLLOWING, KW_FOLL_OR_PREC, KW_TAGGED);
    y = MakeWord(WORD, STR_EMPTY, &fpos(x));
    ReplaceNode(y, x);  DisposeObject(x);
    x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
    return x;
  }

  /* make sure second argument (the new key) is ok */
  Child(tag, LastDown(x));
  tag = Manifest(tag, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
  tag = ReplaceWithTidy(tag, TRUE); /* && */
  if( !is_word(type(tag)) )
  { Error(8, 17, "right parameter of %s must be a simple word",
      WARN, &fpos(tag), KW_TAGGED);
    ifdebug(DOM, DD, DebugObject(tag));
    y = MakeWord(WORD, STR_EMPTY, &fpos(x));
    ReplaceNode(y, x);  DisposeObject(x);
    x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
    return x;
  }

  /* assemble insinuated cross reference which replaces x */
  ReplaceNode(tag, z);
  DisposeObject(z);
  ReplaceNode(y, x);
  DisposeObject(x);
  x = y;
  ReplaceWithSplit(x, bthr, fthr);
  debug1(DCR, DD, "  tagged manifesting %s", EchoObject(x));
  return x;
} /* end ManifestTg */


/*@::ManifestCl@**************************************************************/
/*                                                                           */
/*  OBJECT ManifestCl(x,env,style,bthr, fthr, target, crs, ok, need_expand,  */
/*                    enclose, fcr)                                          */
/*                                                                           */
/*  This procedure implements Manifest (see below) when x is CLOSURE.        */
/*                                                                           */
/*****************************************************************************/

static OBJECT ManifestCl(OBJECT x, OBJECT env, STYLE *style, OBJECT bthr[2],
OBJECT fthr[2], OBJECT *target, OBJECT *crs, BOOLEAN ok, BOOLEAN need_expand,
OBJECT *enclose, BOOLEAN fcr)
{ OBJECT y, link, sym, res_env, hold_env, hold_env2, z, newz, command;
  BOOLEAN symbol_free;

  sym = actual(x);
  StyleCopy(save_style(x), *style);
  debugcond2(DOM, D, StringEqual(SymName(sym), "@Section"),
     "manifesting %s at %s", SymName(sym), EchoFilePos(&fpos(x)));
  debug1(DOM, DD,  "  [ manifesting closure %s", SymName(sym));

  /* enclose, if required */
  if( *enclose != nilobj && (actual(x)==GalleySym || actual(x)==ForceGalleySym) )
  { OBJECT sym, par;
    ReplaceNode(*enclose, x);
    Child(sym, Down(*enclose));
    Child(par, Down(sym));
    DisposeChild(Down(par));
    Link(par, x);
    x = *enclose;
    *enclose = nilobj;
    debug1(DHY, DD, "  Manifest/enclose: %s", EchoObject(x));
    debug1(DOM, DD, "  Manifest/enclose: %s", EchoObject(x));
    x = Manifest(x, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
    debug0(DOM, DD,  "  ] returning from manifesting closure (enclose)");
    return x;
  }

  /* expand parameters where possible, and find if they are all free */
  symbol_free = TRUE;
  debugcond1(DOM, DD, indefinite(sym), "  freeing %s", EchoObject(x));
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    assert( type(y) == PAR, "Manifest/CLOSURE: type(y) != PAR!" );
    Child(z, Down(y));

    /* try to evaluate the actual parameter z */
    if( !is_word(type(z)) && !has_par(actual(y)) )
    {
      if( is_tag(actual(y)) || is_key(actual(y)) )
      {	z = Manifest(z, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
	z = ReplaceWithTidy(z, TRUE);
	if( !is_word(type(z)) )
	{
	  debug2(ANY, D, "z = %s %s", Image(type(z)), EchoObject(z));
	  Error(8, 41, "this %s is not a sequence of one or more words", FATAL,
	    &fpos(y), SymName(actual(y)));
	}
      }
      else if( type(z) == NEXT )
      {	z = Manifest(z, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
	z = ReplaceWithTidy(z, FALSE);
      }
      else if( type(z) == CLOSURE && is_par(type(actual(z))) )
      {
        /* see whether z would come to something simple if expanded */
        newz = ParameterCheck(z, env);
        debugcond2(DOM, DD, indefinite(sym), "  ParameterCheck(%s, env) = %s",
	  EchoObject(z), EchoObject(newz));
        if( newz != nilobj )
        { ReplaceNode(newz, z);
	  DisposeObject(z);
	  z = newz;
        }
      }
    }

    /*  now check z to see whether it is either a word or and ACAT of words */
    /* ***
    if( type(z) == ACAT )
    { int i = 0;  OBJECT t, tlink, g;
      tlink = Down(z);
      for( ; tlink != z && symbol_free;  tlink = NextDown(tlink), i++ )
      { Child(t, tlink);
	switch( type(t) )
	{
	  case WORD:
	  case QWORD:	if( i > 20 )  symbol_free = FALSE;
			break;

	  case GAP_OBJ:	if( Down(t) != t )
			{ Child(g, Down(t));
			  if( !is_word(type(g)) )  symbol_free = FALSE;
			}
			break;

	  default:	symbol_free = FALSE;
			break;

	}
      }
    }
    else
    *** */
    
    if( !is_word(type(z)) )
    { symbol_free = FALSE;
    }
  }
  debugcond2(DOM, DD, indefinite(sym),"  s_f = %s, x = %s",
    bool(symbol_free), EchoObject(x));

  /* if all parameters are free of symbols, optimize environment */
  if( symbol_free && imports(sym) == nilobj && enclosing(sym) != StartSym )
  { y = SearchEnv(env, enclosing(sym));
    if( y != nilobj && type(y) == CLOSURE )
    { OBJECT prntenv;
      Parent(prntenv, Up(y));
      if( type(prntenv) != ENV )  fprintf(stderr, "%s\n", Image(type(prntenv)));
      assert(type(prntenv) == ENV, "Manifest: prntenv!");
      if( Down(prntenv) == LastDown(prntenv) )
      { env = prntenv;
      }
      else
      { debug0(DCR, DDD, "calling SetEnv from Manifest (a)");
        env = SetEnv(y, nilobj);
      }
      New(hold_env2, ACAT);  Link(hold_env2, env);
    }
    else
    { /* *** letting this through now
      if( has_par(enclosing(sym)) )
	Error(8, 18, "symbol %s used outside %s", WARN, &fpos(x), SymName(sym),
	  SymName(enclosing(sym)));
      *** */
      hold_env2 = nilobj;
    }
  }
  else hold_env2 = nilobj;

  debug3(DOM, DD, " expansion: has_target %s, indefinite %s, recursive %s",
    bool(has_target(sym)), bool(indefinite(sym)), bool(recursive(sym)));
  if( has_target(sym) && !need_expand )
  {
    /* convert symbols with targets to unsized galleys */
    OBJECT hd;
    New(hd, HEAD);
    FposCopy(fpos(hd), fpos(x));
    actual(hd) = sym;
    limiter(hd) = opt_components(hd) = opt_constraints(hd) = nilobj;
    gall_dir(hd) = horiz_galley(sym);
    ready_galls(hd) = nilobj;
    must_expand(hd) = TRUE;
    sized(hd) = FALSE;
    ReplaceNode(hd, x);
    Link(hd, x);
    AttachEnv(env, x);
    SetTarget(hd);
    enclose_obj(hd) = (has_enclose(sym) ? BuildEnclose(hd) : nilobj);
    headers(hd) = dead_headers(hd) = nilobj;
    x = hd;
    threaded(x) = bthr[COLM] != nilobj || fthr[COLM] != nilobj;
    ReplaceWithSplit(x, bthr, fthr);
  }
  else if(
	    *target == sym			  ? (*target = nilobj, TRUE) :
	    need_expand				  ? TRUE  :
	    uses_galley(sym) && !recursive(sym)   ? TRUE  :
	    !indefinite(sym) && !recursive(sym)   ? TRUE  :
	    indefinite(sym)  && *target != nilobj ? SearchUses(sym, *target)
						  : FALSE
	 )
  {
    /* expand the closure and manifest the result */
    debug1(DOM, DD, "expanding; style: %s", EchoStyle(style));
    debug0(DCE, DD, "  calling ClosureExpand from Manifest/CLOSURE");
    /* *** now requesting cross refs always, not only if ok
    x = ClosureExpand(x, env, ok, crs, &res_env);
    *** */
    x = ClosureExpand(x, env, TRUE, crs, &res_env);
    New(hold_env, ACAT);  Link(hold_env, res_env);
    debug1(DOM, DD, "recursive call; style: %s", EchoStyle(style));
    if( type(x) == FILTERED )
    { assert( type(sym) == RPAR, "ManifestCl/filtered: type(sym)!" );
      assert( filter(enclosing(sym)) != nilobj, "ManifestCl filter-encl!" );
      New(command, CLOSURE);
      FposCopy(fpos(command), fpos(x));
      actual(command) = filter(enclosing(sym));
      FilterSetFileNames(x);
      command = Manifest(command,env,style,nbt,nft,&ntarget,crs,FALSE,FALSE, &nenclose, fcr);
      command = ReplaceWithTidy(command, TRUE);
      if( !is_word(type(command)) )
	Error(8, 19, "filter parameter of %s symbol is not simple",
	  FATAL, &fpos(command), SymName(enclosing(sym)));
      y = FilterExecute(x, string(command), res_env);
      debug2(DFH, D, "after \"%s\", will manifest result with style %s",
	string(command), EchoStyle(style));
      DisposeObject(command);
      ReplaceNode(y, x);
      DisposeObject(x);
      x = y;
    }
    x = Manifest(x, res_env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
    DisposeObject(hold_env);
  }
  else
  {
    AttachEnv(env, x);
    threaded(x) = bthr[COLM] != nilobj || fthr[COLM] != nilobj;
    debug0(DOM, DD,  "  closure; calling ReplaceWithSplit");
    ReplaceWithSplit(x, bthr, fthr);
  }
  if( hold_env2 != nilobj )  DisposeObject(hold_env2);
  debug0(DOM, DD,  "  ] returning from manifesting closure");
  return x;
} /* end ManifestCl */


/*@::Manifest()@**************************************************************/
/*                                                                           */
/*  OBJECT Manifest(x, env, style, bthr, fthr, target, crs, ok, need_expand, */
/*                  enclose, fcr)                                            */
/*                                                                           */
/*  Manifest object x, interpreted in environment env and style style.       */
/*  The result replaces x, and is returned also.                             */
/*  The manifesting operation converts x from a pure parse tree object       */
/*  containing closures and no threads, to an object ready for sizing,       */
/*  with fonts propagated to the words, fill styles propagated to the        */
/*  ACATs, and line spacings propagated to all interested parties.           */
/*  All non-recursive, non-indefinite closures are expanded.                 */
/*  Threads joining objects on a mark are constructed, and SPLIT objects     */
/*  inserted, so that sizing becomes a trivial operation.                    */
/*                                                                           */
/*  Manifest will construct threads and pass them up as children of bthr[]   */
/*  and fthr[] whenever non-nilobj values of these variables are passed in:  */
/*                                                                           */
/*      bthr[COLM]           protrudes upwards from x                        */
/*      fthr[COLM]           protrudes downwards from x                      */
/*      bthr[ROWM]           protrudes leftwards from x                      */
/*      fthr[ROWM]           protrudes rightwards from x                     */
/*                                                                           */
/*  If *target != nilobj, Manifest will expand indefinite closures leading   */
/*  to the first @Galley lying within an object of type *target.             */
/*                                                                           */
/*  If *target != nilobj and *enclose != nilobj, Manifest will enclose       */
/*  any @Galley or @ForceGalley it comes across in *enclose.                 */
/*                                                                           */
/*  The env parameter contains the environment in which x is to be           */
/*  evaluated.  Environments are shared, so their correct disposal is not    */
/*  simple.  The rule is this:  the code which creates an environment, or    */
/*  detaches it, is responsible for holding it with a dummy parent until     */
/*  it is no longer required.                                                */
/*                                                                           */
/*  Some objects x are not "real" in the sense that they do not give rise    */
/*  to rectangles in the final printed document.  The left parameter of      */
/*  @Wide and similar operators, and the gap following a concatenation       */
/*  operator, are examples of such non-real objects.  The ok flag is true    */
/*  when x is part of a real object.  This is needed because some things,    */
/*  such as the insinuation of cross references, the breaking of             */
/*  lines @Break ACAT objects, and conversion to small capitals, only apply  */
/*  to real objects.                                                         */
/*                                                                           */
/*  If *crs != nilobj, it points to a list of indexes to cross-references    */
/*  which are to be insinuated into the manifested form of x if x is real.   */
/*                                                                           */
/*  If need_expand is TRUE it forces closure x to expand.                    */
/*                                                                           */
/*  If fcr is TRUE, the objective is to expand until a cross-reference is    */
/*  the result; so expansion will stop at a CROSS or FORCE_CROSS object.     */
/*                                                                           */
/*  A postcondition of Manifest() is that the underline() flag is set to     */
/*  either UNDER_ON or UNDER_OFF in every WORD, every QWORD, and every child */
/*  of every ACAT, including the gaps.  This can be verified by checking     */
/*  that the WORD and QWORD cases set underline() to UNDER_OFF, and the ACAT */
/*  case sets every child of the ACAT to UNDER_OFF.  To see that the correct */
/*  subset of these flags gets changed to UNDER_ON, consult SetUnderline().  */
/*  The underline() flag is undefined otherwise, and should have value       */
/*  UNDER_UNDEF.                                                             */
/*                                                                           */
/*****************************************************************************/
#define MAX_DEPTH 1000

OBJECT Manifest(OBJECT x, OBJECT env, STYLE *style, OBJECT bthr[2],
OBJECT fthr[2], OBJECT *target, OBJECT *crs, BOOLEAN ok, BOOLEAN need_expand,
OBJECT *enclose, BOOLEAN fcr)
{ OBJECT bt[2], ft[2], y, link, gaplink, g;  register FULL_CHAR *p;
  OBJECT res, res_env, res_env2, hold_env, hold_env2, z, prev;
  OBJECT link1, link2, x1, x2, y1, y2;
  int par, num1, num2;  GAP res_gap;  unsigned res_inc;  STYLE new_style;
  BOOLEAN done, multiline;  FULL_CHAR ch;  float scale_factor;
  static int depth = 0;
#if DEBUG_ON
  static unsigned int debug_type[MAX_DEPTH];
  static OBJECT	      debug_actual[MAX_DEPTH];
  static int	      debug_lnum[MAX_DEPTH];
  BOOLEAN eee = (*enclose != nilobj);
  debug_type[depth] = type(x);
  debug_lnum[depth] = line_num(fpos(x));
  if( type(x) == CLOSURE ) debug_actual[depth] = actual(x);
  depth++;
  if( depth == MAX_DEPTH )
  { Error(8, 20, "maximum depth of symbol expansion (%d) reached",
      WARN, &fpos(x), MAX_DEPTH);
    Error(8, 21, "the symbols currently being expanded are:", WARN, &fpos(x));
    while( --depth >= 0 )
    {
      Error(8, 22, "at %d: %d %s %s", WARN, &fpos(x), depth, debug_lnum[depth],
	Image(debug_type[depth]), debug_type[depth] == CLOSURE ?
	  FullSymName(debug_actual[depth], AsciiToFull(".")) : STR_EMPTY);
    }
    Error(8, 23, "exiting now", FATAL, &fpos(x));
  }
#else
  depth++;
  if( depth == MAX_DEPTH )
  {
    Error(8, 40, "maximum depth of symbol expansion (%d) reached",
      FATAL, &fpos(x), MAX_DEPTH);
  }
#endif

  debug2(DOM, DD,   "[Manifest(%s %s )", Image(type(x)), EchoObject(x));
  debug1(DOM, DD,  "  environment: %s", EchoObject(env));
  debug6(DOM, DD,  "  style: %s;  target: %s;  threads: %s%s%s%s",
	EchoStyle(style), SymName(*target),
	bthr[COLM] ? " up"    : "",  fthr[COLM] ? " down"  : "",
	bthr[ROWM] ? " left"  : "",  fthr[ROWM] ? " right" : "");
  debugcond2(DHY, DD, eee, "[ Manifest(%s, *enclose = %s)",
    EchoObject(x), EchoObject(*enclose));

  switch( type(x) )
  {

    case ENV_OBJ:

      debug0(DHY, DD, "[Manifest env_obj:");
      ifdebug(DHY, DD, DebugObject(x));
      Child(y, Down(x));
      Child(res_env, NextDown(Down(x)));
      assert( type(res_env) == ENV, "Manifest/ENV_OBJ: res_env!");
      y = Manifest(y, res_env, style, bthr, fthr, target, crs, ok, TRUE, enclose, fcr);
      /* we always expand children of ENV_OBJ (need_expand == TRUE) */
      ReplaceNode(y, x);
      DisposeObject(x);
      x = y;
      debug1(DHY, DD, "]Manifest env_obj returning %s", EchoObject(x));
      break;


    case CLOSURE:
    
      x = ManifestCl(x, env, style, bthr, fthr, target, crs, ok, need_expand, enclose, fcr);
      break;


    case PAGE_LABEL:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, TRUE);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case NULL_CLOS:

      StyleCopy(save_style(x), *style);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case CROSS:
    case FORCE_CROSS:
    
      assert( Down(x) != x && LastDown(x) != Down(x), "Manifest: CROSS child!");
      if( !fcr )  /* stop if fcr, i.e. if purpose was to find a cross-reference */
      {
        debug0(DCR, DD, "  calling CrossExpand from Manifest/CROSS");
        Child(y, Down(x));
        if( type(y) == CLOSURE )
        {
	  /* *** want cross ref now always, not only if ok
	  x = CrossExpand(x, env, style, ok, crs, &res_env);
	  *** */
	  x = CrossExpand(x, env, style, crs, &res_env);
          assert( type(x) == CLOSURE, "Manifest/CROSS: type(x)!" );
          New(hold_env, ACAT);  Link(hold_env, res_env);
          /* expand here (calling Manifest immediately makes unwanted cr) */
          debug0(DCE, DD, "  calling ClosureExpand from Manifest/CROSS");
          x = ClosureExpand(x, res_env, FALSE, crs, &res_env2);
          New(hold_env2, ACAT);  Link(hold_env2, res_env2);
          x = Manifest(x, res_env2, style, bthr, fthr, target, crs, ok, TRUE, enclose, fcr);
          DisposeObject(hold_env);
          DisposeObject(hold_env2);
        }
        else
        { y = MakeWord(WORD, STR_EMPTY, &fpos(x));
          ReplaceNode(y, x);
          DisposeObject(x);
          x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
        }
      }
      break;


    case WORD:
    case QWORD:
    
      if( !ok || *crs == nilobj )
      {	word_font(x) = font(*style);
	word_colour(x) = colour(*style);
	word_outline(x) = outline(*style);
	word_language(x) = language(*style);
	word_hyph(x) = hyph_style(*style) == HYPH_ON;
	debug3(DOM, DDD, "  manfifest/WORD underline() := %s for %s %s",
	  "UNDER_OFF", Image(type(x)), EchoObject(x));
	if( small_caps(*style) && ok )  x = MapSmallCaps(x, style);
	underline(x) = UNDER_OFF;
	ReplaceWithSplit(x, bthr, fthr);
	break;
      }
      New(y, ACAT);
      FposCopy(fpos(y), fpos(x));
      ReplaceNode(y, x);
      Link(y, x);  x = y;
      /* NB NO BREAK! */


    case ACAT:
    
      StyleCopy(save_style(x), *style);
      adjust_cat(x) = padjust(*style);
      StyleCopy(new_style, *style);
      padjust(new_style) = FALSE;
      assert(Down(x) != x, "Manifest: ACAT!" );
      link = Down(x);  Child(y, link);
      assert( type(y) != GAP_OBJ, "Manifest ACAT: GAP_OBJ is first!" );
      multiline = FALSE;

      /* manifest first child and insert any cross references */
      if( is_word(type(y)) )
      { word_font(y) = font(*style);
	word_colour(y) = colour(*style);
	word_outline(y) = outline(*style);
	word_language(y) = language(*style);
	word_hyph(y) = hyph_style(*style) == HYPH_ON;
	if( small_caps(*style) && ok )  y = MapSmallCaps(y, style);
      }
      else y = Manifest(y, env, &new_style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
      debug3(DOM, DDD, "  manfifest/ACAT1 underline() := %s for %s %s",
	"UNDER_OFF", Image(type(y)), EchoObject(y));
      underline(y) = UNDER_OFF;
      /* ??? if( is_word(type(y)) ) */
      if( ok && *crs != nilobj )
      {	
	debug1(DCR, DD, "  insinuating %s", EchoObject(*crs));
	TransferLinks(Down(*crs), *crs, link);
	DisposeObject(*crs);
	*crs = nilobj;
      }
      prev = y;

      /* manifest subsequent gaps and children */
      for( gaplink = Down(link);  gaplink != x;  gaplink = NextDown(link) )
      {
	Child(g, gaplink);
	assert( type(g) == GAP_OBJ, "Manifest ACAT: no GAP_OBJ!" );
        debug3(DOM, DDD, "  manfifest/ACAT2 underline() := %s for %s %s",
	  "UNDER_OFF", Image(type(g)), EchoObject(g));
        underline(g) = UNDER_OFF;
	link = NextDown(gaplink);
	assert( link != x, "Manifest ACAT: GAP_OBJ is last!" );
	Child(y, link);
	assert( type(y) != GAP_OBJ, "Manifest ACAT: double GAP_OBJ!" );

	/* manifest the next child */
        debug1(DOM, DD, "  in ACAT (3), style = %s", EchoStyle(style));
	if( is_word(type(y)) )
	{ word_font(y) = font(*style);
	  word_colour(y) = colour(*style);
	  word_outline(y) = outline(*style);
	  word_language(y) = language(*style);
	  word_hyph(y) = hyph_style(*style) == HYPH_ON;
	  if( small_caps(*style) && ok )  y = MapSmallCaps(y, style);
	}
	else y = Manifest(y, env, &new_style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
        debug3(DOM, DDD, "  manifest/ACAT3 underline() := %s for %s %s",
	  "UNDER_OFF", Image(type(y)), EchoObject(y));
        underline(y) = UNDER_OFF;

	/* manifest the gap object */
	if( Down(g) != g )
	{
	  /* explicit & operator whose value is the child of g */
	  Child(z, Down(g));
	  z = Manifest(z, env, &new_style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
	  z = ReplaceWithTidy(z, FALSE);
	  GetGap(z, style, &gap(g), &res_inc);
	  vspace(g) = hspace(g) = 0;
	}
	else
	{
	  /* implicit & operator */
	  GapCopy(gap(g), space_gap(*style));
	  switch( space_style(*style) )
	  {
	    case SPACE_LOUT:

	      /* usual Lout spacing, the number of white space characters */
	      width(gap(g)) = width(gap(g)) * (vspace(g) + hspace(g));
	      break;


	    case SPACE_COMPRESS:

	      /* either zero or one space */
	      if( vspace(g) + hspace(g) == 0 )
	      { width(gap(g)) = 0;
	      }
	      else
	      { /* else width is like one space, so OK as is */
	      }
	      break;


	    case SPACE_SEPARATE:

	      /* exactly one space always, so do nothing further */
	      break;


	    case SPACE_TROFF:

	      /* Lout spacing plus one extra space for sentence end at eoln */
	      width(gap(g)) = width(gap(g)) * (vspace(g) + hspace(g));
	      debugcond2(DLS, DD, vspace(g) > 0, "  prev = %s %s",
		Image(type(prev)), EchoObject(prev));
	      if( vspace(g) > 0 )
	      { 
		/* set z to the preceding object; may need to search ACATs! */
		z = prev;
		while( type(z) == ACAT
		       || type(z) == ONE_COL || type(z) == ONE_ROW
		       || type(z) == HCONTRACT || type(z) == VCONTRACT )
		{ Child(z, LastDown(z));
		}

		/* if preceding object is a word, check for end sentence */
		if( is_word(type(z)) )
		{
		  for( p = string(z);  *p != '\0';  p++ );
		  debug4(DLS, DD, "  prev = %s, last = %c, LSE = %s, LWES = %s",
		    EchoObject(z), *(p-1), bool(LanguageSentenceEnds[*(p-1)]),
		    bool(LanguageWordEndsSentence(z, FALSE)));
		  if( p != string(z) && LanguageSentenceEnds[*(p-1)]
		      && LanguageWordEndsSentence(z, FALSE) )
		    width(gap(g)) += width(space_gap(*style));
		}
	      }
	      break;


	    case SPACE_TEX:

	      if( vspace(g) + hspace(g) == 0 )
	      {
		/* zero spaces gives zero result, as for compress above */
		width(gap(g)) = 0;
	      }
	      else
	      {
	        /* set z to the preceding object; may need to search ACATs! */
	        z = prev;
		while( type(z) == ACAT
		       || type(z) == ONE_COL || type(z) == ONE_ROW
		       || type(z) == HCONTRACT || type(z) == VCONTRACT )
	        { Child(z, LastDown(z));
	        }

	        /* one extra space if preceding is word ending sentence */
	        if( is_word(type(z)) )
	        {
		  for( p = string(z);  *p != '\0';  p++ );
		  debug4(DLS, DD, "  prev = %s, last = %c, LSE = %s, LWES = %s",
		      EchoObject(z), *(p-1), bool(LanguageSentenceEnds[*(p-1)]),
		      bool(LanguageWordEndsSentence(z, TRUE)));
		  if( p != string(z) && LanguageSentenceEnds[*(p-1)]
		      && LanguageWordEndsSentence(z, TRUE) )
		    width(gap(g)) += width(space_gap(*style));
	        }
	      }
	      break;


	    default:

	      assert(FALSE, "Manifest: unexpected space_style!");
	      break;
	  }
	  nobreak(gap(g)) = (width(gap(g)) == 0);
	  if( line_breaker(g) && is_definite(type(y)) )  multiline = TRUE;
	}
        debug1(DOM, DD, "  in ACAT, gap = %s", EchoLength(width(gap(g))));

	/* compress adjacent juxtaposed words of equal font, etc. */
	if( is_word(type(y)) && width(gap(g)) == 0 && nobreak(gap(g)) &&
	    vspace(g)+hspace(g)==0 &&
	    units(gap(g)) == FIXED_UNIT && mode(gap(g)) == EDGE_MODE &&
	    prev != nilobj && is_word(type(prev)) && !mark(gap(g)) &&
	    word_font(prev) == word_font(y) &&
	    word_colour(prev) == word_colour(y) &&
	    word_outline(prev) == word_outline(y) &&
	    word_language(prev) == word_language(y) )
	    /* no need to compare underline() since both are false */
	{ unsigned typ;
	  assert( underline(prev) == UNDER_OFF, "Manifest/ACAT: underline(prev)!" );
	  assert( underline(y) == UNDER_OFF, "Manifest/ACAT: underline(y)!" );
	  if( StringLength(string(prev))+StringLength(string(y)) >= MAX_BUFF )
	    Error(8, 24, "word %s%s is too long",
	      FATAL, &fpos(prev), string(prev), string(y));
	  z = y;
	  typ = type(prev) == QWORD || type(y) == QWORD ? QWORD : WORD;
	  y = MakeWordTwo(typ, string(prev), string(y), &fpos(prev));
	  word_font(y) = word_font(prev);
	  word_colour(y) = word_colour(prev);
	  word_outline(y) = word_outline(prev);
	  word_language(y) = word_language(prev);
	  word_hyph(y) = word_hyph(prev);
	  underline(y) = UNDER_OFF;
          debug3(DOM, DDD, "  manifest/ACAT4 underline() := %s for %s %s",
	    "UNDER_OFF", Image(type(y)), EchoObject(y));
	  MoveLink(link, y, CHILD);
	  DisposeObject(z);
	  DisposeChild(Up(prev));
	  DisposeChild(gaplink);
	}
	prev = y;

	/* insinuate any cross-references */
	if( ok && *crs != nilobj )
	{
	  debug1(DCR, DD, "  insinuating %s", EchoObject(*crs));
	  TransferLinks(Down(*crs), *crs, link);
	  DisposeObject(*crs);
	  *crs = nilobj;
	}
      }

      /* implement FILL_OFF break option if required */
      /* *** this has been moved now to MinSize, z12.c *** */

      /* ***
      if( ok && multiline && fill_style(*style) == FILL_UNDEF )
	Error(8, 25, "missing %s symbol or option", FATAL, &fpos(x), KW_BREAK);
      if( ok && multiline && fill_style(*style) == FILL_OFF )
      {	OBJECT prev_acat, new_acat;  BOOLEAN jn;

	%* compress any ACAT children of ACAT x *%
	for( link = x;  NextDown(link) != x;  link = NextDown(link) )
	{ Child(y, NextDown(link));
	  if( type(y) == ACAT )
	  { TransferLinks(Down(y), y, NextDown(link));
	    DisposeChild(Up(y));
	    link = PrevDown(link);
	  }
	}

	%* do line breaks now *%
	prev_acat = x;
	New(x, VCAT);
	adjust_cat(x) = FALSE;
	ReplaceNode(x, prev_acat);
	Link(x, prev_acat);
	FirstDefinite(prev_acat, link, y, jn);
	if( link != prev_acat )
	{
	  NextDefiniteWithGap(prev_acat, link, y, g, jn);
	  while( link != prev_acat )
	  {
	    if( mode(gap(g)) != NO_MODE && line_breaker(g) )
	    { OBJECT glink = PrevDown(Up(g));
	      debug2(DOM, DD, "lines gap just before definite %s at %s",
		Image(type(y)), EchoFilePos(&fpos(y)));
	      MoveLink(NextDown(glink), x, PARENT);
	      GapCopy(gap(g), line_gap(*style));
	      width(gap(g)) *= find_max(1, vspace(g));
	      New(new_acat, ACAT);
	      adjust_cat(new_acat) = padjust(*style);
	      FposCopy(fpos(new_acat), fpos(g));
	      if( hspace(g) > 0 )
	      { z = MakeWord(WORD, STR_EMPTY, &fpos(g));
	        word_font(z) = font(*style);
	        word_colour(z) = colour(*style);
	        word_outline(z) = outline(*style);
	        word_language(z) = language(*style);
	        word_hyph(z) = hyph_style(*style) == HYPH_ON;
		underline(z) = UNDER_OFF;
	        Link(new_acat, z);
	        New(z, GAP_OBJ);
	        hspace(z) = hspace(g);
	        vspace(z) = 0;
	        underline(z) = UNDER_OFF;
	        GapCopy(gap(z), space_gap(*style));
	        width(gap(z)) *= hspace(z);
	        Link(new_acat, z);
	      }
	      TransferLinks(NextDown(glink), prev_acat, new_acat);
	      StyleCopy(save_style(new_acat), *style);
	      Link(x, new_acat);
	      prev_acat = new_acat;
	      glink = prev_acat;
	    }
	    NextDefiniteWithGap(prev_acat, link, y, g, jn);
	  }
	}

	%* remove any singleton ACAT objects under x, if they are VCATs *%
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ Child(y, link);
	  if( type(y) == ACAT && Down(y) == LastDown(y) )
	  { Child(z, Down(y));
	    if( type(z) == VCAT )
	    { MoveLink(link, z, CHILD);
	      DisposeObject(y);
	    }
	  }
	}
      }
      *** */

      ReplaceWithSplit(x, bthr, fthr);
      break;


    case HCAT:
    case VCAT:

     x = ManifestCat(x, env, style, bthr, fthr, target, crs, ok, need_expand,
       enclose, fcr);
     break;


    case WIDE:
    case HIGH:
    
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      GetGap(y, style, &res_gap, &res_inc);
      if( res_inc != GAP_ABS || mode(res_gap) != EDGE_MODE ||
	units(res_gap) != FIXED_UNIT )
      {	Error(8, 26, "replacing invalid left parameter of %s by 2i",
	  WARN, &fpos(y), Image(type(x)) );
	units(res_gap) = FIXED_UNIT;
	width(res_gap) = 2*IN;
      }
      SetConstraint(constraint(x), MAX_FULL_LENGTH, width(res_gap), MAX_FULL_LENGTH);
      DisposeChild(Down(x));
      goto ETC;		/* two cases down from here */


    case HSHIFT:
    case VSHIFT:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      GetGap(y, style, &shift_gap(x), &res_inc);
      shift_type(x) = res_inc;
      if( mode(shift_gap(x)) != EDGE_MODE || 
	  (units(shift_gap(x))!=FIXED_UNIT && units(shift_gap(x))!=NEXT_UNIT) )
      {	Error(8, 27, "replacing invalid left parameter of %s by +0i",
	  WARN, &fpos(y), Image(type(x)) );
	shift_type(x) = GAP_INC;
	units(shift_gap(x)) = FIXED_UNIT;
	width(shift_gap(x)) = 0;
	mode(shift_gap(x)) = EDGE_MODE;
      }
      DisposeChild(Down(x));
      goto ETC;		/* next case down from here */


    case HCONTRACT:
    case VCONTRACT:
    case HLIMITED:
    case VLIMITED:
    case HEXPAND:
    case VEXPAND:
    case ONE_COL:
    case ONE_ROW:
    
      ETC:
      par = (type(x)==ONE_COL || type(x)==HEXPAND || type(x) == HCONTRACT ||
	   type(x)==HLIMITED || type(x)==WIDE || type(x)==HSHIFT) ? COLM : ROWM;
      Child(y, Down(x));

      /* manifest the child, propagating perp threads and suppressing pars */
      bt[par] = ft[par] = nilobj;
      bt[1-par] = bthr[1-par];  ft[1-par] = fthr[1-par];
      y = Manifest(y, env, style, bt, ft, target, crs, ok, FALSE, enclose, fcr);

      /* replace with split object if par threads needed */
      bt[par] = bthr[par];  ft[par] = fthr[par];
      bt[1-par] = ft[1-par] = nilobj;
      ReplaceWithSplit(x, bt, ft);
      break;


    case BEGIN_HEADER:
    case SET_HEADER:

      /* first manifest gap, which is left parameter */
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE,
	&nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      GetGap(y, style, &line_gap(save_style(x)), &res_inc);

      /* now the right parameter */
      Child(y, LastDown(x));
      y = Manifest(y, env, style, bthr, fthr, target, crs, ok, need_expand,
	enclose, fcr);
      break;


    case END_HEADER:
    case CLEAR_HEADER:

      /* give these objects a dummy child, just so that threads can attach */
      /* to it and keep the thread code happy.  Don't use ReplaceWithSplit */
      /* because we don't want a split above a header                      */
      y = MakeWord(WORD, STR_EMPTY, &fpos(x));
      Link(x, y);
      y = Manifest(y, env, style, bthr, fthr, target, crs, ok, need_expand,
	enclose, fcr);
      break;


    case HSPAN:
    case VSPAN:

      ReplaceWithSplit(x, bthr, fthr);
      break;


    case ROTATE:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      GetGap(y, style, &res_gap, &res_inc);
      if( res_inc != GAP_ABS || mode(res_gap) != EDGE_MODE ||
		units(res_gap) != DEG_UNIT )
      {	Error(8, 28, "replacing invalid left parameter of %s by 0d",
	  WARN, &fpos(y), Image(type(x)) );
	units(res_gap) = DEG_UNIT;
	width(res_gap) = 0;
      }
      sparec(constraint(x)) = width(res_gap);
      DisposeChild(Down(x));
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE,enclose,fcr);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case BACKGROUND:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE,enclose,fcr);
      Child(y, LastDown(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE,enclose,fcr);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case START_HVSPAN:
    case START_HSPAN:
    case START_VSPAN:
    case HSCALE:
    case VSCALE:
    case HCOVER:
    case VCOVER:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE,enclose,fcr);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case SCALE:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      if( is_word(type(y)) && StringEqual(string(y), STR_EMPTY) )
      {
	/* missing scale factor, meaning to be inserted automatically */
        bc(constraint(x)) = fc(constraint(x)) = 0;  /* work out later */
      }
      else if( type(y) != ACAT )
      {
	/* presumably one word, common factor for horizontal and vertical */
	scale_factor = GetScaleFactor(y);
        bc(constraint(x)) = fc(constraint(x)) = scale_factor * SF;
      }
      else
      {
	/* get horizontal scale factor */
	Child(z, Down(y));
	scale_factor = GetScaleFactor(z);
        bc(constraint(x)) = scale_factor * SF;

	/* get vertical scale factor */
	Child(z, LastDown(y));
	scale_factor = GetScaleFactor(z);
        fc(constraint(x)) = scale_factor * SF;
      }
      DisposeChild(Down(x));
      Child(y, LastDown(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case KERN_SHRINK:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      Child(y, LastDown(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case YIELD:

      Error(8, 29, "%s not expected here", FATAL, &fpos(x), KW_YIELD);
      break;


    case RAW_VERBATIM:
    case VERBATIM:

      Child(y, Down(x));
      y = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      DeleteLink(Down(x));
      MergeNode(y, x);  x = y;
      break;


    case CASE:

      x = ManifestCase(x,env,style, bthr, fthr, target, crs, ok, need_expand, enclose, fcr);
      break;


    case BACKEND:

      res = MakeWord(WORD, BackEnd->name, &fpos(x));
      ReplaceNode(res, x);
      DisposeObject(x);
      x = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      break;


    case XCHAR:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      if( !is_word(type(y)) )
      {	Error(8, 30, "%s dropped (parameter is not a simple word)",
	  WARN, &fpos(y), KW_XCHAR);
	res = MakeWord(WORD, STR_EMPTY, &fpos(x));
      }
      else if( (word_font(y) = font(*style)) == 0 )
      {	Error(8, 31, "%s dropped (no current font at this point)",
	  WARN, &fpos(y), KW_XCHAR);
	res = MakeWord(WORD, STR_EMPTY, &fpos(x));
      }
      else
      { ch = MapCharEncoding(string(y), FontMapping(word_font(y), &fpos(y)));
        if( ch == '\0' )
        { type(y) = QWORD;
	  Error(8, 32, "%s dropped (character %s unknown in font %s)",
	    WARN, &fpos(y), KW_XCHAR, StringQuotedWord(y),
	    FontFamilyAndFace(word_font(y)));
	  res = MakeWord(WORD, STR_EMPTY, &fpos(x));
        }
        else
        { res = MakeWord(QWORD, STR_SPACE, &fpos(x));
	  string(res)[0] = ch;
        }
      }
      ReplaceNode(res, x);
      DisposeObject(x);
      x = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      break;


    case CURR_LANG:

      if( language(*style) == 0 )
      { Error(8, 33, "no current language at this point, using %s",
	  WARN, &fpos(x), STR_NONE);
	res = MakeWord(WORD, STR_NONE, &fpos(x));
      }
      else res = MakeWord(WORD, LanguageString(language(*style)), &fpos(x));
      ReplaceNode(res, x);
      DisposeObject(x);
      x = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      break;


    case CURR_FAMILY:
    case CURR_FACE:

      if( font(*style) == 0 )
      { Error(8, 38, "no current font at this point, using %s",
	  WARN, &fpos(x), STR_NONE);
	res = MakeWord(WORD, STR_NONE, &fpos(x));
      }
      else if( type(x) == CURR_FAMILY )
	res = MakeWord(WORD, FontFamily(font(*style)), &fpos(x));
      else
	res = MakeWord(WORD, FontFace(font(*style)), &fpos(x));
      ReplaceNode(res, x);
      DisposeObject(x);
      x = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      break;


    case CURR_YUNIT:
    case CURR_ZUNIT:

      { FULL_CHAR buff[20];
        if( type(x) == CURR_YUNIT )
          sprintf( (char *) buff, "%dp", yunit(*style) / PT);
        else
	  sprintf( (char *) buff, "%dp", zunit(*style) / PT);
        res = MakeWord(WORD, buff, &fpos(x));
      }
      ReplaceNode(res, x);
      DisposeObject(x);
      x = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      break;


    case FONT:
    case SPACE:
    case YUNIT:
    case ZUNIT:
    case BREAK:
    case COLOUR:
    case LANGUAGE:
    
      assert( Down(x) != x && NextDown(Down(x)) != x, "Manifest: FONT!" );
      StyleCopy(new_style, *style);
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, type(x) == COLOUR);
      switch( type(x) )
      {
	case FONT:	FontChange(&new_style, y);
			break;

	case SPACE:	SpaceChange(&new_style, y);
			break;

	case YUNIT:	YUnitChange(&new_style, y);
			break;

	case ZUNIT:	ZUnitChange(&new_style, y);
			break;

	case BREAK:	BreakChange(&new_style, y);
			break;
	
	case COLOUR:	ColourChange(&new_style, y);
			break;

	case LANGUAGE:	LanguageChange(&new_style, y);
			break;

      }
      DisposeChild(Down(x));
      Child(y, Down(x));
      y = Manifest(y, env, &new_style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      DeleteLink(Down(x));
      MergeNode(y, x);  x = y;
      break;


    case OUTLINE:
    case PADJUST:
    case HADJUST:
    case VADJUST:

      StyleCopy(new_style, *style);
      if(      type(x) == OUTLINE )  outline(new_style) = TRUE;
      else if( type(x) == VADJUST )  vadjust(new_style) = TRUE;
      else if( type(x) == HADJUST )  hadjust(new_style) = TRUE;
      else                           padjust(new_style) = TRUE;
      Child(y, Down(x));
      y = Manifest(y, env, &new_style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      DeleteLink(Down(x));
      MergeNode(y, x);  x = y;
      break;


    case UNDERLINE:

      /* change x to an ACAT and set the underline flags in its child */
      assert( Down(x) != x && NextDown(Down(x)) == x, "Manifest: UNDERLINE!" );
      type(x) = ACAT;
      adjust_cat(x) = padjust(*style);
      padjust(*style) = FALSE;
      StyleCopy(save_style(x), *style);
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
      SetUnderline(x);
      ReplaceWithSplit(x, bthr, fthr);
      break;


    case MELD:
    case COMMON:
    case RUMP:

      assert( Down(x) != x && NextDown(Down(x)) != x, "Manifest: COMMON!" );
      debug2(DHY, DDD, "[Manifest %s %s", EchoObject(x), EchoObject(env));

      /* find the first child of x, make sure it is an ACAT, and manifest */
      Child(x1, Down(x));
      if( type(x1) != ACAT )
      { OBJECT newx1;
	New(newx1, ACAT);
        adjust_cat(newx1) = padjust(*style);
	padjust(*style) = FALSE;
        MoveLink(Down(x), newx1, CHILD);
        Link(newx1, x1);
        x1 = newx1;
      }
      x1 = Manifest(x1, env, style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
      link1 = x1;
      while( NextDown(link1) != x1 )
      { Child(z, NextDown(link1));
	if( type(z) == ACAT )
	{ TransferLinks(Down(z), z, NextDown(link1));
	  DisposeChild(Up(z));
	}
	else link1 = NextDown(link1);
      }
      debug1(DHY, DDD, "  manifested x1 = %s", EchoObject(x1));
 
      /* find the second child of x, make sure it is an ACAT, and manifest */
      Child(x2, NextDown(Down(x)));
      if( type(x2) != ACAT )
      { OBJECT newx2;
	New(newx2, ACAT);
        adjust_cat(newx2) = padjust(*style);
	padjust(*style) = FALSE;
        MoveLink(NextDown(Down(x)), newx2, CHILD);
        Link(newx2, x2);
        x2 = newx2;
      }
      x2 = Manifest(x2, env, style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
      link2 = x2;
      while( NextDown(link2) != x2 )
      { Child(z, NextDown(link2));
	if( type(z) == ACAT )
	{ TransferLinks(Down(z), z, NextDown(link2));
	  DisposeChild(Up(z));
	}
	else link2 = NextDown(link2);
      }
      debug1(DHY, DDD, "  manifested x2 = %s", EchoObject(x2));
     
      if( type(x) == MELD )
      {
        /* if Meld, result is Meld(x1, x2) */
        res = Meld(x1, x2);
      }
      else
      {

        /* find the point where x1 and x2 begin to differ */
        link1 = Down(x1);
        link2 = Down(x2);
        while( link1 != x1 && link2 != x2 )
        {
	  Child(y1, link1);
	  Child(y2, link2);
	  debug1(DHY, DDD, "    y1 = %s", EchoObject(y1));
	  debug1(DHY, DDD, "    y2 = %s", EchoObject(y2));
	  if( is_word(type(y1)) && is_word(type(y2)) )
	  {
	    if( !StringEqual(string(y1), string(y2)) )  break;
	  }
	  else if( type(y1) != type(y2) )  break;
	  link1 = NextDown(link1);
	  link2 = NextDown(link2);
        }

        /* if COMMON, result is x1 or x2 if either ran out,             */
        /* or else x2 (say) up to but not including link2 and prec gap  */
        if( type(x) == COMMON )
        { if( link2 == x2 )
	  { res = x2;
	  }
	  else if( link1 == x1 )
	  { res = x1;
	  }
	  else
	  { if( link2 == Down(x2) )
	      res = MakeWord(WORD, STR_EMPTY, &fpos(x2));
	    else
	    { TransferLinks(PrevDown(link2), x2, x1);
	      res = x2;
	    }
	  }
        }

        /* if RUMP, result is x2 starting from link2 or NextDown(link2) */
        else if( type(x) == RUMP )
        { if( link2 == x2 )
	    res = MakeWord(WORD, STR_EMPTY, &fpos(x2));
	  else if( link1 == x1 )
	  { 
	    TransferLinks(Down(x2), NextDown(link2), x1);
	    res = x2;
	  }
	  else /* link1 != x1 */
	  {
	    TransferLinks(Down(x2), link2, x1);
	    res = x2;
	  }
        }
      }

      /* now res replaces x */
      ReplaceNode(res, x);
      DisposeObject(x);
      x = res;
      ReplaceWithSplit(x, bthr, fthr);
      debug1(DHY, DDD, "]Manifest returning %s", EchoObject(x));
      break;


    case INSERT:

      /* manifest and detach the left parameter, call it z */
      Child(z, Down(x));
      z = Manifest(z, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      DeleteLink(Down(x));

      /* manifest the right parameter and make it the result */
      Child(y, LastDown(x));
      y = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      DeleteLink(LastDown(x));
      MergeNode(y, x);  x = y;

      /* now find the reattachment point for z down inside the result, x */
      x = InsertObject(x, &z, style);
      if( z != nilobj )
      { Error(8, 34, "object dropped by %s: no suitable insert point", WARN,
	  &fpos(x), KW_INSERT);
	DisposeObject(z);
      }
      break;


    case ONE_OF:

      Child(y, Down(x));
      if( type(y) != ACAT )
      {
	/* child is not a sequence of choices, so ignore ONE_OF */
	Error(8, 39, "%s ignored: no choices in right parameter", WARN,
	  &fpos(x), KW_ONE_OF);
        y = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE,
	      enclose, fcr);
        DeleteLink(Down(x));
        MergeNode(y, x);  x = y;
      }
      else
      {
	/* try each child in turn; result is first to find *target */
	OBJECT target_before;
	for( link = Down(y);  link != y;  link = NextDown(link) )
	{
	  Child(z, link);
	  if( type(z) == GAP_OBJ )  continue;
	  target_before = *target;
	  z = Manifest(z, env, style, bthr, fthr, target, crs, ok, FALSE,
	    enclose, fcr);
	  if( *target != target_before )
	    break;
	}
	DeleteLink(Up(z));
	ReplaceNode(z, x);
	DisposeObject(x);
	x = z;
      }
      break;


    case NEXT:

      assert( Down(x) != x, "Manifest/NEXT: Down(x) == x!" );
      Child(y, Down(x));
      debug1(DCS, DD, "  Manifesting Next( %s, 1 )", EchoObject(y));
      y = Manifest(y, env, style, bthr, fthr, target, crs, FALSE, FALSE, enclose, fcr);
      debug1(DCS, DD, "  calling Next( %s, 1 )", EchoObject(y));
      done = FALSE;
      y = Next(y, 1, &done);
      debug2(DCS, DD, "  Next(done = %s) returning %s",
			bool(done), EchoObject(y));
      DeleteLink(Down(x));
      MergeNode(y, x);  x = y;
      break;


    case PLUS:
    case MINUS:

      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      Child(z, NextDown(Down(x)));
      z = Manifest(z, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      z = ReplaceWithTidy(z, FALSE);
      if( is_word(type(y)) && sscanf( (char *) string(y), "%d", &num1) == 1 &&
          is_word(type(z)) && sscanf( (char *) string(z), "%d", &num2) == 1 )
      {
	FULL_CHAR buff[MAX_BUFF];
	sprintf( (char *) buff, "%d", type(x) == PLUS ? num1+num2 : num1-num2);
	res = MakeWord(WORD, buff, &fpos(x));
      }
      else
      { res = MakeWord(WORD, STR_NOCROSS, &fpos(x));
      }
      debug4(DOM, DD, "{ %s } %s { %s } = %s", EchoObject(y), Image(type(x)),
	EchoObject(z), EchoObject(res));
      res = Manifest(res, env, style, bthr, fthr, target, crs, FALSE, FALSE, enclose, fcr);
      ReplaceNode(res, x);
      DisposeObject(x);
      x = res;
      break;


    case OPEN:

      debug0(DCR, DD, "  [ Manifest/OPEN begins:");
      Child(y, Down(x));
      DeleteLink(Down(x));
      Child(res, LastDown(x));
      hold_env = nilobj;
      if( type(y) == CLOSURE )
      { AttachEnv(env, y);
	StyleCopy(save_style(y), *style);
	debug0(DCR, DDD, "calling SetEnv from Manifest (b)");
	res_env = SetEnv(y, nilobj);
	New(hold_env, ACAT);  Link(hold_env, res_env);
	res = Manifest(res, res_env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      }
      else if( is_cross(type(y)) )
      { Child(z, Down(y));
	if( type(z) == CLOSURE )
	{ debug0(DCR, DD, "  calling CrossExpand from Manifest/OPEN");
	  /* *** want cross ref now always, not only if ok
	  y = CrossExpand(y, env, style, ok, crs, &res_env);
	  *** */
	  y = CrossExpand(y, env, style, crs, &res_env);
	  AttachEnv(res_env, y);
	  debug0(DCR, DDD, "calling SetEnv from Manifest (c)");
	  res_env = SetEnv(y, env);
	  New(hold_env, ACAT);  Link(hold_env, res_env);
	  res = Manifest(res, res_env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
	}
	else
	{ Error(8, 35, "invalid left parameter of %s", WARN, &fpos(y), KW_OPEN);
	  res = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
	}
      }
      else
      {	Error(8, 36, "invalid left parameter of %s", WARN, &fpos(y), KW_OPEN);
	res = Manifest(res, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
      }
      ReplaceNode(res, x);
      DisposeObject(x);
      if( hold_env != nilobj )  DisposeObject(hold_env);
      x = res;
      debug0(DCR, DD, "  ] Manifest/OPEN ends");
      break;


    case TAGGED:

      x = ManifestTg(x, env, style, bthr, fthr, target, crs, ok, need_expand, enclose, fcr);
      debug2(DCR, DD, "Manifest returning %ld %s", (long) x, EchoObject(x));
      break;


    case PLAIN_GRAPHIC:
    case GRAPHIC:

      debug1(DRS, DD, "  graphic style in Manifest = %s", EchoStyle(style));
      Child(y, LastDown(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok, FALSE, enclose, fcr);
      StyleCopy(save_style(x), *style);
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      ReplaceWithSplit(x, bthr, fthr);
      break;
	

    case LINK_SOURCE:
    case LINK_DEST:

      Child(y, LastDown(x));
      y = Manifest(y, env, style, nbt, nft, target, crs, ok,FALSE,enclose,fcr);
      StyleCopy(save_style(x), *style);
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE,
	&nenclose, fcr);
      y = ReplaceWithTidy(y, TRUE);
      ReplaceWithSplit(x, bthr, fthr);
      break;
	

    case INCGRAPHIC:
    case SINCGRAPHIC:

      StyleCopy(save_style(x), *style);
      debug2(DGP, DD, "  manifest at %s (style %s)",
	EchoObject(x), EchoStyle(&save_style(x)));
      Child(y, Down(x));
      y = Manifest(y, env, style, nbt, nft, &ntarget, crs, FALSE, FALSE, &nenclose, fcr);
      y = ReplaceWithTidy(y, FALSE);
      if( !is_word(type(y)) )
      { Error(8, 37, "%s deleted (invalid right parameter)", WARN, &fpos(y),
	  type(x) == INCGRAPHIC ? KW_INCGRAPHIC : KW_SINCGRAPHIC);
	y = MakeWord(WORD, STR_EMPTY, &fpos(x));
	ReplaceNode(y, x);  DisposeObject(x);
	x = Manifest(y, env, style, bthr, fthr, target, crs, ok, FALSE, enclose, fcr);
	return x;
      }
      ReplaceWithSplit(x, bthr, fthr);
      break;
	

    default:

      assert1(FALSE, "Manifest:", Image(type(x)));
      break;

  } /* end switch */

  debug2(DOM, DD, "]Manifest returning %s %s", Image(type(x)), EchoObject(x));
  debug1(DOM, DD, "  at exit, style = %s", EchoStyle(style));
  debug1(DOM, DDD, "up:    ", EchoObject(bthr[COLM]));
  debug1(DOM, DDD, "down:  ", EchoObject(fthr[COLM]));
  debug1(DOM, DDD, "left:  ", EchoObject(bthr[ROWM]));
  debug1(DOM, DDD, "right: ", EchoObject(fthr[ROWM]));
  debugcond1(DHY, DD, eee, "] Manifest returning %s", EchoObject(x));
  depth--;
  return x;
} /* end Manifest */
