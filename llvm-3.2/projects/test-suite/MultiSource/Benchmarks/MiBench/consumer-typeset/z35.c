/*@z35.c:Time Keeper: MomentSym(), TimeString()@******************************/
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
/*  FILE:         z35.c                                                      */
/*  MODULE:       Time Keeper                                                */
/*  EXTERNS:      MomentSym, InitTime(), StartMoment(), TimeString()         */
/*                                                                           */
/*****************************************************************************/
#include <time.h>
#include "externs.h"

#define load(str, typ, encl)						\
  sym = InsertSym(str, typ, no_fpos, DEFAULT_PREC,  			\
  FALSE, FALSE, 0, encl, MakeWord(WORD, STR_EMPTY, no_fpos));		\
  if( typ == NPAR )  visible(sym) = TRUE

#define add_par(format, val, sym)					\
  sprintf( (char *) buff, format, val);					\
  New(par, PAR);  actual(par) = sym;					\
  Link(current_moment, par);						\
  tmp = MakeWord(WORD, buff, no_fpos);					\
  Link(par, tmp);

static OBJECT current_moment = nilobj;
static FULL_CHAR time_string[30] = { '\0' };


/*****************************************************************************/
/*                                                                           */
/*  OBJECT MomentSym;                                                        */
/*                                                                           */
/*  The symbol table entry for the @Moment symbol.                           */
/*                                                                           */
/*****************************************************************************/

OBJECT MomentSym = nilobj;


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *TimeString()                                                  */
/*                                                                           */
/*  Returns a pointer to a string containing the current time.               */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *TimeString(void)
{ return time_string;
} /* end TimeString */


/*@::InitTime(), StartMoment()@***********************************************/
/*                                                                           */
/*  InitTime()                                                               */
/*                                                                           */
/*  Place a declaration of the @Moment symbol into the symbol table, and     */
/*  initialize the value of the object StartMoment.                          */
/*                                                                           */
/*****************************************************************************/

void InitTime(void)
{ time_t raw_time; struct tm *now;
  FULL_CHAR buff[20]; OBJECT par, tmp, sym, env;
  OBJECT tag, second, minute, hour, weekday,
	monthday, yearday, month, year, century, dst;
  debug0(DTK, D, "InitTime()");

  /* define @Moment symbol with its host of named parameters */
  MomentSym = load(KW_MOMENT,         LOCAL, StartSym);
  tag       = load(KW_TAG,            NPAR,  MomentSym);
  second    = load(KW_SECOND,         NPAR,  MomentSym);
  minute    = load(KW_MINUTE,         NPAR,  MomentSym);
  hour      = load(KW_HOUR,           NPAR,  MomentSym);
  monthday  = load(KW_DAY,            NPAR,  MomentSym);
  month     = load(KW_MONTH,          NPAR,  MomentSym);
  year      = load(KW_YEAR,           NPAR,  MomentSym);
  century   = load(KW_CENTURY,        NPAR,  MomentSym);
  weekday   = load(KW_WEEKDAY,        NPAR,  MomentSym);
  yearday   = load(KW_YEARDAY,        NPAR,  MomentSym);
  dst       = load(KW_DAYLIGHTSAVING, NPAR,  MomentSym);

  /* get current time and convert to ASCII */
  if( time(&raw_time) == -1 )
    Error(35, 1, "unable to obtain the current time", WARN, no_fpos);
  now = localtime(&raw_time);
  StringCopy(time_string, AsciiToFull(asctime(now)));

  /* start of current_moment */
  New(current_moment, CLOSURE);
  actual(current_moment) = MomentSym;

  /* attach its many parameters */
  add_par("%s",   KW_NOW,                      tag);
  add_par("%.2d", now->tm_sec,                 second);
  add_par("%.2d", now->tm_min,                 minute);
  add_par("%.2d", now->tm_hour,                hour);
  add_par("%d",   now->tm_mday,                monthday);
  add_par("%d",   now->tm_mon + 1,             month);
  add_par("%.2d", now->tm_year % 100,          year);
  add_par("%d",   (now->tm_year+1900) / 100,   century);
  add_par("%d",   now->tm_wday + 1,            weekday);
  add_par("%d",   now->tm_yday,                yearday);
  add_par("%d",   now->tm_isdst,               dst);

  /* add a null environment */
  New(env, ENV);
  AttachEnv(env, current_moment);
  debug0(DTK, D, "InitTime() returning.");
  debug0(DTK, DD, "current_moment =");
  ifdebug(DTK, DD, DebugObject(current_moment));
} /* end InitTime */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT StartMoment()                                                     */
/*                                                                           */
/*  Returns a copy of the initial time.                                      */
/*                                                                           */
/*****************************************************************************/

OBJECT StartMoment(void)
{ OBJECT res;
  debug0(DTK, D, "StartMoment()");
  assert(current_moment != nilobj, "StartMoment: current_moment == nilobj!");
  res = CopyObject(current_moment, no_fpos);
  debug0(DTK, D, "StartMoment returning");
  ifdebug(DTK, D, DebugObject(res));
  return res;
}
