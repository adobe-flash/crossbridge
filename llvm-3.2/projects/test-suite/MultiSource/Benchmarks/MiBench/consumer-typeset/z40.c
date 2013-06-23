/*@z40.c:Filter Handler:FilterInit()@*****************************************/
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
/*  FILE:         z40.c                                                      */
/*  MODULE:       Filter Handler                                             */
/*  EXTERNS:      FilterInit(), FilterCreate(), FilterSetFileNames(),        */
/*                FilterExecute(), FilterWrite(), FilterScavenge()           */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


static int	filter_count;		/* number of filter files            */
static OBJECT	filter_active;		/* the active filter file records    */
static OBJECT	filter_in_filename;	/* initial name of filter input file */
static OBJECT	filter_out_filename;	/* initial name of filter ouput file */


/*****************************************************************************/
/*                                                                           */
/*  FilterInit()                                                             */
/*                                                                           */
/*  Initialize this module.                                                  */
/*                                                                           */
/*****************************************************************************/

void FilterInit(void)
{ filter_count = 0;
  New(filter_active, ACAT);
  sym_body(FilterInSym)  = MakeWord(WORD, FILTER_IN,  no_fpos);
  sym_body(FilterOutSym) = MakeWord(WORD, FILTER_OUT, no_fpos);
  sym_body(FilterErrSym) = MakeWord(WORD, FILTER_ERR, no_fpos);
  filter_in_filename = sym_body(FilterInSym);
  filter_out_filename = sym_body(FilterOutSym);
} /* end FilterInit */


/*@::FilterCreate(), FilterSetFileNames()@************************************/
/*                                                                           */
/*  OBJECT FilterCreate(use_begin, act, xfpos)                               */
/*                                                                           */
/*  Create and return a new FILTERED object.  Open the corresponding file    */
/*  for writing and dump the parameter text to be filtered into it.          */
/*                                                                           */
/*****************************************************************************/

OBJECT FilterCreate(BOOLEAN use_begin, OBJECT act, FILE_POS *xfpos)
{ FULL_CHAR buff[MAX_LINE];  FILE *fp;  OBJECT x, res, junk;
  debug3(DFH, D, "FilterCreate(%s, %s, %s)", bool(use_begin),
    SymName(act), EchoFilePos(xfpos));
  New(res, FILTERED);
  FposCopy(fpos(res), *xfpos);
  ++filter_count;
  sprintf( (char *) buff, "%s%d", FILTER_IN, filter_count);
  fp = StringFOpen(buff, WRITE_TEXT);
  if( fp == NULL )
    Error(40, 1, "cannot open temporary filter file %s", FATAL, xfpos, buff);
  x = MakeWord(WORD, buff, xfpos);
  filter_use_begin(x) = use_begin;
  filter_actual(x) = act;
  Link(res, x);
  Link(filter_active, x);
  junk = LexScanVerbatim(fp, use_begin, xfpos, FALSE);
  fclose(fp);
  sprintf( (char *) buff, "%s%d", FILTER_OUT, filter_count);
  x = MakeWord(WORD, buff, xfpos);
  Link(res, x);
  if( has_body(act) )  PushScope(act, FALSE, TRUE);
  x = GetScopeSnapshot();
  if( has_body(act) )  PopScope();
  Link(res, x);
  debug2(DFH, D, "FilterCreate returning %d %s", (int) res, EchoObject(res));
  return res;
} /* end FilterCreate */


/*****************************************************************************/
/*                                                                           */
/*  FilterSetFileNames(x)                                                    */
/*                                                                           */
/*  Set @FilterIn, @FilterOut, and @FilterErr to suitable values for the     */
/*  manifesting of the command which runs filter x.                          */
/*                                                                           */
/*****************************************************************************/

void FilterSetFileNames(OBJECT x)
{ OBJECT y;
  assert( type(x) == FILTERED, "FilterSetFileNames: type(x)!" );
  assert( Down(x) != x, "FilterSetFileNames: x has no children!" );
  debug2(DFH, D, "FilterSetFileNames(%d %s)", (int) x, EchoObject(x));
  Child(y, Down(x));
  assert( type(y) == WORD, "FilterSetFileNames: type(y)!" );
  sym_body(FilterInSym) = y;
  Child(y, NextDown(Down(x)));
  assert( type(y) == WORD, "FilterSetFileNames: type(y) (2)!" );
  sym_body(FilterOutSym) = y;
  debug0(DFH, D, "FilterSetFileNames returning.");
} /* end FilterSetFileNames */


/*@::FilterExecute()@*********************************************************/
/*                                                                           */
/*  OBJECT FilterExecute(x, command, env)                                    */
/*                                                                           */
/*  Execute the filter command on FILTERED object x, and return the result.  */
/*                                                                           */
/*****************************************************************************/

OBJECT FilterExecute(OBJECT x, FULL_CHAR *command, OBJECT env)
{ int status;  OBJECT t, res, scope_snapshot;  char line[MAX_LINE];
  FILE *err_fp;  FILE_NUM filter_out_file;

  assert( type(x) == FILTERED, "FilterExecute: type(x)!" );
  assert( type(env) == ENV, "FilterExecute: type(env)!" );
  debug4(DFH, D, "FilterExecute(%d %s, \"%s\", %s)", (int) x, EchoObject(x),
    command, EchoObject(env));

  /* reset FilterInSym since Manifest of @Filter is now complete */
  sym_body(FilterInSym) = filter_in_filename;

  if( SafeExecution )
  {
    /* if safe execution, print error message and return empty object */
    Error(40, 2, "safe execution prohibiting command: %s", WARN, &fpos(x),
      command);
    res = MakeWord(WORD, STR_EMPTY, &fpos(x));
  }
  else
  {
    /* execute the command, echo error messages, and exit if status problem */
    status = system( (char *) command);
    err_fp = StringFOpen(FILTER_ERR, READ_TEXT);
    if( err_fp != NULL )
    { while( fgets(line, MAX_LINE, err_fp) != NULL )
      { if( line[strlen(line)-1] == '\n' )
	  line[strlen(line)-1] = '\0';
        Error(40, 3, "%s", WARN, &fpos(x), line);
      }
      fclose(err_fp);
      StringRemove(FILTER_ERR);
    }
    if( status != 0 )
      Error(40, 4, "failure (non-zero status) of filter: %s",
        FATAL, &fpos(x), command);

    /* read in output of system command as a Lout object */
    /* *** using scope snapshot now
    SwitchScope(nilobj);
    count = 0;
    SetScope(env, &count, TRUE);
    *** */
    Child(scope_snapshot, LastDown(x));
    LoadScopeSnapshot(scope_snapshot);
    debug0(DFS, D, "  calling DefineFile from FilterExecute");
    filter_out_file =
      DefineFile(string(sym_body(FilterOutSym)), STR_EMPTY, &fpos(x),
        FILTER_FILE, SOURCE_PATH);
    LexPush(filter_out_file, 0, FILTER_FILE, 1, FALSE);
    t = NewToken(BEGIN, &fpos(x), 0, 0, BEGIN_PREC, FilterOutSym);
    res = Parse(&t, nilobj, FALSE, FALSE);
    LexPop();
    /* *** using scope snapshot now
    for( i = 1;  i <= count;  i++ )  PopScope();
    UnSwitchScope(nilobj);
    *** */
    ClearScopeSnapshot(scope_snapshot);
    StringRemove(string(sym_body(FilterOutSym)));
    sym_body(FilterOutSym) = filter_out_filename;
  }

  debug1(DFH, D, "FilterExecute returning %s", EchoObject(res));
  return res;
} /* end FilterExecute */


/*@::FilterWrite(), FilterScavenge()@*****************************************/
/*                                                                           */
/*  FilterWrite(x, fp, linecount)                                            */
/*                                                                           */
/*  Write out the active FILTERED object x by copying the file.              */
/*  Increment *linecount by the number of lines written.                     */
/*                                                                           */
/*****************************************************************************/

void FilterWrite(OBJECT x, FILE *fp, int *linecount)
{ FILE *in_fp;  OBJECT y;  int ch;
  assert( type(x) == FILTERED, "FilterWrite: type(x)!" );
  debug2(DFH, D, "[ FilterWrite(%d %s, fp)", (int) x, EchoObject(x));
  Child(y, Down(x));
  in_fp = StringFOpen(string(y), READ_TEXT);
  if( in_fp == NULL )
    Error(40, 5, "cannot read filter temporary file %s",
      FATAL, &fpos(x), string(y));
  if( filter_use_begin(y) )
  { StringFPuts(KW_BEGIN, fp);
    StringFPuts("\n", fp);
    *linecount += 1;
    while( (ch = getc(in_fp)) != EOF )
    { putc(ch, fp);
      if( ch == '\n' )  *linecount += 1;
    }
    StringFPuts(KW_END, fp);
    StringFPuts(" ", fp);
    StringFPuts(SymName(filter_actual(y)), fp);
  }
  else
  { StringFPuts(KW_LBR, fp);
    StringFPuts("\n", fp);
    *linecount += 1;
    while( (ch = getc(in_fp)) != EOF )
    { putc(ch, fp);
      ifdebug(DFH, D, putc(ch, stderr));
      if( ch == '\n' )  *linecount += 1;
    }
    StringFPuts(KW_RBR, fp);
  }
  StringFPuts("\n", fp);
  *linecount += 1;
  fclose(in_fp);
  debug0(DFH, D, "] FilterWrite returning.");
} /* end FilterWrite */


/*****************************************************************************/
/*                                                                           */
/*  FilterScavenge(all)                                                      */
/*                                                                           */
/*  Unlink unneeded filter files, or all remaining filter files if all.      */
/*                                                                           */
/*****************************************************************************/

void FilterScavenge(BOOLEAN all)
{ OBJECT y, link, nextlink;
  ifdebug(DFH, D, return);
  debug1(DFH, D, "FilterScavenge(%s)", bool(all));
  for( link = Down(filter_active);  link != filter_active;  link = nextlink )
  { Child(y, link);
    nextlink = NextDown(link);
    if( all || Up(y) == LastUp(y) )
    { debug1(DFH, D, "FilterScavenge scavenging %s", string(y));
      StringRemove(string(y));
      DisposeChild(link);
    }
  }
  debug0(DFH, D, "FilterScavenge returning.");
} /* end FilterScavenge */
