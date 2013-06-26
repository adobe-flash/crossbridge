/*@z27.c:Debug Service:Debug flags@*******************************************/
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
/*  FILE:         z27.c                                                      */
/*  MODULE:       Debug Service                                              */
/*  EXTERNS:      dbg[], DebugInit(), Debug()                                */
/*                ProfileOn(), ProfileOff(), ProfilePrint()                  */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

#if DEBUG_ON
struct dbs  dbg[] = {
    {"zz",    {0, 0, 0}},		/* - unused -                */
    {"sp",    {0, 0, 0}},		/* Supervise                 */
    {"la",    {0, 0, 0}},		/* Lexical Analyser          */
    {"fs",    {0, 0, 0}},		/* File Service              */
    {"ts",    {0, 0, 0}},		/* Token Service             */
    {"rd",    {0, 0, 0}},		/* Read Definitions          */
    {"op",    {0, 0, 0}},		/* Object Parser             */
    {"os",    {0, 0, 0}},		/* Object Service            */
    {"om",    {0, 0, 0}},		/* Object Manifest           */
    {"ce",    {0, 0, 0}},		/* Closure Expansion         */
    {"cr",    {0, 0, 0}},		/* Cross References          */
    {"ss",    {0, 0, 0}},		/* Style Service             */
    {"sf",    {0, 0, 0}},		/* Size Finder               */
    {"ob",    {0, 0, 0}},		/* Object Breaking           */
    {"of",    {0, 0, 0}},		/* Object Filling            */
    {"sc",    {0, 0, 0}},		/* Size Constraints          */
    {"sa",    {0, 0, 0}},		/* Size Adjustments          */
    {"gw",    {0, 0, 0}},		/* Gap Widths                */
    {"gt",    {0, 0, 0}},		/* Galley Transfer           */
    {"ga",    {0, 0, 0}},		/* Galley Attaching          */
    {"gf",    {0, 0, 0}},		/* Galley Flusher            */
    {"gm",    {0, 0, 0}},		/* Galley Maker              */
    {"gs",    {0, 0, 0}},		/* Galley Service            */
    {"gp",    {0, 0, 0}},		/* Galley Printer            */
    {"ps",    {0, 0, 0}},		/* Print Service             */
    {"oe",    {0, 0, 0}},		/* Object Echo               */
    {"es",    {0, 0, 0}},		/* Echo Service              */
    {"zz",    {0, 0, 0}},		/* Debug Service (unused)    */
    {"yy",    {0, 0, 0}},		/* Error Service             */
    {"st",    {0, 0, 0}},		/* Symbol Table              */
    {"su",    {0, 0, 0}},		/* Symbol Uses               */
    {"ma",    {0, 0, 0}},		/* Memory Allocator          */
    {"cs",    {0, 0, 0}},		/* Counter Service           */
    {"bs",    {0, 0, 0}},		/* Database Service          */
    {"rs",    {0, 0, 0}},		/* Rotation Service          */
    {"tk",    {0, 0, 0}},		/* Time Keeper               */
    {"hy",    {0, 0, 0}},		/* Hyphenation               */
    {"ft",    {0, 0, 0}},		/* Font Tables               */
    {"cm",    {0, 0, 0}},		/* Character Mappings        */
    {"sh",    {0, 0, 0}},		/* String Handler            */
    {"fh",    {0, 0, 0}},		/* Filter Handler            */
    {"io",    {0, 0, 0}},		/* Object Input-Output       */
    {"co",    {0, 0, 0}},		/* Colour Service            */
    {"ls",    {0, 0, 0}},		/* Language Service          */
    {"vh",    {0, 0, 0}},		/* Vertical Hyphenation      */
    {"ex",    {0, 0, 0}},		/* External Sort             */
    {"og",    {0, 0, 0}},		/* Optimal Galleys           */
    {"et",    {0, 0, 0}},		/* Environment Table         */
    {"pd",    {0, 0, 0}},		/* PDF Back End (old)        */
    {"po",    {0, 0, 0}},		/* PostScript Back End       */
    {"pf",    {0, 0, 0}},		/* PDF Back End              */
    {"pt",    {0, 0, 0}},		/* Plain Text Back End       */
    {"pp",    {0, 0, 0}},		/* Profiling                 */
    {"",      {0, 0, 0}},		/* any                       */
};

/*@::DebugInit(), Debug()@****************************************************/
/*                                                                           */
/*  DebugInit(str)                                                           */
/*                                                                           */
/*  Turn on the debug flag given by str.                                     */
/*                                                                           */
/*****************************************************************************/

void DebugInit(FULL_CHAR *str)
{ int j, urg;
  for( urg = 0;  urg < 2 && str[urg+2] == CH_FLAG_DEBUG;  urg++ );
  for( j = 1;  ;  j++ )
  { if( StringEqual(AsciiToFull(dbg[j].flag), &str[urg+2]) )  break;
    if( StringEqual(AsciiToFull(dbg[j].flag), STR_EMPTY) )
      Error(27, 1, "unknown debug flag %s", FATAL, no_fpos, str);
  }
  for( ;  urg >= 0;  urg-- )  dbg[j].on[urg] = dbg[ANY].on[urg] = TRUE;
} /* end DebugInit */


/*****************************************************************************/
/*                                                                           */
/*  Debug(category, urgency, str, ...)                                       */
/*                                                                           */
/*  Print str on debug output, if the flag corresponding to the given        */
/*  debug category and urgency is on.                                        */
/*                                                                           */
/*****************************************************************************/

void Debug(int category, int urgency, char *str, ...)
{ static BOOLEAN first_message = TRUE;
  va_list ap;
  if( first_message )
  { fprintf(stderr, "\nLout Debug Output:\n");
    first_message = FALSE;
  }
  fprintf(stderr, "%2s: ", dbg[category].flag);
  va_start(ap, str);
  vfprintf(stderr, str, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  fflush(stderr);
} /* end Debug */


/*@::ProfileOn(), ProfileOff(), ProfilePrint()@*******************************/
/*                                                                           */
/*  ProfileOn(str)                                                           */
/*                                                                           */
/*  Start profiling label str.                                               */
/*                                                                           */
/*****************************************************************************/
#define MAXPROF	20
#include <time.h>

struct profrec
{ char *label;			/* label of the profile              */
  int calls;			/* number of calls with this label   */
  long time;			/* total time of this label          */
};

static struct profrec profstack[MAXPROF];
static struct profrec profstore[MAXPROF];
static int proftop = 0, profsize = 0;

void ProfileOn(char *str)
{ int i;  time_t raw_time;
  for( i = 0;  i < proftop;  i++ )
  { if( strcmp(profstack[i].label, str) == 0 )
    { for( i = 0;  i < proftop;  i++ )
	fprintf(stderr, "profstack[%d] = %s\n", i, profstack[i].label);
      assert1(FALSE, "ProfileOn: restarted", str);
    }
  }
  assert(proftop < MAXPROF, "ProfileOn: overflow");
  time(&raw_time);  profstack[proftop].label = str;
  profstack[proftop++].time  = raw_time;
} /* end ProfileOn */


/*****************************************************************************/
/*                                                                           */
/*  ProfileOff(str)                                                          */
/*                                                                           */
/*  Stop profiling label str.                                                */
/*                                                                           */
/*****************************************************************************/

void ProfileOff(char *str)
{ int i;  time_t raw_time;
  assert1(proftop > 0 && strcmp(profstack[proftop-1].label, str) == 0,
    "ProfileOff: not current", str);
  for( i = 0;  i < profsize && strcmp(profstore[i].label, str) != 0; i++ );
  if( i >= profsize )
  { profsize++;
    assert(profsize < MAXPROF, "ProfileOff: overflow");
    profstore[i].label = str;
    profstore[i].calls = 0;
    profstore[i].time  = 0;
  }
  time(&raw_time);  profstore[i].calls += 1;
  profstore[i].time  += (raw_time - profstack[--proftop].time);
} /* end ProfileOff */


/*****************************************************************************/
/*                                                                           */
/*  ProfilePrint()                                                           */
/*                                                                           */
/*  Print results of profiling.                                              */
/*                                                                           */
/*****************************************************************************/

void ProfilePrint(void)
{ int i;
  for( i = 0;  i < profsize;  i++ )
  { fprintf(stderr, "Profile %-20s  %6ld secs, %3d calls, %6.2f secs/call\n",
	profstore[i].label, profstore[i].time, profstore[i].calls,
	(float) profstore[i].time / profstore[i].calls );
  }
} /* end ProfilePrint */
#endif
