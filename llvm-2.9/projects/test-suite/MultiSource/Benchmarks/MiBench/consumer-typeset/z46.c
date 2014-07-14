/*@z46.c:Optimal Galleys:FindOptimize()@**************************************/
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
/*  FILE:         z46.c                                                      */
/*  MODULE:       Optimal Galleys                                            */
/*  EXTERNS:      FindOptimize(), SetOptimize(), GazumpOptimize(),           */
/*                CalculateOptimize(), DebugOptimize()                       */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN FindOptimize(x, env)                                             */
/*                                                                           */
/*  Object x is a CLOSURE which represents an at present unsized galley.     */
/*  Return TRUE if x has an @Optimize parameter which is Yes.                */
/*                                                                           */
/*****************************************************************************/

BOOLEAN FindOptimize(OBJECT x, OBJECT env)
{ OBJECT y, link, res;
  OBJECT bt[2], ft[2], ntarget, nenclose, crs;
  debug1(DOG, D, "FindOptimize( %s )", EchoObject(x));
  assert( type(x) == CLOSURE, "FindOptimize: type(x) != CLOSURE!" );
  assert( has_target(actual(x)), "FindOptimize: x has no target!" );

  /* search the parameter list of x for @Optimize */
  res = nilobj;
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == PAR && is_optimize(actual(y)) )
    { assert( Down(y) != y, "FindOptimize: Down(PAR)!" );
      Child(res, Down(y));
      res = CopyObject(res, &fpos(x));
      break;
    }
  }

  /* search the children list of actual(x) for a default value of @Target */
  if( res == nilobj )
  for( link = Down(actual(x));  link != actual(x);  link = NextDown(link) )
  { Child(y, link);
    if( is_optimize(y) )
    { res = CopyObject(sym_body(y), &fpos(x));
      break;
    }
  }
  
  /* should have found it by now */
  assert( res != nilobj, "FindOptimize: res == nilobj!" );

  /* manifest and tidy the parameter, return TRUE if Yes */
  bt[COLM] = ft[COLM] = bt[ROWM] = ft[ROWM] = ntarget = nenclose = crs = nilobj;
  res = Manifest(res, env, &save_style(x), bt, ft, &ntarget, &crs, TRUE, FALSE,
    &nenclose, FALSE);
  res = ReplaceWithTidy(res, TRUE);
  if( !is_word(type(res)) )
  { Error(46, 1, "unable to evaluate %s parameter, assuming value is No",
      WARN, &fpos(x), KW_OPTIMIZE);
    debug2(DOG, D, "FindOptimize returning FALSE; found %s %s",
      Image(type(res)), EchoObject(res));
    return FALSE;
  }
  else if( StringEqual(string(res), AsciiToFull("Yes")) )
  { debug0(DOG, D, "FindOptimize returning TRUE");
    return TRUE;
  }
  else if( StringEqual(string(res), AsciiToFull("No")) )
  { debug0(DOG, D, "FindOptimize returning FALSE");
    return FALSE;
  }
  else
  { Error(46, 2, "value of %s operator is neither Yes nor No, assuming No",
      WARN, &fpos(x), KW_OPTIMIZE);
    debug1(DOG, D, "FindOptimize returning FALSE (found WORD %s)", string(res));
    return FALSE;
  }
} /* end FindOptimize */


/*****************************************************************************/
/*                                                                           */
/*  SetOptimize(hd, style)                                                   */
/*                                                                           */
/*  Initialize the optimization data of galley hd.  Search the cross ref     */
/*  database for information about its fate on the previous run.             */
/*                                                                           */
/*****************************************************************************/

void SetOptimize(OBJECT hd, STYLE *style)
{ FULL_CHAR buff[MAX_BUFF], seq[MAX_BUFF];
  OBJECT res, y, link, z;  FILE_NUM dfnum;  long dfpos, cont;  int dlnum;
  debug2(DOG, D, "SetOptimize(%s, %s)", SymName(actual(hd)), EchoStyle(style));

  /* set opt_counts(hd) to result of previous run, if any */
  StringCopy(buff, SymName(actual(hd)));
  StringCat(buff, AsciiToFull("."));
  StringCat(buff, StringInt(line_num(fpos(hd))));
  if( DbRetrieve(OldCrossDb, FALSE, OptGallSym, buff, seq, &dfnum,
    &dfpos, &dlnum, &cont) )
  {
    SwitchScope(nilobj);
    res = ReadFromFile(dfnum, dfpos, dlnum);
    UnSwitchScope(nilobj);
    assert( res != nilobj, "SetOptimize: res == nilobj!" );
    assert( type(res) == CLOSURE, "SetOptimize: type(res) != CLOSURE!" );
    assert( actual(res) == OptGallSym, "SetOptimize: actual(res) != Opt!" );
    assert( Down(res) != res, "SetOptimize: Down(res) == res!" );
    Child(y, Down(res));
    assert( type(y) == PAR, "SetOptimize: type(y) != PAR!" );
    Child(y, Down(y));
    assert( type(y) == ACAT, "SetOptimize: type(y) != ACAT!" );
    y = ReplaceWithTidy(y, FALSE);
    opt_hyph(hd) = FALSE;
    assert( type(y) == ACAT, "SetOptimize: type(y) != ACAT (2)!" );
    for( link = y;  NextDown(link) != y;  link = NextDown(link) )
    { Child(z, NextDown(link));
      if( type(z) == GAP_OBJ )
      { DisposeChild(NextDown(link));
	link = PrevDown(link);
      }
      else if( is_word(type(z)) )
      { if( StringEqual(string(z), AsciiToFull("h")) )
	{ opt_hyph(hd) = TRUE;
	  DisposeChild(NextDown(link));
	  link = PrevDown(link);
	}
	else
	{ int num = 0;
	  sscanf( (char *) string(z), "%d", &num);
	  assert( num > 0, "SetOptimize: num <= 0!" );
	  comp_count(z) = num;
	}
      }
      else
      { assert( FALSE, "SetOptimize: type(z)!" );
      }
    }
    DeleteLink(Up(y));
    DisposeObject(res);
    opt_counts(hd) = y;
  }
  else opt_counts(hd) = nilobj;

  /* set up first opt_comps_permitted value */
  if( opt_counts(hd) != nilobj && Down(opt_counts(hd)) != opt_counts(hd) )
  { Child(z, Down(opt_counts(hd)));
    opt_comps_permitted(hd) = comp_count(z) - 1;
    DisposeChild(Up(z));
  }
  else opt_comps_permitted(hd) = MAX_FILES;  /* a large number */
  debug1(DOG, D, "  initial permitted = %2d", opt_comps_permitted(hd));

  /* set opt_components(hd) and opt_constraints(hd) for storing this run */
  New(opt_components(hd), ACAT);
  opt_gazumped(hd) = FALSE;
  New(opt_constraints(hd), ACAT);
  StyleCopy(save_style(opt_components(hd)), *style);
  if( gall_dir(hd) == ROWM )
    hyph_style(save_style(opt_components(hd))) = HYPH_OFF;

  debug0(DOG, D, "SetOptimize returning:");
  ifdebug(DOG, D, DebugOptimize(hd));
} /* end SetOptimize */


/*****************************************************************************/
/*                                                                           */
/*  GazumpOptimize(hd, dest)                                                 */
/*                                                                           */
/*  Optimizing galley hd, currently attached to @Galley dest, is to be       */
/*  gazumped by some other galley.  Record the current size constraint and   */
/*  add &1rt {} to the list of components.                                   */
/*                                                                           */
/*****************************************************************************/

void GazumpOptimize(OBJECT hd, OBJECT dest)
{ OBJECT g, tmp, junk, prnt;

  debug2(DOG, D, "GazumpOptimize(%s, %s)", SymName(actual(hd)),
    EchoObject(dest));
  assert( type(hd) == HEAD, "GazumpOptimize: type(hd) != HEAD!" );
  assert( opt_components(hd) != nilobj, "GazumpOptimize: opt_c!" );

  /* record the size of this just-completed target area for hd */
  New(tmp, WIDE);
  if( (gall_dir(hd) == COLM && external_hor(dest)) ||
      (gall_dir(hd) == COLM && external_hor(dest)) )
  { SetConstraint(constraint(tmp), MAX_FULL_LENGTH, MAX_FULL_LENGTH, MAX_FULL_LENGTH);
  }
  else
  { Parent(prnt, Up(dest));
    Constrained(prnt, &constraint(tmp), gall_dir(hd), &junk);
  }
  Link(opt_constraints(hd), tmp);
  debug2(DOG, D, "GazumpOptimize(%s) adding constraint %s",
    SymName(actual(hd)), EchoConstraint(&constraint(tmp)));

  /* optimizing galley is being gazumped; record this as &1rt {} &1c */
  if( LastDown(opt_components(hd)) != opt_components(hd) )
  { Child(g, LastDown(opt_components(hd)));
    assert( type(g) == GAP_OBJ, "FlushGalley: type(g) != GAP_OBJ!" );

    /* ***
    SetGap(gap(g), FALSE, FALSE, TRUE, FRAME_UNIT, EDGE_MODE, 2 * FR);
    if( Down(g) == g )
    { junk = MakeWord(WORD, AsciiToFull("2b"), &fpos(g));
      Link(g, junk);
    }
    *** */

    /* first we overwrite whatever is there now by &1rt */
    SetGap(gap(g), FALSE, FALSE, TRUE, AVAIL_UNIT, TAB_MODE, 1 * FR);
    if( Down(g) != g )  DisposeChild(Down(g));
    tmp = MakeWord(WORD, AsciiToFull("1rt"), &fpos(g));
    Link(g, tmp);

    /* next we add an empty word */
    tmp = MakeWord(WORD, STR_EMPTY, &fpos(g));
    back(tmp, COLM) = fwd(tmp, COLM) = 0;
    back(tmp, ROWM) = fwd(tmp, ROWM) = 0;
    word_font(tmp) = word_colour(tmp) = 0;
    word_outline(tmp) = FALSE;
    word_language(tmp) = word_hyph(tmp) = 0;
    Link(opt_components(hd), tmp);

    /* finally we add &1c */
    New(g, GAP_OBJ);
    hspace(g) = 1;  vspace(g) = 0;
    FposCopy(fpos(g), fpos(tmp));
    SetGap(gap(g), FALSE, FALSE, TRUE, FIXED_UNIT, EDGE_MODE, 1 * CM);
    tmp = MakeWord(WORD, AsciiToFull("1c"), &fpos(g));
    Link(g, tmp);
    Link(opt_components(hd), g);

    opt_gazumped(hd) = TRUE;
    debug2(DOG, D, "GazumpOptimize(%s) new gap is %s",
      SymName(actual(hd)), EchoGap(&gap(g)));
  }

  /* refresh the number of comps permitted into the next target */
  if( opt_counts(hd) != nilobj && Down(opt_counts(hd)) != opt_counts(hd) )
  { Child(tmp, Down(opt_counts(hd)));
    opt_comps_permitted(hd) += comp_count(tmp) - 1;
    DisposeChild(Up(tmp));
  }
  else opt_comps_permitted(hd) = MAX_FILES;

  debug1(DOG, D, "GazumpOptimize returning, permitted = %2d",
    opt_comps_permitted(hd));
} /* end GazumpOptimize */


/*****************************************************************************/
/*                                                                           */
/*  CalculateOptimize(hd)                                                    */
/*                                                                           */
/*  Calculate the optimal break for galley hd and write the result into      */
/*  the cross reference database.                                            */
/*                                                                           */
/*****************************************************************************/

void CalculateOptimize(OBJECT hd)
{ OBJECT z, y, ylink, og, og_par, para, link, wd, g, last;
  int count, compcount;  FULL_CHAR buff[MAX_BUFF];
  FILE_NUM fnum;  int write_pos, write_lnum;  BOOLEAN hyph_used;
  debug1(DOG, D, "CalculateOptimize(%s)", SymName(actual(hd)));

  /* delete the concluding GAP_OBJ stuck in by Promote() */
  assert( LastDown(opt_components(hd)) != opt_components(hd), "CO!" );
  Child(last, LastDown(opt_components(hd)));
  assert( type(last) == GAP_OBJ, "CalculateOptimize: type(last)!" );
  DisposeChild(Up(last));
  ifdebug(DOG, D, DebugOptimize(hd));

  /* break the paragraph; don't let user see any error messages */
  assert( opt_constraints(hd) != nilobj, "KillGalley: no opt_constraints!" );
  assert( Down(opt_constraints(hd)) != opt_constraints(hd), "KillGalleyo!" );
  /* *** no longer needed since z14 doesn't refer to these fields
  back(opt_components(hd), COLM) = 0;
  fwd(opt_components(hd), COLM) = MAX_FULL_LENGTH;
  *** */
  Child(y, LastDown(opt_constraints(hd)));
  EnterErrorBlock(FALSE);
  opt_components(hd) = FillObject(opt_components(hd), &constraint(y),
    opt_constraints(hd), FALSE, FALSE, TRUE, &hyph_used);
  LeaveErrorBlock(FALSE);
  debug1(DOG, D, "after breaking (%shyph_used):", hyph_used ? "" : "not ");
  ifdebug(DOG, D, DebugOptimize(hd));

  /* quit if one line only */
  if( type(opt_components(hd)) != VCAT ||
      Down(opt_components(hd)) == LastDown(opt_components(hd)) )
  {
    debug0(DOG, D, "CalculateOptimize returning (one target only)");
    return;
  }

  /* construct a new @OptGall symbol */
  New(og, CLOSURE);
  actual(og) = OptGallSym;
  FposCopy(fpos(og), fpos(hd));
  New(og_par, PAR);
  actual(og_par) = ChildSym(OptGallSym, RPAR);
  Link(og, og_par);
  New(para, ACAT);
  Link(og_par, para);

  /* begin with "h" if hyphenation was used */
  if( hyph_used )
  { wd = MakeWord(WORD, AsciiToFull("h"), &fpos(hd));
    Link(para, wd);
  }

  /* attach words showing the number of components per target */
  compcount = 0;
  for( link = Down(opt_components(hd));  link != opt_components(hd);
       link = NextDown(link) )
  { Child(y, link);
    if( type(y) != ACAT )  continue;

    /* let wd be a word containing the number of components in this target */
    count = 0;
    for( ylink = Down(y);  ylink != y;  ylink = NextDown(ylink) )
    { Child(z, ylink);
      if( type(z) != GAP_OBJ ) count++;
    }
    wd = MakeWord(WORD, StringInt(count), &fpos(y));

    /* link wd to para, prepended by a gap if not first */
    if( Down(para) != para )
    { New(g, GAP_OBJ);
      SetGap(gap(g), FALSE, FALSE, TRUE, FIXED_UNIT, EDGE_MODE, 1*EM);
      if( ++compcount % 20 == 0 )
      { hspace(g) = 0;
	vspace(g) = 1;
      }
      else
      { hspace(g) = 1;
        vspace(g) = 0;
      }
      Link(para, g);
    }
    Link(para, wd);
  }
  debug2(DOG, D, "CalculateOptimize(%s) made object %s",
    SymName(actual(hd)), EchoObject(og));

  /* dispose the optimizing data structures */
  DisposeObject(opt_components(hd));
  opt_components(hd) = nilobj;
  DisposeObject(opt_constraints(hd));
  opt_constraints(hd) = nilobj;

  /* write result onto cross-reference database */
  if( AllowCrossDb )
  {
    /* construct a suitable tag for this galley's entry */
    StringCopy(buff, SymName(actual(hd)));
    StringCat(buff, AsciiToFull("."));
    StringCat(buff, StringInt(line_num(fpos(hd))));
    fnum = DatabaseFileNum(&fpos(hd));
    AppendToFile(og, fnum, &write_pos, &write_lnum);
    DbInsert(NewCrossDb, FALSE, OptGallSym, buff, &fpos(hd),
      STR_ZERO, fnum, write_pos, write_lnum, FALSE);
  }
  debug0(DOG, D, "CalculateOptimize returning.");
}

#if DEBUG_ON
/*****************************************************************************/
/*                                                                           */
/*  DebugOptimizedAcat(x)                                                    */
/*                                                                           */
/*  Debug output of one line of optimized ACAT.                              */
/*                                                                           */
/*****************************************************************************/

static void DebugOptimizedAcat(OBJECT x)
{ OBJECT link, y;
  assert( type(x) == ACAT, "DebugOptimizedAcat!" );
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( type(y) == GAP_OBJ )
    { debug1(DOG, D, "  GAP_OBJ %s", EchoGap(&gap(y)));
    }
    else if( is_word(type(y)) )
    { debug2(DOG, D, "  word (%s, %s)", EchoLength(back(y, COLM)),
	EchoLength(fwd(y, COLM)));
    }
    else
    { debug1(DOG, D, "  %s", Image(type(y)));
    }
  }
} /* end DebugOptimizedAcat */


/*****************************************************************************/
/*                                                                           */
/*  DebugOptimize(hd)                                                        */
/*                                                                           */
/*  Debug output of optimized galley hd.                                     */
/*                                                                           */
/*****************************************************************************/

void DebugOptimize(OBJECT hd)
{ OBJECT link, y;

  assert( opt_components(hd) != nilobj, "DebugOptimize!");
  debug3(DOG, D, "Optimized Galley %s %sinto %s", SymName(actual(hd)),
    gall_dir(hd) == COLM ? "horizontally " : "", SymName(whereto(hd)));

  /* print components */
  /* *** believe this now ***
  if( type(opt_components(hd)) == ACAT )
    DebugOptimizedAcat(opt_components(hd));
  else if( type(opt_components(hd)) == VCAT )
  {
    for( link = Down(opt_components(hd));  link != opt_components(hd);
	 link = NextDown(link) )
    {
      Child(y, link);
      if( type(y) == ACAT )  DebugOptimizedAcat(y);
      debug0(DOG, D, "----------------");
    }
  }
  else debug1(DOG, D, "? %s ?", Image(type(opt_components(hd))));
  *** */
  debug0(DOG, D, "components:");
  ifdebug(DOG, D, DebugObject(opt_components(hd)));
  debug0(DOG, D, "");

  /* print constraints */
  debug0(DOG, D, "constraints:");
  for( link = Down(opt_constraints(hd));  link != opt_constraints(hd);
       link = NextDown(link) )
  {
    Child(y, link);
    debug1(DOG, D, "%s", EchoConstraint(&constraint(y)));
  }
  debug0(DOG, D, "");

  /* print counts */
  debug0(DOG, D, "counts");
  if( opt_counts(hd) != nilobj )
  {
    if( opt_hyph(hd) )
      fprintf(stderr, "hyph");
    for( link = Down(opt_counts(hd));  link != opt_counts(hd);
	 link = NextDown(link) )
    { Child(y, link);
      fprintf(stderr, " %d", comp_count(y));
    }
    fprintf(stderr, "\n");
  }
  debug0(DOG, D, "");
} /* end DebugOptimize */
#endif
