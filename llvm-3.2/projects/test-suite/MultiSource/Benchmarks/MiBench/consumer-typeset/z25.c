/*@z25.c:Object Echo:aprint(), cprint(), printnum()@**************************/
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
/*  FILE:         z25.c                                                      */
/*  MODULE:       Object Echo                                                */
/*  EXTERNS:      EchoObject(), DebugObject()                                */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#if DEBUG_ON

static	int	limit;			/* column where newline is needed    */
static	int	indent;			/* current indent                    */
static	int	col;			/* current output column             */
static	FILE	*fp;			/* current output file               */

#define	moveright()	(indent += 3)
#define	moveleft()	(indent -= 3)


/*****************************************************************************/
/*                                                                           */
/*  static aprint(x)                                                         */
/*  static cprint(x)                                                         */
/*                                                                           */
/*  Print the ASCII or FULL_CHAR string x onto the appropriate output.       */
/*                                                                           */
/*****************************************************************************/

static void cprint(FULL_CHAR *x)
{ col += StringLength(x);
  if( fp == null ) AppendString(x);
  else StringFPuts(x, fp);
} /* end print */

static void aprint(char *x)
{ cprint(AsciiToFull(x));
} /* end aprint */


/*****************************************************************************/
/*                                                                           */
/*  static printnum(x)                                                       */
/*                                                                           */
/*  Print the number x onto the appropriate output.                          */
/*                                                                           */
/*****************************************************************************/

static void printnum(int x)
{ cprint(StringInt(x));
} /* end printnum */


/*@::tab(), newline(), space()@***********************************************/
/*                                                                           */
/*  static tab(x)                                                            */
/*                                                                           */
/*  Tab to column x, or anyway insert at least one space.                    */
/*                                                                           */
/*****************************************************************************/

static void tab(int x)
{  do
     aprint(" ");
   while( col < x );
} /* end tab */


/*****************************************************************************/
/*                                                                           */
/*  static newline()                                                         */
/*                                                                           */
/*  Echo a newline to the appropriate output (unless output is a string).    */
/*  Correct indenting and right limits are maintained, if possible.          */
/*                                                                           */
/*****************************************************************************/

static void newline(void)
{ if( fp == null )  AppendString(STR_SPACE);
  else
  { fputs("\n", fp);
    fflush(fp);
    for( col = 0;  col < indent;  col++ )  fputs(" ", fp);
  }
} /* end newline */


/*@::echo()@******************************************************************/
/*                                                                           */
/*  static echo(x, outer_prec, count)                                        */
/*                                                                           */
/*  Echo x.  The result will be enclosed in braces only if its precedence    */
/*  is less than or equal to outer_prec (words and parameterless closures    */
/*  are taken to have infinite precedence, i.e. never enclosed in braces).   */
/*                                                                           */
/*  x is child number count of its parent.  Used by COL_THR and ROW_THR      */
/*  only.                                                                    */
/*                                                                           */
/*****************************************************************************/

static void echo(OBJECT x, unsigned outer_prec, int count)
{ OBJECT link, y, tmp, sym, z;
  char *op;  int prec, i, childcount, ycount;
  BOOLEAN npar_seen, name_printed, lbr_printed, braces_needed;

  switch( type(x) )
  {

    case DEAD:

	aprint("#dead");
	break;


    case UNDER_REC:
    
	aprint("#under_rec");
	break;


    case UNATTACHED:
    
	aprint( "#unattached " );
	moveright();
	if( Down(x) != x )
	{ CountChild(y, Down(x), count);
	  if( y != x ) echo(y, NO_PREC, count);
	  else aprint("<child is self!>");
	}
	else aprint("<no child!>");
	moveleft();
	break;


    case SCALE_IND:
    case COVER_IND:
    case EXPAND_IND:
    case GALL_PREC:
    case GALL_FOLL:
    case GALL_FOLL_OR_PREC:
    case GALL_TARG:
    case CROSS_PREC:
    case CROSS_FOLL:
    case CROSS_FOLL_OR_PREC:
    case CROSS_TARG:
    case RECURSIVE:
    case PAGE_LABEL_IND:
    
	/* aprint("#"); cprint(Image(type(x))); aprint(" "); */
	echo(actual(x), NO_PREC, 1);
	break;

		
    case RECEPTIVE:
    case RECEIVING:
    
	aprint(type(x) == RECEIVING ? "#receiving " : "#receptive ");
	if( external_ver(actual(x)) )  aprint("(external_ver) ");
	if( external_hor(actual(x)) )  aprint("(external_hor) ");
	if( threaded(actual(x)) )  aprint("(threaded) ");
	if( blocked(x) )           aprint("(blocked) " );
	if( trigger_externs(x) )   aprint("(trigger_externs) " );
	if( non_blocking(x) )      aprint("(non_blocking) " );
	cprint( type(actual(x)) == CLOSURE ?
		SymName(actual(actual(x))) : Image(type(actual(x))) );
	aprint(" ");
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  moveright();
	  echo(y, NO_PREC, count);
	  moveleft();
	}
	break;


    case PRECEDES:
    
	aprint("#precedes");
	break;


    case FOLLOWS:
    
	aprint("#follows");
	if( blocked(x) )  aprint(" (blocked)");
	CountChild(y, Down(x), count);
	if( Up(y) == LastUp(y) )  aprint(" (no precedes!)");
	break;


    case HEAD:
    
	aprint("Galley ");  cprint(SymName(actual(x)));
	aprint(" into ");   cprint(SymName(whereto(x)));
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  newline();
	  echo(y, type(y) == GAP_OBJ ? VCAT : VCAT_PREC, count);
	}
	break;


    case ROW_THR:

	aprint("{R ");
	for( i=0, link = Down(x);  link != x && i < count ;  link = NextDown(link), i++ );
	if( link != x )
	{ CountChild(y, link, count);
	  echo(y, VCAT_PREC, count);
	  /* newline(); */
	}
	aprint(" R}");
	break;


    /* ***
    case COL_THR:

	aprint("{C ");
	for( i=0, link = Down(x);  link != x && i < count ;  link = NextDown(link), i++ );
	if( link != x )
	{ CountChild(y, link, count);
	  echo(y, HCAT_PREC, count);
	}
	aprint(" C}");
	break;
    *** */


    case COL_THR:

	aprint("{C ");
	newline();
	for( i=1, link = Down(x);  link != x;  link = NextDown(link), i++ )
	{
	  if( i == count )
	    aprint("C@ ");
	  else
	    aprint("C: ");
	  CountChild(y, link, ycount);
	  echo(y, HCAT_PREC, ycount);
	  newline();
	}
	aprint(" C}");
	break;


    case HSPANNER:

	aprint("{HS ");
	CountChild(y, Down(x), count);
	echo(y, NO_PREC, count);
	aprint(" HS}");
	break;


    case VSPANNER:

	aprint("{VS ");
	CountChild(y, Down(x), count);
	echo(y, NO_PREC, count);
	aprint(" VS}");
	break;


    case THREAD:

	aprint("<thread>");
	break;


    case VCAT: op = "/", prec = VCAT_PREC;  goto ETC;
    case HCAT: op = "|", prec = HCAT_PREC;  goto ETC;
    
	ETC:
	if( Down(x) == x )
	{ aprint(op);
	  aprint("<empty>");
	  break;
	}
	if( prec <= outer_prec ) aprint("{ ");
	/* *** if( Down(x) == LastDown(x) )  aprint(op);  must be manifested */
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  if( is_index(type(y)) )
	    newline();
	  else if( (type(y) == GAP_OBJ && type(x) != ACAT) )
	    newline();
	  if( type(y) == GAP_OBJ )  echo(y, type(x), count);
	  else echo(y, prec, count);
	}
	if( prec <= outer_prec )  aprint(" }");
	break;


    case ACAT: op = "&", prec = ACAT_PREC;

	childcount = 0;
	aprint("[[ ");
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  if( type(y) == GAP_OBJ )
	  {
	    echo(y, ACAT, count);
	    continue;
	  }
	  childcount++;
	  aprint("[");
	  echo(y, prec, count);
	  aprint("]");
	  /* ***
	  if( link == Down(x) || link == LastDown(x) )
	    echo(y, prec, count);
	  else if( NextDown(NextDown(link)) == LastDown(x) )
	  { sprintf(buff, " ++%d++ ", childcount+1);
	    aprint(buff);
	  }
	  *** */
	}
	aprint(" ]]");
	break;


    case GAP_OBJ:

	/* in this case the outer_prec argument is VCAT, HCAT or ACAT */
	if( Down(x) != x )
	{ if( outer_prec == ACAT )  aprint(" ");
	  cprint( EchoCatOp(outer_prec, mark(gap(x)), join(gap(x))) );
	  CountChild(y, Down(x), count);
	  echo(y, FORCE_PREC, count);
	  aprint(" ");
	}
	/* ***
	else if( outer_prec == ACAT )
	{ for( i = 1;  i <= vspace(x);  i++ )  newline();
	  for( i = 1;  i <= hspace(x);  i++ )  aprint(" ");
	}
	*** */
	else
	{ cprint( EchoCatOp(outer_prec, mark(gap(x)), join(gap(x))) );
	  cprint( EchoGap(&gap(x)) );
	  aprint(" ");
	}
	break;


    case WORD:
    
	if( StringLength(string(x)) == 0 )
	  aprint("{}");
	else
	{ aprint("\"");
	  cprint( string(x) );
	  aprint("\"");
	}
	break;


    case QWORD:
    
	cprint( StringQuotedWord(x) );
	break;


    case ENV:
    
	/* debug only */
	aprint("<");
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  if( type(y) == CLOSURE )
	  { cprint( SymName(actual(y)) );
	    if( LastDown(y) != y )  echo(GetEnv(y), NO_PREC, count);
	  }
	  else if( type(y) == ENV )  echo(y, NO_PREC, count);
	  else cprint(Image(type(y)));
	  if( NextDown(link) != x )  aprint(" ");
	}
	aprint(">");
	break;


    case CROSS:
    case FORCE_CROSS:

	assert( Down(x) != x, "echo: CROSS Down(x)!" );
	CountChild(y, Down(x), count);
	if( type(y) == CLOSURE )  cprint(SymName(actual(y)));
	else
	{ cprint(KW_LBR);
	  echo(y, NO_PREC, count);
	  cprint(KW_RBR);
	}
	cprint(Image(type(x)));
	/* ***
	cprint(KW_CROSS);
	aprint("<");
	cprint(Image(cross_type(x)));
	aprint(">");
	*** */
	aprint(" ");
	if( NextDown(Down(x)) != x )
	{ CountChild(y, NextDown(Down(x)), count);
	  echo(y, NO_PREC, count);
	}
	else aprint("??");
	break;


    case CLOSURE:
    
	sym = actual(x);
	braces_needed =
	    precedence(sym) <= outer_prec && (has_lpar(sym) || has_rpar(sym));

	/* print brace if needed */
	if( braces_needed )  aprint("{ ");

	npar_seen = FALSE;  name_printed = FALSE;
	for( link = Down(x); link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  if( type(y) == PAR )
	  { assert( Down(y) != y, "EchoObject: Down(PAR)!" );
	    switch( type(actual(y)) )
	    {
	     case LPAR:	Child(tmp, Down(y));
			echo(tmp, (unsigned) precedence(sym), 1);
			aprint(" ");
			break;

	     case NPAR:	if( !name_printed )
			{ cprint(SymName(sym));
			  aprint("%");
			  cprint(SymName(enclosing(sym)));
			  if( external_ver(x) || external_hor(x) || threaded(x) )
			  { aprint(" #");
			    if( external_ver(x) )  aprint(" external_ver");
			    if( external_hor(x) )  aprint(" external_hor");
			    if( threaded(x) )  aprint(" threaded");
			    newline();
			  }
			  name_printed = TRUE;
			}
			newline();  aprint("  ");
			cprint( SymName(actual(y)) );
			aprint(" { ");
			Child(tmp, Down(y));
			echo(tmp, NO_PREC, 1);
			aprint(" }");
			npar_seen = TRUE;
			break;

	     case RPAR:	if( !name_printed )
			{ cprint(SymName(sym));
			  aprint("%");
			  cprint(SymName(enclosing(sym)));
			  if( external_ver(x) || external_hor(x) || threaded(x) )
			  { aprint(" #");
			    if( external_ver(x) )  aprint(" external_ver");
			    if( external_hor(x) )  aprint(" external_hor");
			    if( threaded(x) )  aprint(" threaded");
			    newline();
			  }
			  name_printed = TRUE;
			}
			if( npar_seen ) newline();
			else aprint(" ");
			Child(tmp, Down(y));
			if( has_body(sym) )
			{ aprint("{ ");
			  echo(tmp, NO_PREC, 1);
			  aprint(" }");
			}
			else echo(tmp, (unsigned) precedence(sym), 1);
			break;
	
	     default:	assert1(FALSE, "echo:", Image(type(actual(y))));
			break;

	    }
	  }
	}
	if( !name_printed )
	{ cprint( SymName(sym) );
	  aprint("%");
	  cprint(SymName(enclosing(sym)));
	  if( external_ver(x) || external_hor(x) || threaded(x) )
	  { aprint(" #");
	    if( external_ver(x) )  aprint(" external_ver");
	    if( external_hor(x) )  aprint(" external_hor");
	    if( threaded(x) )  aprint(" threaded");
	    newline();
	  }
	}

	/* print closing brace if needed */
	if( braces_needed ) aprint(" }");
	break;


    case SPLIT:
    
	/* this should occur only in debug output case */
	cprint(KW_SPLIT);  moveright();
	CountChild(y, DownDim(x, COLM), count);
	aprint(" COLM:");
	echo(y, FORCE_PREC, count);
	newline();
	/* ***
	Child(y, DownDim(x, ROWM));
	aprint(" ROWM:");
	echo(y, FORCE_PREC);
	*** */
	moveleft();
	break;


    case PAR:
    
	/* this should occur only in debug output case */
	aprint("par ");  cprint(SymName(actual(x)));
	break;


    case CR_LIST:

	aprint("(");
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  echo(y, NO_PREC, count);
	  if( NextDown(link) != x )  aprint(", ");
	}
	aprint(")");
	break;


    case MACRO:
    
	newline();  cprint(KW_MACRO);
	aprint(" ");  cprint(SymName(x));
	if( sym_body(x) != nilobj )
	{ newline();  cprint(KW_LBR);
	  y = sym_body(x);
	  do
	  { for( i = 1;  i <= vspace(y);  i++ )  newline();
	    for( i = 1;  i <= hspace(y);  i++ )  aprint(" ");
	    cprint(EchoToken(y));
	    y = succ(y, PARENT);
	  } while( y != sym_body(x) );
	  newline();  cprint(KW_RBR);
	}
	else aprint(" {}");
	if( visible(x) )  aprint(" # (visible)");
	break;


    case NPAR:
    case LOCAL:
    
	/* print predefined operators in abbreviated form */
	if( sym_body(x) == nilobj && enclosing(x) != nilobj )
	{ tab(3); aprint("# sys ");
	  cprint(SymName(x));
	  break;
	}

	/* print def line and miscellaneous debug info */
	if( type(x) == LOCAL ) newline();
	cprint(type(x) == NPAR ? KW_NAMED : KW_DEF);
	aprint(" ");  cprint( SymName(x) );
	if( recursive(x) || indefinite(x) || visible(x) ||
	    is_extern_target(x) || uses_extern_target(x) || uses_galley(x) )
	{ tab(25);  aprint("#");
	  if( visible(x)  )  aprint(" visible");
	  if( recursive(x)  )  aprint(" recursive");
	  if( indefinite(x) )  aprint(" indefinite");
	  if( is_extern_target(x) )  aprint(" is_extern_target");
	  if( uses_extern_target(x) )  aprint(" uses_extern_target");
	  if( uses_galley(x) )  aprint(" uses_galley");
	}

	/* print uses list, if necessary */
	if( uses(x) != nilobj || dirty(x) )
	{ newline();  aprint("   # ");
	  if( dirty(x) ) aprint("dirty, ");
	  aprint("uses");
	  if( uses(x) != nilobj )
	  { tmp = next(uses(x));
	    do
	    { aprint(" "), cprint( SymName(item(tmp)) );
	      tmp = next(tmp);
	    } while( tmp != next(uses(x)) );
	  }
	  /* ***
	  for( tmp = uses(x);  tmp != nilobj;  tmp = next(tmp) )
	  { aprint(" "), cprint( SymName(item(tmp)) );
	  }
	  *** */
	}

	/* print precedence, if necessary */
	if( precedence(x) != DEFAULT_PREC )
	{ newline();  aprint("   ");  cprint(KW_PRECEDENCE);
	  aprint(" ");  printnum(precedence(x));
	}

	/* print associativity, if necessary */
	if( !right_assoc(x) )
	{ newline();  aprint("   ");
	  cprint(KW_ASSOC);  aprint(" ");  cprint(KW_LEFT);
	}

	/* print named parameters and local objects */
	lbr_printed = FALSE;
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  assert( enclosing(y) == x, "echo: enclosing(y) != x!" );
	  switch( type(y) )
	  {
	    case LPAR:
	    case RPAR:	newline();  aprint("   ");
			cprint( type(y) == LPAR ? KW_LEFT :
			    has_body(x) ? KW_BODY : KW_RIGHT);
			aprint(" ");
			cprint( SymName(y) );
			aprint("   # uses_count = ");
			printnum(uses_count(y));
			if( visible(y) )  aprint(" (visible)");
			break;

	    case NPAR:	moveright();  newline();
			echo(y, NO_PREC, count);
			aprint("   # uses_count = ");
			printnum(uses_count(y));
			moveleft();
			break;

	    case MACRO:
	    case LOCAL:	if( !lbr_printed )
			{ newline();
			  cprint(KW_LBR);
			  lbr_printed = TRUE;
			}
			moveright();
			echo(y, NO_PREC, count);
			moveleft();  newline();
			break;

	    default:	assert1(FALSE, "echo:", Image(type(y)));
			break;
	  }
	}
	if( type(x) == NPAR && Down(x) == x )  aprint(" ");
	else newline();
	if( !lbr_printed )
	{ cprint(KW_LBR);  aprint("  ");
	  lbr_printed = TRUE;
	}
	else aprint("   ");

	/* print body */
	moveright();
	if( sym_body(x) != nilobj )  echo(sym_body(x), NO_PREC, 1);
	moveleft();  if( type(x) == LOCAL ) newline();
	cprint(KW_RBR);
	break;


    case BEGIN_HEADER:
    case SET_HEADER:
    case ONE_COL:
    case ONE_ROW:
    case HCONTRACT:
    case VCONTRACT:
    case HLIMITED:
    case VLIMITED:
    case HEXPAND:
    case VEXPAND:
    case START_HVSPAN:
    case START_HSPAN:
    case START_VSPAN:
    case PADJUST:
    case HADJUST:
    case VADJUST:
    case HSCALE:
    case VSCALE:
    case HCOVER:
    case VCOVER:
    case COMMON:
    case RUMP:
    case MELD:
    case INSERT:
    case ONE_OF:
    case NEXT:
    case PLUS:
    case MINUS:
    case WIDE:
    case HIGH:
    case HSHIFT:
    case VSHIFT:
    case INCGRAPHIC:
    case SINCGRAPHIC:
    case PLAIN_GRAPHIC:
    case GRAPHIC:
    case LINK_SOURCE:
    case LINK_DEST:
    case ROTATE:
    case BACKGROUND:
    case SCALE:
    case KERN_SHRINK:
    case CASE:
    case YIELD:
    case XCHAR:
    case FONT:
    case SPACE:
    case YUNIT:
    case ZUNIT:
    case BREAK:
    case UNDERLINE:
    case COLOUR:
    case OUTLINE:
    case LANGUAGE:
    case OPEN:
    case TAGGED:
    case ENV_OBJ:

    
	/* print enclosing left brace if needed */
	braces_needed = (DEFAULT_PREC <= outer_prec);
	if( braces_needed )  cprint(KW_LBR), aprint(" ");

	/* print left parameter */
	if( Down(x) != LastDown(x) )
	{ CountChild(y, Down(x), count);
	  echo(y, find_max(outer_prec, DEFAULT_PREC), count);
	  aprint(" ");
	}

	cprint(Image(type(x)));

	/* print right parameter */
	assert( LastDown(x) != x, "echo: right parameter of predefined!" );
	aprint(" ");
	CountChild(y, LastDown(x), count);
	echo(y, type(x)==OPEN ? FORCE_PREC : find_max(outer_prec,DEFAULT_PREC),
	  count);
	if( braces_needed )  aprint(" "), cprint(KW_RBR);
	break;


    case RAW_VERBATIM:
    case VERBATIM:

	cprint(type(x) == VERBATIM ? KW_VERBATIM : KW_RAWVERBATIM);
	aprint(" ");
	cprint(KW_LBR);
	CountChild(y, Down(x), count);
	if( type(y) == WORD )
	{ cprint(string(y));
	}
	else
	{ newline();
	  for( link = Down(y);  link != y;  link = NextDown(link) )
	  { Child(z, link)
	    cprint(string(z));
	    newline();
	  }
	}
	cprint(KW_RBR);
	break;


    case CURR_LANG:
    case CURR_FAMILY:
    case CURR_FACE:
    case CURR_YUNIT:
    case CURR_ZUNIT:
    case BACKEND:
    case PAGE_LABEL:
    case HSPAN:
    case VSPAN:
    case END_HEADER:
    case CLEAR_HEADER:

	/* predefined symbols that have (or may have) no parameters */
	cprint(Image(type(x)));
	break;


    case FILTERED:

	aprint("[filtered ");
	if( Down(x) != x )
	{ Child(y, Down(x));
	  if( type(y) != WORD ) cprint(Image(type(y)));
	  else cprint(string(y));
	}
	else aprint("?");
	aprint("]");
	break;


    case NULL_CLOS:
    
	cprint(Image(type(x)));
	break;


    case CR_ROOT:

	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ CountChild(y, link, count);
	  echo(y, NO_PREC, count);  newline();
	}
	break;


    case CROSS_SYM:

	aprint("Cross-references for ");
	cprint(SymName(symb(x)));  newline();
	switch( target_state(x) )
	{
	  case 0:	aprint("NO_TARGET");
			break;

	  case 1:	aprint("SEEN_TARGET ");
			printnum(target_seq(x));
			aprint(": ");
			echo(target_val(x), NO_PREC, 1);
			break;

	  case 2:	aprint("WRITTEN_TARGET ");
			printnum(target_seq(x));
			aprint(": to file ");
			cprint(FileName(target_file(x)));
			aprint(" at ");
			printnum(target_pos(x));
			break;
	
	  default:	aprint("ILLEGAL!");
			break;
	}
	newline();
	for( link = Down(x);  link != x;  link = NextDown(link) )
	{ Child(y, link);
	  aprint("   ");
	  cprint(Image(cs_type(y)));
	  aprint(": ");
	  cprint(string(y));
	  newline();
	}
	break;


    default:
    
	assert1(FALSE, "echo:", Image(type(x)));
	break;

  } /* end switch */
} /* end echo */


/*@::EchoObject(), DebugObject()@*********************************************/
/*                                                                           */
/*  FULL_CHAR *EchoObject(x)                                                 */
/*                                                                           */
/*  Return an image of unsized object x in result.                           */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *EchoObject(OBJECT x)
{ debug0(DOE, D, "EchoObject()");
  fp = null;
  col = 0;
  indent = 0;
  limit  = 60;
  if( fp == null )
  BeginString();
  if( x == nilobj )  AppendString(AsciiToFull("<nilobj>"));
  else echo(x, type(x) == GAP_OBJ ? VCAT : 0, 1);
  debug0(DOE, D, "EchoObject returning");
  return EndString();
} /* end EchoObject */


/*****************************************************************************/
/*                                                                           */
/*  DebugObject(x)                                                           */
/*                                                                           */
/*  Send an image of unsized object x to result.                             */
/*                                                                           */
/*****************************************************************************/

void DebugObject(OBJECT x)
{ debug0(DOE, D, "DebugObject()");
  fp = stderr;
  col = 0;
  indent = 0;
  limit  = 60;
  if( x == nilobj )  fprintf(stderr, "<nilobj>");
  else echo(x, type(x) == GAP_OBJ ? VCAT : 0, 1);
  fprintf(stderr, "\n");
  debug0(DOE, D, "DebugObject returning");
} /* end DebugObject */


/*@::EchoIndex()@*************************************************************/
/*                                                                           */
/*  FULL_CHAR *EchoIndex()                                                   */
/*                                                                           */
/*  Echo a component of a galley, briefly.                                   */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *EchoIndex(OBJECT index)
{ static char buff[MAX_BUFF];  OBJECT z;
  if( index == nilobj )
  { sprintf(buff, "<nilobj>");
  }
  else switch( type(index) )
  {
    case RECEIVING:

      sprintf(buff, "receiving %s%s", type(actual(index)) == CLOSURE ?
	SymName(actual(actual(index))) : Image(type(actual(index))),
	non_blocking(index) ? " (non_blocking)" : "");
      break;


    case RECEPTIVE:

      sprintf(buff, "receptive %s%s", type(actual(index)) == CLOSURE ?
	SymName(actual(actual(index))) : Image(type(actual(index))),
	non_blocking(index) ? " (non_blocking)" : "");
      break;


    case UNATTACHED:

      if( Down(index) != index )
      { Child(z, Down(index));
      }
      else z = nilobj;
      sprintf(buff, "unattached %s",
	z == nilobj ? AsciiToFull("<nilobj>") : SymName(actual(z)));
      break;


    case WORD:
    case QWORD:

      sprintf(buff, "\"%s\"", string(index));
      break;


    default:

      sprintf(buff, "%s", Image(type(index)));
      break;
  }
  return AsciiToFull(buff);
} /* end EchoIndex */


/*@::DebugGalley()@***********************************************************/
/*                                                                           */
/*  DebugGalley(hd, pinpt, indent)                                           */
/*                                                                           */
/*  Print overview of galley hd on stderr; mark pinpoint if found            */
/*                                                                           */
/*****************************************************************************/
#define eprint(x, a, b, c) fprintf(stderr, "| %d %-7s %20s %s\n", x, a, b, c)

void DebugGalley(OBJECT hd, OBJECT pinpt, int indent)
{ OBJECT link, y;  char istr[30];  int i;
  for( i = 0;  i < indent;  i++ )  istr[i] = ' ';
  istr[i] = '\0';
  if( type(hd) != HEAD )
  { fprintf(stderr, "%shd is %s\n", istr, Image(type(hd)));
    return;
  }
  fprintf(stderr, "%sgalley %s into %s\n", istr,
    SymName(actual(hd)), SymName(whereto(hd)));
  for( link = Down(hd);  link != hd;  link = NextDown(link) )
  { Child(y, link);
    if( y == pinpt || link == pinpt )
    { fprintf(stderr, "++ %d %s ", (int) y, Image(type(y)));
      DebugObject(y);
    }
    else
    if( type(y) == GAP_OBJ )
      eprint((int) y, "gap_obj", Image(type(y)), EchoGap(&gap(y)));
    else if( is_index(type(y)) )
      eprint((int) y, "index", Image(type(y)), "");
    else if( is_definite(type(y)) )
      eprint((int) y, "def_obj", Image(type(y)), is_word(type(y)) ? string(y):STR_EMPTY);
    else if( is_indefinite(type(y)) )
      eprint((int) y, "indefin", Image(type(y)),
	type(y) == CLOSURE ? SymName(actual(y)) : STR_EMPTY);
    else
      eprint((int) y, "unknown", Image(type(y)), "");
  }
} /* end DebugGalley */
#endif
