/*@z47.c:Environment Table:EnvReadRetrieve()@*********************************/
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
/*  FILE:         z47.c                                                      */
/*  MODULE:       Environment Table                                          */
/*  EXTERNS:      EnvInit(), EnvWriteRetrieve(), EnvWriteInsert(),           */
/*                EnvReadRetrieve(), EnvReadInsert()                         */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define	TAB_SIZE	211
#define	MAX_CACHE	180

#define	env_offset(x)	back(x, ROWM)
#define	env_lnum(x)	line_num(fpos(x))
#define	env_fnum(x)	file_num(fpos(x))
#define	env_read(x)	sized(x)

static	OBJECT	env_cache;			/* cache of envts in use     */
static	int	cache_count;			/* current size of cache     */
static	int	stat_writes;			/* calls to WriteRetrieve    */
static	int	stat_write_hits;		/* hits in WriteRetrieve     */
static	int	stat_reads;			/* calls to ReadRetrieve     */
static	int	stat_read_hits;			/* hits in ReadRetrieve      */

static OBJECT tab[TAB_SIZE];

#define hash1(pos, env, fnum)						\
{									\
  pos = ( (unsigned int) env + fnum ) % TAB_SIZE;			\
}

#define hash2(pos, fnum, offset)					\
{									\
  pos = ( offset + fnum ) % TAB_SIZE;					\
}


/*****************************************************************************/
/*                                                                           */
/*  void EnvInit(void)                                                       */
/*                                                                           */
/*  Initialize this module.                                                  */
/*                                                                           */
/*****************************************************************************/

void EnvInit(void)
{ int i;
  debug0(DET, DD, "EnvInit()");
  stat_reads = 0;
  stat_read_hits = 0;
  stat_writes = 0;
  stat_write_hits = 0;
  New(env_cache, ACAT);
  cache_count = 0;
  for( i = 0;  i < TAB_SIZE;  i++ )
  { tab[i] = nilobj;
  }
  debug0(DET, DD, "EnvInit returning");
} /* end EnvInit */


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN EnvWriteRetrieve(OBJECT env, FILE_NUM fnum, int *offset, *lnum)  */
/*                                                                           */
/*  Return the offset in file fnum where environment env has been written,   */
/*  or FALSE if env has not been written to this file.                       */
/*                                                                           */
/*****************************************************************************/

BOOLEAN EnvWriteRetrieve(OBJECT env, FILE_NUM fnum, int *offset, int *lnum)
{ unsigned int pos;  OBJECT link, y, z;
  debug2(DET, DD, "EnvWriteRetrieve(env %d, %s)", (int) env, FileName(fnum));
  debug1(DET, DDD, "  %s", EchoObject(env));
  stat_writes++;
  hash1(pos, env, fnum);
  if( tab[pos] != nilobj )
  {
    for( link = Down(tab[pos]);  link != tab[pos];  link = NextDown(link) )
    { Child(y, link);
      Child(z, Down(y));
      if( env_fnum(y) == fnum && z == env && !env_read(y) )
      { MoveLink(LastUp(y), env_cache, PARENT);
	*offset = env_offset(y);
	*lnum   = env_lnum(y);
	stat_write_hits++;
	debug2(DET, DD, "EnvWriteRetrieve returning TRUE (offset %d, lnum %d)",
	  *offset, *lnum);
	return TRUE;
      }
    }
  }
  debug0(DET, DD, "EnvWriteRetrieve returning FALSE");
  return FALSE;
} /* end EnvWriteRetrieve */


/*****************************************************************************/
/*                                                                           */
/*  void EnvWriteInsert(OBJECT env, FILE_NUM fnum, int offset, int lnum)     */
/*                                                                           */
/*  Record the fact that environment env has been written to file fnum       */
/*  at the given offset.                                                     */
/*                                                                           */
/*****************************************************************************/

void EnvWriteInsert(OBJECT env, FILE_NUM fnum, int offset, int lnum)
{ unsigned int pos;  OBJECT loser, x;
  debug3(DET, DD, "EnvWriteInsert(env %d, %s, %d)", (int) env,
    FileName(fnum), offset);

  /* to limit the cache size, remove least recently used entry if full */
  if( cache_count >= MAX_CACHE )
  {
    Child(loser, Down(env_cache));
    DeleteLink(Up(loser));
    DisposeChild(Up(loser));
    cache_count--;
  }

  /* insert the new entry */
  hash1(pos, env, fnum);
  if( tab[pos] == nilobj )  New(tab[pos], ACAT);
  New(x, ACAT);
  env_fnum(x) = fnum;
  env_offset(x) = offset;
  env_lnum(x) = lnum;
  env_read(x) = FALSE;
  Link(tab[pos], x);
  Link(env_cache, x);
  Link(x, env);
  cache_count++;

  debug1(DET, DD, "EnvWriteInsert returning (cache_count = %d)", cache_count);
} /* end EnvWriteInsert */


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN EnvReadRetrieve(FILE_NUM fnum, int offset, OBJECT *env)          */
/*                                                                           */
/*  Return the environment that appears in file fnum at the given offset,    */
/*  or FALSE if this is not currently known.                                 */
/*                                                                           */
/*****************************************************************************/

BOOLEAN EnvReadRetrieve(FILE_NUM fnum, int offset, OBJECT *env)
{ int pos;  OBJECT link, y, z;
  debug2(DET, DD, "EnvReadRetrieve(%s, %d)", FileName(fnum), offset);
  stat_reads++;

  hash2(pos, fnum, offset);
  if( tab[pos] != nilobj )
  {
    for( link = Down(tab[pos]);  link != tab[pos];  link = NextDown(link) )
    { Child(y, link);
      Child(z, Down(y));
      if( env_fnum(y) == fnum && env_offset(y) == offset && env_read(y) )
      { MoveLink(LastUp(y), env_cache, PARENT);
	Child(*env, Down(y));
	stat_read_hits++;
        debug1(DET, DD, "EnvReadRetrieve returning env %d", (int) *env);
	  return TRUE;
      }
    }
  }
  debug0(DET, DD, "EnvReadRetrieve returning FALSE");
  return FALSE;
} /* end EnvReadRetrieve */


/*****************************************************************************/
/*                                                                           */
/*  void EnvReadInsert(FILE_NUM fnum, int offset, OBJECT env)               */
/*                                                                           */
/*  Record the fact that environment env has just been read from file fnum   */
/*  at position offset.                                                      */
/*                                                                           */
/*****************************************************************************/

void EnvReadInsert(FILE_NUM fnum, int offset, OBJECT env)
{ int pos; OBJECT x, loser;
  debug3(DET, DD, "EnvReadInsert(%s, %d, env %d)",
    FileName(fnum), offset, (int) env);

  /* to limit the cache size, remove least recently used entry if full */
  if( cache_count >= MAX_CACHE )
  {
    Child(loser, Down(env_cache));
    DeleteLink(Up(loser));
    DisposeChild(Up(loser));
    cache_count--;
  }

  /* insert the new entry */
  hash2(pos, fnum, offset);
  if( tab[pos] == nilobj )  New(tab[pos], ACAT);
  New(x, ACAT);
  env_fnum(x) = fnum;
  env_offset(x) = offset;
  env_read(x) = TRUE;
  Link(tab[pos], x);
  Link(env_cache, x);
  Link(x, env);
  cache_count++;

  debug1(DET, DD, "EnvReadInsert returning (cache_count = %d)", cache_count);
} /* end EnvReadInsert */


#if DEBUG_ON
/*****************************************************************************/
/*                                                                           */
/*  void EnvDebug()                                                          */
/*                                                                           */
/*  Debug statistics of this module's performance.                           */
/*                                                                           */
/*****************************************************************************/

void EnvDebug(void)
{
  debug3(DET, D, "Env Table  %6s %6s %6s", "calls", "hits", "% hits");
  debug3(DET, D, "reading    %6d %6d %6.1f", stat_reads, stat_read_hits,
  stat_reads == 0 ? (float) 0 : (float) 100 * stat_read_hits / stat_reads);
  debug3(DET, D, "writing    %6d %6d %6.1f", stat_writes, stat_write_hits,
  stat_writes == 0 ? (float) 0 : (float) 100 * stat_write_hits / stat_writes);
} /* end EnvDebug */
#endif
