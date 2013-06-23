/*@z21.c:Galley Maker:SizeGalley()@*******************************************/
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
/*  FILE:         z21.c                                                      */
/*  MODULE:       Galley Maker                                               */
/*  EXTERNS:      SizeGalley()                                               */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

/*****************************************************************************/
/*                                                                           */
/*  SizeGalley(hd, env, rows, joined, nonblock, trig, style, c, target,      */
/*                                     dest_index, recs, inners, enclose)    */
/*                                                                           */
/*  Convert unsized galley hd into sized format.  The input parameters are:  */
/*                                                                           */
/*    hd          the galley to be converted                                 */
/*    env         its environment (needs to be "held" while manifesting)     */
/*    rows        TRUE if the resulting galley may have more than one row    */
/*    joined      TRUE if the resulting galley must be simply joined         */
/*    nonblock    Set the non_blocking() field of RECEPTIVEs to this value   */
/*    trig        TRUE if indefinites of hd may trigger external galleys     */
/*    *style      The initial style                                          */
/*    *c          the width constraint hd should conform to                  */
/*    target      if non-nilobj, expand indefinite objects to reveal a       */
/*                @Galley within this symbol                                 */
/*    enclose     If non-nilobj, enclose any @Galley symbol encountered      */
/*                during manifesting by this symbol.                         */
/*                                                                           */
/*  The output parameters, in addition to the converted hd, are:             */
/*                                                                           */
/*    dest_index  the index of the @Galley found within target, if any       */
/*    recs        list of all RECURSIVE indexes found (or nilobj if none)    */
/*    inners      list of all UNATTACHED indexes found (or nilobj if none),  */
/*                not including any that come after the target or InputSym.  */
/*                                                                           */
/*****************************************************************************/

void SizeGalley(OBJECT hd, OBJECT env, BOOLEAN rows, BOOLEAN joined,
BOOLEAN nonblock, BOOLEAN trig, STYLE *style, CONSTRAINT *c, OBJECT target,
OBJECT *dest_index, OBJECT *recs, OBJECT *inners, OBJECT enclose)
{ OBJECT y, link, z, crs, t, tlink, zlink, tmp, why;
  OBJECT extras, tmp1, tmp2, bt[2], ft[2], hold_env;
  BOOLEAN after_target;
  
  assert( type(hd) == HEAD && Down(hd) != hd, "SizeGalley: precondition!" );
  assert( !sized(hd), "SizeGalley: already sized!" );
  debug6(DGM, D, "SizeGalley(%s, -, %s, %s, %s, %s, -, %s, -, -, -), hd =",
	SymName(actual(hd)), bool(rows), bool(joined), bool(nonblock),
	bool(trig), EchoConstraint(c));
  debug1(DGM, DD, "  env = %s", EchoObject(env));
  ifdebug(DGM, D, DebugObject(hd));

  /* manifest the child of hd, making sure it is simply joined if required */
  Child(y, Down(hd));
  tmp1 = target;
  tmp2 = enclose;
  crs = nilobj;
  bt[COLM] = ft[COLM] = bt[ROWM] = ft[ROWM] = nilobj;
  New(hold_env, ACAT);  Link(hold_env, env);
  if( AllowCrossDb && type(y) == CLOSURE && has_optimize(actual(y))
      && FindOptimize(y, env) )
  {
    SetOptimize(hd, style);
  }
  debug2(DOM, D, "[ calling Manifest(%s) from SizeGalley(%s)",
    Image(type(y)), SymName(actual(hd)));
  debug2(DOB, D, "[ calling Manifest(%s) from SizeGalley(%s)",
    Image(type(y)), SymName(actual(hd)));
  if( joined )
  { New(bt[COLM], THREAD);  New(ft[COLM], THREAD);
    debug0(DGM, DD, "SizeGalley calling Manifest (joined)");
    y = Manifest(y, env, style, bt, ft, &tmp1, &crs, TRUE, must_expand(hd),
      &tmp2, FALSE);
    assert( Down(bt[COLM]) != bt[COLM] && Down(ft[COLM]) != ft[COLM],
	"SizeGalley: threads!" );
    Child(tmp1, Down(bt[COLM]));  Child(tmp2, Down(ft[COLM]));
    if( Down(bt[COLM]) != LastDown(bt[COLM]) ||
	  Down(ft[COLM]) != LastDown(ft[COLM]) || tmp1 != tmp2 )
      Error(21, 1, "galley %s must have just one column mark",
	FATAL, &fpos(y), SymName(actual(hd)) );
    DisposeObject(bt[COLM]);  DisposeObject(ft[COLM]);
  }
  else
  { debug0(DGM, DD, "SizeGalley calling Manifest (not joined)");
    y = Manifest(y, env, style, bt, ft, &tmp1, &crs, TRUE, must_expand(hd),
      &tmp2, FALSE);
  }
  debug2(DOM, D, "] returning Manifest(%s) from SizeGalley(%s)",
    Image(type(y)), SymName(actual(hd)));
  debug2(DOB, D, "] returning Manifest(%s) from SizeGalley(%s)",
    Image(type(y)), SymName(actual(hd)));
  DisposeObject(hold_env);
  debug0(DGM, DD, "SizeGalley: after manifesting, hd =");
  ifdebug(DGM, DD, DebugObject(hd));

  /* horizontally size hd */
  debug0(DGM, DD, "SizeGalley horizontally sizing hd:");
  New(extras, ACAT);
  debug2(DSF, D, "[ calling MinSize(%s) from SizeGalley(%s)",
    Image(type(y)), SymName(actual(hd)));
  y = MinSize(y, COLM, &extras);
  debug2(DSF, D, "] returning MinSize(%s) from SizeGalley(%s)",
    Image(type(y)), SymName(actual(hd)));

  /* break hd if vertical galley */
  if( gall_dir(hd) == ROWM )
  {
    CopyConstraint(constraint(hd), *c);
    debug0(DGM, DD, "SizeGalley calling BreakObject:");
    debug2(DOB, D, "[ calling BreakObject(%s) from SizeGalley(%s)",
      Image(type(y)), SymName(actual(hd)));
    y = BreakObject(y, c);
    debug2(DOB, D, "] returning BreakObject(%s) from SizeGalley(%s)",
      Image(type(y)), SymName(actual(hd)));
    if( !FitsConstraint(back(y, COLM), fwd(y, COLM), *c) )
      Error(21, 13, "%s,%s object too wide for available space",
        FATAL, &fpos(y), EchoLength(back(y, COLM)), EchoLength(fwd(y, COLM)));
    back(hd, COLM) = back(y, COLM);
    fwd(hd, COLM)  = fwd(y, COLM);
    assert( FitsConstraint(back(hd, COLM), fwd(hd, COLM), *c),
	"SizeGalley: BreakObject failed to fit!" );
    debug2(DSF, D, "MinSize(hd, COLM) = %s,%s",
	  EchoLength(back(hd, COLM)), EchoLength(fwd(hd, COLM)) );
  }

  /* hyphenate hd if horizontal optimal galley says so */
  else if( opt_components(hd) != nilobj && opt_hyph(hd) && type(y) == ACAT )
  { debug0(DOG, D, "SizeGalley calling Hyphenate()");
    y = Hyphenate(y);
  }

  /* get the rows of hd to the top level, if required */
  seen_nojoin(hd) = FALSE;
  if( rows )
  { /* OBJECT prev_gap = nilobj; */
    debug0(DGM, DD, "SizeGalley cleaning up rows of hd:");
    for( link = hd;  NextDown(link) != hd;  link = NextDown(link) )
    { Child(y, NextDown(link));
      debug2(DGM, DD, "  cleaning %s: %s", Image(type(y)), EchoObject(y));
      switch( type(y) )
      {
	case GAP_OBJ:

	  /* prev_gap = y; */
	  if( !join(gap(y)) )  seen_nojoin(hd) = TRUE;
	  break;


	case VCAT:
	  
	  if( gall_dir(hd) == ROWM )
	  { TransferLinks(Down(y), y, Up(y));
	    DisposeChild(Up(y));
	    link = PrevDown(link);
	  }
	  break;


	case ACAT:
	  
	  if( gall_dir(hd) == COLM )
	  { TransferLinks(Down(y), y, Up(y));
	    DisposeChild(Up(y));
	    link = PrevDown(link);
	  }
	  break;


	case SPLIT:
	  
	  assert(Up(y)==LastUp(y), "SizeGalley COL_THR: Up(y)!=LastUp(y)!");
	  Child(z, DownDim(y, ROWM));
	  if( is_indefinite(type(z)) )
	  {
	    debug1(DGT, D, "SizeGalley setting external_ver(%s) to TRUE (a)",
	      EchoObject(z));
	    external_ver(z) = TRUE;
	  }
	  else if( type(z) == VCAT )
	  { OBJECT hor, thor, clink, dlink;
	    Child(hor, DownDim(y, COLM));
	    assert( type(hor) == COL_THR, "SizeGalley: missing COL_THR!" );
	    Parent(thor, UpDim(z, COLM));
	    assert( hor == thor, "SizeGalley/SPLIT: hor != thor!" );
	    clink = DownDim(y, COLM);
	    dlink = UpDim(z, COLM);
	    for( tlink = LastDown(z);  tlink != z;  tlink = PrevDown(tlink) )
	    { Child(t, tlink);
	      if( type(t) == GAP_OBJ )
	      { Link(NextDown(link), t);
	      }
	      else
	      {	New(tmp, SPLIT);
		back(tmp, COLM) = back(hor, COLM);
		fwd(tmp, COLM) = fwd(hor, COLM);
		Link(NextDown(link), tmp);
		Link(tmp, NextUp(clink));
		Link(NextDown(dlink), t);
		Link(tmp, t);
	      }
	    }
	    DeleteLink(dlink);
	    assert(Up(y)==LastUp(y), "SizeGalley COL_THR: Up(y) != LastUp(y)!");
	    DisposeChild(Up(y));
	    link = PrevDown(link);
	  }
	  break;


	case CLOSURE:
	case HEAD:
	  
	  if( gall_dir(hd) == COLM )
	    external_hor(y) = TRUE;
	  else
	  {
	    debug1(DGT, D, "SizeGalley setting external_ver(%s) to TRUE (b)",
	      EchoObject(y));
	    external_ver(y) = TRUE;
	  }
	  break;


	default:
	  
	  break;
      }
    }
  }

  /* determine a scale factor for {} @Scale objects */
  /* NB AdjustSize cannot be done correctly until after seen_nojoin is set */
  for( link = Down(extras);  link != extras;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == SCALE_IND )
    {
      /* check that all is in order */
      CONSTRAINT zc;  OBJECT t;  FULL_LENGTH b, f;
      z = actual(y);
      assert( type(z) == SCALE, "SizeObject: type(z) != SCALE!" );
      assert( bc(constraint(z)) == 0, "SizeObject: bc(constraint(z)) != 0" );
      assert( Down(z) != z, "SizeObject SCALE: Down(z) == z!" );
      Child(t, Down(z));

      /* use @Scale COLM size constraint to determine a suitable scale factor */
      /* check that @Scale is not in a horizontal galley */
      if( gall_dir(hd) == COLM )
      { Error(21, 2, "%s with unspecified scale factor in horizontal galley",
	  FATAL, &fpos(z), KW_SCALE);
      }

      Constrained(z, &zc, COLM, &why);
      debug2(DGM, DD, "Constrained(%s, -, COLM) = %s", EchoObject(z),
	EchoConstraint(&zc));
      if( !constrained(zc) )
      { Error(21, 3, "replacing infinite scale factor (unconstrained width) by 1.0",
	  WARN, &fpos(z));
	bc(constraint(z)) = fc(constraint(z)) = 1 * SF;
      }
      else if( size(t, COLM) == 0 )
      { Error(21, 4, "replacing infinite scale factor (zero width object) by 1.0",
	  WARN, &fpos(z));
	bc(constraint(z)) = fc(constraint(z)) = 1 * SF;
      }
      else if( (float) bfc(zc) / size(t, COLM) > 100.0 )
      { Error(21, 5, "replacing very large scale factor (over 100) by 1.0",
	  WARN, &fpos(z));
	bc(constraint(z)) = fc(constraint(z)) = 1 * SF;
      }
      else if( (float) bfc(zc) / size(t, COLM) < 0.01 )
      { if( bfc(zc) == 0 )
	  Error(21, 6, "object deleted (scale factor is zero)",
	    WARN, &fpos(z));
	else
	  Error(21, 7, "object deleted (scale factor is smaller than 0.01)",
	    WARN, &fpos(z));
	bc(constraint(z)) = fc(constraint(z)) = 1 * SF;
	tmp = MakeWord(WORD, STR_EMPTY, &fpos(t));
	back(tmp, COLM) = fwd(tmp, COLM) = 0;
	back(tmp, ROWM) = fwd(tmp, ROWM) = 0;
	word_font(tmp) = word_colour(tmp) = word_language(tmp) = 0;
	word_outline(tmp) = FALSE;
	word_hyph(tmp) = FALSE;
	ReplaceNode(tmp, t);
	DisposeObject(t);
	t = tmp;
      }
      else bc(constraint(z)) = fc(constraint(z)) = (bfc(zc) * SF)/size(t, COLM);

      /* calculate scaled size and adjust */
      b = (back(t, COLM) * fc(constraint(z))) / SF;
      f = (fwd(t, COLM) * fc(constraint(z))) / SF;
      debug3(DGM, DD, "AdjustSize(%s, %s, %s, COLM)", EchoObject(z),
	EchoLength(b), EchoLength(f));
      AdjustSize(z, b, f, COLM);

      /* if already vertically sized (because inside @Rotate) adjust that */
      if( vert_sized(z) )
      { b = (back(t, ROWM) * fc(constraint(z))) / SF;
	f = (fwd(t, ROWM) * fc(constraint(z))) / SF;
	debug4(DGM, DD, "AdjustSize(%s, %s, %s, %s)", EchoObject(z),
	  EchoLength(b), EchoLength(f), dimen(ROWM));
	AdjustSize(z, b, f, ROWM);
      }
    }
  }
  DisposeObject(extras);

  /* size the rows of hd and attach indices where needed */
  debug0(DGM, DD, "  SizeGalley calling MinSize(ROWM):");
  debug0(DGM, DD, "SizeGalley sizing rows of hd =");
  ifdebug(DGM, DD, DebugObject(hd));
  *recs = *inners = *dest_index = nilobj;
  after_target = FALSE;
  for( link = Down(hd);  link != hd;  link = NextDown(link) )
  { Child(y, link);

    if( type(y) == GAP_OBJ || is_index(type(y)) )  continue;
    debug0(DGM, DDD, "  ROWM sizing:");
    ifdebug(DGM, DDD, DebugObject(y));
    New(extras, ACAT);
    y = MinSize(y, ROWM, &extras);
    debug3(DSF, DD, "MinSize( %s , ROWM ) = %s,%s", EchoObject(y),
	  EchoLength(back(y, ROWM)), EchoLength(fwd(y, ROWM)) );
    debug0(DGM, DDD, "  ROWM result:");
    ifdebug(DGM, DDD, DebugObject(y));

    /* now attach indexes in front of y */
    for( zlink = Down(extras);  zlink != extras;  zlink = NextDown(zlink) )
    { Child(z, zlink);
      blocked(z) = FALSE;
      /* debug1(DCR, DD, "  extra: %s", EchoObject(z)); */
      debug2(DGM, DD, "  extra%s: %s",
	after_target ? " after_target" : "", EchoObject(z));
      switch( type(z) )
      {
	case RECEPTIVE:

	  /* debug2(DCR, DD, "  ... uses_ext  = %s, trig = %s",
	    bool(uses_extern_target(actual(actual(z)))), bool(trig)); */
	  trigger_externs(z) = uses_extern_target(actual(actual(z))) && trig;
	  non_blocking(z) = nonblock;
	  if( actual(actual(z)) == GalleySym || actual(actual(z)) == ForceGalleySym )
	    *dest_index = z;
	  if( actual(actual(z)) == GalleySym || actual(actual(z)) == ForceGalleySym
	    || actual(actual(z)) == InputSym )
	    after_target = TRUE;
	  break;


	case RECURSIVE:

	  if( *recs == nilobj )  New(*recs, ACAT);
	  Link(*recs, z);
	  break;


	case UNATTACHED:

	  if( !after_target )	/* *** new semantics *** */
	  { if( *inners == nilobj )  New(*inners, ACAT);
	    Link(*inners, z);
	  }
	  break;

		
	case SCALE_IND:
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

	  debug1(DCR, DD, "  SizeGalley: %s", EchoObject(z));
	  break;


	case COVER_IND:

	  /* adjust size of the COVER object, change it to @Scale etc. */
	  { OBJECT cover, prnt, chld;  int dirn, thr_type, ok1, ok2, sf,subst, esubst;
	    float sf1, sf2;  CONSTRAINT c;  FULL_LENGTH b, f;
	    cover = actual(z);
	    if( type(cover) == HCOVER )
	    { dirn = COLM;
	      thr_type = COL_THR;
	      ok1 = VCAT;
	      ok2 = VCAT;
	      subst = HSCALE;
	      esubst = ONE_COL;
	    }
	    else
	    { dirn = ROWM;
	      thr_type = ROW_THR;
	      ok1 = ACAT;
	      ok2 = HCAT;
	      subst = VSCALE;
	      esubst = ONE_ROW;
	    }
	    Parent(prnt, UpDim(cover, dirn));
	    while( type(prnt) == SPLIT || type(prnt) == thr_type )
	      Parent(prnt, UpDim(prnt, dirn));
	    Child(chld, Down(cover));
	    if( type(prnt) != ok1 && type(prnt) != ok2 )
	    {
	      Error(21, 8, "%s replaced by %s (mark not shared)",
		WARN, &fpos(cover), Image(type(cover)), Image(subst));
	      debug2(DGM, DDD, "  cover = %s %s", Image(type(cover)),
		EchoObject(cover));
	      debug1(DGM, DDD, "  prnt = %s:", Image(type(prnt)));
	      ifdebug(DGM, DDD, DebugObject(prnt));
	      type(cover) = subst;
	    }
	    else if( back(chld, dirn) == 0 && fwd(chld, dirn) == 0 )
	    {
	      /* empty object, this is treated as a no-op */
	      type(cover) = esubst;
	    }
	    else if( back(chld, dirn) == 0 || fwd(chld, dirn) == 0 )
	    { Error(21, 9, "%s replaced by %s (infinite scale factor)",
		WARN, &fpos(cover), Image(type(cover)), Image(subst));
	      type(cover) = subst;
	    }
	    else if( size(prnt, dirn) == 0 )
	    { Error(21, 10, "%s replaced by %s (zero scale factor)",
		WARN, &fpos(cover), Image(type(cover)), Image(subst));
	      type(cover) = subst;
	    }
	    else /* sensible scale factor exists */
	    {
	      /* work out proposed scale factor and sizes for cover */
	      sf1 = (float) back(prnt, dirn) / back(chld, dirn);
	      sf2 = (float) fwd(prnt, dirn)  / fwd(chld, dirn);
	      sf = find_max(sf1, sf2) * SF;
	      b = (back(chld, dirn) * sf) / SF;
	      f = (fwd(chld,  dirn) * sf) / SF;

	      /* check whether new object fits */
	      Constrained(cover, &c, dirn, &why);
	      if( FitsConstraint(b, f, c) )
	      {
		/* it fits, so make cover a SCALE object with this size */
		type(cover) = SCALE;
		if( dirn == COLM )
		{ bc(constraint(cover)) = sf;
		  fc(constraint(cover)) = SF;
		}
		else
		{ bc(constraint(cover)) = SF;
		  fc(constraint(cover)) = sf;
		}
		AdjustSize(cover, b, f, dirn);
	      }
	      else
	      { Error(21, 11, "%s replaced by %s (insufficient space)",
		  WARN, &fpos(cover), Image(type(cover)), Image(subst));
		type(cover) = subst;
	      }
	    }
	  }
	  break;


	default:
	  
	  assert1(FALSE, "SizeGalley:", Image(type(z)));
	  break;

      }
    }
    TransferLinks(Down(extras), extras, link);
    assert( Down(extras) == extras && Up(extras) == extras, "SizeG: extras!");
    Dispose(extras);
  }
  
  /* insinuate cross references */
  if( crs != nilobj )
  { 
    debug1(DCR, DD, "SizeGalley insinuating %s", EchoObject(crs));
    TransferLinks(Down(crs), crs, Down(hd));
    DisposeObject(crs);
  }

  /* check that *dest_index was found if it was required, and exit */
  if( target != nilobj && *dest_index == nilobj )
    Error(21, 12, "unexpected absence of %s from the body of %s",
      FATAL, &fpos(hd), SymName(target), SymName(actual(hd)));
  debug3(DGM, D, "SizeGalley returning %s,%s  %s;  hd =",
    EchoLength(back(hd, COLM)), EchoLength(fwd(hd, COLM)),
    EchoConstraint(&constraint(hd)));
  ifdebug(DGM, D, DebugGalley(hd, nilobj, 4));
  sized(hd) = TRUE;

} /* end SizeGalley */
