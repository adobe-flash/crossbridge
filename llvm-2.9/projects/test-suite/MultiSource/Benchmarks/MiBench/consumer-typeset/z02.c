/*@z02.c:Lexical Analyser:Declarations@***************************************/
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
/*  FILE:         z02.c                                                      */
/*  MODULE:       Lexical Analyser                                           */
/*  EXTERNS:      LexLegalName(), LexInit(), LexPush(), LexPop(),            */
/*                LexNextTokenPos(), LexGetToken()                           */
/*                                                                           */
/*  Implementation note:  this fast and cryptic lexical analyser is adapted  */
/*  from Waite, W. M.: The Cost of Lexical Analysis, in Software - Practice  */
/*  and Experience, v16, pp473-488 (May 1986).                               */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define	BUFFER_SIZE    8192		/* size of buffer for block read     */
#define	OTHER		0		/* punctuation or other character    */
#define	LETTER		1		/* letter type                       */
#define	QUOTE		2		/* quoted string delimiter type      */
#define	ESCAPE		3		/* escape character inside strings   */
#define	COMMENT		4		/* comment delimiter type            */
#define	CSPACE		5		/* space character type              */
#define	FORMFEED	6		/* formfeed character type           */
#define	TAB		7		/* tab character type                */
#define	NEWLINE		8		/* newline character type            */
#define	ENDFILE		9		/* end of file character type        */

static	unsigned char	chtbl[256];	/* type table indexed by a FULL_CHAR */
static	FULL_CHAR	*chpt;		/* pointer to current text character */
static	FULL_CHAR	*frst;		/* address of first buffer character */
static	FULL_CHAR	*limit;		/* just past last char in buffer     */
static	FULL_CHAR	*buf;		/* the character buffer start pos    */
static	int		blksize;	/* size of block read; others too    */
static	FULL_CHAR	last_char;	/* last char read in from file       */
static	FULL_CHAR	*startline;	/* position in buff of last newline  */
static	FILE_NUM	this_file;	/* number of currently open file     */
static	FILE		*fp;		/* current input file                */
static	FILE_POS	file_pos;	/* current file position             */
static	short		ftype;		/* the type of the current file      */
static	OBJECT		next_token;	/* next token if already read	     */
static	int		offset;		/* where to start reading in file    */
static	int		first_line_num;	/* number of first line (if offset)  */
static	BOOLEAN		same_file;	/* TRUE if same file as preceding    */
static	FULL_CHAR	*mem_block;	/* file buffer                       */

static int stack_free;		/* first free slot in lexical stack  */
static struct {
  FULL_CHAR	*chpt;		/* pointer to current text character */
  FULL_CHAR	*frst;		/* address of first buffer character */
  FULL_CHAR	*limit;		/* just past last char in buffer     */
  FULL_CHAR	*buf;		/* the character buffer start pos    */
  int		blksize;	/* size of block read; others too    */
  FULL_CHAR	last_char;	/* last char read in from file	     */
  FULL_CHAR	*startline;	/* position in buff of last newline  */
  FILE_NUM	this_file;	/* number of currently open file     */
  FILE		*fp;		/* current input file                */
  FILE_POS	file_pos;	/* current file position             */
  short		ftype;		/* the type of the current file      */
  OBJECT	next_token;	/* next token if already read	     */
  int		offset;		/* where to start reading in file    */
  int		first_line_num;	/* number of first line (if offset)  */
  BOOLEAN	same_file;	/* TRUE if same file as preceding    */
  long		save_ftell;	/* ftell() position if same_file     */
  FULL_CHAR	*mem_block;	/* file buffer                       */
} lex_stack[MAX_LEX_STACK];

/*@::LexLegalName(), LexInit()@***********************************************/
/*                                                                           */
/*  BOOLEAN LexLegalName(str)                                                */
/*                                                                           */
/*  Check whether str is a valid name for a symbol table entry.              */
/*  Valid names have the BNF form                                            */
/*                                                                           */
/*       <name> ::= <letter>  { <letter> }                                   */
/*       <name> ::= <special> { <special> }                                  */
/*       <name> ::= <escape>  { <letter> }                                   */
/*                                                                           */
/*  The third form is inaccessible to users and is for internal use only.    */
/*                                                                           */
/*****************************************************************************/

BOOLEAN LexLegalName(FULL_CHAR *str)
{ int i;  BOOLEAN res;
  debug1(DLA, DDD, "LexLegalName( %s )", str);
  switch( chtbl[str[0]] )
  {
    case ESCAPE:
    case LETTER:
    
      for( i = 1;  chtbl[str[i]] == LETTER;  i++ );
      res = str[i] == '\0';
      break;


    case OTHER:
    
      for( i = 1;  chtbl[str[i]] == OTHER;  i++ );
      res = str[i] == '\0';
      break;


    default:
    
      res = FALSE;
      break;

  }
  debug1(DLA, DDD, "LexLegalName returning %s", bool(res));
  return res;
} /* end LexLegalName */


/*****************************************************************************/
/*                                                                           */
/*  LexInit()                                                                */
/*                                                                           */
/*  Initialise character types.  Those not touched are 0 (OTHER).            */
/*  The function initchtbl() assists in initializing the chtbl.              */
/*                                                                           */
/*****************************************************************************/

static void initchtbl(val, str)
int val;  FULL_CHAR *str;
{ int i;
  for( i = 0;  str[i] != '\0';  i++ )
	chtbl[ str[i] ] = val;
} /* end initchtbl */

void LexInit(void)
{ initchtbl(LETTER,  STR_LETTERS_LOWER);
  initchtbl(LETTER,  STR_LETTERS_UPPER);
  initchtbl(LETTER,  STR_LETTERS_SYMSTART);
  initchtbl(LETTER,  STR_LETTERS_UNDERSCORE);
  initchtbl(LETTER,  STR_LETTERS_EXTRA0);
  initchtbl(LETTER,  STR_LETTERS_EXTRA1);
  initchtbl(LETTER,  STR_LETTERS_EXTRA2);
  initchtbl(LETTER,  STR_LETTERS_EXTRA3);
  initchtbl(LETTER,  STR_LETTERS_EXTRA4);
  initchtbl(LETTER,  STR_LETTERS_EXTRA5);
  initchtbl(LETTER,  STR_LETTERS_EXTRA6);
  initchtbl(LETTER,  STR_LETTERS_EXTRA7);
  initchtbl(QUOTE,   STR_QUOTE);
  initchtbl(ESCAPE,  STR_ESCAPE);
  initchtbl(COMMENT, STR_COMMENT);
  initchtbl(CSPACE,  STR_SPACE);
  initchtbl(FORMFEED,STR_FORMFEED);
  initchtbl(TAB,     STR_TAB);
  initchtbl(NEWLINE, STR_NEWLINE);
  chtbl['\0'] = ENDFILE;
  stack_free = -1;
} /* end LexInit */

/*@::LexPush(), LexPop()@*****************************************************/
/*                                                                           */
/*  LexPush(x, offs, ftype, lnum, same)                                      */
/*                                                                           */
/*  Start reading from the file sequence whose first file is x (subsequent   */
/*  files are obtained from NextFile).  The first file (x) is to be fseeked  */
/*  to offs.  When the sequence is done, ftype determines how to continue:   */
/*                                                                           */
/*      ftype          action                                                */
/*                                                                           */
/*      SOURCE_FILE    last input file ends, return @End \Input              */
/*      DATABASE_FILE  database file, return @End \Input                     */
/*      INCLUDE_FILE   include file, must pop lexical analyser and continue  */
/*      FILTER_FILE    filter file, return @End @FilterOut                   */
/*                                                                           */
/*  lnum is the line number at offs, to be used when creating file pos's     */
/*  in the tokens returned.  same is TRUE when this file is the same as      */
/*  the file currently being read, in which case there is no need to         */
/*  close that file and open this one; just an fseek is required.            */
/*                                                                           */
/*****************************************************************************/

void LexPush(FILE_NUM x, int offs, int ftyp, int lnum, BOOLEAN same)
{ int i;
  debug5(DLA, DD, "LexPush(%s, %d, %s, %d, %s)", FileName(x), offs,
    ftyp==SOURCE_FILE ? "source" : ftyp==INCLUDE_FILE ? "include":"database",
    lnum, bool(same));
  if( stack_free >= MAX_LEX_STACK - 1 )
  { if( ftyp == INCLUDE_FILE )
      Error(2, 1, "too many open files when opening include file %s; open files are:",
        WARN, PosOfFile(x), FullFileName(x));
    else
      Error(2, 2, "too many open files when opening database file %s; open files are:",
        WARN, PosOfFile(x), FileName(x));
    for( i = stack_free - 1;  i >= 0;  i-- )
    {
      Error(2, 23, "  %s", WARN, no_fpos,
	EchoFileSource(lex_stack[i].this_file));
    }
    Error(2, 24, "exiting now", FATAL, no_fpos);
  }
  if( stack_free >= 0 )  /* save current state */
  { lex_stack[stack_free].chpt		 = chpt;
    lex_stack[stack_free].frst		 = frst;
    lex_stack[stack_free].limit		 = limit;
    lex_stack[stack_free].buf		 = buf;
    lex_stack[stack_free].blksize	 = blksize;
    lex_stack[stack_free].last_char	 = last_char;
    lex_stack[stack_free].startline	 = startline;
    lex_stack[stack_free].this_file	 = this_file;
    lex_stack[stack_free].fp		 = fp;
    lex_stack[stack_free].ftype		 = ftype;
    lex_stack[stack_free].next_token	 = next_token;
    lex_stack[stack_free].offset	 = offset;
    lex_stack[stack_free].first_line_num = first_line_num;
    lex_stack[stack_free].same_file	 = same_file;
    lex_stack[stack_free].mem_block	 = mem_block;
    FposCopy( lex_stack[stack_free].file_pos, file_pos );
  }
  stack_free += 1;
  ifdebug(DMA, D,
    DebugRegisterUsage(MEM_LEX,1, (MAX_LINE+BUFFER_SIZE+2)*sizeof(FULL_CHAR)));
  mem_block = (FULL_CHAR *) malloc((MAX_LINE+BUFFER_SIZE+2)*sizeof(FULL_CHAR));
  if( mem_block == NULL )
    Error(2, 3, "run out of memory when opening file %s",
      FATAL, PosOfFile(x), FullFileName(x));
  buf = chpt = &mem_block[MAX_LINE];
  last_char = CH_NEWLINE;
  this_file = x;  offset = offs;
  first_line_num = lnum;  same_file = same;
  ftype = ftyp;  next_token = nilobj;
  *chpt = '\0';
  if( same_file )
  { lex_stack[stack_free-1].save_ftell = ftell(fp);
  }
  else
  { fp = null;
  }
} /* end LexPush */


/*****************************************************************************/
/*                                                                           */
/*  LexPop() - pop lexical analyser.                                         */
/*                                                                           */
/*****************************************************************************/

void LexPop(void)
{ debug0(DLA, DD, "LexPop()");
  assert( stack_free > 0, "LexPop: stack_free <= 0!" );
  stack_free--;
  if( same_file )
  { fseek(fp, lex_stack[stack_free].save_ftell, SEEK_SET);
  }
  else
  { if( fp != null )  fclose(fp);
  }
  ifdebug(DMA, D,
   DebugRegisterUsage(MEM_LEX,-1,-(MAX_LINE+BUFFER_SIZE+2)* (int) sizeof(FULL_CHAR))
  );
  free( (char *) mem_block);
  mem_block	 = lex_stack[stack_free].mem_block;
  chpt		 = lex_stack[stack_free].chpt;
  frst		 = lex_stack[stack_free].frst;
  limit		 = lex_stack[stack_free].limit;
  buf		 = lex_stack[stack_free].buf;
  blksize	 = lex_stack[stack_free].blksize;
  last_char	 = lex_stack[stack_free].last_char;
  startline	 = lex_stack[stack_free].startline;
  this_file	 = lex_stack[stack_free].this_file;
  fp		 = lex_stack[stack_free].fp;
  ftype		 = lex_stack[stack_free].ftype;
  next_token	 = lex_stack[stack_free].next_token;
  offset	 = lex_stack[stack_free].offset;
  first_line_num = lex_stack[stack_free].first_line_num;
  same_file	 = lex_stack[stack_free].same_file;
  FposCopy( file_pos, lex_stack[stack_free].file_pos );
} /* end LexPop */


/*@::setword(), LexNextTokenPos(), srcnext()@*********************************/
/*                                                                           */
/*  setword(typ, res, file_pos, str, len)                                    */
/*                                                                           */
/*  Set variable res to a WORD or QWORD token containing string str, etc.    */
/*                                                                           */
/*****************************************************************************/

#define setword(typ, res, file_pos, str, len)				\
{ NewWord(res, typ, len, &file_pos);					\
  FposCopy(fpos(res), file_pos);					\
  for( c = 0;  c < len;  c++ ) string(res)[c] = str[c];			\
  string(res)[c] = '\0';						\
}


/*****************************************************************************/
/*                                                                           */
/*  long LexNextTokenPos()                                                   */
/*                                                                           */
/*  Equivalent to ftell() on the (buffered) current lex file.                */
/*                                                                           */
/*****************************************************************************/

long LexNextTokenPos(void)
{ long res;
  if( next_token != nilobj )
    Error(2, 4, "illegal macro invocation in database",
      FATAL, &fpos(next_token));
  res = ftell(fp) - (limit - chpt) - (buf - frst);
#if DB_FIX
  /* uwe: 1997-11-04
   *
   * On NT under Visual C++ ftell() and fseek() always use binary
   * positions, even if the file was opened in text mode.  This means
   * that every LF in between the CHPT and LIMIT was counted by
   * ftell() as *TWO* bytes.  The pointer arithmetic above adjusts the
   * ftold value as lout has not yet read chars past CHPT, but it
   * counts each LF as *ONE* byte, naturally.
   *
   * The code below compensates for this binary/text brain death.
   *
   * PS: gcc from Cygnus' gnuwin32 has sane ftell() and does *NOT*
   * need this workaround (I haven't tried compiling lout with gcc
   * though, as the result will need cygwin.dll to run).
   */
  {
    register FULL_CHAR *p;
    for (p = chpt; p < limit; ++p) {
      if (*p == (FULL_CHAR) CH_NEWLINE)
        --res;
    }
  }
#endif /* DB_FIX */

  debug1(DLA, DD, "LexNextTokenPos() returning %ld", res);
  return res;
}


/*****************************************************************************/
/*                                                                           */
/*  static srcnext()                                                         */
/*                                                                           */
/*  Move to new line of input file.  May need to recharge buffer.            */
/*                                                                           */
/*****************************************************************************/

static void srcnext(void)
{ register FULL_CHAR *col;
  debugcond4(DLA, DD, stack_free <= 1,
    "srcnext();  buf: %d, chpt: %d, frst: %d, limit: %d",
    buf - mem_block, chpt - mem_block, frst - mem_block, limit - mem_block);

  /* if time to transfer last line to area preceding buffer, do so */
  if( blksize != 0 && chpt < limit )
  { debugcond0(DLA, DD, stack_free <= 1, "srcnext: transferring.");
    col = buf;
    while( (*--col = *--limit) != CH_NEWLINE );
    frst = col + 1;  limit++;  blksize = 0;
  }

  /* if buffer is empty, read next block */
  /*** changed by JK 9/92 from "if( chpt == limit )" to fix long lines bug */
  if( chpt >= limit )
  { if( chpt > limit )
    { col_num(file_pos) = 1;
      Error(2, 5, "line is too long (or final newline missing)",
	FATAL, &file_pos);
    }
    chpt = frst;
    blksize = fread( (char *) buf, sizeof(char), BUFFER_SIZE, fp);
    if( blksize > 0 )
      last_char = *(buf + blksize - 1);
    if( blksize < BUFFER_SIZE && last_char != CH_NEWLINE )
    {
      /* at end of file since blksize = 0; so add missing newline char */
      blksize++;
      last_char = *(buf+blksize-1) = CH_NEWLINE;

      /* this adjustment breaks LexNextTokenPos, so fatal error if database */
      if( ftype == DATABASE_FILE )
      {
	line_num(file_pos) = col_num(file_pos) = 0;
	Error(2, 25, "a database file must end with a newline; this one doesn't",
	  FATAL, &file_pos);
      }
    }
    debugcond4(DLA, DD, stack_free <= 1,
      "srcnext: %d = fread(0x%x, %d, %d, fp)",
      blksize, buf, sizeof(char), BUFFER_SIZE);
    frst = buf;  limit = buf + blksize;  *limit = CH_NEWLINE;
  }

  /* if nothing more to read, make this clear */
  if( chpt >= limit )
  { debugcond0(DLA, DD, stack_free <= 1, "srcnext: nothing more to read");
    chpt = limit = buf;  *limit = '\0';
  }
  debugcond4(DLA, DD, stack_free <= 1,
    "srcnext returning;  buf: %d, chpt: %d, frst: %d, limit: %d",
    buf - mem_block, chpt - mem_block, frst - mem_block, limit - mem_block);
} /* end srcnext */


/*@::LexGetToken()@***********************************************************/
/*                                                                           */
/*  OBJECT LexGetToken()                                                     */
/*                                                                           */
/*  Get next token from input.  Look it up in symbol table.                  */
/*                                                                           */
/*****************************************************************************/

OBJECT LexGetToken(void)
{
	   FULL_CHAR *startpos;		/* where the latest token started    */
  register FULL_CHAR *p, *q;		/* pointer to current input char     */
  register int      c;			/* temporary character (really char) */
  OBJECT   res;				/* result token                      */
  int vcount, hcount;			/* no. of newlines and spaces seen   */

  if( next_token != nilobj )
  { next_token = Delete(res = next_token, PARENT);
    debugcond4(DLA, DD, stack_free <= 1,
      "LexGetToken%s (in macro) returning %d.%d %s",
      EchoFilePos(&file_pos), vspace(res), hspace(res), EchoToken(res));
    return res;
  }

  res = nilobj;  p = chpt;
  vcount = hcount = 0;
  do switch( chtbl[*p++] )
  {
      case ESCAPE:
      
	if( ftype==DATABASE_FILE && *p>='a' && *p<='z' && *(p+1) == '{' /*}*/ )
	{ res = NewToken(LBR, &file_pos, 0, 0, (unsigned) *p, StartSym);
	  p += 2;
	}
	else
	{
	  col_num(file_pos) = (startpos = p-1) - startline;
	  Error(2, 6, "character %c outside quoted string",
	    WARN, &file_pos, *startpos);
	}
	break;


      case COMMENT:
      
	debug1(DLA, DDD, "LexGetToken%s: comment", EchoFilePos(&file_pos));
	while( (c = *p++) != CH_NEWLINE && c != '\0' );
	if( c == CH_NEWLINE )
	{
	  /* do NEWLINE action, only preserve existing horizontal space */
	  /* and don't count the newline in the vcount. */
	  chpt = p;  srcnext();
	  line_num(file_pos)++;
	  col_num(file_pos) = 0;
	  startline = (p = chpt) - 1;
	}
	else
	{
	  --p;
	}
	break;


      case CSPACE:
      case FORMFEED:

	hcount++;
	break;


      case TAB:

	hcount += 8;
	break;


      case NEWLINE:
      
	chpt = p;  srcnext();
	line_num(file_pos)++;
	col_num(file_pos) = 0;
	vcount++;  hcount = 0;
	startline = (p = chpt) - 1;
	break;


      case ENDFILE:
      
	debug0(DLA, DDD, "LexGetToken: endfile");
	if( !same_file )
	{
	  /* close current file, if any */
	  if( fp != null )
	  { fclose(fp);  fp = null;
	    this_file = ftype == SOURCE_FILE ? NextFile(this_file) : NO_FILE;
	  }

	  /* open next file */
	  while( this_file != NO_FILE )
	  { file_num(file_pos) = this_file;
	    line_num(file_pos) = 1;
	    col_num(file_pos) = 0;
	    fp = OpenFile(this_file, FALSE, TRUE);
	    if( fp != null )  break;
	    Error(2, 7, "cannot open file %s",
	      WARN, &file_pos, FullFileName(this_file));
	    this_file = ftype == SOURCE_FILE ? NextFile(this_file) : NO_FILE;
	  }
	}

	if( fp != null )
	{ if( offset != 0 )
	  { debugcond1(DLA, DD, stack_free <= 1, "fseek(fp, %d, SEEK_SET)", offset);
	    fseek(fp, (long) offset, SEEK_SET);
	    offset = 0L;
	    line_num(file_pos) = first_line_num;
	  }
	  frst = limit = chpt = buf;
	  blksize = 0;  last_char = CH_NEWLINE;
	  srcnext();
	  startline = (p = chpt) - 1;
	  hcount = 0;
	}

	/* no next file, so take continuation */
	else switch( ftype )
	{

	  case SOURCE_FILE:
	  case DATABASE_FILE:
	  
	    /* input ends with "@End \Input" then UNEXPECTED_EOF */
	    res = NewToken(END, &file_pos, 0, 0, END_PREC, StartSym);
	    next_token = NewToken(UNEXPECTED_EOF, &file_pos,0,0,NO_PREC,nilobj);
	    --p;  startline = p;
	    break;


	  case FILTER_FILE:
	  
	    /* input ends with "@End @FilterOut" */
	    res = NewToken(END, &file_pos, 0, 0, END_PREC, FilterOutSym);
	    /* ***
	    next_token = NewToken(CLOSURE,&file_pos,0,0,NO_PREC,FilterOutSym);
	    *** */
	    --p;  startline = p;
	    break;


	  case INCLUDE_FILE:

	    LexPop();
	    p = chpt;
	    hcount = 0;
	    break;


	  default:
	  
	    assert(FALSE, "unknown file type");
	    break;

	} /* end switch */
	break;


      case OTHER:
      
	col_num(file_pos) = (startpos = p-1) - startline;
	while( chtbl[*p++] == OTHER );
	c = p - startpos - 1;
	do
	{ res = SearchSym(startpos, c);
	  --c; --p;
	} while( c > 0 && res == nilobj );
	goto MORE;  /* 7 lines down */


      case LETTER:
      
	col_num(file_pos) = (startpos = p-1) - startline;
	while( chtbl[*p++] == LETTER );  --p;
	res = SearchSym(startpos, p - startpos);

	MORE: if( res == nilobj )
	{ setword(WORD, res, file_pos, startpos, p-startpos);
	}
	else if( type(res) == MACRO )
	{ if( recursive(res) )
	  { Error(2, 8, "recursion in macro", WARN, &file_pos);
	    setword(WORD, res, file_pos, startpos, p-startpos);
	  }
	  else
	  { res = CopyTokenList( sym_body(res), &file_pos );
	    if( res != nilobj ) next_token = Delete(res, PARENT);
	    else hcount = 0;
	  }
	}
	else if( predefined(res) == 0 )
	{ res = NewToken(CLOSURE, &file_pos, 0, 0, precedence(res), res);
	}
	else if( predefined(res) == INCLUDE || predefined(res) == SYS_INCLUDE )
	{ OBJECT t, fname;  FILE_NUM fnum;  int len;  BOOLEAN scope_suppressed;
	  chpt = p;
	  t = LexGetToken();
	  scope_suppressed = (type(t)==WORD && StringEqual(string(t), KW_LBR));

	  if( type(t)!=LBR && !scope_suppressed )
	  { Error(2, 9, "%s expected (after %s)",
	      WARN, &fpos(t), KW_LBR, SymName(res));
	    Dispose(t);
	    res = nilobj;
	    break;
	  }
	  if( scope_suppressed )
	  { UnSuppressScope();
	    Dispose(t);
	    New(t, LBR);
	  }
	  fname = Parse(&t, nilobj, FALSE, FALSE);
	  fname = ReplaceWithTidy(fname, FALSE);
	  if( scope_suppressed ) SuppressScope();
	  if( !is_word(type(fname)) )
	  { Error(2, 10, "name of include file expected here",
	      WARN, &fpos(fname));
	    Dispose(fname);
	    res = nilobj;
	    break;
	  }
	  len = StringLength(string(fname)) - StringLength(SOURCE_SUFFIX);
	  if( len >= 0 && StringEqual(&string(fname)[len], SOURCE_SUFFIX) )
	    StringCopy(&string(fname)[len], STR_EMPTY);
	  debug0(DFS, D, "  calling DefineFile from LexGetToken");
	  fnum = DefineFile(string(fname), STR_EMPTY, &fpos(fname),
	      INCLUDE_FILE,
	      predefined(res)==INCLUDE ? INCLUDE_PATH : SYSINCLUDE_PATH);
	  Dispose(fname);
	  LexPush(fnum, 0, INCLUDE_FILE, 1, FALSE);
	  res = LexGetToken();
	  vcount++; /** TEST ADDITION! **/
	  p = chpt;
	}
	else if( predefined(res) == END )
	  res = NewToken(predefined(res), &file_pos,0,0,precedence(res),nilobj);
	else
	  res = NewToken(predefined(res), &file_pos,0,0,precedence(res),res);
	break;


      case QUOTE:
      
	col_num(file_pos) = (startpos = q = p) - 1 - startline;
	do switch( chtbl[*q++ = *p++] )
	{
	  case OTHER:
	  case LETTER:
	  case COMMENT:
	  case CSPACE:
	  case FORMFEED:
	  case TAB:	break;

	  case NEWLINE:
	  case ENDFILE:	--p;
			Error(2, 11, "unterminated string", WARN, &file_pos);
			setword(QWORD, res, file_pos, startpos, q-1-startpos);
			break;

	  case QUOTE:	setword(QWORD, res, file_pos, startpos, q-1-startpos);
			break;

	  case ESCAPE:	q--;
			if( chtbl[*p] == NEWLINE || chtbl[*p] == ENDFILE )
			{ Error(2, 12, "unterminated string", WARN, &file_pos);
			  setword(QWORD, res, file_pos, startpos, q-startpos);
			}
			else if( octaldigit(*p) )
			{ int count, ch;
			  count = ch = 0;
			  do
			  { ch = ch * 8 + digitchartonum(*p++);
			    count++;
			  } while( octaldigit(*p) && count < 3 );
			  if( ch == '\0' )
			    Error(2, 13, "skipping null character in string",
			      WARN, &file_pos);
			  else *q++ = ch;
			}
			else *q++ = *p++;
			break;

	  default:	Error(2, 14, "LexGetToken: error in quoted string",
			  INTERN, &file_pos);
			break;

	} while( res == nilobj );
	break;


      default:
      
	assert(FALSE, "LexGetToken: bad chtbl[]");
	break;

  } while( res == nilobj );

  if( p - startline >= MAX_LINE )
  { col_num(file_pos) = 1;
    Error(2, 15, "line is too long (or final newline missing)",FATAL,&file_pos);
  }

  chpt = p;
  vspace(res) = vcount;
  hspace(res) = hcount;
  debugcond5(DLA, DD, stack_free <= 1, "LexGetToken%s returning %s %s %d.%d",
    EchoFilePos(&file_pos), Image(type(res)), EchoToken(res),
    vspace(res), hspace(res));
  return res;
} /* end LexGetToken */


/*@::LexScanVerbatim@*********************************************************/
/*                                                                           */
/*  OBJECT LexScanVerbatim(fp, end_stop, err_pos, lessskip)                  */
/*                                                                           */
/*  Scan input file and transfer to filter file fp, or if that is NULL, make */
/*  a VCAT of objects, one per line (or just a WORD if one line only), and   */
/*  return that object as the result.  If end_stop, terminate at @End, else  */
/*  terminate at matching right brace.                                       */
/*                                                                           */
/*  If lessskip is true it means that we should skip only up to and          */
/*  including the first newline character, as opposed to the usual           */
/*  skipping of all initial white space characters.                          */
/*                                                                           */
/*****************************************************************************/

#define print(ch)							\
{ debug2(DLA, D, "print(%c), bufftop = %d", ch, bufftop);		\
  if( fp == NULL )							\
  { if( bufftop < MAX_BUFF )						\
    { if( chtbl[ch] == NEWLINE )					\
      { res = BuildLines(res, buff, &bufftop);				\
      }									\
      else buff[bufftop++] = ch;					\
    }									\
  }									\
  else putc(ch, fp);							\
}

#define clear()								\
{ int i;								\
  for( i = 0;  i < hs_top;  i++ )  print(hs_buff[i]);			\
  hs_top = 0;								\
}

#define hold(ch)							\
{ if( hs_top == MAX_BUFF )  clear();					\
  hs_buff[hs_top++] = ch;						\
}

static OBJECT BuildLines(OBJECT current, FULL_CHAR *buff, int *bufftop)
{ OBJECT wd, res, gp, gpword;  int c;

  /* build a new word and reset the buffer */
  setword(WORD, wd, file_pos, buff, *bufftop);
  debug1(DLA, D, "BuildLines(current, %s)", EchoObject(wd));
  *bufftop = 0;

  if( current == nilobj )
  {
    /* if this is the first word, make it the result */
    res = wd;
  }
  else
  {
    /* if this is the second word, make the result a VCAT */
    if( type(current) == WORD )
    { New(res, VCAT);
      FposCopy(fpos(res), fpos(current));
      Link(res, current);
    }
    else res = current;

    /* now attach the new word to res, preceded by a one-line gap */
    New(gp, GAP_OBJ);
    mark(gap(gp)) = FALSE;
    join(gap(gp)) = FALSE;
    FposCopy(fpos(gp), file_pos);
    gpword = MakeWord(WORD, AsciiToFull("1vx"), &file_pos);
    Link(gp, gpword);
    Link(res, gp);
    Link(res, wd);
  }
  debug1(DLA, D, "BuildLines returning %s", EchoObject(res));
  return res;
}

OBJECT LexScanVerbatim(fp, end_stop, err_pos, lessskip)
FILE *fp;  BOOLEAN end_stop;  FILE_POS *err_pos;  BOOLEAN lessskip;
{
  register FULL_CHAR *p;		/* pointer to current input char     */
  int depth;				/* depth of nesting of { ... }       */
  BOOLEAN finished;			/* TRUE when finished                */
  BOOLEAN skipping;			/* TRUE when skipping initial spaces */
  FULL_CHAR hs_buff[MAX_BUFF];		/* hold spaces here in case last     */
  int hs_top;				/* next free spot in hs_buff         */
  FULL_CHAR buff[MAX_BUFF];		/* hold line here if not to file     */
  int bufftop;				/* top of buff                       */
  OBJECT res = nilobj;			/* result object if not to file      */

  debug3(DLA, D, "LexScanVerbatim(fp, %s, %s, %s)",
    bool(end_stop), EchoFilePos(err_pos), bool(lessskip));
  if( next_token != nilobj )
  { Error(2, 16, "filter parameter in macro", FATAL, err_pos);
  }

  p = chpt;  depth = 0;
  finished = FALSE;
  skipping = TRUE;
  hs_top = 0;
  bufftop = 0;
  while( !finished ) switch( chtbl[*p++] )
  {
      case ESCAPE:
      case COMMENT:
      case QUOTE:
      
	skipping = FALSE;
	clear();
	print(*(p-1));
	break;


      case CSPACE:
      case TAB:
      case FORMFEED:
      
	if( !skipping )  hold(*(p-1));
	break;


      case NEWLINE:
      
	if( !skipping )  hold(*(p-1));
	if( lessskip ) skipping = FALSE;
	chpt = p;  srcnext();
	line_num(file_pos)++;
	col_num(file_pos) = 0;
	startline = (p = chpt) - 1;
	break;


      case ENDFILE:
      
	if( fp == NULL )
	  Error(2, 22, "end of file reached while reading %s",
	    FATAL, err_pos, lessskip ? KW_RAWVERBATIM : KW_VERBATIM);
	else
	  Error(2, 17, "end of file reached while reading filter parameter",
	    FATAL, err_pos);
	break;


      case OTHER:
      
	skipping = FALSE;
	if( *(p-1) == '{' /*}*/ )
	{ clear();
	  print(*(p-1));
	  depth++;
	}
	else if( *(p-1) == /*{*/ '}' )
	{ if( !end_stop && depth == 0 )
	  { p--;
	    finished = TRUE;
	  }
	  else
	  { clear();
	    print(*(p-1));
	    depth--;
	  }
	}
	else
	{ clear();
	  print(*(p-1));
	}
	break;


      case LETTER:
      
	skipping = FALSE;
	if( *(p-1) == '@' )
	{
	  p--;
	  if( end_stop && StringBeginsWith(p, KW_END) )
	  { finished = TRUE;
	  }
	  else if( StringBeginsWith(p, KW_INCLUDE) ||
		   StringBeginsWith(p, KW_SYSINCLUDE) )
	  { OBJECT incl_fname, t;  FILE *incl_fp;  int ch;  FILE_NUM fnum;
	    BOOLEAN sysinc = StringBeginsWith(p, KW_SYSINCLUDE);
	    clear();
	    p += sysinc ? StringLength(KW_SYSINCLUDE):StringLength(KW_INCLUDE);
	    chpt = p;
	    t = LexGetToken();
	    if( type(t) != LBR )  Error(2, 18, "expected %s here (after %s)",
		FATAL, &fpos(t), KW_LBR, sysinc ? KW_SYSINCLUDE : KW_INCLUDE);
	    incl_fname = Parse(&t, nilobj, FALSE, FALSE);
	    p = chpt;
	    incl_fname = ReplaceWithTidy(incl_fname, FALSE);
	    if( !is_word(type(incl_fname)) )
	      Error(2, 19, "expected file name here", FATAL,&fpos(incl_fname));
	    debug0(DFS, D, "  calling DefineFile from LexScanVerbatim");
	    fnum = DefineFile(string(incl_fname), STR_EMPTY, &fpos(incl_fname),
	      INCLUDE_FILE, sysinc ? SYSINCLUDE_PATH : INCLUDE_PATH);
	    Dispose(incl_fname);
	    incl_fp = OpenFile(fnum, FALSE, TRUE);
	    if( incl_fp == NULL )
	      Error(2, 20, "cannot open include file %s",
		FATAL, PosOfFile(fnum), FullFileName(fnum));
	    while( (ch = getc(incl_fp)) != EOF )
	      print(ch);
	    fclose(incl_fp);
	  }
	  else
	  { clear();
	    print(*p);
	    p++;
	  }
	}
	else
	{ clear();
	  print(*(p-1));
	}
	break;


      default:
      
	Error(2, 22, "unreadable character (octal %o)",INTERN,&file_pos,*(p-1));
	assert(FALSE, "LexScanVerbatim: bad chtbl[]");
	break;

  };
  print('\n');

  if( p - startline >= MAX_LINE )
  { col_num(file_pos) = 1;
    Error(2, 21, "line is too long (or final newline missing)",FATAL,&file_pos);
  }

  chpt = p;
  if( fp == NULL && res == nilobj )
    res = MakeWord(WORD, STR_EMPTY, &file_pos);

  debug2(DLA, D, "LexScanVerbatim returning %s at %s",
    EchoObject(res), EchoFilePos(&file_pos));
  return res;
} /* end LexScanVerbatim */
