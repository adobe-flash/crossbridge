/*@z48.c:PDF back end@********************************************************/
/*                                                                           */
/*  THE LOUT DOCUMENT FORMATTING SYSTEM (VERSION 3.24)                       */
/*  COPYRIGHT (C) 1991, 2000 Jeffrey H. Kingston                             */
/*                                                                           */
/*  Jeffrey H. Kingston (jeff@cs.usyd.edu.au)                                */
/*  Basser Department of Computer Science                                    */
/*  The University of Sydney 2006                                            */
/*  AUSTRALIA                                                                */
/*                                                                           */
/*  This PDF Back End module written by Vincent Tan, March 1998.             */
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
/*  FILE:         z48.c                                                      */
/*  MODULE:       PDF back end                                               */
/*  EXTERNS:      PDFFile_Init(), PDFFile_BeginFontEncoding(),               */
/*                PDFFile_EndFontEncoding(), PDFFile_Cleanup(),              */
/*                PDFPage_Init(), PDFPage_Cleanup(), PDFPage_Write(),        */
/*                PDFPage_Push(), PDFPage_Pop(), PDFPage_Scale(),            */
/*                PDFPage_Translate(), PDFPage_Rotate(), PDFPage_SetVars(),  */
/*                PDFPage_WriteGraphic(), PDFPage_PrintUnderline(),          */
/*                PDFFont_AddFont(), PDFFont_Set(), PDFText_OpenXY(),        */
/*                PDFText_OpenX(), PDFText_Open(), PDFText_Kern(),           */
/*                PDFText_Close(), PDFHasValidTextMatrix()                   */
/*                                                                           */
/*****************************************************************************/
#define  PI   3.1415926535897931160
#include "externs.h"


/* ANSI headers */
#include <ctype.h>
#include <math.h>
#include <time.h>

/* zlib headers: define PDF_COMPRESSION = 0 if you don't have zlib library */
#if PDF_COMPRESSION
#include "zlib.h"
#endif

static void Assert(BOOLEAN condition, FILE_POS *inFilePos)
{
  if (!condition)		/* allows me to set a breakpoint here */
  assert(condition, inFilePos);
}

/* #define's and typedefs */
#undef USE_MATRICES
#undef _CALC_LARGEST_PAGE_OBJECT_

enum {
  kBase14FontCount = 14,    /* there are 14 base PDF fonts */
  kBufferSize = 1024	    /* size of buffer for skipping non-marking commands */
};

#if PDF_COMPRESSION
enum {
  kRawOutputBufferSize = 4096,		/* arbitrary choice */
  kCompressedOutputBufferSize = 4096	/* arbitrary choice */
};
#endif

enum {
  kNumberOfObjectsPerBlock = 256,	/* arbitrary choice */
  kNumberOfPagesPerBlock = 64		/* arbitrary choice */
};

typedef enum {
  kFitNoChange = 0,	/* special default case */
  kFit,			/* [ /Fit ]: fit the page to the window */
  kFitH,		/* [ /FitH top ]: fit the width of the page to window; */
			/* top specifies y-coord of the top edge of the window */
  kFitV,		/* [ /FitV left ]: fit the height of the page to the   */
			/* window. left specifies x-coord of left edge of win. */
  kFitR,		/* [ /FitR left bottom right top ]: fit the rectangle  */
			/* specified by left bottom right top in the window.   */
			/* If the height (top - bottom) and width (right-left) */
			/* imply different zoom factors, the numerically       */
			/* smaller zoom factor is used, to ensure that the     */
			/* specified rectangle fits in the window              */
  kFitB,		/* [ /FitB ]: fit the page's bounding box to window    */
  kFitBH,		/* [ /FitBH top ]: fit the width of the page's bound.  */
			/* box to the window. top specifies the y-coordinate   */
			/* of the top edge of the window                       */
  kFitBV,		/* [ /FitBV left ]: fit the height of the page's       */
			/* bounding box to the window. left specifies the      */
			/* x-coordinate of the left edge of the window         */

  kNumberOfDestLinkOptions
} PDF_LINK_DEST_OPTION;

enum eUnitsKeywords {
  k_in = 0,
  k_cm,
  k_pt,
  k_em,
  k_loutf,
  k_loutv,
  k_louts,
  kNumberOfUnitKeywords
};

enum eGraphicsKeywords {
  k_xsize = 0,
  k_ysize,
  k_xmark,
  k_ymark,
  kNumberOfGraphicsKeywords
};

enum eArithmeticKeywords {
  k_add = 0,
  k_sub,
  k_mul,
  k_div,
  k_sin,
  k_cos,
  k_pick,
  kNumberOfArithmeticKeywords
};

typedef enum {
  k_link_source = 0,	/* source of a link to an internal document target */
  k_link_external,	/* source of a link to an external document target */
  k_link_URI,		/* source of a link to an (external) URI target    */
  k_link_target,	/* internal document target                        */
  k_link_target_for_export,	/* external document target                */
  kNumberOfLinkKeywords
} PDF_LINK_KEYWORD;

enum {
  k_author = 0,
  k_title,
  k_subject,
  k_keywords,
  kNumberOfDocInfoKeywords
};


/* basic types */
#ifdef	USE_MATRICES
typedef	double			t_matrix[9];
#endif

typedef	char		t_tempbuf[512];
typedef	unsigned int	PDF_OBJECT_NUM;
typedef	PDF_OBJECT_NUM	PDF_PAGE_OBJECT_NUM;  /* an object number that can */
					      /* refer ONLY to page object */
typedef	unsigned int	PDF_FONT_NUM;
typedef	unsigned int	PDF_PAGE_NUM;
typedef	unsigned int	PDF_FILE_OFFSET;


/* font list */
struct t_font_list_entry {
  struct t_font_list_entry *m_next_font_entry;
  FULL_CHAR *m_PDF_font_name;
  FULL_CHAR *m_short_font_name;
  FULL_CHAR *m_actual_font_name;
  PDF_OBJECT_NUM m_font_encoding_obj;	/* valid for entire PDF file */
  PDF_OBJECT_NUM m_pdf_object_number;	/* valid for entire PDF file */
  BOOLEAN m_font_resource_in_pdf;	/* TRUE when PDF file has       */
					/* /Type /Font resource         */
  BOOLEAN m_in_use;			/* used on a per-page basis */
};

typedef struct t_font_list_entry t_font_list_entry, *t_font_list_entry_ptr;


/* offsets of all objects (for xref list) */
typedef	PDF_FILE_OFFSET t_offset_array[kNumberOfObjectsPerBlock];

struct t_offset_block {
  struct t_offset_block *m_next_block;
  t_offset_array m_block;
};

typedef struct t_offset_block t_offset_block, *t_offset_block_ptr;


/* for /Pages object */
typedef PDF_PAGE_OBJECT_NUM t_page_array[kNumberOfPagesPerBlock];

struct t_page_block {
  struct t_page_block *m_next_block;
  t_page_array m_block;
};

typedef	struct t_page_block t_page_block, *t_page_block_ptr;


/* for font encodings */
struct	t_font_encoding_entry {
  struct t_font_encoding_entry* m_next_entry;
  PDF_OBJECT_NUM m_object_num;
  FULL_CHAR *m_font_encoding;
};

typedef struct t_font_encoding_entry
  t_font_encoding_entry, *t_font_encoding_entry_ptr;


/* for qsave/qrestore [see PDFPage_Push()] */
struct t_qsave_entry {
  struct t_qsave_entry *m_next_entry;
  int m_page_h_origin, m_page_v_origin;
  float m_page_h_scale_factor, m_page_v_scale_factor;
};

typedef struct t_qsave_entry t_qsave_entry, *t_qsave_entry_ptr;


/* for qsave/qrestore [see PDFPage_Push()] */
struct t_qsave_marking_entry {
  struct t_qsave_marking_entry* m_next_entry;
  unsigned int m_buffer_pos;
};

typedef	struct t_qsave_marking_entry t_qsave_marking_entry, *t_qsave_marking_entry_ptr;


/* target of link annotations */
struct t_target_annot_entry {
  struct t_target_annot_entry* m_next_entry;

  /* all of the following are always defined */
  FULL_CHAR *m_name;
  PDF_PAGE_OBJECT_NUM m_page_object_num;

  /* these are in PDF's default user space coordinates */
  int m_ll_x;
  int m_ll_y;
  int m_ur_x;
  int m_ur_y;

  BOOLEAN m_for_export;
};

typedef struct t_target_annot_entry t_target_annot_entry, *t_target_annot_entry_ptr;

/* source of link annotations */
struct t_source_annot_entry {
  struct t_source_annot_entry* m_next_entry;

  t_target_annot_entry* m_target;	/* if is a link and this is NULL then */
					/* the link is a fwd link and remains */
					/* unresolvable until the page is     */
					/* encountered - instead, the m_name  */
					/* field is defined; m_target will be */
					/* NULL for URI type links            */

  FULL_CHAR *m_name;			/* this string is defined if m_target */
					/* is NULL otherwise it is null       */
					/* for URI links, this contains the   */
					/* URI to link to                     */
  FULL_CHAR *m_file_spec;		/* only defined for link_type ==      */
					/* k_link_external                    */

  /* all of the following are always defined */
  /* these are in PDF's default user space coordinates */
  int m_ll_x;
  int m_ll_y;
  int m_ur_x;
  int m_ur_y;

  PDF_OBJECT_NUM m_this_object_num;	/* obj num of this "/Type /Annot" obj */
  PDF_PAGE_OBJECT_NUM m_this_page_object_num;	/* obj num of the page that   */
					/* this annot lies in                 */
  PDF_LINK_DEST_OPTION m_dest_option;
  PDF_LINK_KEYWORD m_link_type;

  BOOLEAN m_written_to_PDF_file;
};

typedef struct t_source_annot_entry t_source_annot_entry, *t_source_annot_entry_ptr;


/* matrices */
#ifdef USE_MATRICES
struct t_matrix_entry {
  struct t_matrix_entry* m_next_entry;
  t_matrix m_matrix;
};

typedef struct t_matrix_entry t_matrix_entry, *t_matrix_entry_ptr;
#endif


/* statics */

/* general */
static BOOLEAN g_PDF_debug;

/* objects */
static PDF_OBJECT_NUM		g_next_objnum;
static t_offset_block_ptr	g_obj_offset_list;	/* first block       */
static t_offset_block_ptr	g_cur_obj_offset_block;

/* fonts */
static t_font_list_entry_ptr	g_font_list;		/* backwards         */
static t_font_encoding_entry_ptr g_font_encoding_list;	/* backwards         */

/* pages */
static PDF_PAGE_NUM		g_page_count;		/* current page num, */
							/* starting at 1     */
static PDF_PAGE_OBJECT_NUM	g_page_object_num;	/* obj num of current*/
							/* "/Type /Page" obj,*/
							/* corr. to page     */
							/* num g_page_count  */
static t_page_block_ptr		g_page_block_list;	/* first block       */
static t_page_block_ptr		g_cur_page_block;
static PDF_OBJECT_NUM		g_pages_root;

/* document */
static int g_doc_h_bound;
static int g_doc_v_bound;
static FULL_CHAR* g_doc_author;
static FULL_CHAR* g_doc_title;
static FULL_CHAR* g_doc_subject;
static FULL_CHAR* g_doc_keywords;

/* link annotations */
static t_target_annot_entry_ptr g_target_annot_list;
static BOOLEAN g_has_exported_targets;

/* globals for each page */
/* these indicate what kind of content the page has */
static BOOLEAN g_page_uses_fonts;
static BOOLEAN g_page_has_text;
static BOOLEAN g_page_has_graphics;

/* these are only defined when the page has some content */
static PDF_OBJECT_NUM g_page_contents_obj_num;
static PDF_OBJECT_NUM g_page_length_obj_num;
static PDF_FILE_OFFSET g_page_start_offset;

/* valid after a PDF_Push and PDF_Pop */
static t_qsave_entry_ptr g_qsave_stack;

static t_qsave_marking_entry_ptr g_qsave_marking_stack;	/* implemented as a   */
							/* linked list; pts   */
							/* to top of stack    */
static BOOLEAN g_in_buffering_mode;
static char g_buffer[kBufferSize];			/* this buffer is used*/
							/* for removing redundant operations */
static unsigned int g_buffer_pos;

/* valid after a link annotation has been defined */
static t_source_annot_entry_ptr g_source_annot_list;

#ifdef USE_MATRICES
static t_matrix g_cur_matrix;
static t_matrix_entry_ptr g_matrix_stack;
#endif

/* track these values in case they are ever required */
static float g_page_h_scale_factor, g_page_v_scale_factor;
static int g_page_h_origin, g_page_v_origin;
static int g_page_line_width;

/* magic keywords (actually they will appear in Lout documents as "__in", "__cm", etc.) */
static char *g_unit_keywords[kNumberOfUnitKeywords] =
{
  "in", "cm", "pt", "em", "loutf", "loutv", "louts"	/* MUST be followed by a fp number */
};

static char *g_graphic_keywords[kNumberOfGraphicsKeywords] =
{
  "xsize", "ysize", "xmark", "ymark"	/* like macros, these expand to the actual value */
};

static char *g_arithmetic_keywords[kNumberOfArithmeticKeywords] =
{
  /* syntax: "__mul(x, y)" emits (x * y) to 2 decimal places              */
  /*                                                                      */
  /* Notes:                                                               */
  /*                                                                      */
  /*    sin and cos expect their arguments in degrees                     */
  /*                                                                      */
  /*    for negation, use "__sub(0, arg)"                                 */
  /*                                                                      */
  /*    __pick(i, expr1, expr2, expr3...) picks the ith expr from the     */
  /*    list of expr the "," are optional (if they are not used, you      */
  /*    should separate values with whitespace)                           */

  "add", "sub", "mul", "div", "sin", "cos", "pick"	/* like macros, these expand to the actual value */
};

static char *g_link_keywords[kNumberOfLinkKeywords] =
{
  /* syntax: "__link_source=<<name_of_target_link [dest_link_option]>>"   */
  /*                                                                      */
  /* example: "__link_source=<<chapter6>>"                                */
  /* example: "__link_source=<<part7 __FitH>>"                            */

  "link_source=<<",

  /* syntax: "__link_external=<<name_of_target_link __link_to=file_spec>>"*/
  /* syntax: "__link_external=<<name_of_target_link __link_to=<< /FS /URL /F (url)>>>>" */
  /*                                                                      */
  /* **	note the special format required for URL links **                 */
  /*                                                                      */
  /* example: "__link_external=<<chapter6 __link_to=/usr/bin/file.pdf>>"  */
  /* example: "__link_external=<<chapter6 __link_to=<< /FS /URL /F        */
  /*          (ftp://ftp.cs.usyd.edu.au/jeff/lout/user.pdf) >>>>"         */

  "link_external=<<",

  /* syntax: "__link_URI=<<URL>>"                                         */
  /*                                                                      */
  /* example: "__link_URI=<<http://www.adobe.com>>"                       */

  "link_URI=<<",

  /* syntax: "__link_target=<<name_of_target_link>>" where                */
  /* name_of_target_link is in this PDF file; name_of_target_link CANNOT  */
  /* be accessed by external documents in links                           */
  /*                                                                      */
  /* example: "__link_target=<<my_internal_target>>"                      */

  "link_target=<<",

  /* syntax: "__link_target_for_export=<<name_of_target_link>>" where     */
  /* name_of_target_link is in this file; name_of_target_link can be      */
  /* accessed by external documents in links                              */
  /*                                                                      */
  /* example: "__link_target_for_export=<<my_exported_target>>"           */

  "link_target_for_export=<<"
};

static char *g_dest_link_options[kNumberOfDestLinkOptions] =
{
  /* see PDF_LINK_DEST_OPTION for descriptions of the meanings of these */
  "__FitNoChange",
  "__Fit",
  "__FitH",
  "__FitV",
  "__FitR",
  "__FitB",
  "__FitBH",
  "__FitBV"
};

static char* g_external_file_spec_keyword[1] =
{
  "__link_to="
};

static char* g_doc_info_keywords[kNumberOfDocInfoKeywords] =
{
  "author=", "title=", "subject=", "keywords="
};

static int g_units[kNumberOfUnitKeywords];

static int g_graphics_vars[kNumberOfGraphicsKeywords];

/* text state */
static BOOLEAN g_TJ_pending;
static BOOLEAN g_ET_pending;
static BOOLEAN g_valid_text_matrix;	/* true when BT...ET block open */


/* expressions */
static int g_expr_depth = 0;
static int g_expr_index;
static t_tempbuf g_expr;

/* links */
static int g_link_depth = 0;
static int g_link_index;
static t_tempbuf g_link;
static PDF_LINK_KEYWORD g_link_keyword;

/* the 14 base fonts */
static char *g_standard_base_14_fonts[kBase14FontCount] = {
  "Courier",
  "Courier-Bold",
  "Courier-Oblique",
  "Courier-BoldOblique",
  "Helvetica",
  "Helvetica-Bold",
  "Helvetica-Oblique",
  "Helvetica-BoldOblique",
  "Symbol",
  "Times",
  "Times-Bold",
  "Times-Italic",
  "Times-BoldItalic",
  "ZapfDingbats"
};


#if PDF_COMPRESSION
static BOOLEAN		g_apply_compression;
static z_stream		g_comp_stream;           /* zlib compression stream */
static unsigned char*	g_raw_buffer_ptr;

/* compression buffers */
static unsigned char	g_raw_output[kRawOutputBufferSize];
static unsigned char	g_compressed_output[kCompressedOutputBufferSize];
#endif

/* for calculating largest page object */
#ifdef _CALC_LARGEST_PAGE_OBJECT_
static PDF_FILE_OFFSET g_max_page_length = 0;
#endif

BOOLEAN PDFHasValidTextMatrix(void)	/* this is called from z24.c */
{
  return g_valid_text_matrix;
}


/*****************************************************************************/
/*                                                                           */
/*  t_offset_block_ptr  PDFObject_FindOffsetBlock(PDF_OBJECT_NUM in_obj_num) */
/*                                                                           */
/*  Find the offset block for the given object number.                       */
/*                                                                           */
/*****************************************************************************/

static t_offset_block_ptr PDFObject_FindOffsetBlock(PDF_OBJECT_NUM in_obj_num,
  unsigned int* out_block_pos)
{
  int wanted_block_num = (in_obj_num - 1) / kNumberOfObjectsPerBlock;
  int block_pos = (in_obj_num - 1) % kNumberOfObjectsPerBlock;
  t_offset_block_ptr the_block = g_obj_offset_list;

  Assert((in_obj_num > 0) && (in_obj_num < g_next_objnum), no_fpos);

  /* find block */
  while (wanted_block_num != 0) {
    Assert(the_block != NULL, no_fpos);
    the_block = the_block->m_next_block;
    wanted_block_num--;
  }
  Assert(the_block != NULL, no_fpos);

  if (out_block_pos != NULL)
    *out_block_pos = block_pos;
  return the_block;
}


/*****************************************************************************/
/*                                                                           */
/*  PDF_OBJECT_NUM PDFObject_New(FILE* in_fp)                                */
/*                                                                           */
/*  Return the next available object number.                                 */
/*                                                                           */
/*****************************************************************************/

static PDF_OBJECT_NUM PDFObject_New(/* FILE* in_fp */)
{
  int wanted_block_num = (g_next_objnum - 1) / kNumberOfObjectsPerBlock;
  int block_pos = (g_next_objnum - 1) % kNumberOfObjectsPerBlock;
  t_offset_block_ptr the_block = g_cur_obj_offset_block;

  /* if first obj in a block then allocate the block */
  if (block_pos == 0)
  {
    the_block = (t_offset_block_ptr) malloc(sizeof(t_offset_block));
    if (the_block == NULL)
      Error(48, 1, "PDFObject_New: run out of memory", FATAL, no_fpos);
    if (wanted_block_num == 0)  /* if first block in file */
    {
      Assert(g_obj_offset_list == NULL, no_fpos);
      g_obj_offset_list = the_block;
    }
    else
    {
      Assert(g_cur_obj_offset_block != NULL, no_fpos);
      g_cur_obj_offset_block->m_next_block = the_block;
    }
    the_block->m_next_block = NULL;	/* don't forget to init this! */
    g_cur_obj_offset_block = the_block;
  }
  else
  {
    Assert(the_block != NULL, no_fpos);
  }

  /* initialise the offset of this object to zero (it hasn't been written    */
  /* to the PDF file yet)                                                    */
  the_block->m_block[block_pos] = 0;                  /* ftell(in_fp);       */

  return g_next_objnum++;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFObject_WriteRef(FILE* in_fp, PDF_OBJECT_NUM in_object_number)    */
/*                                                                           */
/*  Return the next available object number and write a reference to it.     */
/*                                                                           */
/*****************************************************************************/

static void PDFObject_WriteRef(FILE* in_fp, PDF_OBJECT_NUM in_object_number)
{
  fprintf(in_fp, "%u 0 R", in_object_number);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFObject_WriteObj(FILE* in_fp, PDF_OBJECT_NUM in_object_number)    */
/*                                                                           */
/*  Write the object's definition (and remember its file position).          */
/*                                                                           */
/*****************************************************************************/

static void PDFObject_WriteObj(FILE* in_fp, PDF_OBJECT_NUM in_object_number)
{
  unsigned int block_pos;
  t_offset_block_ptr block =
    PDFObject_FindOffsetBlock(in_object_number, &block_pos);
  Assert(block->m_block[block_pos]==0, no_fpos); /* offset shd be "unknown"  */
  block->m_block[block_pos] = ftell(in_fp);	 /* remember offset for xref */

  fprintf(in_fp, "%u 0 obj\n", in_object_number);
}

/*****************************************************************************/
/*                                                                           */
/*  PDF_OBJECT_NUM PDFObject_WriteNewObj(FILE* in_fp)                        */
/*                                                                           */
/*  Return the next available object number and write its definition.        */
/*                                                                           */
/*****************************************************************************/

static PDF_OBJECT_NUM PDFObject_WriteNewObj(FILE* in_fp)
{
  PDF_OBJECT_NUM next_ref = PDFObject_New(/* in_fp */);
  PDFObject_WriteObj(in_fp, next_ref);
  return next_ref;
}


/*****************************************************************************/
/*                                                                           */
/*  PDF_OBJECT_NUM PDFObject_WriteNextRef(FILE* in_fp)                       */
/*                                                                           */
/*  Return the next available object number and write a reference to it.     */
/*                                                                           */
/*****************************************************************************/

/* ***
static PDF_OBJECT_NUM PDFObject_WriteNextRef(FILE* in_fp)
{
  PDF_OBJECT_NUM next_ref = PDFObject_New(in_fp);
  PDFObject_WriteRef(in_fp, next_ref);
  return next_ref;
}
*** */


/*****************************************************************************/
/*                                                                           */
/* void PDFFile_BeginFontEncoding(FILE* in_fp, const char* in_encoding_name) */
/*                                                                           */
/*  Begin font encoding.                                                     */
/*                                                                           */
/*****************************************************************************/

void PDFFile_BeginFontEncoding(FILE* in_fp, const char* in_encoding_name)
{
  PDF_OBJECT_NUM encoding_num;
  t_font_encoding_entry_ptr encoding_entry;

  /* TO FILL IN: create entry in font-encoding list and add this encoding    */
  if (g_PDF_debug)
    fprintf(in_fp, "%%\n%% font encoding '%s':\n%%\n", in_encoding_name);

  encoding_num = PDFObject_WriteNewObj(in_fp);
  fputs("<<\n/Type /Encoding\n/Differences [ 0\n", in_fp);

  /* add font encoding to list of encodings (assume we don't get passed the  */
  /* same encoding more than once)                                           */
  encoding_entry =
    (t_font_encoding_entry_ptr) malloc(sizeof(t_font_encoding_entry));
  if (encoding_entry == NULL)
    Error(48, 2, "PDFFile_BeginFontEncoding: run out of memory",FATAL,no_fpos);

  encoding_entry->m_font_encoding =
    (FULL_CHAR*) malloc(strlen(in_encoding_name) + 1);
  if (encoding_entry->m_font_encoding == NULL)
    Error(48, 3, "PDFFile_BeginFontEncoding: out of memory", FATAL, no_fpos);

  encoding_entry->m_next_entry = g_font_encoding_list;
  encoding_entry->m_object_num = encoding_num;
  strcpy((char*) encoding_entry->m_font_encoding, (char*) in_encoding_name);
  g_font_encoding_list = encoding_entry;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFFile_EndFontEncoding(FILE* in_fp)                                */
/*                                                                           */
/*  End of font encoding.                                                    */
/*                                                                           */
/*****************************************************************************/

void PDFFile_EndFontEncoding(FILE* in_fp)
{
  fputs("]\n>>\nendobj\n", in_fp);
}


/*****************************************************************************/
/*                                                                           */
/* const FULL_CHAR* PDFFont_FindListEntry(const FULL_CHAR* in_real_font_name)*/
/*                                                                           */
/*  Try to find the font list entry with the specified real font name;       */
/*  return the entry's reference if found else return NULL.                  */
/*                                                                           */
/*****************************************************************************/

static t_font_list_entry_ptr
  PDFFont_FindListEntry(const FULL_CHAR* in_real_font_name)
{
  t_font_list_entry_ptr entry = g_font_list;

  while (entry != NULL) {
    if (strcmp((char*)in_real_font_name, (char*)entry->m_actual_font_name)==0)
      break;
    entry = entry->m_next_font_entry;
  }
  return entry;
}


/*****************************************************************************/
/*                                                                           */
/*  const FULL_CHAR*                                                         */
/*    PDFFont_FindListEntry_Short(const FULL_CHAR* in_short_font_name)       */
/*                                                                           */
/*  Try to find the font list entry with the specified real font name;       */
/*  return the entry's reference if found else return NULL.                  */
/*                                                                           */
/*****************************************************************************/

static t_font_list_entry_ptr
  PDFFont_FindListEntry_Short(const FULL_CHAR* in_short_font_name)
{
  t_font_list_entry_ptr entry = g_font_list;

  while (entry != NULL) {
    if (strcmp((char*)in_short_font_name, (char*)entry->m_short_font_name)==0)
      break;
    entry = entry->m_next_font_entry;
  }
  return entry;
}


/*****************************************************************************/
/*                                                                           */
/*  const t_font_list_entry_ptr PDFFont_NewListEntry(                        */
/*   const FULL_CHAR* in_short_font_name, const FULL_CHAR* in_real_font_name)*/
/*                                                                           */
/*  Create a new font entry and return the short name of the font.           */
/*                                                                           */
/*****************************************************************************/

static t_font_list_entry_ptr
  PDFFont_NewListEntry(const FULL_CHAR* in_short_font_name,
			const FULL_CHAR* in_real_font_name,
			PDF_OBJECT_NUM in_font_encoding_obj)
{
  PDF_FONT_NUM next_font_num = 0;
  t_font_list_entry_ptr new_entry = g_font_list;
  /* t_font_list_entry_ptr last_font_list_entry = NULL; */

  /* find next available font number */
  {
    while (new_entry != NULL) {
      next_font_num++;
      new_entry = new_entry->m_next_font_entry;
    }
  }

  /* make a new font list entry */
  {
    char PDF_font_name[64] = "/F";
    char num[32];

    new_entry = (t_font_list_entry_ptr) malloc(sizeof(t_font_list_entry));
    if (new_entry == NULL)
      Error(48, 4, "PDFFont_NewListEntry: run out of memory", FATAL, no_fpos);

    sprintf(num, "%u", next_font_num);
    strcat(PDF_font_name, num);

    new_entry->m_PDF_font_name =
      (FULL_CHAR*) malloc(strlen((char*) PDF_font_name) + 1);
    if (new_entry->m_PDF_font_name == NULL)
      Error(48, 5, "PDFFont_NewListEntry: run out of memory", FATAL, no_fpos);
    strcpy((char*) new_entry->m_PDF_font_name, PDF_font_name);

    new_entry->m_short_font_name =
      (FULL_CHAR*) malloc(strlen((char*) in_short_font_name) + 1);
    if (new_entry->m_short_font_name == NULL)
      Error(48, 6, "PDFFont_NewListEntry: run out of memory", FATAL, no_fpos);
    strcpy((char*) new_entry->m_short_font_name, (char*) in_short_font_name);

    new_entry->m_actual_font_name =
      (FULL_CHAR*) malloc(strlen((char*) in_real_font_name) + 1);
    if (new_entry->m_actual_font_name == NULL)
      Error(48, 7, "PDFFont_NewListEntry: run out of memory", FATAL, no_fpos);
    strcpy((char*) new_entry->m_actual_font_name, (char*) in_real_font_name);

    new_entry->m_font_encoding_obj = in_font_encoding_obj;

    new_entry->m_pdf_object_number = 0;	/* don't give this font resource an  */
					/* object number until needed        */
    /* new_entry->m_in_use = TRUE; */	/* should be cleared after each page */
    /* g_page_uses_fonts = TRUE;   */
    new_entry->m_font_resource_in_pdf = FALSE;	/* not in PDF file yet       */

    new_entry->m_next_font_entry = g_font_list;
    g_font_list = new_entry;
  }
  debug1(DPD, D, "new PDF font entry with short name %s",
    new_entry->m_short_font_name);
  return new_entry;
}


/*****************************************************************************/
/*                                                                           */
/*  const t_font_list_entry_ptr PDFGetFont(const char* in_real_font_name)    */
/*                                                                           */
/*  Return the reference of a font entry. Never returns NULL.                */
/*                                                                           */
/*****************************************************************************/
/*
static t_font_list_entry_ptr PDFGetFont(const FULL_CHAR* in_real_font_name)
{
  t_font_list_entry_ptr entry = PDFFont_FindListEntry(in_real_font_name);
  if (entry == NULL)
    entry = PDFFont_NewListEntry(in_real_font_name);
  return entry;
}
*/

/*****************************************************************************/
/*                                                                           */
/*  PDFFont_WriteObjectRef(FILE* in_fp, t_font_list_entry_ptr in_font_entry) */
/*                                                                           */
/*  Write a reference to the object to the file.                             */
/*                                                                           */
/*****************************************************************************/

static void PDFFont_WriteObjectRef(FILE* in_fp,
  const t_font_list_entry* in_font_entry)
{
  Assert(in_font_entry->m_pdf_object_number != 0, no_fpos);
  PDFObject_WriteRef(in_fp, in_font_entry->m_pdf_object_number);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFFont_WriteObject(FILE* in_fp,                                    */
/*    t_font_list_entry_ptr in_font_entry)                                   */
/*                                                                           */
/*  Write a reference to the object to the file.                             */
/*                                                                           */
/*****************************************************************************/

static void PDFFont_WriteObject(FILE* in_fp, t_font_list_entry_ptr in_font_entry)
{
  if (in_font_entry->m_pdf_object_number == 0)
    in_font_entry->m_pdf_object_number = PDFObject_New(/* in_fp */);
  PDFObject_WriteObj(in_fp, in_font_entry->m_pdf_object_number);
}


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN PDFFont_IsOneOfTheBase14Fonts(const FULL_CHAR* in_real_font_name)*/
/*                                                                           */
/*  Returns true if given font is one of the base 14 fonts.                  */
/*                                                                           */
/*****************************************************************************/

static BOOLEAN PDFFont_IsOneOfTheBase14Fonts(const FULL_CHAR* in_real_font_name)
{
  int i;
  for (i = 0; i < kBase14FontCount; i++)
    if (strcmp(g_standard_base_14_fonts[i], (char*) in_real_font_name) == 0)
      return TRUE;
  return FALSE;
}


/*****************************************************************************/
/*                                                                           */
/*  PDFFont_WriteFontResource(FILE* in_fp,                                   */
/*    t_font_list_entry_ptr in_font_entry)                                   */
/*                                                                           */
/*  Writes out the PDF idea of a Font resource.                              */
/*                                                                           */
/*****************************************************************************/

static void PDFFont_WriteFontResource(FILE* in_fp,
  t_font_list_entry_ptr in_font_entry)
{
  if (! in_font_entry->m_font_resource_in_pdf)
  {
    in_font_entry->m_font_resource_in_pdf = TRUE;

    if (g_PDF_debug)
      fprintf(in_fp, "%%\n%% declare use of font %s:\n%%\n",
	in_font_entry->m_actual_font_name);

    PDFFont_WriteObject(in_fp, in_font_entry);
    fputs("<<\n/Type /Font\n/Subtype /Type1\n", in_fp);
    fprintf(in_fp, "/Name %s\n", (char*) in_font_entry->m_PDF_font_name);
    fprintf(in_fp, "/BaseFont /%s\n", (char*) in_font_entry->m_actual_font_name);
    if (! PDFFont_IsOneOfTheBase14Fonts(in_font_entry->m_actual_font_name))
    {
      /* ***
      fputs("/FirstChar 0"\n, in_fp);	- we don't do first chars (yet)
      fputs("/LastChar 255\n", in_fp);	- we don't do last chars (yet)
      fputs("/Widths ", in_fp);		- we don't do last chars (yet)
      fputs("/FontDescriptor ", in_fp);	- we don't do font descriptors (yet)
      *** */
    }

    if (in_font_entry->m_font_encoding_obj != 0)
    {
      fputs("/Encoding ", in_fp);
      PDFObject_WriteRef(in_fp, in_font_entry->m_font_encoding_obj);
      fputs("\n", in_fp);
    }

    /* ***
    else
      Error(48, 8, "PDFFont_WriteFontResource: a font has no encoding",
	WARN, no_fpos);
    *** */
    fputs(">>\nendobj\n", in_fp);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  PDFFont_WriteFontResource_name(FILE* in_fp,                              */
/*    const FULL_CHAR* in_real_font_name)                                    */
/*                                                                           */
/*  Writes out the PDF idea of a Font resource.                              */
/*                                                                           */
/*****************************************************************************/

/* ***
static void PDFFont_WriteFontResource_name(FILE* in_fp,
  const FULL_CHAR* in_real_font_name)
{
  t_font_list_entry_ptr entry = PDFFont_FindListEntry(in_real_font_name);
  Assert(entry != NULL, no_fpos);
  PDFFont_WriteFontResource(in_fp, entry);
}
*** */


/*****************************************************************************/
/*                                                                           */
/*  const FULL_CHAR* PDFGetPDFFontName(const FULL_CHAR* in_real_font_name)   */
/*                                                                           */
/*  Return the short name of a font.                                         */
/*                                                                           */
/*****************************************************************************/

/* ***
static const FULL_CHAR* PDFGetPDFFontName(const FULL_CHAR* in_real_font_name)
{
  t_font_list_entry_ptr entry = PDFFont_FindListEntry(in_real_font_name);
  Assert(entry != NULL, no_fpos);
  return entry->m_PDF_font_name;
}
*** */


/*****************************************************************************/
/*                                                                           */
/* PDF_OBJECT_NUM PDFFont_FindFontEncoding(FULL_CHAR* in_font_encoding_name) */
/*                                                                           */
/*  Return the object number of a given font encoding.                       */
/*                                                                           */
/*****************************************************************************/

static PDF_OBJECT_NUM PDFFont_FindFontEncoding(
  const FULL_CHAR* in_font_encoding_name)
{
  t_font_encoding_entry_ptr entry = g_font_encoding_list;

  /* these two lines are Uwe patch Jul 18, 2000 */
  if (in_font_encoding_name == NULL)
    return 0;

  while (entry != NULL)
  {
    if (strcmp((char*)in_font_encoding_name, (char*)entry->m_font_encoding)==0)
      break;
    entry = entry->m_next_entry;
  }
  return (entry != NULL) ? entry->m_object_num : 0;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFFont_AddFont(                                                    */
/*           FILE* in_fp, const FULL_CHAR* in_short_font_name,               */
/*           const FULL_CHAR* in_real_font_name)                             */
/*                                                                           */
/*  Add this font to the list of fonts that the document uses. Also remember */
/*  that this font is "in use" (output when page resources are written).     */
/*                                                                           */
/*****************************************************************************/

void PDFFont_AddFont(FILE* in_fp, const FULL_CHAR* in_short_font_name,
  const FULL_CHAR* in_real_font_name, const FULL_CHAR* in_font_encoding_name)
{
  t_font_list_entry_ptr entry = PDFFont_FindListEntry(in_real_font_name);
  debug4(DPD, D, "PDFFont_AddFont(-, %s, %s, %s) [new = %s]",
    in_short_font_name, in_real_font_name,
    (in_font_encoding_name ? in_font_encoding_name : ""),
    bool(entry == NULL));
  /* *** this attempted bug fix by Jeff K. problem may be multiple font
	 entries for the same font
  if (entry == NULL)
  *** */
  if (TRUE)
    entry = PDFFont_NewListEntry(in_short_font_name, in_real_font_name,
      PDFFont_FindFontEncoding(in_font_encoding_name));

  /* ***
  entry->m_in_use = TRUE;
  g_page_uses_fonts = TRUE;
  *** */
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_SetVars(int xsize, int ysize, int xmark, int ymark,         */
/*    int loutf, int loutv, int louts)                                       */
/*                                                                           */
/*  Writes a string to the page's stream.                                    */
/*                                                                           */
/*****************************************************************************/

void PDFPage_SetVars(int xsize, int ysize, int xmark, int ymark,
  int loutf, int loutv, int louts)
{
  g_graphics_vars[k_xsize] = xsize;
  g_graphics_vars[k_ysize] = ysize;
  g_graphics_vars[k_xmark] = xmark;
  g_graphics_vars[k_ymark] = ymark;

  g_units[k_loutf] = loutf;
  g_units[k_loutv] = loutv;
  g_units[k_louts] = louts;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_FlushCompressedBuffer(FILE* in_fp)                          */
/*                                                                           */
/*  Flushes the compressed output buffer to the page's stream.               */
/*                                                                           */
/*****************************************************************************/

#if PDF_COMPRESSION
static void PDFPage_FlushCompressedBuffer(FILE* in_fp)
{
  int err;

  Assert(g_apply_compression, no_fpos);
  do {
    err = deflate(&g_comp_stream, Z_FINISH);

    fwrite(g_compressed_output,
      sizeof(g_compressed_output) - g_comp_stream.avail_out, 1, in_fp);
    g_comp_stream.next_out = g_compressed_output;
    g_comp_stream.avail_out = sizeof(g_compressed_output);
  } while (err == Z_OK);

  if (err != Z_STREAM_END)
    Error(48, 9, "PDFPage_FlushCompressedBuffer: zlib error occurred",
      FATAL, no_fpos);

  err = deflateEnd(&g_comp_stream);
  if (err != Z_OK)
    Error(48, 10, "PDFPage_FlushCompressedBuffer: zlib error occurred",
      FATAL, no_fpos);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_FlushRawBuffer(FILE* in_fp)                                 */
/*                                                                           */
/*  Attempts to compress the raw buffer; also flushes the compressed output  */
/*  buffer to the page's stream if it is full.                               */
/*                                                                           */
/*****************************************************************************/

static void PDFPage_FlushRawBuffer(FILE* in_fp)
{
  int err;

  /* compress the raw buffer */
  Assert(g_apply_compression, no_fpos);
  g_comp_stream.next_in = g_raw_output;
  g_comp_stream.avail_in = (uInt) (g_raw_buffer_ptr - g_raw_output);
  Assert(g_comp_stream.avail_out != 0, no_fpos);

  /* always compress to the point where the raw buffer is empty */
  do {
    err = deflate(&g_comp_stream, Z_NO_FLUSH);
    /* bug fix from newman-andy@yale.edu Feb 23 2000 if (err != Z_OK) */
    if ( err != Z_OK && g_comp_stream.avail_in != 0 )
      Error(48, 11, "PDFPage_FlushRawBuffer: zlib error occurred",FATAL,no_fpos);

    /* IF compressed output buffer is full THEN flush it to disk and reset it */
    if (g_comp_stream.avail_out == 0)
    {
      if (fwrite(g_compressed_output, sizeof(g_compressed_output), 1, in_fp)!=1)
	Error(48, 12, "PDFPage_FlushRawBuffer: write error occurred",
	  FATAL, no_fpos);
      g_comp_stream.next_out = g_compressed_output;
      g_comp_stream.avail_out = sizeof(g_compressed_output);
    }
  } while (g_comp_stream.avail_in != 0);

  /* reset raw buffer for next call */
  g_raw_buffer_ptr = g_raw_output;
}
#endif


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_WriteStream(FILE* in_fp, char* in_str)                      */
/*                                                                           */
/*  Writes a string to the page's stream.                                    */
/*                                                                           */
/*****************************************************************************/

static void PDFPage_WriteStream(FILE* in_fp, char* in_str)
{
  if (*in_str == 0)
    return;

#if PDF_COMPRESSION
  if (g_apply_compression)
  {
    unsigned int total = strlen(in_str);
    char *ptr = in_str;

    while (total != 0)
    {
      unsigned int len = total;
      BOOLEAN needToFlush =
	((g_raw_buffer_ptr + len) > (g_raw_output + sizeof(g_raw_output)));
      if (needToFlush)
	len = g_raw_output + sizeof(g_raw_output) - g_raw_buffer_ptr;
      memcpy(g_raw_buffer_ptr, ptr, len);

      ptr += len;
      g_raw_buffer_ptr += len;
      total -= len;

      /* IF need to flush raw buffer THEN do so */
      if (needToFlush) PDFPage_FlushRawBuffer(in_fp);
    } /* while still have bytes to process */
  }
  else
#endif
    fputs(in_str, in_fp);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_Begin(FILE* in_fp)                                          */
/*                                                                           */
/*  Begins the page's stream.                                                */
/*                                                                           */
/*****************************************************************************/

static void PDFPage_Begin(FILE* in_fp)
{
  if (g_page_contents_obj_num == 0)
  {
    t_tempbuf str;

    if (g_PDF_debug)
      fprintf(in_fp, "%%\n%% page %u's contents:\n%%\n", g_page_count);

    g_page_contents_obj_num = PDFObject_WriteNewObj(in_fp);
    g_page_length_obj_num = PDFObject_New(/* in_fp */);
    fputs("<< /Length ", in_fp);
    PDFObject_WriteRef(in_fp, g_page_length_obj_num);
#if PDF_COMPRESSION
    if (g_apply_compression) fputs(" /Filter /FlateDecode", in_fp);
#endif
    fputs(" >>\nstream\n", in_fp);
    g_page_start_offset = ftell(in_fp);

#if PDF_COMPRESSION
    if (g_apply_compression)
    {
      int err;

      g_raw_buffer_ptr = g_raw_output;
      g_comp_stream.zalloc = (alloc_func) Z_NULL;
      g_comp_stream.zfree = (free_func) Z_NULL;
      g_comp_stream.opaque = (voidpf) Z_NULL;

      err = deflateInit(&g_comp_stream, Z_DEFAULT_COMPRESSION);
      if (err != Z_OK)
	Error(48, 13, "PDFPage_Begin: zlib error occurred", FATAL, no_fpos);

      g_comp_stream.next_out = g_compressed_output;
      g_comp_stream.avail_out = sizeof(g_compressed_output);
    }
#endif

#ifndef USE_MATRICES
    sprintf(str, "%.2f 0 0 %.2f 0 0 cm\n",
      g_page_h_scale_factor, g_page_v_scale_factor);
    PDFPage_WriteStream(in_fp, str);
#endif
    sprintf(str, "%u w\n", g_page_line_width);
    PDFPage_WriteStream(in_fp, str);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_FlushBuffer(FILE* in_fp)                                    */
/*                                                                           */
/*  Flush the buffer to the page's stream and turn off buffering mode.       */
/*                                                                           */
/*****************************************************************************/

static void PDFPage_FlushBuffer(FILE* in_fp)
{
  if (g_in_buffering_mode)
  {
    g_in_buffering_mode = FALSE;

    /* empty the stack since it's no longer needed */
    while (g_qsave_marking_stack != NULL)
    {
      t_qsave_marking_entry_ptr entry = g_qsave_marking_stack;
      g_qsave_marking_stack = entry->m_next_entry;
      free(entry);
    }

    /* output the buffer */
    PDFPage_WriteStream(in_fp, g_buffer);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  PDF_FILE_OFFSET PDFPage_End(FILE* in_fp)                                 */
/*                                                                           */
/*  Ends the page's stream.                                                  */
/*                                                                           */
/*****************************************************************************/

static PDF_FILE_OFFSET PDFPage_End(FILE* in_fp)
{

  /* if page has no marks on it then write out an empty stream */
  if (g_in_buffering_mode)
  {
    g_buffer_pos = 0;
    g_buffer[0] = '\0';			/* force empty string to be written  */
    PDFPage_FlushBuffer(in_fp);	  /* all I really want is to empty the stack */
  }

  /* IF applying compression THEN first flush the raw buffer and then flush */
  /* the compressed buffer (must be performed in that order!)               */
#if PDF_COMPRESSION
  if (g_apply_compression)
  {
    if ((g_raw_buffer_ptr > g_raw_output) && (g_raw_buffer_ptr[-1] == '\n'))
      g_raw_buffer_ptr--;		/* remove last linefeed */

    PDFPage_FlushRawBuffer(in_fp);
    PDFPage_FlushCompressedBuffer(in_fp);
    fputs("\n", in_fp);
  }
#endif

  /* close page's stream */
  Assert(g_page_contents_obj_num != 0, no_fpos);
  {
    PDF_FILE_OFFSET page_length = ftell(in_fp) - g_page_start_offset;

    /* close page's stream */
    fputs("endstream\nendobj\n", in_fp);
    return page_length;
  }
  return 0;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_Write(FILE* in_fp, char* in_str)                            */
/*                                                                           */
/*  Writes a string to the page's stream.                                    */
/*                                                                           */
/*****************************************************************************/

void PDFPage_Write(FILE* in_fp, char* in_str)
{
  if (*in_str == 0)
    return;

  PDFPage_Begin(in_fp);	/* write page content's hdr "<< /Length >> stream"...*/

  /* IF trying to remove redundant operations THEN */
  if (g_in_buffering_mode)
  {

    /* if buffer will overflow then turn off buffering and flush buffer */
    unsigned int len = strlen(in_str);
    if ( (g_buffer_pos + len) > (kBufferSize-1) )	/* -1 for NULL char */
    {
      PDFPage_FlushBuffer(in_fp);
      PDFPage_WriteStream(in_fp, in_str);
    }
    else
    {
      strcpy(g_buffer + g_buffer_pos, in_str);		/* save into buffer */
      g_buffer_pos += len;
    }
  }
  else
  {
    if (g_TJ_pending)
    {
      g_TJ_pending  = FALSE;				/* clear it */
      PDFPage_WriteStream(in_fp, ")]TJ\n");
    }

    if (g_ET_pending)
    {
      g_ET_pending  = FALSE;				/* clear it */
      PDFPage_WriteStream(in_fp, "ET\n");
      g_valid_text_matrix = FALSE;			/* Td is not allowed */
    }

    PDFPage_WriteStream(in_fp, in_str);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  PDFPage_Push(FILE* in_fp)                                                */
/*                                                                           */
/*  Saves the current graphics state.                                        */
/*                                                                           */
/*****************************************************************************/

void PDFPage_Push(FILE* in_fp)
{

  /* push origin coords */
  {
    t_qsave_entry_ptr entry = (t_qsave_entry_ptr) malloc(sizeof(t_qsave_entry));
    if (entry == NULL)
      Error(48, 14, "PDFPage_Push: run out of memory", FATAL, no_fpos);

    entry->m_page_h_origin = g_page_h_origin;
    entry->m_page_v_origin = g_page_v_origin;
    /* entry->m_page_h_scale_factor = g_page_h_scale_factor; */
    /* entry->m_page_v_scale_factor = g_page_v_scale_factor; */
    entry->m_next_entry = g_qsave_stack;
    g_qsave_stack = entry;
  }

  /* if buffering text */
  if (g_in_buffering_mode)
  {

    /* push current state */
    t_qsave_marking_entry_ptr entry =
      (t_qsave_marking_entry_ptr) malloc(sizeof(t_qsave_marking_entry));
    if (entry == NULL)
      Error(48, 15, "PDFPage_Push: run out of memory", FATAL, no_fpos);

    entry->m_next_entry = g_qsave_marking_stack;    /* next-to-top-of-stack */
    entry->m_buffer_pos = g_buffer_pos;
    g_qsave_marking_stack = entry;		    /* new TOS              */
    /* g_in_buffering_mode = TRUE; */
  }

  /* write out push op */
  PDFPage_Write(in_fp, "q\n");
}


/*****************************************************************************/
/*                                                                           */
/*  PDFPage_Pop(FILE* in_fp)                                                 */
/*                                                                           */
/*  Restores the current graphics state.                                     */
/*                                                                           */
/*****************************************************************************/

void PDFPage_Pop(FILE* in_fp)
{

  /* pop origin coords */
  {
    t_qsave_entry_ptr entry = g_qsave_stack;

    g_page_h_origin = entry->m_page_h_origin;
    g_page_v_origin = entry->m_page_v_origin;
    /* g_page_h_scale_factor = entry->m_page_h_scale_factor; */
    /* g_page_v_scale_factor = entry->m_page_v_scale_factor; */

    g_qsave_stack = entry->m_next_entry;

    free(entry);
  }

  /* if no marks on page since last push (and thus there should be a stack) */
  if (g_in_buffering_mode)
  {

    /* pop state: behave as if the q...Q never existed */
    t_qsave_marking_entry_ptr entry = g_qsave_marking_stack;

    Assert(entry != NULL, no_fpos);

    g_qsave_marking_stack = entry->m_next_entry;	/* new TOS */
    g_buffer_pos   = entry->m_buffer_pos;
    g_buffer[g_buffer_pos] = '\0';	/* chop off unwanted text */

    free(entry);
  }
  else
  {
    Assert(g_qsave_marking_stack == NULL, no_fpos);
    PDFPage_Write(in_fp, "\nQ\n");
  }
}


/*****************************************************************************/
/*                                                                           */
/*  PDFFont_Set(FILE* in_fp, FULL_LENGTH in_font_size,                       */
/*    FULL_CHAR* in_font_name)                                               */
/*                                                                           */
/*  Sets the font name and size for subsequent text write statements.        */
/*                                                                           */
/*****************************************************************************/

void PDFFont_Set(FILE* in_fp, FULL_LENGTH in_font_size,
  FULL_CHAR *in_short_font_name)
{
  t_tempbuf str;

  t_font_list_entry_ptr entry = PDFFont_FindListEntry_Short(in_short_font_name);
  if( entry == NULL )
  {
    Error(48, 42, "cannot find font entry for name %s", FATAL, no_fpos,
      in_short_font_name);
  }
  /* Assert(entry != NULL, no_fpos); */
#ifdef USE_MATRICES
  sprintf(str, "%s %u Tf\n", entry->m_PDF_font_name,
    (int) (g_page_v_scale_factor * in_font_size));
#else
  sprintf(str, "%s %u Tf\n", entry->m_PDF_font_name, in_font_size);
  /* g_text_font_size_in_ems = g_page_v_scale_factor * in_font_size; */
#endif

#if 1

  /* font changes can occur within BT...ET blocks, so temporarily turn off   */
  /* g_ET_pending. I do it this way so that the qsave_marking_stack          */
  /* optimisation can still be applied (this avoids output such as           */
  /* "/F0 240 Tf /F0 240 Tf /F1 600 Tf" and instead produces "")             */
  if (g_TJ_pending)
  {
    g_TJ_pending = FALSE;		/* clear it */
    PDFPage_WriteStream(in_fp, ")]TJ\n");
  }

  {
    BOOLEAN cur_ET_pending = g_ET_pending;

    g_ET_pending = FALSE;		/* clear it */
    PDFPage_Write(in_fp, str);
    g_ET_pending = cur_ET_pending;	/* restore it */
  }
#else
  /* font changes can occur within BT...ET blocks, so bypass PDFPage_Write() */
  PDFPage_WriteStream(in_fp, str);
#endif
  entry->m_in_use = TRUE;
  g_page_uses_fonts = TRUE;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_RMove(FILE* in_fp, int hdelta, int vdelta)                  */
/*                                                                           */
/*  Offsets text pen by the given offsets.                                   */
/*                                                                           */
/*****************************************************************************/

/* ***
void PDFText_RMove(FILE* in_fp, int hdelta, int vdelta)
{
  t_tempbuf str;

  g_tx_hpos += hdelta;
  g_tx_vpos += vdelta;
#if 1
  sprintf(str, "ET\n1 0 0 1 %d %d cm\nBT\n", hdelta, vdelta);
#else
  sprintf(str, "1 0 0 1 %d %d Tm\n", g_tx_hpos, g_tx_vpos);
#endif
  PDFPage_Write(in_fp, str);
}
*** */


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_MoveTo(FILE* in_fp, int hpos, int vpos)                     */
/*                                                                           */
/*  Move text pen to the given coords.                                       */
/*                                                                           */
/*****************************************************************************/

/* ***
static void PDFText_MoveTo(FILE* in_fp, int hpos, int vpos)
{
  g_tx_hpos = 0;
  g_tx_vpos = 0;
#if 1
  PDFText_RMove(in_fp, hpos, vpos);
#else
  {
    t_tempbuf str;
    sprintf(str, "1 0 0 1 %d %d Tm\n", hpos, vpos);
    PDFPage_Write(in_fp, str);
  }
#endif
}
*** */


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_OpenString(FILE* in_fp)                                     */
/*                                                                           */
/*  Open TJ block                                                            */
/*                                                                           */
/*****************************************************************************/

static void PDFText_OpenString(FILE* in_fp)
{
  if (g_TJ_pending)
    g_TJ_pending = FALSE;	/* clear it */
  else
    PDFPage_Write(in_fp, "[(");
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_MoveToXYAndOpen(FILE* in_fp, int hpos, int vpos)           */
/*                                                                           */
/*  Move text pen to the given coords.                                       */
/*                                                                           */
/*****************************************************************************/

static void PDFText_MoveToXYAndOpen(FILE* in_fp, int hpos, int vpos)
{
#if 1
  t_tempbuf str;
  sprintf(str, "1 0 0 1 %d %d Tm\n", hpos, vpos);
  PDFPage_Write(in_fp, str);
#else
  PDFText_MoveTo(in_fp, hpos, vpos);
#endif
  PDFText_OpenString(in_fp);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_MoveToXAndOpen(FILE* in_fp, int hpos, int vpos)             */
/*                                                                           */
/*  Move text pen to the given coords.                                       */
/*                                                                           */
/*****************************************************************************/

static void PDFText_MoveToXAndOpen(FILE* in_fp, int hpos)
{
#if 1
  t_tempbuf str;
  sprintf(str, "%d 0 Td\n", hpos);
  PDFPage_Write(in_fp, str);
#else
  PDFText_MoveTo(in_fp, hpos, vpos);
#endif
  PDFText_OpenString(in_fp);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_OpenBT(FILE* in_fp)                                         */
/*                                                                           */
/*  Opens a text object at the given coords.                                 */
/*                                                                           */
/*****************************************************************************/

static void PDFText_OpenBT(FILE* in_fp)
{
  PDFPage_FlushBuffer(in_fp);	/* about to mark page: flush buffered PDF    */

  g_page_has_text = TRUE;

  if (g_TJ_pending)
  {
    g_TJ_pending = FALSE;		/* clear it */
    PDFPage_WriteStream(in_fp, ")]TJ\n");
  }

  if (g_ET_pending)
    g_ET_pending = FALSE;		/* clear it */
  else
  {
    PDFPage_Write(in_fp, "BT\n");
    g_valid_text_matrix = TRUE;		/* Td is allowed */
  }
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_OpenXY(FILE* in_fp, int hpos, int vpos)                     */
/*                                                                           */
/*  Opens a text object at the given coords.                                 */
/*                                                                           */
/*****************************************************************************/

void PDFText_OpenXY(FILE* in_fp, int hpos, int vpos)
{
  PDFText_OpenBT(in_fp);
  PDFText_MoveToXYAndOpen(in_fp, hpos, vpos);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_OpenX(FILE* in_fp, int hpos)                                */
/*                                                                           */
/*  Opens a text object at the given coords.                                 */
/*                                                                           */
/*****************************************************************************/

void PDFText_OpenX(FILE* in_fp, int hpos)
{
  PDFText_OpenBT(in_fp);
  PDFText_MoveToXAndOpen(in_fp, hpos);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_Open(FILE* in_fp)                                           */
/*                                                                           */
/*  Opens a text object.                                                     */
/*                                                                           */
/*****************************************************************************/

void PDFText_Open(FILE* in_fp)
{
  if (g_TJ_pending)
  {
    g_TJ_pending = FALSE;		/* clear it */
    Assert(g_ET_pending == TRUE, no_fpos);
    g_ET_pending = FALSE;		/* clear it */
  }
  else
  {
    PDFText_OpenBT(in_fp);
    PDFText_OpenString(in_fp);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_Kern(FILE* in_fp, int in_kern)                              */
/*                                                                           */
/*  Apply kerning to a text string.                                          */
/*                                                                           */
/*  Note: in_kern is in 1/1000 of font size                                  */
/*                                                                           */
/*****************************************************************************/

void PDFText_Kern(FILE* in_fp, int in_kern)
{
  t_tempbuf str;

  /* sprintf(str, ")%d(", -in_kern * 1000 / g_text_font_size_in_ems); */
  sprintf(str, ")%d(", -in_kern);
  PDFPage_Write(in_fp, str);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFText_Close(FILE* in_fp)                                          */
/*                                                                           */
/*  Closes a previously opened text object.                                  */
/*                                                                           */
/*****************************************************************************/

void PDFText_Close(FILE* in_fp)
{
  /* PDFPage_Begin(in_fp); - shouldn't be needed */
  Assert(g_page_contents_obj_num != 0, no_fpos);

  g_TJ_pending = TRUE;
  /* PDFPage_Write(in_fp, ")] TJ\n"); */
  g_ET_pending = TRUE;
}


#ifdef USE_MATRICES

/*****************************************************************************/
/*                                                                           */
/*  void PDF_Matrix_XY(double* in_out_x, double* in_out_y)                   */
/*                                                                           */
/*  Returns (x, y) after applying the current matrix:                        */
/*                                                                           */
/*                                  [ a b 0 ]                                */
/*  [ x y 1 ]       x       [ c d 0 ]       =       [ ax+cy+e  bx+dy+f  1 ]  */
/*                                  [ e f 1 ]                                */
/*                                                                           */
/*****************************************************************************/

static void PDF_Matrix_XY(double* in_out_x, double* in_out_y)
{
  double result_x, result_y;

  result_x = g_cur_matrix[0] * *in_out_x + g_cur_matrix[3] * *in_out_y +
    g_cur_matrix[6];
  result_y = g_cur_matrix[1] * *in_out_x + g_cur_matrix[4] * *in_out_y +
    g_cur_matrix[7];
  *in_out_x = result_x;
  *in_out_y = result_y;
}


/*****************************************************************************/
/*                                                                           */
/*  PDF_Matrix_Mul(t_matrix in_left, t_matrix in_right, t_matrix out_result) */
/*                                                                           */
/*  Multiplies the given matrices.                                           */
/*                                                                           */
/*  [ a b 0 ]               [ g h 0 ]               [ ag+bi   ah+bj   0 ]    */
/*  [ c d 0 ]       x       [ i j 0 ]       =       [ cg+di   ch+dj   0 ]    */
/*  [ e f 1 ]               [ k l 1 ]               [ eg+fi+k eh+fj+l 1 ]    */
/*                                                                           */
/*****************************************************************************/

static void PDF_Matrix_Mul(t_matrix in_left, t_matrix in_right,
  t_matrix out_result)
{
  t_matrix result;
  result[0] = in_left[0] * in_right[0] + in_left[1] * in_right[3];
  result[1] = in_left[0] * in_right[1] + in_left[1] * in_right[4];
  result[2] = 0;
  result[3] = in_left[3] * in_right[0] + in_left[4] * in_right[3];
  result[4] = in_left[3] * in_right[1] + in_left[4] * in_right[4];
  result[5] = 0;
  result[6] = in_left[6] * in_right[0] + in_left[7] * in_right[3] + in_right[6];
  result[7] = in_left[6] * in_right[1] + in_left[7] * in_right[4] + in_right[7];
  result[8] = 1;

  memcpy(out_result, result, sizeof(t_matrix));
}
#endif


/*****************************************************************************/
/*                                                                           */
/*  void  PDFPage_Scale(float in_h_scale_factor, float in_v_scale_factor)    */
/*                                                                           */
/*  Changes CTM by scale factor:                                             */
/*                                                                           */
/*  [  sh   0  0 ]                                                           */
/*  [   0  sv  0 ]                                                           */
/*  [   0   0  1 ]                                                           */
/*                                                                           */
/*****************************************************************************/

void PDFPage_Scale(FILE* in_fp, float in_h_scale_factor, float in_v_scale_factor)
{
#ifdef USE_MATRICES
  t_matrix m = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  m[0] = in_h_scale_factor;
  m[4] = in_v_scale_factor;

  PDF_Matrix_Mul(m, g_cur_matrix, g_cur_matrix);
#else
  t_tempbuf str;

  sprintf(str, "%.2f 0 0 %.2f 0 0 cm\n", in_h_scale_factor, in_v_scale_factor);
  PDFPage_Write(in_fp, str);
#endif
  g_page_h_scale_factor *= in_h_scale_factor;
  g_page_v_scale_factor *= in_v_scale_factor;
}


/*****************************************************************************/
/*                                                                           */
/*  void  PDFPage_Rotate(FILE* in_fp, float in_angle_in_radians)             */
/*                                                                           */
/*  Changes CTM by rotation factor.                                          */
/*                                                                           */
/*  [  cos a  sin a  0 ]                                                     */
/*  [ -sin a  cos a  0 ]                                                     */
/*  [   0       0    1 ]                                                     */
/*                                                                           */
/*****************************************************************************/

void PDFPage_Rotate(FILE* in_fp, float in_angle_in_radians)
{
  float cos_radians = cos(in_angle_in_radians);
  float sin_radians = sin(in_angle_in_radians);
#ifdef USE_MATRICES
  t_matrix m = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  m[0] = m[4] = cos_radians;
  m[1] = sin_radians;
  m[3] = -sin_radians;
  PDF_Matrix_Mul(m, g_cur_matrix, g_cur_matrix);
#else
  t_tempbuf str;

  sprintf(str, "%.2f %.2f %.2f %.2f 0 0 cm\n", cos_radians, sin_radians,
    -sin_radians, cos_radians);
  PDFPage_Write(in_fp, str);
#endif
}


/*****************************************************************************/
/*                                                                           */
/*  void  PDFPage_Translate(FILE* in_fp, float in_delta_h, float in_delta_v) */
/*                                                                           */
/*  Changes CTM by translation:                                              */
/*                                                                           */
/*  [  1  0  0 ]                                                             */
/*  [  0  1  0 ]                                                             */
/*  [  dh dv 1 ]                                                             */
/*                                                                           */
/*****************************************************************************/

void PDFPage_Translate(FILE* in_fp, float in_delta_h, float in_delta_v)
{
#ifdef USE_MATRICES
  t_matrix m = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
  m[6] = in_delta_h;
  m[7] = in_delta_v;
  PDF_Matrix_Mul(m, g_cur_matrix, g_cur_matrix);
#else
  t_tempbuf str;

  sprintf(str, "1 0 0 1 %.2f %.2f cm\n", in_delta_h, in_delta_v);
  PDFPage_Write(in_fp, str);
#endif
  g_page_h_origin += in_delta_h;
  g_page_v_origin += in_delta_v;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFTargetAnnot_New(FULL_CHAR* in_annot_name, ...)                   */
/*                                                                           */
/*  Create a new target annotation entry.                                    */
/*                                                                           */
/*****************************************************************************/

static void PDFTargetAnnot_New(FULL_CHAR* in_annot_name,
  unsigned int in_annot_name_length, int in_ll_x, int in_ll_y, int in_ur_x,
  int in_ur_y, BOOLEAN in_for_export)
{
  t_target_annot_entry_ptr entry =
    (t_target_annot_entry_ptr) malloc(sizeof(t_target_annot_entry));

  if (entry == NULL)
    Error(48, 16, "PDFTargetAnnot_New: run out of memory", FATAL, no_fpos);

  entry->m_name = (FULL_CHAR*) malloc(in_annot_name_length + 1);
  if (entry->m_name == NULL)
    Error(48, 17, "PDFTargetAnnot_New: run out of memory", FATAL, no_fpos);
  memcpy(entry->m_name, in_annot_name, in_annot_name_length);
  entry->m_name[in_annot_name_length] = '\0';

  Assert(g_page_contents_obj_num != 0, no_fpos);
  entry->m_page_object_num = g_page_object_num;

  entry->m_ll_x = in_ll_x;
  entry->m_ll_y = in_ll_y;
  entry->m_ur_x = in_ur_x;
  entry->m_ur_y = in_ur_y;
  entry->m_for_export = in_for_export;

  entry->m_next_entry = g_target_annot_list;
  g_target_annot_list = entry;

  if (in_for_export)
    g_has_exported_targets = in_for_export;
}


/*****************************************************************************/
/*                                                                           */
/*  t_target_annot_entry_ptr PDFTargetAnnot_Find(FULL_CHAR* in_annot_name)   */
/*                                                                           */
/*  Finds an annotation. Returns NULL if not found.                          */
/*                                                                           */
/*****************************************************************************/

static t_target_annot_entry_ptr PDFTargetAnnot_Find(FULL_CHAR* in_annot_name)
{
  t_target_annot_entry_ptr entry = g_target_annot_list;

  /* this takes O(n) time; may need re-implementing if speed is a factor */
  while (entry != NULL)
  {
    if (strcmp((char*) in_annot_name, (char*) entry->m_name) == 0)
      break;
    entry = entry->m_next_entry;
  }

  return entry;
}


/*****************************************************************************/
/*                                                                           */
/*  PDFSourceAnnot_Write(FILE* in_fp,                                        */
/*    t_source_annot_entry_ptr in_source_entry)                              */
/*                                                                           */
/*  Write an annot which specifies the source and target of the link.        */
/*                                                                           */
/*****************************************************************************/

static void PDFSourceAnnot_Write(FILE* in_fp, t_source_annot_entry_ptr in_entry)
{
  t_target_annot_entry_ptr target;

  Assert(in_entry != NULL, no_fpos);

  target = in_entry->m_target;

  /* if it is an unresolved forward link then exit */
  if ( (in_entry->m_link_type == k_link_source) && (target == NULL) )
    return;

  /* green light: write it out */
  if (g_PDF_debug)
  {
    fprintf(in_fp, "%%\n%% annotation in page object # %u to %s:\n%%\n",
      in_entry->m_this_page_object_num, in_entry->m_target->m_name);
  }

  PDFObject_WriteObj(in_fp, in_entry->m_this_object_num);
  fprintf(in_fp, "<<\n/Type /Annot\n/Subtype /Link\n"
    /* this is what Adobe does (it's also more flexible) */
    "/Rect [ %d %d %d %d ]\n/Border [ 0 0 0 ]\n",
    /* "/BS << /Type /Border /S /U >>\n" */
    /* border appearance is "underline" */
    in_entry->m_ll_x, in_entry->m_ll_y, in_entry->m_ur_x, in_entry->m_ur_y);

  switch (in_entry->m_link_type)
  {
    case k_link_source:

      fprintf(in_fp, "/Dest [ ");
      PDFObject_WriteRef(in_fp, in_entry->m_target->m_page_object_num);
      switch (in_entry->m_dest_option)
      {
	case kFitNoChange:

	  fprintf(in_fp, " /XYZ null null null");
	  /* NB NO BREAK */

	case kFit:

	  fprintf(in_fp, " /Fit");
	  break;


	case kFitH:

	  /* [ /FitH top ]: fit the width of the page to the window; top    */
	  /* specifies the y-coordinate of the top edge of the window       */
	  fprintf(in_fp, " /FitH %u", target->m_ur_y);
	  break;


	case kFitV:

	  /* [ /FitV left ]: fit the height of the page to the window;      */
	  /* left specifies the x-coordinate of the left edge of the window */
	  fprintf(in_fp, " /FitV %u", target->m_ll_x);
	  break;


	case kFitR:

	  /* [ /FitR left bottom right top ]: fit the rectangle specified   */
	  /* by left bottom right top in the window. If the height (top -   */
	  /* bottom) and width (right - left) imply different zoom factors, */
	  /* the numerically smaller zoom factor is used, to ensure that    */
	  /* the specified rectangle fits in the window                     */
	  fprintf(in_fp, " /FitR %u %u %u %u", target->m_ll_x, target->m_ll_y,
	    target->m_ur_x, target->m_ur_y);
	  break;


	case kFitB:

	  /* [ /FitB ]: fit the page's bounding box to the window           */
	  fprintf(in_fp, " /FitB");
	  break;


	case kFitBH:

	  /* [ /FitBH top ]: fit the width of the page's bounding box to    */
	  /* the window. top specifies the y-coord of top edge of window    */
	  fprintf(in_fp, " /FitBH %u", target->m_ur_y);
	  break;


	case kFitBV:

	  /* [ /FitBV left ]: fit the height of the page' bounding box to   */
	  /* the window. left specifies the x-coordinate of the left edge   */
	  /* of the window                                                  */
	  fprintf(in_fp, " /FitBV %u", target->m_ll_x);
	  break;


	default:

	  Error(48, 18, "PDFSourceAnnot_Write: invalid link dest option",
	    FATAL, no_fpos);
      }
      fprintf(in_fp, " ]\n");
      break;


    case k_link_external:

#if 1	/* required wrapper for URLs is now in the Lout libraries */
      fprintf(in_fp, "/A << /Type /Action /S /GoToR /D (%s) /F\n"
	/* <= split across lines for LONG file specs */
	"(%s) >>\n", in_entry->m_name, in_entry->m_file_spec);
#else
      if (in_entry->m_file_spec[0] != '<')
      {
	/* note: destination/target is specified as a string, as is file spec */
	fprintf(in_fp, "/A << /Type /Action /S /GoToR /D (%s) /F\n"
	  /* <= split across lines for LONG file specs */
	  "(%s) >>\n", in_entry->m_name, in_entry->m_file_spec);
      }
      else
      {
        /* if file spec starts with '<' then URL, eg <http://www.adobe.com> */
	Assert(in_entry->m_file_spec[strlen((char*) in_entry->m_file_spec)-1]
	  == '>', no_fpos);
	fprintf(in_fp, "/A << /Type /Action /S /GoToR /D (%s) /F\n"
	  /* <= split across lines for LONG file specs */
	  "<< /FS /URL /F (%s) >> >>\n", in_entry->m_name, in_entry->m_file_spec);
      }
#endif
      break;


    case k_link_URI:

      fprintf(in_fp, "/A << /Type /Action /S /URI /URI\n"
	/* <= split across lines for LONG URI's */
	"(%s) >>\n", in_entry->m_name);
      break;


    case k_link_target:
    case k_link_target_for_export:
    case kNumberOfLinkKeywords:

      break;
  }

  fprintf(in_fp, ">>\nendobj\n");
  in_entry->m_written_to_PDF_file = TRUE;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFSourceAnnot_New(FULL_CHAR* in_annot_name)                        */
/*                                                                           */
/*  Create an entry in the g_source_annot_list which links to in_annot_name. */
/*                                                                           */
/*****************************************************************************/

static t_source_annot_entry_ptr
PDFSourceAnnot_New(PDF_LINK_KEYWORD in_link_type, FULL_CHAR* in_annot_name,
  unsigned int in_annot_name_length, int in_ll_x, int in_ll_y, int in_ur_x,
  int in_ur_y, PDF_LINK_DEST_OPTION in_link_dest_option)
{
  t_target_annot_entry_ptr target = NULL;
  t_source_annot_entry_ptr entry =
    (t_source_annot_entry_ptr) malloc(sizeof(t_source_annot_entry));

  if (entry == NULL)
    Error(48, 19, "PDFSourceAnnot_New: run out of memory", FATAL, no_fpos);

  entry->m_ll_x = in_ll_x;
  entry->m_ll_y = in_ll_y;
  entry->m_ur_x = in_ur_x;
  entry->m_ur_y = in_ur_y;

  entry->m_this_object_num = PDFObject_New(/* in_fp */);
  entry->m_this_page_object_num = g_page_object_num;
  entry->m_link_type = in_link_type;
  Assert((in_link_dest_option >= kFitNoChange) &&
    (in_link_dest_option <= kFitBV), no_fpos);
  entry->m_dest_option = in_link_dest_option;
  entry->m_file_spec = NULL;
  entry->m_written_to_PDF_file = FALSE;

  if (in_link_type == k_link_source)
    target = PDFTargetAnnot_Find(in_annot_name);

  if (target != NULL)
  {
    entry->m_target = target;
    entry->m_name = NULL;
  }
  else
  {
    entry->m_target = NULL;	/* fwd link */
    entry->m_name = (FULL_CHAR*) malloc(in_annot_name_length + 1);
    if (entry->m_name == NULL)
      Error(48, 20, "PDFSourceAnnot_New: run out of memory", FATAL, no_fpos);
    memcpy(entry->m_name, in_annot_name, in_annot_name_length);
    entry->m_name[in_annot_name_length] = '\0';
  }

  entry->m_next_entry = g_source_annot_list;
  g_source_annot_list = entry;

  return entry;
}


/*****************************************************************************/
/*                                                                           */
/*  PDFSourceAnnot_Dispose(t_source_annot_entry_ptr in_source_annot)         */
/*                                                                           */
/*  Dispose of a source annot entry; returns the next entry in the list.     */
/*                                                                           */
/*****************************************************************************/

static t_source_annot_entry_ptr
  PDFSourceAnnot_Dispose(t_source_annot_entry_ptr in_source_annot)
{
  t_source_annot_entry_ptr next_entry = in_source_annot->m_next_entry;

  if (in_source_annot->m_name != NULL)
    free(in_source_annot->m_name);
  if (in_source_annot->m_file_spec != NULL)
    free(in_source_annot->m_file_spec);
  free(in_source_annot);
  return next_entry;
}


/*****************************************************************************/
/*                                                                           */
/*  float  PDFPage_GetFloat(FULL_CHAR* in_str)                               */
/*                                                                           */
/*  Outputs an appropriate PDF string for drawing a graphic element.         */
/*                                                                           */
/*****************************************************************************/

static FULL_CHAR *PDFPage_GetFloat(FULL_CHAR* in_str, float* out_value)
{
  if (sscanf((char*) in_str, "%f", out_value) == 1)
  {
    /* skip (presumed) floating point number: [ ]*[+|-][0-9.]* */
    while (isspace(*in_str))
      in_str++;
    if ( (*in_str == '-') || (*in_str == '+') )
      in_str++;
    while (isdigit(*in_str) || (*in_str == '.'))
      in_str++;
  }
  else Error(48, 21, "PDFPage_GetFloat: unable to evaluate number for Lout graphic keyword processing",
    FATAL, no_fpos);
  return in_str;
}


/*****************************************************************************/
/*                                                                           */
/*  int PDFKeyword_Find(int in_number_of_array_elements,                     */
/*    char* in_keyword_array[], FULL_CHAR* in_str)                           */
/*                                                                           */
/*  Return index into keyword array if an element matches the given string.  */
/*  Returns -1 if not found.                                                 */
/*                                                                           */
/*****************************************************************************/

static int PDFKeyword_Find(int in_number_of_array_elements,
  char* in_keyword_array[], FULL_CHAR* in_str)
{
  unsigned int i;

  /* look for keyword */
  for (i = 0; i < in_number_of_array_elements; i++)
  {
    unsigned int len = strlen(in_keyword_array[i]);

    if (memcmp(in_keyword_array[i], in_str, len) == 0)
      break;
  }

  return (i < in_number_of_array_elements) ? i : -1;
}


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *PDFPage_ProcessGraphicsKeyword(FULL_CHAR* charPtr, int i)     */
/*                                                                           */
/*  Processes a link keyword.                                                */
/*                                                                           */
/*****************************************************************************/

#if 0	/* this function is no longer used */

static FULL_CHAR *PDFPage_ProcessGraphicsKeyword(FULL_CHAR* charPtr, int i,
  char** strPtr)
{
  float value;

  /* if need be, expand this later to a full blown expression evaluator (ugh) */
  switch (*charPtr)
  {
    case '+':

      Assert(FALSE, no_fpos);
      charPtr = PDFPage_GetFloat(++charPtr, &value);
      sprintf(*strPtr, "%.2f", g_graphics_vars[i] + value);
      break;


    case '-':

      Assert(FALSE, no_fpos);
      charPtr = PDFPage_GetFloat(++charPtr, &value);
      sprintf(*strPtr, "%.2f", g_graphics_vars[i] - value);
      break;


    case '*':

      Assert(FALSE, no_fpos);
      charPtr = PDFPage_GetFloat(++charPtr, &value);
      sprintf(*strPtr, "%.2f", g_graphics_vars[i] * value);
      break;


    case '/':

      Assert(FALSE, no_fpos);
      charPtr = PDFPage_GetFloat(++charPtr, &value);
      Assert(value != 0, no_fpos);	/* not great since value is a float... */
      sprintf(*strPtr, "%.2f", g_graphics_vars[i] / value);
      break;


    default:

      sprintf(*strPtr, "%d", g_graphics_vars[i]);
      break;
  }
  *strPtr += strlen(*strPtr);
  return charPtr;
}

#endif


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_ProcessLinkKeyword(void)                                    */
/*                                                                           */
/*  Processes a link keyword.                                                */
/*                                                                           */
/*****************************************************************************/

static void PDFPage_ProcessLinkKeyword(void)
{
  FULL_CHAR* charPtr = (FULL_CHAR*) g_link;
  PDF_LINK_KEYWORD keyword = g_link_keyword;
  unsigned int link_len = 0;
  FULL_CHAR* link_name = charPtr;

  /* scan for name of link; scan until end of string or until ' __' reached  */
  /* (scan for name of link; scan until end of string or whitespace reached) */
#if 1

  FULL_CHAR* parm = NULL;
  debug1(DPD, D, "PDFPage_ProcessLinkKeyword(g_link = %s", g_link);

  while ((*charPtr != '\0') &&
    !(isspace(charPtr[0]) && (charPtr[1] == '_') && (charPtr[2] == '_')))
  {
    link_len++;
    charPtr++;
  }

  if (*charPtr != '\0')
    parm = ++charPtr;

  while (*charPtr != '\0')
    charPtr++;
#else
  while ((*charPtr != '\0') && ! isspace(*charPtr))
  {
    link_len++;
    charPtr++;
  }
#endif
  if (link_len == 0)
    Error(48, 22, "PDFPage_ProcessLinkKeyword: empty link-name / URI; ignored.",
      WARN, no_fpos);
  else
  {

    /* see documentaton for @Graphic for the meaning of the x, y parms */
    /* translate the object's box into PDF's default user space */
    int ll_x = g_page_h_origin * g_page_h_scale_factor;
    int ll_y = g_page_v_origin * g_page_v_scale_factor;
    int ur_x = (g_page_h_origin + g_graphics_vars[k_xsize]) * g_page_h_scale_factor;
    int ur_y = (g_page_v_origin + g_graphics_vars[k_ysize]) * g_page_v_scale_factor;

    /* remove this block later (it produces debugging output): */
#if 0
    {
      t_tempbuf strz = "PDFPage_ProcessLinkKeyword: ";
      switch (keyword)
      {
	case k_link_source:

	  strcat(strz, "link_source           =");
	  break;


	case k_link_external:

	  strcat(strz, "link_external         =");
	  break;


	case k_link_URI:

	  strcat(strz, "link_URI              =");
	  break;


	case k_link_target:

	  strcat(strz, "link_target           =");
	  break;


	case k_link_target_for_export:

	  strcat(strz, "link_target_for_export=");
	  break;
      }
      strcat(strz, (char*) link_name);
      fprintf(stderr, "%s", strz);
      /* Err or(48, 23, strz, WARN, no_fpos); */
    }
#endif
    switch (keyword)
    {
      case k_link_source:

	{
	  int j;

	  /* if there is a dest option specified then get it */
	  if (parm != NULL)
	  {
	    j = PDFKeyword_Find(kNumberOfDestLinkOptions, g_dest_link_options,
		charPtr);
	    if (j >= 0)		/* note signed comparison */
	      charPtr += strlen(g_dest_link_options[j]);
	    else
	    {
	      j = (int) kFitNoChange;	/* default */
	      /* must consume the rest of the string */
	      while (*charPtr != '\0')
		charPtr++;
	      link_len = charPtr - link_name;
	    }
	  }
	  else
	    j = (int) kFitNoChange;	/* default */

	  PDFSourceAnnot_New(keyword, link_name, link_len,
	    ll_x, ll_y, ur_x, ur_y, (PDF_LINK_DEST_OPTION) j);
	  break;
	}


      case k_link_external:
      case k_link_URI:

	{
	  t_source_annot_entry_ptr source;
	  source = PDFSourceAnnot_New(keyword, link_name, link_len, ll_x,
	    ll_y, ur_x, ur_y, (PDF_LINK_DEST_OPTION) 0 /* doesn't matter */);
	  if (keyword == k_link_external)
	  {
	    int j;

	    link_len = 0;

	    if (parm != NULL)
	    {
	      j = PDFKeyword_Find(1, g_external_file_spec_keyword, parm);
	      if (j == 0)
	      {
		parm += strlen(g_external_file_spec_keyword[0]);
		link_len = strlen((char*) parm);
#if 0
		/* scan for name of file spec; scan until end of string or */
		/* until whitespace reached                                */
		link_name = charPtr;
		while ((*charPtr != '\0') && ! isspace(*charPtr))
		{
		  link_len++;
		  charPtr++;
		}
#endif
	      }
	    }

	    if (link_len == 0)
	      Error(48, 24, "PDFPage_ProcessLinkKeyword: empty file spec",
		FATAL, no_fpos);
	    else
	    {
	      source->m_file_spec = (FULL_CHAR*) malloc(link_len + 1);
	      if (source->m_file_spec == NULL)
		Error(48, 25, "PDFPage_ProcessLinkKeyword: out of memory",
		  FATAL, no_fpos);
#if 1
	      strcpy((char*) source->m_file_spec, (char*) parm);
#else
	      memcpy(source->m_file_spec, link_name, link_len);
	      source->m_file_spec[link_len] = '\0';
#endif
	    }
	  }
	  break;
	}


      case k_link_target:
      case k_link_target_for_export:

	PDFTargetAnnot_New(link_name, link_len, ll_x, ll_y, ur_x, ur_y,
	  keyword == k_link_target_for_export);
	break;


      case kNumberOfLinkKeywords:
	break;

    } /* switch */
  } /* else */

  debug0(DPD, D, "PDFPage_ProcessLinkKeyword returning");
  /* return charPtr; */
} /* PDFPage_ProcessLinkKeyword */


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR* PDFPage_ProcessDocInfoKeyword(FULL_CHAR* charPtr, int i)      */
/*                                                                           */
/*  Processes a document info keyword.                                       */
/*                                                                           */
/*****************************************************************************/

static FULL_CHAR *PDFPage_ProcessDocInfoKeyword(FULL_CHAR* charPtr, int i)
{
  switch (i)
  {
    case k_author:

      if (g_doc_author != NULL)
	free(g_doc_author);
      g_doc_author = (FULL_CHAR*) malloc(strlen((char*) charPtr) + 1);
      if (g_doc_author == NULL)
	Error(48, 26, "PDFPage_ProcessDocInfoKeyword: no memory for __author=",
	  WARN, no_fpos);
      else
	strcpy((char*) g_doc_author, (char*) charPtr);
      break;


    case k_title:

      if (g_doc_title != NULL)
	free(g_doc_title);
      g_doc_title = (FULL_CHAR*) malloc(strlen((char*) charPtr) + 1);
      if (g_doc_title == NULL)
	Error(48, 27, "PDFPage_ProcessDocInfoKeyword: no memory for __title=",
	  WARN, no_fpos);
       else
	strcpy((char*) g_doc_title, (char*) charPtr);
      break;


    case k_subject:

      if (g_doc_subject != NULL)
	free(g_doc_subject);
      g_doc_subject = (FULL_CHAR*) malloc(strlen((char*) charPtr) + 1);
      if (g_doc_subject == NULL)
	Error(47, 28, "PDFPage_ProcessDocInfoKeyword: no memory for __subject=",
	  WARN, no_fpos);
      else
	strcpy((char*) g_doc_subject, (char*) charPtr);
      break;


    case k_keywords:
      if (g_doc_keywords != NULL)
	free(g_doc_keywords);
      g_doc_keywords = (FULL_CHAR*) malloc(strlen((char*) charPtr) + 1);
      if (g_doc_keywords == NULL)
	Error(48, 29, "PDFPage_ProcessDocInfoKeyword: no memory for __keywords=",
	  WARN, no_fpos);
      else
	strcpy((char*) g_doc_keywords, (char*) charPtr);
      break;

  }
  return (charPtr + strlen((char*) charPtr));
}


/*****************************************************************************/
/*                                                                           */
/*  void  PDFPage_EvalExpr(char* inExpr)                                     */
/*                                                                           */
/*  Evaluate collected expression in the given expression buffer.            */
/*                                                                           */
/*****************************************************************************/

static char *PDFPage_EvalExpr(char* inExpr, float* outValue)
{
  int i;
  char* chp = inExpr;

  while (isspace( (int) *chp))	/* ignore leading white space */
    chp++;

  while (*chp == '_')
    chp++;

  while (*chp == '+')	/* ignore unary + */
    chp++;

  if (isdigit((int) *chp) || (*chp == '.'))
  {
    chp = (char*) PDFPage_GetFloat((FULL_CHAR*) chp, outValue);
  }
  else if (*chp == '-')	/* handle unary negation */
  {
    float val;

    chp = PDFPage_EvalExpr(++chp, &val);
    *outValue = -val;
  }
  else
  {
    i = PDFKeyword_Find(kNumberOfArithmeticKeywords,
	  g_arithmetic_keywords, (FULL_CHAR*) chp);
    if (i >= 0)
    {
      float val1, val2;

      chp += strlen(g_arithmetic_keywords[i]);

      while (isspace( (int) *chp))
	chp++;
      if (*chp != '(')
	Error(48, 30, "PDFPage_EvalExpr: '(' expected", FATAL, no_fpos);
      chp = PDFPage_EvalExpr(++chp, &val1);
      if ( (i <= k_div) || (i == k_pick) )
      {
	int count;

	if (i == k_pick)
	{
	  count = floor(val1);
	  Assert(count != 0, no_fpos);
	}
	else
	  count = 1;

	if (*chp != ',')
	  Error(48, 31, "PDFPage_EvalExpr: ',' expected", FATAL, no_fpos);

	do {
	  chp = PDFPage_EvalExpr(++chp, &val2);
	  if ((count != 1) && (*chp == ','))
	    ++chp;
	} while (--count != 0);
      }
      if (*chp != ')')
	Error(48, 32, "PDFPage_EvalExpr: ')' expected", FATAL, no_fpos);
      ++chp;
      switch (i)
      {

	case k_add:
	  *outValue = val1 + val2;
	  break;


	case k_sub:

	  *outValue = val1 - val2;
	  break;


	case k_mul:

	  *outValue = val1 * val2;
	  break;


	case k_div:

	  Assert(val2 != 0, no_fpos); /* not great since value is a float... */
	  *outValue = val1 / val2;
	  break;


	case k_sin:

	  *outValue = sin((double) val1 * (double) PI / (double) 180.0);
	  break;


	case k_cos:

	  *outValue = cos((double) val1 * (double) PI / (double) 180.0);
	  break;


	case k_pick:

	  *outValue = val2;
	  break;
      }
    }
    else
    {
      i = PDFKeyword_Find(kNumberOfGraphicsKeywords, g_graphic_keywords,
	    (FULL_CHAR*) chp);
      if (i >= 0)
      {
	chp += strlen(g_graphic_keywords[i]);
	*outValue = g_graphics_vars[i];
      }
      else
      {
	i = PDFKeyword_Find(kNumberOfUnitKeywords, g_unit_keywords, (FULL_CHAR*) chp);
	if (i >= 0)
	{
	  chp += strlen(g_unit_keywords[i]);
	  *outValue = g_units[i];
	}
	else
	{
	  Error(48, 33, "PDFPage_EvalExpr: __add, __sub, __mul, __div, or a unit keyword was expected",
	    FATAL, no_fpos);
	}
      }
    }
  }
  return chp;
}


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *PDFPage_CollectExpr(FULL_CHAR* charPtr)                       */
/*                                                                           */
/*  Collect expression into the expression buffer.                           */
/*                                                                           */
/*****************************************************************************/

static FULL_CHAR *PDFPage_CollectExpr(FULL_CHAR* charPtr, BOOLEAN* outHasResult,
  float* outResult)
{
  *outHasResult = FALSE;
  while (*charPtr != 0)
  {
    char ch;

    if ( g_expr_index >= sizeof(g_expr) )
      Error(48, 34, "PDFPage_CollectExpr: expression too long (max. 512 chars)",
	FATAL, no_fpos);

    g_expr[g_expr_index++] = ch = *charPtr++;
    if (ch == '(')
      g_expr_depth++;
    else if (ch == ')')
    {
      Assert(g_expr_depth != 0, no_fpos);
      g_expr_depth--;
      if (g_expr_depth == 0)
      {
	g_expr[g_expr_index] = '\0';	/* terminate the string */
	(char*) PDFPage_EvalExpr(g_expr, outResult);
	*outHasResult = TRUE;
	break;	/* exit while */
      }
    }
  }
  return charPtr;
}


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *PDFPage_CollectLink(FULL_CHAR* charPtr)                       */
/*                                                                           */
/*  Collect link into the link buffer.                                       */
/*                                                                           */
/*****************************************************************************/

static FULL_CHAR *PDFPage_CollectLink(FULL_CHAR* charPtr
  /*, BOOLEAN* outHasResult, float* outResult*/)
{
  debug1(DPD, D, "PDFPage_CollectLink(\"%s\")", charPtr);
  while (*charPtr != 0)
  {
    char ch;

    if ( g_link_index >= sizeof(g_link) )
      Error(48, 35, "PDFPage_CollectLink: link too long (max. 512 chars)",
	FATAL, no_fpos);

    g_link[g_link_index++] = ch = *charPtr++;
    if ((ch == '<') && (*charPtr == '<'))
    {
      g_link[g_link_index++] = *charPtr++;
      g_link_depth++;
    }
    else if ((ch == '>') && (*charPtr == '>'))
    {
      Assert(g_link_depth != 0, no_fpos);

      g_link_depth--;
      if (g_link_depth == 0)
      {
	/* I don't want the outermost '<<' '>>' pair */
	g_link[--g_link_index] = '\0';	/* terminate the string */

	PDFPage_ProcessLinkKeyword();

	charPtr++;
	break;	/* exit while */
      }
      else
	g_link[g_link_index++] = *charPtr++;
    }
  }
  debug0(DPD, D, "PDFPage_CollectLink returning");
  return charPtr;
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_WriteGraphic(FILE* in_fp, FULL_CHAR* in_str)                */
/*                                                                           */
/*  Outputs an appropriate PDF string for drawing a graphic element.         */
/*                                                                           */
/*****************************************************************************/

void PDFPage_WriteGraphic(FILE* in_fp, FULL_CHAR* in_str)
{
  t_tempbuf str;

  FULL_CHAR *charPtr = in_str;
  char *strPtr = str;

  if (*charPtr == 0)
    return;

  /* if in collecting an expression mode then collect until terminating ')' */
  if (g_expr_depth != 0)
  {
    BOOLEAN hasResult;
    float value;

    charPtr = PDFPage_CollectExpr(charPtr, &hasResult, &value);
    if (hasResult)
    {
      sprintf(strPtr, "%.2f", value);
      strPtr += strlen(strPtr);
    }
  }

  /* if in collecting-a-link mode then collect until terminating '>>'        */
  if (g_link_depth != 0)
    charPtr = PDFPage_CollectLink(charPtr);

  /* scan the string for '__' otherwise output it */
  while (*charPtr != 0)
  {
    int i;
    float value;

    Assert(strPtr < (str + sizeof(t_tempbuf)), no_fpos);

    /* look for "__" (double underline) */
    if ( (charPtr[0] == '_') && (charPtr[1] == '_') )
    {
      charPtr += 2;

      /* "in", "cm", "pt", "em", "loutf", "loutv", "louts" */
#if 0
      i = PDFKeyword_Find(kNumberOfUnitKeywords, g_unit_keywords, charPtr);
      if (i >= 0)
      {
	Assert(FALSE, no_fpos);
	charPtr += strlen(g_unit_keywords[i]);	/* skip keyword */
	charPtr = PDFPage_GetFloat(charPtr, &value); /* get value */
	sprintf(strPtr, "%.2f", g_units[i] * value); /* output it */
	strPtr += strlen(strPtr);
      }
      else
#endif
      {

	/* "xsize", "ysize", "xmark", "ymark" */
	i = PDFKeyword_Find(kNumberOfGraphicsKeywords, g_graphic_keywords, charPtr);
	if (i >= 0)
	{
	  charPtr += strlen(g_graphic_keywords[i]);
#if 1
	  sprintf(strPtr, "%d", g_graphics_vars[i]);
	  strPtr += strlen(strPtr);
#else
	  charPtr = PDFPage_ProcessGraphicsKeyword(charPtr, i, &strPtr);
#endif
	}
	else
	{
	  /* "link_source=<<", "link_target=<<", "link_target_for_export=<<", "link_URI=<<" */
	  i = PDFKeyword_Find(kNumberOfLinkKeywords, g_link_keywords, charPtr);
	  if (i >= 0)
	  {
	    charPtr += strlen(g_link_keywords[i]);
#if 1
	    while (isspace(*charPtr))
	      charPtr++;

	    g_link_index = 0;
	    g_link_depth++;
	    g_link_keyword = (PDF_LINK_KEYWORD) i;
	    charPtr = PDFPage_CollectLink(charPtr);
#else
	    charPtr = PDFPage_ProcessLinkKeyword(charPtr, (PDF_LINK_KEYWORD) i);
#endif
	  } /* if */
	  else
	  {

	    /* "author=", "title=", "subject=", "keywords=" */
	    i = PDFKeyword_Find(kNumberOfDocInfoKeywords, g_doc_info_keywords, charPtr);
	    if (i >= 0)
	    {
	      charPtr += strlen(g_doc_info_keywords[i]);
	      charPtr = PDFPage_ProcessDocInfoKeyword(charPtr, i);
	    }
	    else
	    {

	      /* "add" "sub" "mul" "div", "sin", "cos" */
	      i = PDFKeyword_Find(kNumberOfArithmeticKeywords, g_arithmetic_keywords, charPtr);
	      if (i >= 0)
	      {
		strcpy(g_expr, g_arithmetic_keywords[i]);
		charPtr += strlen(g_arithmetic_keywords[i]);
		while (isspace(*charPtr))
		  charPtr++;
		if (*charPtr != '(')
		  Error(48, 36, "PDFPage_WriteGraphic: '(' expected", FATAL, no_fpos);

		strcat(g_expr, "(");
		g_expr_index = strlen(g_expr);
		g_expr_depth++;
		{
		  BOOLEAN hasResult;

		  charPtr = PDFPage_CollectExpr(++charPtr, &hasResult, &value);
		  if (hasResult)
		  {
		    sprintf(strPtr, "%.2f", value);
		    strPtr += strlen(strPtr);
		  }
		}
	      }
	      else
	      {
		/* alert user in case there was a spelling mistake */
		Error(48, 37, "PDFPage_WriteGraphic: '__' encountered while processing @Graphic", WARN, no_fpos);
		*strPtr++ = '_';
		*strPtr++ = '_';
	      } /* else */
	    } /* else */
	  } /* else */
	} /* else */
      } /* else */
    } /* if */
    else
    {
      *strPtr++ = *charPtr++;
    }
  }

  *strPtr = 0;

  PDFPage_FlushBuffer(in_fp);	/* this is a marking operation, so flush */
  PDFPage_Write(in_fp, str);
}


/*****************************************************************************/
/*                                                                           */
/* PDFPage_PrintUnderline(FILE* in_fp, int x1, int x2, int y, int thickness) */
/*                                                                           */
/*  Implements underlining (just draws a horizontal line).                   */
/*                                                                           */
/*****************************************************************************/

void PDFPage_PrintUnderline(FILE* in_fp, int in_x1, int in_x2, int in_y,
  int in_thickness)
{
  t_tempbuf str;

  /* this is a marking operation, so flush and turn off buffering */
  PDFPage_FlushBuffer(in_fp);

  /* fprintf(out_fp, "/ul { gsave setlinewidth dup 3 1 roll\n"); */
  /* fprintf(out_fp, "      moveto lineto stroke grestore } bind def\n");  */

  sprintf(str, "q %d w %d %d m %d %d l s Q\n",in_thickness,in_x1,in_y,in_x2,in_y);
  PDFPage_Write(in_fp, str);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_Init(FILE* in_fp, float in_scale_factor, int in_line_width) */
/*                                                                           */
/*  Inits the vars for the start of processing a new page.                   */
/*                                                                           */
/*  [ 0 1 2 ]               [ s 0 0 ]                                        */
/*  [ 3 4 5 ]       =       [ 0 s 0 ]                                        */
/*  [ 6 7 8 ]               [ 0 0 1 ]                                        */
/*                                                                           */
/*****************************************************************************/

void  PDFPage_Init(FILE* in_fp, float in_scale_factor, int in_line_width)
{

#ifdef USE_MATRICES
  g_cur_matrix[0] = g_cur_matrix[4] = in_scale_factor;
  g_cur_matrix[1] = g_cur_matrix[2] = g_cur_matrix[3] =
  g_cur_matrix[5] = g_cur_matrix[6] = g_cur_matrix[7] = 0;
  g_cur_matrix[8] = 1;
  g_matrix_stack = NULL;
#endif

  /* clear/init page vars */
  g_page_uses_fonts = FALSE;
  g_page_has_text  = FALSE;
  g_page_has_graphics = FALSE;

  g_page_contents_obj_num = 0; /* undefined */
  g_page_length_obj_num = 0; /* undefined */
  g_page_start_offset = 0; /* undefined */
  /* g_text_font_size_in_ems = 0; */ /* undefined */

  g_page_h_scale_factor = g_page_v_scale_factor = in_scale_factor;
  g_page_h_origin = g_page_v_origin = 0;
  g_page_line_width = in_line_width;

  /* ***
  g_graphics_vars[k_in] = IN;
  g_graphics_vars[k_cm] = CM;
  g_graphics_vars[k_pt] = PT;
  g_graphics_vars[k_em] = EM;
  *** */
  g_graphics_vars[k_xsize] = 0; /* undefined */
  g_graphics_vars[k_ysize] = 0; /* undefined */
  g_graphics_vars[k_xmark] = 0; /* undefined */
  g_graphics_vars[k_ymark] = 0; /* undefined */
  /* ***
  g_graphics_vars[k_loutf] = 0;
  g_graphics_vars[k_loutv] = 0;
  g_graphics_vars[k_louts] = 0;
  *** */

  /* No need to touch k_in other constant units */
  g_units[k_loutf] = 0; /* undefined */
  g_units[k_loutv] = 0; /* undefined */
  g_units[k_louts] = 0; /* undefined */

  g_ET_pending = FALSE;
  g_TJ_pending = FALSE;
  g_valid_text_matrix = FALSE;	/* Td is not allowed */

  /* mark all fonts "not in use" */
  {
    t_font_list_entry_ptr entry = g_font_list;
    while (entry != NULL) {
      entry->m_in_use = FALSE;	/* set the "in use" state to "not in use" */
      entry = entry->m_next_font_entry;
    }
  }

  /* init qsave stack */
  g_qsave_stack = NULL;

  /* init qsave_marking stack */
  g_qsave_marking_stack = NULL;
  g_buffer_pos   = 0;
  /* buffer contains empty string */
  g_buffer[0]    = '\0';
  /* try to chop entire stream if possible! Originally: FALSE; */
  /* turn on buffering only AFTER a save request */
  g_in_buffering_mode  = FALSE;
  /* try to chop entire stream if possible! Originally: FALSE; */
  /* turn on buffering only AFTER a save request */
  g_in_buffering_mode  = TRUE;

  /* bump page number */
  ++g_page_count;
  g_page_object_num  = PDFObject_New(/* in_fp */);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFPage_Cleanup(FILE* in_fp)                                        */
/*                                                                           */
/*  Cleans up the processing after a page's contents have been written out.  */
/*                                                                           */
/*****************************************************************************/

void PDFPage_Cleanup(FILE* in_fp)
{
  BOOLEAN hasAnnot = FALSE;

  Assert(g_qsave_stack == NULL, no_fpos);

  /* if page has some content then close its stream object */
  if (g_page_contents_obj_num != 0)
  {
    PDF_FILE_OFFSET page_length = PDFPage_End(in_fp);

#ifdef _CALC_LARGEST_PAGE_OBJECT_
    if (page_length > g_max_page_length)
      g_max_page_length = page_length;
#endif

    /* write page's length object */
    if (g_PDF_debug)
      fprintf(in_fp, "%%\n%% length object for page %u:\n%%\n", g_page_count);

    PDFObject_WriteObj(in_fp, g_page_length_obj_num);
    fprintf(in_fp, "%u\nendobj\n", page_length);

    /* write out any used font resources */
    {
      t_font_list_entry_ptr entry = g_font_list;
      while (entry != NULL) {
	PDFFont_WriteFontResource(in_fp, entry);
	entry = entry->m_next_font_entry;
      }
    }
  }

  /* write out annotations */
  {
    t_source_annot_entry_ptr source = g_source_annot_list;

    while (source != NULL)
    {
      if (source->m_this_page_object_num == g_page_object_num)
      {

	/* even if the annotation(s) cannot be written out now, flag the */
	/* fact that this page has annotations                           */
	hasAnnot = TRUE;

	/* attempt to write out annotation */
	PDFSourceAnnot_Write(in_fp, source);
      } /* if annot entry belongs to this page */
      source = source->m_next_entry;
    } /* while */
  }

  /* start writing page object ("/Type /Page"); remember its number */
  {
    unsigned int wanted_block_num = (g_page_count - 1) / kNumberOfPagesPerBlock;
    unsigned int block_pos = (g_page_count - 1) % kNumberOfPagesPerBlock;
    t_page_block_ptr the_block = g_cur_page_block;

    /* if first obj in a block then allocate the block */
    if (block_pos == 0)
    {
      the_block = (t_page_block_ptr) malloc(sizeof(t_page_block));
      if (the_block == NULL)
        Error(48, 38, "PDFPage_Cleanup: run out of memory", FATAL, no_fpos);
      if (wanted_block_num == 0)	/* if first block in file */
      {
	Assert(g_page_block_list == NULL, no_fpos);
	g_page_block_list = the_block;
      }
      else
      {
	Assert(g_cur_page_block != NULL, no_fpos);
	g_cur_page_block->m_next_block = the_block;
      }
      the_block->m_next_block = NULL;	/* don't forget to init this! */
      g_cur_page_block = the_block;
    }
    else
    {
      Assert(the_block != NULL, no_fpos);
    }

    /* save object number of this page for later use in the /Pages list */
    if (g_PDF_debug)
      fprintf(in_fp, "%%\n%% page number %u:\n%%\n", g_page_count);
    the_block->m_block[block_pos] = g_page_object_num;
    PDFObject_WriteObj(in_fp, g_page_object_num);
    /* PDFObject_WriteNewObj(in_fp); */
  }

  /* write out /Page ID */
  fputs("<<\n/Type /Page\n", in_fp);

  /* write out page size and orientation */
  fprintf(in_fp, "/CropBox [ 0 0 %u %u ]\n",g_doc_h_bound,g_doc_v_bound);

  /* write out parent object ref */
  fputs("/Parent ", in_fp);
  PDFObject_WriteRef(in_fp, g_pages_root);
  fputs("\n", in_fp);

  /* write out contents object ref (if it exists) */
  if (g_page_contents_obj_num != 0)
  {
    fputs("/Contents ", in_fp);
    PDFObject_WriteRef(in_fp, g_page_contents_obj_num);
    fputs("\n", in_fp);
  }

  /* open resources dictionary */
  if (g_page_uses_fonts || g_page_has_text || g_page_has_graphics)
    fputs("/Resources\n<<\n", in_fp);

  /* write out font resources used */
  if (g_page_uses_fonts)
  {
    t_font_list_entry_ptr entry = g_font_list;
    fputs("/Font <<", in_fp);
    while (entry != NULL) {
      if (entry->m_in_use) {
	fprintf(in_fp, " %s ", entry->m_PDF_font_name);
	PDFFont_WriteObjectRef(in_fp, entry);
      }
      entry = entry->m_next_font_entry;
    }
    fputs(" >>\n", in_fp);
  }

  /* write out any procsets used */
  if (g_page_has_text || g_page_has_graphics)
  {
    fputs("/ProcSet [ /PDF", in_fp);
    if (g_page_has_text)
      fputs(" /Text", in_fp);
    fputs(" ]\n", in_fp);
  }

  /* close resources dictionary */
  if (g_page_uses_fonts || g_page_has_text || g_page_has_graphics)
    fputs(">>\n", in_fp);

  /* write out annot array */
  if (hasAnnot)
  {
    t_source_annot_entry_ptr entry = g_source_annot_list;
    t_source_annot_entry_ptr previous_entry = NULL;

    /* write out annotations */
    fputs("/Annots [", in_fp);
    while (entry != NULL)
    {
      if (entry->m_this_page_object_num == g_page_object_num)
      {
	fputs(" ", in_fp);
	PDFObject_WriteRef(in_fp, entry->m_this_object_num);

	/* if the annotation has just been written out above then delete it */
	if (entry->m_written_to_PDF_file)
	{
	  t_source_annot_entry_ptr next_entry = entry->m_next_entry;
	  if (g_source_annot_list == entry)
		g_source_annot_list = next_entry;
	  if (previous_entry != NULL)
		previous_entry->m_next_entry = next_entry;
	  PDFSourceAnnot_Dispose(entry);
	  entry = next_entry;
	}
	else /* annot is a fwd referring one: defer deleting it */
	{
	  previous_entry = entry;
	  entry = entry->m_next_entry;
	}
      } /* if annot entry belongs to this page */
      else /* annot does not belong to this page; skip it */
      {
	previous_entry = entry;
	entry = entry->m_next_entry;
      }
    } /* while */
    fputs(" ]\n", in_fp);
  } /* if */

  /* close object */
  fputs(">>\nendobj\n", in_fp);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDFFile_Init(FILE* in_fp, int in_h_bound, int in_v_bound)           */
/*                                                                           */
/*  Initialize this module.                                                  */
/*                                                                           */
/*****************************************************************************/

void PDFFile_Init(FILE* in_fp, int in_h_bound, int in_v_bound,
  int in_IN, int in_CM, int in_PT, int in_EM)
{
  /* write PDF header */
  fputs("%PDF-1.2\n", in_fp);			/* identifies this as PDF   */
  fputs("\045\342\343\317\323\n", in_fp);	/* 0x25 0xE2 0xE3 0xCF 0xD3 */

  /* set debugging status */
#if DEBUG_ON
  g_PDF_debug = dbg[DPD].on[D] || dbg[DPD].on[DD] || dbg[DPD].on[DDD];
#else
  g_PDF_debug = FALSE;
#endif

#if PDF_COMPRESSION
  g_apply_compression = !g_PDF_debug;
#endif

  /* objects */
  g_next_objnum = 1; /* object numbers start at one */
  g_obj_offset_list = NULL;
  g_cur_obj_offset_block = NULL;

  /* fonts */
  g_font_list = NULL;
  g_font_encoding_list = NULL;

  /* pages */
  g_page_count = 0;
  g_page_block_list = NULL;
  g_cur_page_block = NULL;
  g_pages_root = PDFObject_New(/* in_fp */);

  /* doc */
  g_doc_h_bound = in_h_bound;
  g_doc_v_bound = in_v_bound;
  g_doc_author = NULL;
  g_doc_title = NULL;
  g_doc_subject = NULL;
  g_doc_keywords = NULL;

  /* link annotations */
  g_target_annot_list = NULL;
  g_has_exported_targets = FALSE;
  g_source_annot_list = NULL;

  /* units */
  g_units[k_in] = in_IN;
  g_units[k_cm] = in_CM;
  g_units[k_pt] = in_PT;
  g_units[k_em] = in_EM;

}


/*****************************************************************************/
/*                                                                           */
/*  void PDFFile_WritePagesObject(FILE* in_fp)                               */
/*                                                                           */
/*  Cleans up processing after all pages has been written out.               */
/*                                                                           */
/*****************************************************************************/

static void PDFFile_WritePagesObject(FILE* in_fp)
{
  unsigned int  i;
  t_page_block_ptr the_block  = g_page_block_list;

  if (g_PDF_debug)
    fprintf(in_fp, "%%\n%% root of pages tree:\n%%\n");

  /* write out the root of the Pages tree */
  PDFObject_WriteObj(in_fp, g_pages_root);
  fputs("<<\n", in_fp);
  fputs("/Type /Pages\n", in_fp);
  fputs("/Kids [ ", in_fp);
  for (i = 0; i < g_page_count; i++)
  {
    int block_pos = i % kNumberOfPagesPerBlock;
    PDFObject_WriteRef(in_fp, the_block->m_block[block_pos]);
    if (block_pos == (kNumberOfPagesPerBlock - 1))
    {
      the_block = the_block->m_next_block;
      /* Assert(the_block != NULL, no_fpos);  not always true! */
    }
    fputs(" ", in_fp);
  }
  fprintf(in_fp, " ]\n/Count %u\n", g_page_count);
  /* ***
  fprintf(in_fp, "/MediaBox [ 0 0 612 792 ]\n");
  fprintf(in_fp, "/MediaBox [ 0 0 %u %u ]\n",g_doc_h_bound,g_doc_v_bound);
  *** */
  fprintf(in_fp, "/MediaBox [ 0 0 %u %u ]\n", g_doc_h_bound, g_doc_v_bound);
  fputs(">>\nendobj\n", in_fp);
}


/*****************************************************************************/
/*                                                                           */
/*  PDF_FILE_OFFSET PDFFile_WriteXREF(FILE* in_fp)                           */
/*                                                                           */
/*  Writes out the XREF table.                                               */
/*                                                                           */
/*****************************************************************************/

static PDF_FILE_OFFSET PDFFile_WriteXREF(FILE* in_fp)
{
  int i;
  PDF_FILE_OFFSET xref_start;
  t_offset_block_ptr the_block = g_obj_offset_list;

  if (g_PDF_debug)
    fprintf(in_fp, "%%\n%% xref table:\n%%\n");

  xref_start = ftell(in_fp);
  fputs("xref\n", in_fp);
  fprintf(in_fp, "0 %u\n", g_next_objnum);
  fputs("0000000000 65535 f \n", in_fp);	/* object 0 is a deleted obj */
  Assert( (g_next_objnum == 1) || (the_block != NULL), no_fpos);
  for (i = 1; i < g_next_objnum; i++)	/* write out list of object offsets */
  {
    int block_pos = (i - 1) % kNumberOfObjectsPerBlock;

    /* always write an entry (even if the object doesn't exist) */
    fprintf(in_fp, "%010u 00000 n \n", the_block->m_block[block_pos]);

    if (the_block->m_block[block_pos] == 0)
    {
      t_tempbuf str;

      strcpy(str, "PDFFile_WriteXREF: undefined object number: ");
      sprintf(str + strlen(str), "%u", i);
      Error(48, 39, "%s", WARN, no_fpos, str);
    }

    if (block_pos == (kNumberOfObjectsPerBlock - 1))
    {
      the_block = the_block->m_next_block;
      /* Assert(the_block != NULL, no_fpos);  not always true! */
    }
  }
  return xref_start;
}

/*****************************************************************************/
/*                                                                           */
/*  void PDFFile_Cleanup(FILE* in_fp)                                        */
/*                                                                           */
/*  Cleans up processing after all pages has been written out.               */
/*                                                                           */
/*****************************************************************************/

void PDFFile_Cleanup(FILE* in_fp)
{
  PDF_FILE_OFFSET xref_start;	/* file offset of start of xref table */
  PDF_OBJECT_NUM catalog_obj_num;
  PDF_OBJECT_NUM info_obj_num;
  PDF_OBJECT_NUM dests_obj_num;

  /* write out any unresolved link annotations.  This could be done earlier  */
  /* (in fact, it can be done as each new target is defined) but I've        */
  /* arbitrarily decided to do it here.                                      */
  {
    t_source_annot_entry_ptr source = g_source_annot_list;

    while (source != NULL)
    {
      t_target_annot_entry_ptr target;

      Assert(source->m_target == NULL, no_fpos);
      target = PDFTargetAnnot_Find(source->m_name);
      if (target != NULL)
      {
	source->m_target = target;
	PDFSourceAnnot_Write(in_fp, source);
      }
      source = source->m_next_entry;
    }
  }

  /* write out pages object */
  PDFFile_WritePagesObject(in_fp);

  /* if file has exported targets for links then write out /Dests dictionary */
  if (g_has_exported_targets)
  {
    t_target_annot_entry_ptr entry = g_target_annot_list;

    Assert(entry != NULL, no_fpos);	/* should be at least an entry! */

    /* write PDF 1.1 style /Dests dictionary */
    if (g_PDF_debug)
      fprintf(in_fp, "%%\n%% /Dests dictionary (exported links):\n%%\n");

    dests_obj_num = PDFObject_WriteNewObj(in_fp);
    fputs("<<\n", in_fp);

    while (entry != NULL)
    {
      if (entry->m_for_export)
      {
	fprintf(in_fp, "/%s [ ", entry->m_name);
	PDFObject_WriteRef(in_fp, entry->m_page_object_num);
	fprintf(in_fp, " /XYZ null null null ]\n");
      }
      entry = entry->m_next_entry;
    }
    fputs(">>\nendobj\n", in_fp);
  }

  /* write out catalog object */
  if (g_PDF_debug)
    fprintf(in_fp, "%%\n%% catalog:\n%%\n");

  catalog_obj_num = PDFObject_WriteNewObj(in_fp);
  fputs("<<\n", in_fp);
  fputs("/Type /Catalog\n", in_fp);
  fputs("/Pages ", in_fp);
  PDFObject_WriteRef(in_fp, g_pages_root);
  fputs("\n", in_fp);

  /* if file has exported targets for links then write out a /Dest dictionary */
  if (g_has_exported_targets)
  {
    fputs("/Dests ", in_fp);
    PDFObject_WriteRef(in_fp, dests_obj_num);
    fputs("\n", in_fp);
  }

  /* ***
  fputs("/PageMode ", in_fp);
  switch ()
  {
  }
  fputs("\n", in_fp);
  *** */

  fputs(">>\nendobj\n", in_fp);

  /* write out info object */
  if (g_PDF_debug)
    fprintf(in_fp, "%%\n%% Info object:\n%%\n");

  /* ***
  Author string (Optional) The name of the person who created the document.
  CreationDate Date (Optional) The date the document was created.
  ModDate Date (Optional) The date the document was last modified.
  Creator string (Optional) If the document was converted into a PDF document from another
  form, this is the name of the application that created the original document.
  Producer string (Optional) The name of the application that converted the document from its native
  format to PDF.
  Title string (Optional) The document’s title.
  Subject string (Optional) The subject of the document.
  Keywords string (Optional) Keywords associated with the document.

  example:

  /Creator (Adobe Illustrator)
  /CreationDate (D:19930204080603-08'00')
  /Author (Werner Heisenberg)
  /Producer (Acrobat Network Distiller 1.0 for Macintosh)
  *** */

  info_obj_num = PDFObject_WriteNewObj(in_fp);
  fputs("<<\n", in_fp);

  fprintf(in_fp, "/Creator (%s)\n", LOUT_VERSION);
  fprintf(in_fp, "/Producer (%s)\n", LOUT_VERSION);

  {
    time_t now;
    struct tm *date;

    /* I will presume that localtime() is Y2K compliant.  If it isn't    */
    /* on your system, feel free to tweak this code. :-)                 */

    now = time( NULL );
    date = localtime( &now );
    fprintf(in_fp, "/CreationDate (Sometime Today)\n");
  }

  if (g_doc_author != NULL)
    fprintf(in_fp, "/Author (%s)\n", g_doc_author);

  if (g_doc_title != NULL)
    fprintf(in_fp, "/Title (%s)\n", g_doc_title);

  if (g_doc_subject != NULL)
    fprintf(in_fp, "/Subject (%s)\n", g_doc_subject);

  if (g_doc_keywords != NULL)
    fprintf(in_fp, "/Keywords (%s)\n", g_doc_keywords);

  fputs(">>\nendobj\n", in_fp);

  /* write out xref table */
  xref_start = PDFFile_WriteXREF(in_fp);

  /* write out trailer */
  /* *** uwe: comments can appear in the body only.
  if (g_PDF_debug)
    fprintf(in_fp, "%%\n%% trailer:\n%%\n");
  *** */

  fputs("trailer\n<<\n", in_fp);
  fprintf(in_fp, "/Size %u\n", g_next_objnum);
  fputs("/Root ", in_fp);
  PDFObject_WriteRef(in_fp, catalog_obj_num);
  fputs("\n/Info ", in_fp);
  PDFObject_WriteRef(in_fp, info_obj_num);

  fprintf(in_fp, " >>\nstartxref\n%u\n", xref_start);
  fputs("%%EOF\n", in_fp);

  /* memory deallocation (no need to dispose of the qsave_marking_stack  */
  /* because it's always empty after a page has been processed)          */

  while (g_obj_offset_list != NULL)
  {
    t_offset_block_ptr the_block = g_obj_offset_list;
    g_obj_offset_list = the_block->m_next_block;
    free(the_block);
  }

  while (g_font_encoding_list != NULL)
  {
    t_font_encoding_entry_ptr the_block = g_font_encoding_list;
    g_font_encoding_list = the_block->m_next_entry;
    free(the_block->m_font_encoding);
    free(the_block);
  }

  while (g_font_list != NULL)
  {
    t_font_list_entry_ptr the_block = g_font_list;
    g_font_list = the_block->m_next_font_entry;
    free(the_block->m_PDF_font_name);
    free(the_block->m_short_font_name);
    free(the_block->m_actual_font_name);
    free(the_block);
  }

  while (g_page_block_list != NULL)
  {
    t_page_block_ptr the_block = g_page_block_list;
    g_page_block_list = the_block->m_next_block;
    free(the_block);
  }

  while (g_source_annot_list != NULL)
  {
    t_source_annot_entry_ptr entry = g_source_annot_list;

    if (entry->m_target == NULL)
    {
      t_tempbuf str;
      strcpy(str, "PDFFile_Cleanup: unresolved link annotation named ");
      strcat(str, (char*) entry->m_name);
      Error(48, 40, "%s", WARN, no_fpos, str);
    }

    g_source_annot_list = PDFSourceAnnot_Dispose(entry);
  }

  while (g_target_annot_list != NULL)
  {
    t_target_annot_entry_ptr entry = g_target_annot_list;
    g_target_annot_list = entry->m_next_entry;
    free(entry->m_name);
    free(entry);
  }

#ifdef _CALC_LARGEST_PAGE_OBJECT_
  /* display largest page object */
  {
    t_tempbuf str;
    /* JK sprintf(str, "The largest page object is %u bytes long.", g_max_page_length); */
    Error(48, 41, "The largest page object is %u bytes long.", WARN, no_fpos, g_max_page_length);
  }
#endif
}
