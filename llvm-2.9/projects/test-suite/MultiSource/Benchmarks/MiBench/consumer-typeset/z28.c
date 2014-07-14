/*@z28.c:Error Service:ErrorInit(), ErrorSeen()@******************************/
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
/*  FILE:         z28.c                                                      */
/*  MODULE:       Error Service                                              */
/*  EXTERNS:      ErrorInit(), Error(), ErrorSeen()                          */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

#define	MAX_BLOCKS	 20		/* max number of error blocks        */
#define	MAX_ERRORS	 20		/* max number of held error messages */

static BOOLEAN	print_block[MAX_BLOCKS];	/* TRUE if print this block  */
static int	start_block[MAX_BLOCKS];	/* first message of block    */
static char	message[MAX_ERRORS][MAX_BUFF];	/* the error messages        */
static int	message_fnum[MAX_ERRORS];	/* file number of error mess */
static FILE	*fp = NULL;			/* file pointer of log file  */
static BOOLEAN	error_seen = FALSE;		/* TRUE after first error    */
static int	block_top = 0;			/* first free error block    */
static int	mess_top = 0;			/* first free error message  */


/*****************************************************************************/
/*                                                                           */
/*  ErrorInit(str)                                                           */
/*                                                                           */
/*  Open log file str and initialise this module.                            */
/*                                                                           */
/*****************************************************************************/

void ErrorInit(FULL_CHAR *str)
{ if( fp != NULL )
    Error(28, 1, "-e argument appears twice in command line", FATAL, no_fpos);
  fp = StringFOpen(str, WRITE_TEXT);
  if( fp == NULL )
    Error(28, 2, "cannot open error file %s", FATAL, no_fpos, str);
} /* end ErrorInit */


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN ErrorSeen()                                                      */
/*                                                                           */
/*  TRUE once an error has been found.                                       */
/*                                                                           */
/*****************************************************************************/

BOOLEAN ErrorSeen(void)
{ return error_seen;
} /* end ErrorSeen */


/*****************************************************************************/
/*                                                                           */
/*  PrintFileBanner(fnum)                                                    */
/*                                                                           */
/*  If fnum was not the subject of the previous call to PrintFileBanner,     */
/*  print a file banner for fnum.                                            */
/*                                                                           */
/*****************************************************************************/

static void PrintFileBanner(int fnum)
{ static int CurrentFileNum = -1;
  if( fnum != CurrentFileNum )
  { fprintf(fp, "lout%s:\n", EchoFileSource(fnum));
    CurrentFileNum = fnum;
  }
} /* end PrintFileBanner */


/*@::EnterErrorBlock(), LeaveErrorBlock()@************************************/
/*                                                                           */
/*  EnterErrorBlock(ok_to_print)                                             */
/*                                                                           */
/*  Start off a new block of error messages.  If ok_to_print, they do not    */
/*  need to be held for a later commit.                                      */
/*                                                                           */
/*****************************************************************************/

void EnterErrorBlock(BOOLEAN ok_to_print)
{ if( block_top < MAX_BLOCKS )
  { print_block[block_top] = ok_to_print;
    start_block[block_top] = mess_top;
    block_top++;
  }
  else Error(28, 3, "too many levels of error messages", FATAL, no_fpos);
} /* end EnterErrorBlock */


/*****************************************************************************/
/*                                                                           */
/*  LeaveErrorBlock(commit)                                                  */
/*                                                                           */
/*  Finish off a block of error messages.  If commit is true, print them,    */
/*  otherwise discard them.                                                  */
/*                                                                           */
/*****************************************************************************/

void LeaveErrorBlock(BOOLEAN commit)
{ int i;
  debug0(DYY, D, "  leaving error block");
  assert( block_top > 0, "LeaveErrorBlock: no matching EnterErrorBlock!" );
  assert( commit || !print_block[block_top - 1], "LeaveErrorBlock: commit!" );
  if( fp == NULL )  fp = stderr;
  if( commit )
  { for( i = start_block[block_top - 1];  i < mess_top;  i++ )
    {
      if( AltErrorFormat )
      { fputs(message[i], fp);
      }
      else
      { PrintFileBanner(message_fnum[i]);
        fputs(message[i], fp);
      }
    }
  }
  block_top--;
  mess_top = start_block[block_top];
} /* end LeaveErrorBlock */


/*****************************************************************************/
/*                                                                           */
/*  CheckErrorBlocks()                                                       */
/*                                                                           */
/*  Check (at end of run) that all error blocks have been unstacked.         */
/*                                                                           */
/*****************************************************************************/

void CheckErrorBlocks(void)
{ assert( block_top == 0, "CheckErrorBlocks: block_top != 0!" );
} /* end CheckErrorBlocks */


/*@::Error()@*****************************************************************/
/*                                                                           */
/*  Error(etype, pos, str, p1, p2, p3, p4, p5, p6)                           */
/*                                                                           */
/*  Report error of type etype at position *pos in input.                    */
/*  The error message is str with parameters p1 - p6.                        */
/*                                                                           */
/*****************************************************************************/

POINTER Error(int set_num, int msg_num, char *str, int etype, FILE_POS *pos, ...)
{
  va_list ap;
  char val[MAX_BUFF];
  va_start(ap, pos);
  vsprintf(val, condcatgets(MsgCat, set_num, msg_num, str), ap);
  if( fp == NULL )  fp = stderr;
  switch( etype )
  {

    case INTERN:
    
      while( block_top > 0 )  LeaveErrorBlock(TRUE);
      if( AltErrorFormat )
      {
        fprintf(fp, condcatgets(MsgCat, 28, 7, "%s internal error: %s\n"),
	  EchoAltFilePos(pos), val);
        /* for estrip's benefit: Error(28, 7, "%s internal error: %s\n") */
      }
      else
      {
        PrintFileBanner(file_num(*pos));
        fprintf(fp, condcatgets(MsgCat, 28, 4, "  %6s internal error: %s\n"),
	  EchoFileLine(pos), val);
        /* for estrip's benefit: Error(28, 4, "  %6s internal error: %s\n") */
      }
#if DEBUG_ON
      abort();
#else
      exit(1);
#endif
      break;


    case FATAL:
    
      while( block_top > 0 )  LeaveErrorBlock(TRUE);
      if( AltErrorFormat )
      {
        fprintf(fp, condcatgets(MsgCat, 28, 8, "%s: fatal error: %s\n"),
	  EchoAltFilePos(pos), val);
      }
      else
      {
        PrintFileBanner(file_num(*pos));
        fprintf(fp, condcatgets(MsgCat, 28, 5, "  %6s: fatal error: %s\n"),
	  EchoFileLine(pos), val);
      }
      /* for estrip's benefit: Error(28, 5, "  %6s: fatal error: %s\n") */
      /* for estrip's benefit: Error(28, 8, "%s: fatal error: %s\n") */
      exit(1);
      break;


    case WARN:
    
      if( block_top == 0 || print_block[block_top - 1] )
      {
	if( AltErrorFormat )
	{
	  fprintf(fp, "%s: %s\n", EchoAltFilePos(pos), val);
	}
	else
	{
	  PrintFileBanner(file_num(*pos));
	  fprintf(fp, "  %6s: %s\n", EchoFileLine(pos), val);
	}
      }
      else if( mess_top < MAX_ERRORS )
      {
	if( AltErrorFormat )
	{
	  sprintf(message[mess_top++], "%s: %s\n", EchoAltFilePos(pos), val);
	}
	else
	{ message_fnum[mess_top] = file_num(*pos);
	  sprintf(message[mess_top++], "  %6s: %s\n",
	    EchoFileLine(pos), val);
	}
      }
      else Error(28, 6, "too many error messages", FATAL, pos);
      error_seen = TRUE;
      break;


    default:
    
      assert(FALSE, "Error: invalid error type");
      break;

  }
  va_end(ap);
  return 0;
} /* end Error */
