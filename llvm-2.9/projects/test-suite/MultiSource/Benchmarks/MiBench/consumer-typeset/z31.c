/*@z31.c:Memory Allocator:DebugMemory()@**************************************/
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
/*  FILE:         z31.c                                                      */
/*  MODULE:       Memory Allocator                                           */
/*  EXTERNS:      DebugMemory(), zz_free[], MemInit(), GetMemory()           */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"

#define	MEM_CHUNK	1020		/* how many ALIGNs to get from sys   */


#if DEBUG_ON
static	int	no_of_calls	= 0;	/* number of calls to calloc()       */
static  int     recs_created    = 0;	/* number of records created         */
static  int     bytes_created   = 0;	/* number of bytes in created recs   */
	int	zz_newcount	= 0;	/* number of calls to New()          */
	int	zz_disposecount	= 0;	/* number of calls to Dispose()      */
	int	zz_listcount	= 0;	/* number of elements in zz_free[]   */

static	int	usage_nums[MEM_USAGE_MAX] = { 0 };
static	int	usage_bytes[MEM_USAGE_MAX] = { 0 };
static	int	max_usage_bytes[MEM_USAGE_MAX] = { 0 };
static	int	curr_total_bytes, max_total_bytes = 0;
static	char	*usage_strings[] = {
			"lout binary",
			"object memory chunks",
			"font tables",
			"lex buffers",
			"file tables",
			"cross reference tables",
			"plain text output grids",
			"database check tables",
			"hyphenation pattern tables",
			"character mappings",
			"colour tables",
			"language tables",
};

/*****************************************************************************/
/*                                                                           */
/*  DebugRegisterUsage(typ, delta_num, delta_size)                           */
/*                                                                           */
/*  Register a change in the number of things of type typ that               */
/*  have been allocated memory, and the change in the number of bytes.       */
/*                                                                           */
/*****************************************************************************/

void DebugRegisterUsage(int typ, int delta_num, int delta_size)
{ int i;
  assert(0 <= typ && typ < MEM_USAGE_MAX, "DebugRegisterUsage!");
  usage_nums[typ] += delta_num;
  usage_bytes[typ] += delta_size;
  curr_total_bytes += delta_size;
  if( curr_total_bytes > max_total_bytes )
  { max_total_bytes = curr_total_bytes;
    for( i = 0; i < MEM_USAGE_MAX;  i++ )
      max_usage_bytes[i] = usage_bytes[i];
  }
} /* end DebugRegisterUsage */


/*****************************************************************************/
/*                                                                           */
/*  DebugMemory()                                                            */
/*                                                                           */
/*  Print memory usage.                                                      */
/*                                                                           */
/*****************************************************************************/

void DebugMemory(void)
{ int i, j;  OBJECT p;  int recs_free, bytes_free;

  recs_free = bytes_free = 0;
  for( i = 0;  i < MAX_OBJECT_REC;  i++ )
  { if( zz_free[i] != nilobj )
    { j = 0;
      for( p = zz_free[i];  p != nilobj;  p = pred(p, CHILD) )  j++;
      debug3(DMA, DD, "zz_free[%2d]: %5d (%d bytes)", i, j,
	i * j * sizeof(ALIGN));
      recs_free += j;
      bytes_free += i* j * sizeof(ALIGN);
    }
  }

  debug4(DMA, D, "%-35s %8s %8s %8s",
    "Summary of malloc() memory usage", "Quantity", "Bytes", "At max.");

  for( i = 1;  i < MEM_USAGE_MAX;  i++ )
  {
    debug4(DMA, D, "%-35s %8d %8d %8d", usage_strings[i], usage_nums[i],
      usage_bytes[i], max_usage_bytes[i]);
  }
  debug4(DMA, D, "%-35s %8s %8s %8s", "", "", "--------", "--------");
  debug4(DMA, D, "%-35s %8s %8d %8d", "","",curr_total_bytes,max_total_bytes);


  /***
  debug3(DMA, D, "%-12s %8s %8s", "", "records", "bytes");
  debug4(DMA, D, "%-12s %8s %8d (%d calls)", "calloc", "-",
    no_of_calls * MEM_CHUNK * sizeof(ALIGN), no_of_calls);
  debug3(DMA, D, "%-12s %8d %8d", "created", recs_created, bytes_created);
  debug3(DMA, D, "%-12s %8d %8d", "free (count)",    recs_free,    bytes_free);
  debug3(DMA, D, "%-12s %8d %8s", "free (var)",      zz_listcount, "-");
  debug3(DMA, D, "%-12s %8d %8s", "new-dispose",
    zz_newcount - zz_disposecount, "-");
  debug3(DMA, D, "%-12s %8d %8s", "created-free",
    recs_created - recs_free, "-");
  debug2(DMA, D, "newcount %d, disposecount %d", zz_newcount, zz_disposecount);
  *** */

  debug0(DMA, D, "");

} /* end DebugMemory */
#endif


/*@::zz_free[], zz_lengths[], MemInit()@**************************************/
/*                                                                           */
/*  OBJECT         zz_free[], zz_hold, zz_tmp, zz_res                        */
/*  int            zz_size                                                   */
/*  unsigned char  zz_lengths[]                                              */
/*                                                                           */
/*  zz_free[i]:    free records of size i*sizeof(ALIGN).                     */
/*  zz_lengths[i]: the number of ALIGNs in a record of type i.               */
/*  These variables are used only within the New() and Dispose() macros,     */
/*  and the list handling macros.                                            */
/*                                                                           */
/*****************************************************************************/

OBJECT		zz_free[MAX_OBJECT_REC], zz_hold, zz_tmp, zz_res;
int		zz_size;
unsigned char	zz_lengths[DISPOSED];		/* DISPOSED is 1 + max type */
OBJECT 		xx_link, xx_tmp, xx_res, xx_hold;


/*****************************************************************************/
/*                                                                           */
/*  MemInit()                                                                */
/*                                                                           */
/*  Initialise memory allocator.                                             */
/*                                                                           */
/*****************************************************************************/

void MemInit(void)
{
  zz_lengths[ WORD         ] = 0;
  zz_lengths[ QWORD        ] = 0;
  zz_lengths[ LINK         ] = ceiling(sizeof(struct link_type), sizeof(ALIGN));

  /* object types, except closure NB have actual() field in token phase! */
  zz_lengths[ CLOSURE      ] =
  zz_lengths[ NULL_CLOS    ] =
  zz_lengths[ PAGE_LABEL   ] =
  zz_lengths[ UNDER_REC    ] =
  zz_lengths[ CROSS        ] =
  zz_lengths[ FORCE_CROSS  ] =
  zz_lengths[ SPLIT        ] =
  zz_lengths[ PAR          ] =
  zz_lengths[ ROW_THR      ] =
  zz_lengths[ COL_THR      ] =
  zz_lengths[ HSPANNER     ] =
  zz_lengths[ VSPANNER     ] =
  zz_lengths[ ACAT         ] =
  zz_lengths[ HCAT         ] =
  zz_lengths[ VCAT         ] =
  zz_lengths[ BEGIN_HEADER ] =
  zz_lengths[ END_HEADER   ] =
  zz_lengths[ SET_HEADER   ] =
  zz_lengths[ CLEAR_HEADER ] =
  zz_lengths[ ONE_COL      ] =
  zz_lengths[ ONE_ROW      ] =
  zz_lengths[ WIDE         ] =
  zz_lengths[ HIGH         ] =
  zz_lengths[ HSHIFT       ] =
  zz_lengths[ VSHIFT       ] =
  zz_lengths[ HSCALE       ] =
  zz_lengths[ VSCALE       ] =
  zz_lengths[ HCOVER       ] =
  zz_lengths[ VCOVER       ] =
  zz_lengths[ SCALE        ] =
  zz_lengths[ KERN_SHRINK  ] =
  zz_lengths[ HCONTRACT    ] =
  zz_lengths[ VCONTRACT    ] =
  zz_lengths[ HLIMITED     ] =
  zz_lengths[ VLIMITED     ] =
  zz_lengths[ HEXPAND      ] =
  zz_lengths[ VEXPAND      ] =
  zz_lengths[ START_HVSPAN ] =
  zz_lengths[ START_HSPAN  ] =
  zz_lengths[ START_VSPAN  ] =
  zz_lengths[ HSPAN        ] =
  zz_lengths[ VSPAN        ] =
  zz_lengths[ PADJUST      ] =
  zz_lengths[ HADJUST      ] =
  zz_lengths[ VADJUST      ] =
  zz_lengths[ ROTATE       ] =
  zz_lengths[ BACKGROUND   ] =
  zz_lengths[ VERBATIM     ] =
  zz_lengths[ RAW_VERBATIM ] =
  zz_lengths[ CASE         ] =
  zz_lengths[ YIELD        ] =
  zz_lengths[ BACKEND      ] =
  zz_lengths[ FILTERED     ] =
  zz_lengths[ XCHAR        ] =
  zz_lengths[ FONT         ] =
  zz_lengths[ SPACE        ] =
  zz_lengths[ YUNIT        ] =
  zz_lengths[ ZUNIT        ] =
  zz_lengths[ BREAK        ] =
  zz_lengths[ UNDERLINE    ] =
  zz_lengths[ COLOUR       ] =
  zz_lengths[ OUTLINE      ] =
  zz_lengths[ LANGUAGE     ] =
  zz_lengths[ CURR_LANG    ] =
  zz_lengths[ CURR_FAMILY  ] =
  zz_lengths[ CURR_FACE    ] =
  zz_lengths[ CURR_YUNIT   ] =
  zz_lengths[ CURR_ZUNIT   ] =
  zz_lengths[ COMMON       ] =
  zz_lengths[ RUMP         ] =
  zz_lengths[ MELD         ] =
  zz_lengths[ INSERT       ] =
  zz_lengths[ ONE_OF       ] =
  zz_lengths[ NEXT         ] =
  zz_lengths[ PLUS         ] =
  zz_lengths[ MINUS        ] =
  zz_lengths[ ENV_OBJ      ] =
  zz_lengths[ ENV          ] =
  zz_lengths[ ENVA         ] =
  zz_lengths[ ENVB         ] =
  zz_lengths[ ENVC         ] =
  zz_lengths[ ENVD         ] =
  zz_lengths[ CENV         ] =
  zz_lengths[ CLOS         ] =
  zz_lengths[ LVIS         ] =
  zz_lengths[ LUSE         ] =
  zz_lengths[ LEO          ] =
  zz_lengths[ OPEN         ] =
  zz_lengths[ TAGGED       ] =
  zz_lengths[ INCGRAPHIC   ] =
  zz_lengths[ SINCGRAPHIC  ] =
  zz_lengths[ PLAIN_GRAPHIC] =
  zz_lengths[ GRAPHIC      ] =
  zz_lengths[ LINK_SOURCE  ] =
  zz_lengths[ LINK_DEST    ] =
	ceiling(sizeof(struct closure_type), sizeof(ALIGN));

  zz_lengths[ HEAD         ] =
	ceiling(sizeof(struct head_type), sizeof(ALIGN));

  zz_lengths[ LBR          ] =
  zz_lengths[ RBR          ] =
  zz_lengths[ BEGIN        ] =
  zz_lengths[ END          ] =
  zz_lengths[ USE          ] =
  zz_lengths[ NOT_REVEALED ] =
  zz_lengths[ GSTUB_NONE   ] =
  zz_lengths[ GSTUB_INT    ] =
  zz_lengths[ GSTUB_EXT    ] =
  zz_lengths[ UNEXPECTED_EOF] =
  zz_lengths[ PREPEND      ] =
  zz_lengths[ SYS_PREPEND  ] =
  zz_lengths[ DATABASE     ] =
  zz_lengths[ SYS_DATABASE ] =
  zz_lengths[ DEAD         ] =
  zz_lengths[ UNATTACHED   ] =
  zz_lengths[ RECEPTIVE    ] =
  zz_lengths[ RECEIVING    ] =
  zz_lengths[ RECURSIVE    ] =
  zz_lengths[ PRECEDES     ] =
  zz_lengths[ FOLLOWS      ] =
  zz_lengths[ CROSS_FOLL   ] =
  zz_lengths[ CROSS_FOLL_OR_PREC] =
  zz_lengths[ GALL_FOLL    ] =
  zz_lengths[ GALL_FOLL_OR_PREC ] =
  zz_lengths[ CROSS_TARG   ] =
  zz_lengths[ GALL_TARG    ] =
  zz_lengths[ GALL_PREC    ] =
  zz_lengths[ CROSS_PREC   ] =
  zz_lengths[ PAGE_LABEL_IND] =
  zz_lengths[ SCALE_IND    ] =
  zz_lengths[ COVER_IND    ] =
  zz_lengths[ EXPAND_IND   ] =
  zz_lengths[ THREAD       ] =
  zz_lengths[ CR_LIST      ] =
  zz_lengths[ SCOPE_SNAPSHOT] =
	ceiling(sizeof(struct closure_type), sizeof(ALIGN));

  /* symbol types */
  zz_lengths[ MACRO        ] =
  zz_lengths[ LOCAL        ] =
  zz_lengths[ LPAR         ] =
  zz_lengths[ RPAR         ] =
  zz_lengths[ NPAR         ] =
	ceiling(sizeof(struct symbol_type), sizeof(ALIGN));

  /* gap objects */
  zz_lengths[ TSPACE       ] =
  zz_lengths[ TJUXTA       ] =
  zz_lengths[ GAP_OBJ      ] =
	ceiling(sizeof(struct gapobj_type), sizeof(ALIGN));

  /* cross-reference and data base types */
  zz_lengths[ CROSS_SYM    ] =
  zz_lengths[ CR_ROOT      ] = ceiling(sizeof(struct cr_type) , sizeof(ALIGN));

  /* external galley record */
  zz_lengths[ EXT_GALL  ] = ceiling(sizeof(struct ext_gall_type),sizeof(ALIGN));

} /* end MemInit() */


/*@::GetMemory()@*************************************************************/
/*                                                                           */
/*  OBJECT GetMemory(siz, pos)                                               */
/*                                                                           */
/*  Return a pointer to siz ALIGNs of memory (0 < siz < MAX_OBJECT_REC).     */
/*                                                                           */
/*****************************************************************************/

OBJECT GetMemory(int siz, FILE_POS *pos)
{ static ALIGN *next_free = (ALIGN *) nilobj;
  static ALIGN *top_free  = (ALIGN *) nilobj;
  OBJECT res;

  debug1(DMA, DDD, "GetMemory( %d )", siz);

  /* get memory from operating system, if not enough left here */
  if( &next_free[siz] > top_free )
  {
#if DEBUG_ON
    DebugRegisterUsage(MEM_OBJECTS, 1, MEM_CHUNK * sizeof(ALIGN));
#endif
    next_free = (ALIGN *) calloc(MEM_CHUNK, sizeof(ALIGN));
    ifdebug(DMA, D, no_of_calls++; )
    if( next_free == NULL )
      Error(31, 1, "exiting now (run out of memory)", FATAL, pos);
    top_free = &next_free[MEM_CHUNK];
    debug2(DMA, DD, "GetMemory: calloc returned %ld - %ld",
      (long) next_free, (long) top_free);
  }

  res = (OBJECT) next_free;
  next_free = &next_free[siz];
#if DEBUG_ON
  recs_created++; bytes_created += siz * sizeof(ALIGN);
#endif
  debug3(DMA, DDD, "GetMemory returning @%ld (next_free = @%ld, top_free = @%ld",
    (long) res, (long) next_free, (long) top_free);
  return res;
} /* end GetMemory */
