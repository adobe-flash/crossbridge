/*@z50.c:PDF Back End:PDF_BackEnd@********************************************/
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
/*  FILE:         z49.c                                                      */
/*  MODULE:       PDF Back End                                               */
/*  EXTERNS:      PDF_BackEnd                                                */
/*                                                                           */
/*****************************************************************************/
#include <math.h>			/* for fabs()                        */
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  State variables for this module                                          */
/*                                                                           */
/*****************************************************************************/
#define	NO_FONT		0		/* actually stolen from z37.c        */
#define	NO_COLOUR	0
#define	MAX_GS		50		/* maximum depth of graphics states  */

static FILE		*out_fp;	/* file to print PDF on              */

typedef struct
{
  FONT_NUM	gs_font;		/* font number of this state         */
  COLOUR_NUM	gs_colour;		/* colour number of this state       */
  BOOLEAN	gs_cpexists;		/* TRUE if a current point exists    */
  FULL_LENGTH	gs_currenty;		/* if cpexists, its y coordinate     */
  short		gs_xheight2;		/* of font exists, half xheight      */
} GRAPHICS_STATE;

static GRAPHICS_STATE	gs_stack[MAX_GS];/* graphics state stack             */
static int		gs_stack_top;	/* top of graphics state stack       */

static FONT_NUM		currentfont;	/* font of most recent atom          */
static COLOUR_NUM	currentcolour;	/* colour of most recent atom        */
static short		currentxheight2;/* half xheight in current font      */
static BOOLEAN		cpexists;	/* true if a current point exists    */
static FULL_LENGTH	currenty;	/* if cpexists, its y coordinate     */

static int		wordcount;	/* atoms printed since last newline  */
static int		pagecount;	/* total number of pages printed     */
static BOOLEAN		prologue_done;	/* TRUE after prologue is printed    */
static OBJECT		needs;		/* Resource needs of included EPSFs  */
static OBJECT		supplied;	/* Resources supplied by this file   */


/*****************************************************************************/
/*                                                                           */
/*  Print a number x on file fp.                                             */
/*                                                                           */
/*****************************************************************************/

#define printnum(x, fp)							\
{ char buff[20];  register int i, y;					\
  if( x >= 0 )  y = x;							\
  else { y = -x; putc(CH_MINUS, fp); }					\
  i = 0;								\
  do { buff[i++] = numtodigitchar(y % 10);				\
     } while( (y = (y / 10)) > 0 );					\
  do { --i; putc(buff[i], fp);						\
     } while( i );							\
}


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintInitialize(FILE *fp)                                            */
/*                                                                           */
/*  Initialize this module; fp is the output file.                           */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintInitialize(FILE *fp)
{
  debug0(DPF, DD, "PDF_PrintInitialize(fp)");
  out_fp = fp;
  prologue_done = FALSE;
  gs_stack_top = -1;
  currentfont = NO_FONT;
  currentcolour = NO_COLOUR;
  cpexists = FALSE;
  wordcount = pagecount = 0;
  New(needs, ACAT);
  New(supplied, ACAT);
  debug0(DPF, DD, "PDF_PrintInitialize returning.");
} /* end PDF_PrintInitialize */


/*****************************************************************************/
/*                                                                           */
/*  void PDF_PrintLength(FULL_CHAR *buff, int length, int length_dim)        */
/*                                                                           */
/*  Print a length (debugging only)                                          */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintLength(FULL_CHAR *buff, int length, int length_dim)
{
  sprintf( (char *) buff, "%.3fc", (float) length/CM);
}


/*****************************************************************************/
/*                                                                           */
/*  void PDF_PrintPageSetupForFont(OBJECT face, int font_curr_page,          */
/*    FULL_CHAR *font_name, FULL_CHAR *first_size_str)                       */
/*                                                                           */
/*  Print the page setup commands required to use a font on some page:       */
/*                                                                           */
/*    face            The font face record, defining which font we need      */
/*    font_curr_page  The current page number                                */
/*    fp              The file to print the command(s) on                    */
/*    font_name       The name of the font                                   */
/*    first_size_str  No idea, have to check                                 */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintPageSetupForFont(OBJECT face, int font_curr_page,
  FULL_CHAR *font_name, FULL_CHAR *first_size_str)
{
  FULL_CHAR *enc = NULL;
  fprintf(out_fp, "%%%%IncludeResource: font %s\n", font_name);
  /***
  PDFFont_AddFont(out_fp, first_size_str, font_name,
    MapEncodingName(font_mapping(face)));
  ***/
  if (font_recoded(face)) {
    MAPPING m = font_mapping(face);
    /* This is a NASTY hack.  Need to rework the interface Since
       PDF is random-access format - we don't care which page this
       encoding is for and we need to only print it once -- Uwe */
    MapEnsurePrinted(m, 1);
    enc = MapEncodingName(m);
  }
  PDFFont_AddFont(out_fp, first_size_str, font_name, enc);
} /* end PDF_PrintPageSetupForFont */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintPageResourceForFont(FULL_CHAR *font_name, BOOLEAN first)        */
/*                                                                           */
/*  Print page resource info on file fp for font font_name; first is true    */
/*  if this is the first resource on this page.                              */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintPageResourceForFont(FULL_CHAR *font_name, BOOLEAN first)
{
  /* JK: this was always commented out */
  /* PDFWriteFontResource(out_fp, font_name); */
} /* end PDF_PrintPageResourceForFont */


/*****************************************************************************/
/*                                                                           */
/*  static void PDF_PrintMapping(MAPPING m)                                  */
/*                                                                           */
/*  Print mapping m.                                                         */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintMapping(MAPPING m)
{ MAP_VEC map = MapTable[m]; int i;
  PDFFile_BeginFontEncoding(out_fp, (char*) string(map->name));
  for( i = 0;  i < MAX_CHARS;  i++ )
    fprintf(out_fp, "/%s%c", string(map->vector[i]), (i+1)%8 != 0 ? ' ' : '\n');
  PDFFile_EndFontEncoding(out_fp);
} /* end PDF_PrintMapping */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintBeforeFirstPage(h, v, label)                                    */
/*                                                                           */
/*  This procedure is called just before starting to print the first         */
/*  component of the output.  Its size is h, v, and label is the page        */
/*  label to attach to the %%Page comment.                                   */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintBeforeFirstPage(FULL_LENGTH h, FULL_LENGTH v,
  FULL_CHAR *label)
{
  debug2(DPF, DD, "PrintBeforeFirst(%d, %d)", h, v);
  PDFFile_Init(out_fp, h/PT, v/PT, IN, CM, PT, EM);
  FontPrintPageSetup(out_fp);
  PDFPage_Init(out_fp, 1.0 / PT, PT/2);
  FontPrintPageResources(out_fp);	/*	write out font objects	*/
  FontAdvanceCurrentPage();
  prologue_done = TRUE;
} /* end PDF_PrintBeforeFirstPage */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintBetweenPages(h, v, label)                                       */
/*                                                                           */
/*  Start a new output component, of size h by v; label is the page label    */
/*  to attach to the %%Page comment.                                         */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintBetweenPages(FULL_LENGTH h, FULL_LENGTH v,
  FULL_CHAR *label)
{
  debug2(DPF, DD, "PrintBetween(%d, %d)", h, v);

  /* write out page objects	*/
  PDFPage_Cleanup(out_fp);
  PDFPage_Init(out_fp, 1.0 / PT, PT/2);

  /* write out font objects	*/
  FontPrintPageResources(out_fp);
  FontPrintPageSetup(out_fp);
  FontAdvanceCurrentPage();
} /* end PDF_PrintBetweenPages */


/*****************************************************************************/
/*                                                                           */
/*  KernLength(fnum, ch1, ch2, res)                                          */
/*                                                                           */
/*  Set res to the kern length between ch1 and ch2 in font fnum, or 0 if     */
/*  none.                                                                    */
/*                                                                           */
/*****************************************************************************/

#define KernLength(fnum, mp, ch1, ch2, res)				\
{ int ua_ch1 = mp[ch1];							\
  int ua_ch2 = mp[ch2];							\
  int i, j;								\
  i = finfo[fnum].kern_table[ua_ch1], j;				\
  if( i == 0 )  res = 0;						\
  else									\
  { FULL_CHAR *kc = finfo[fnum].kern_chars;				\
    for( j = i;  kc[j] > ua_ch2;  j++ );				\
    res = (kc[j] == ua_ch2) ?						\
      finfo[fnum].kern_sizes[finfo[fnum].kern_value[j]] : 0;		\
  }									\
} /* end KernLength */


/*****************************************************************************/
/*                                                                           */
/*  static void PrintComposite(COMPOSITE *cp, BOOLEAN outline, FILE *fp)     */
/*                                                                           */
/*  This routine is unused in this module because it is the PostScript       */
/*  version and no PDF version has been written so far.  JeffK 2/5/00.       */
/*                                                                           */
/*  Print composite character cp, assuming that the current point is         */
/*  set to the correct origin.  If outline is true, we want to print the     */
/*  composite character in outline.                                          */
/*                                                                           */
/*****************************************************************************/

static void PrintComposite(COMPOSITE *cp, BOOLEAN outline, FILE *fp)
{ debug1(DPF, D, "PrintComposite(cp, %s, fp)", bool(outline));
  while( cp->char_code != '\0' )
  {
    debug4(DPF, D, "  cp = %d printing code %d (%d, %d)", (int) cp,
      cp->char_code, cp->x_offset, cp->y_offset);
    fprintf(fp, "%d %d (%c)%s ", cp->x_offset, cp->y_offset,
      cp->char_code, outline ? "co" : "c");
    cp++;
  }
} /* end PrintComposite */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintWord(x, hpos, vpos)                                             */
/*                                                                           */
/*  Print non-empty word x; its marks cross at the point (hpos, vpos).       */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintWord(OBJECT x, int hpos, int vpos)
{ FULL_CHAR *p, *q, *a, *b, *lig, *unacc;
  int ksize;  char *command;  MAPPING m;
  unsigned short *composite; COMPOSITE *cmp; /* currently unused - JeffK */
  static int last_hpos;	/* does not need to be initialised */
  static int next_hpos = -1;
#if 0
  struct metrics *fnt;
#endif

  debug6(DPF, DD, "PrintWord( %s, %d, %d ) font %d colour %d%s", string(x),
    hpos, vpos, word_font(x), word_colour(x),
    word_outline(x) ? " outline" : "");
  TotalWordCount++;

  /* if font is different to previous word then print change */
  if( word_font(x) != currentfont )
  { currentfont = word_font(x);
    currentxheight2 = FontHalfXHeight(currentfont);
    PDFFont_Set(out_fp, FontSize(currentfont, x), FontName(currentfont));
  }

  /* if colour is different to previous word then print change */
  if( word_colour(x) != currentcolour )
  {
    currentcolour = word_colour(x);
    if( currentcolour > 0 )
    { char str[256];
      sprintf(str, "%s ", ColourCommand(currentcolour));
      PDFPage_Write(out_fp, str);
    }
  }

  /* move to coordinate of x */
  debug1(DPF, DDD, "  currentxheight2 = %d", currentxheight2);
  vpos = vpos - currentxheight2;
  if( cpexists && (currenty == vpos) && PDFHasValidTextMatrix() )
  { /* printnum(hpos, out_fp); */
    command = "s";

    /* Note: I calculate the width of the space char here in case the
       font has changed. This prevents subtle spacing errors.  */
#if 0
    fnt = finfo[currentfont].size_table;
    if( (next_hpos + fnt[' '].right /* width of space char */ ) == hpos )
      command = " ";
#endif
  }
  else
  { currenty = vpos;
    /* printnum(hpos, out_fp);
    fputs(" ", out_fp);
    printnum(currenty, out_fp); */
    command = "m";
    cpexists = TRUE;
  }

  /* convert ligature sequences into ligature characters */
  lig = finfo[word_font(x)].lig_table;
  p = q = string(x);
  do
  { 
    /* check for missing glyph (lig[] == 1) or ligatures (lig[] > 1) */
    if( lig[*q++ = *p++] )
    {
      if( lig[*(q-1)] == 1 ) continue;
      else
      {	a = &lig[ lig[*(p-1)] + MAX_CHARS ];
	while( *a++ == *(p-1) )
	{ b = p;
	  while( *a == *b && *(a+1) != '\0' && *b != '\0' )  a++, b++;
	  if( *(a+1) == '\0' )
	  { *(q-1) = *a;
	    p = b;
	    break;
	  }
	  else
	  { while( *++a );
	    a++;
	  }
	}
      }
    }
  } while( *p );
  *q = '\0';

  switch (command[0])
  {
    case 'm':

      PDFText_OpenXY(out_fp, hpos, vpos);
      last_hpos = hpos;
      next_hpos = hpos + fwd(x, COLM);	/* fwd(x, COLM) = width of wd */
      break;


    case 's':
#if 0
      PDFText_Open(out_fp);
      PDFText_Kern(out_fp, hpos - next_hpos);
#else
      PDFText_OpenX(out_fp, hpos - last_hpos);
#endif
      last_hpos = hpos;
      next_hpos = hpos + fwd(x, COLM);	/* fwd(x, COLM) = width of wd */
      break;
#if 0


    case ' ':

      PDFText_Open(out_fp);
#if 1
      /* try kerning to get correct position */
      PDFText_Kern(out_fp, fnt[' '].right);
#else
      PDFPage_Write(out_fp, EightBitToPrintForm[' ']);
#endif
      next_hpos += fwd(x, COLM) + fnt[' '].right;	/* width of space ch */
      break;
#endif

  }

  p = string(x);
  PDFPage_Write(out_fp, EightBitToPrintForm[*p]);

  m = font_mapping(finfo[word_font(x)].font_table);
  unacc = MapTable[m]->map[MAP_UNACCENTED];
  /* acc   = MapTable[m]->map[MAP_ACCENT]; */
  for( p++;  *p;  p++ )
  {
    /* *** this seems right but is actually wrong for PDF,
    which according to Uwe uses original units for kerning
    KernLength(word_font(x), unacc, *(p-1), *p, ksize);
    *** */
    KernLength(font_num(finfo[word_font(x)].original_face),
      unacc, *(p-1), *p, ksize);
    if ( ksize != 0 )
    {
      PDFText_Kern(out_fp, ksize);
    }
    PDFPage_Write(out_fp, EightBitToPrintForm[*p]);
  }
  PDFText_Close(out_fp);

  debug0(DPF, DDD, "PDF_PrintWord returning");
} /* end PDF_PrintWord */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintPlainGraphic(OBJECT x, FULL_LENGTH xmk, ymk, OBJECT z)          */
/*                                                                           */
/*  Print a plain graphic object                                             */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintPlainGraphic(OBJECT x, FULL_LENGTH xmk,
  FULL_LENGTH ymk, OBJECT z)
{
  assert(FALSE, "PDF_PrintPlainGraphic: this routine should never be called!");
} /* end PDF_PrintPlainGraphic */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintUnderline(fnum, col, xstart, xstop, ymk)                        */
/*                                                                           */
/*  Draw an underline suitable for font fnum, in colour col from xstart to   */
/*  xstop at the appropriate distance below mark ymk.                        */
/*                                                                           */
/*****************************************************************************/

static void PDF_PrintUnderline(FONT_NUM fnum, COLOUR_NUM col,
  FULL_LENGTH xstart, FULL_LENGTH xstop, FULL_LENGTH ymk)
{
  debug5(DPF, DD, "PDF_PrintUnderline(ft %d, co %d, xstrt %s, xstp %s, ymk %s)",
    fnum, col, EchoLength(xstart), EchoLength(xstop), EchoLength(ymk));
  PDFPage_PrintUnderline(out_fp,  xstart, xstop,
    ymk - finfo[fnum].underline_pos, finfo[fnum].underline_thick);
  debug0(DPF, DD, "PrintUnderline returning.");
} /* end PDF_PrintUnderline */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintAfterLastPage()                                                 */
/*                                                                           */
/*  Clean up this module and close output stream.                            */
/*                                                                           */
/*****************************************************************************/

void PDF_PrintAfterLastPage(void)
{
  if( prologue_done )
  { 
    PDFPage_Cleanup(out_fp);		/* write out page objects */
    /* MapPrintResources(out_fp); not needed */
    PDFFile_Cleanup(out_fp);
  }
} /* end PDF_PrintAfterLastPage */


/*****************************************************************************/
/*                                                                           */
/*  PDF_CoordTranslate(xdist, ydist)                                         */
/*                                                                           */
/*  Translate coordinate system by the given x and y distances.              */
/*                                                                           */
/*****************************************************************************/

static void PDF_CoordTranslate(FULL_LENGTH xdist, FULL_LENGTH ydist)
{ debug2(DPF, D, "CoordTranslate(%s, %s)",
  EchoLength(xdist), EchoLength(ydist));
  if ((xdist != 0) || (ydist != 0))
  {
#if 1
    PDFPage_Translate(out_fp, xdist, ydist);
#else
    char	temp_str[64];
    sprintf(temp_str, "1 0 0 1 %d %d cm\n", xdist, ydist);
    PDFPage_Write(out_fp, temp_str);
#endif
  }
  cpexists = FALSE;
  debug0(DPF, D, "PDF_CoordTranslate returning.");
} /* end PDF_CoordTranslate */


/*****************************************************************************/
/*                                                                           */
/*  PDF_CoordRotate(amount)                                                  */
/*                                                                           */
/*  Rotate coordinate system by given amount (in internal DG units)          */
/*                                                                           */
/*****************************************************************************/
#define PI 3.1415926535897931160

static void PDF_CoordRotate(FULL_LENGTH amount)
{ int theAmount;
  debug1(DPF, D, "PDF_CoordRotate(%.1f degrees)", (float) amount / DG);
  theAmount = ((amount / DG) % 360);
  if( theAmount != 0 )
    PDFPage_Rotate(out_fp, (double) theAmount * (double) PI / (double) 180.0);
  cpexists = FALSE;
  debug0(DPF, D, "CoordRotate returning.");
} /* end PDF_CoordRotate */


/*****************************************************************************/
/*                                                                           */
/*  PDF_CoordScale(ratio, dim)                                               */
/*                                                                           */
/*  Scale coordinate system by ratio in the given dimension.                 */
/*                                                                           */
/*****************************************************************************/

static void PDF_CoordScale(float hfactor, float vfactor)
{
#if DEBUG_ON
  char buff[20];
#endif
  ifdebug(DPF, D, sprintf(buff, "%.3f, %.3f", hfactor, vfactor));
  debug1(DPF, D, "CoordScale(%s)", buff);
  if ( (fabs(hfactor - 1.0) > 0.01) || (fabs(vfactor - 1.0) > 0.01) )
  {
#if 1
    PDFPage_Scale(out_fp, hfactor, vfactor);
#else
    char temp_str[64];
    sprintf(temp_str, "%.2f 0 0 %.2f 0 0 cm\n", hfactor, vfactor);
    PDFPage_Write(out_fp, temp_str);
#endif
  }
  cpexists = FALSE;
  debug0(DPF, D, "CoordScale returning.");
} /* end PDF_CoordScale */


/*****************************************************************************/
/*                                                                           */
/*  PDF_SaveGraphicState(x)                                                  */
/*                                                                           */
/*  Save current coord system on stack for later restoration.                */
/*  Object x is just for error reporting, not really used at all.            */
/*                                                                           */
/*****************************************************************************/

void PDF_SaveGraphicState(OBJECT x)
{ debug0(DPF, D, "PDF_SaveGraphicState()");
  PDFPage_Push(out_fp);
  gs_stack_top++;
  if( gs_stack_top >= MAX_GS )
    Error(50, 1, "rotations, graphics etc. too deeply nested (max is %d)",
      FATAL, &fpos(x), MAX_GS);
  gs_stack[gs_stack_top].gs_font	= currentfont;
  gs_stack[gs_stack_top].gs_colour	= currentcolour;
  gs_stack[gs_stack_top].gs_cpexists	= cpexists;
  gs_stack[gs_stack_top].gs_currenty	= currenty;
  gs_stack[gs_stack_top].gs_xheight2	= currentxheight2;
  debug0(DPF, D, "PDF_SaveGraphicState returning.");
} /* end PDF_SaveGraphicState */


/*****************************************************************************/
/*                                                                           */
/*  PDF_RestoreGraphicState()                                                */
/*                                                                           */
/*  Restore previously saved coordinate system.                              */
/*                                                                           */
/*  The following note probably only applies to the PostScript back end      */
/*  but I have not looked into this issue myself:                            */
/*                                                                           */
/*                                               NB we normally assume that  */
/*  no white space is needed before any item of output, but since this       */
/*  procedure is sometimes called immediately after PrintGraphicObject(),    */
/*  which does not append a concluding space, we prepend one here.           */
/*                                                                           */
/*****************************************************************************/

void PDF_RestoreGraphicState(void)
{ debug0(DPF, D, "PDF_RestoreGraphicState()");
  PDFPage_Pop(out_fp);
  currentfont	  = gs_stack[gs_stack_top].gs_font;
  currentcolour	  = gs_stack[gs_stack_top].gs_colour;
  cpexists	  = gs_stack[gs_stack_top].gs_cpexists;
  currenty	  = gs_stack[gs_stack_top].gs_currenty;
  currentxheight2 = gs_stack[gs_stack_top].gs_xheight2;
  gs_stack_top--;
  debug0(DPF, D, "PDF_RestoreGraphicState returning.");
} /* end PDF_RestoreGraphicState */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintGraphicObject(x)                                                */
/*                                                                           */
/*  Print object x on out_fp                                                 */
/*                                                                           */
/*****************************************************************************/

void PDF_PrintGraphicObject(OBJECT x)
{ OBJECT y, link;
  debug3(DPF, D, "PDF_PrintGraphicObject(%s %s %s)",
    EchoFilePos(&fpos(x)), Image(type(x)), EchoObject(x));
  switch( type(x) )
  {
    case WORD:
    case QWORD:

      PDFPage_WriteGraphic(out_fp, string(x));
      break;
	

    case ACAT:
    
      for( link = Down(x);  link != x;  link = NextDown(link) )
      {	Child(y, link);
	if( type(y) == GAP_OBJ )
	{
	  if( vspace(y) > 0 )  PDFPage_Write(out_fp, "\n");
	  else if( hspace(y) > 0 ) PDFPage_Write(out_fp, " ");
	}
	else if( is_word(type(y)) || type(y) == ACAT )
	  PDF_PrintGraphicObject(y);
	else if( type(y) == WIDE || is_index(type(y)) )
	{
	  /* ignore: @Wide, indexes are sometimes inserted by Manifest */
	}
	else
	{ Error(50, 2, "error in left parameter of %s",
	    WARN, &fpos(x), KW_GRAPHIC);
	  debug1(DPF, D, "  type(y) = %s, y =", Image(type(y)));
	  ifdebug(DPF, D, DebugObject(y));
	}
      }
      break;


    default:
    
      Error(50, 3, "error in left parameter of %s", WARN, &fpos(x), KW_GRAPHIC);
      debug1(DPF, D, "  type(x) = %s, x =", Image(type(x)));
      ifdebug(DPF, D, DebugObject(x));
      break;

  }
  debug0(DPF, D, "PDF_PrintGraphicObject returning");
} /* end PDF_PrintGraphicObject */


/*****************************************************************************/
/*                                                                           */
/*  PDF_DefineGraphicNames(x)                                                */
/*                                                                           */
/*  Generate PostScript for xsize, ysize etc. names of graphic object.       */
/*                                                                           */
/*****************************************************************************/

void PDF_DefineGraphicNames(OBJECT x)
{ assert( type(x) == GRAPHIC, "PrintGraphic: type(x) != GRAPHIC!" );
  debug1(DPF, D, "DefineGraphicNames( %s )", EchoObject(x));
  debug1(DPF, DD, "  style = %s", EchoStyle(&save_style(x)));

  /* if font is different to previous word then print change */
  if( font(save_style(x)) != currentfont )
  { currentfont = font(save_style(x));
    if( currentfont > 0 )
    { currentxheight2 = FontHalfXHeight(currentfont);
      PDFFont_Set(out_fp, FontSize(currentfont, x), FontName(currentfont));
    }
  }

  /* if colour is different to previous word then print change */
  if( colour(save_style(x)) != currentcolour )
  { currentcolour = colour(save_style(x));
    if( currentcolour > 0 )
    { char str[256];
      sprintf(str, "%s ", ColourCommand(currentcolour));
      PDFPage_Write(out_fp, str);
    }
  }

  PDFPage_SetVars(size(x, COLM), size(x, ROWM), back(x, COLM), fwd(x, ROWM),
    currentfont <= 0 ? 12*PT : FontSize(currentfont, x),
    width(line_gap(save_style(x))), width(space_gap(save_style(x))));

  debug0(DPF, D, "PDF_DefineGraphicNames returning.");
} /* end PDF_DefineGraphicNames */


/*****************************************************************************/
/*                                                                           */
/*  PDF_SaveTranslateDefineSave(x, xdist, ydist)                             */
/*                                                                           */
/*  Equivalent to the sequence of calls                                      */
/*                                                                           */
/*      SaveGraphicState(x)                                                  */
/*      CoordTranslate(xdist, ydist)                                         */
/*      DefineGraphicNames(x)                                                */
/*      SaveGraphicState(x)                                                  */
/*                                                                           */
/*  but offers prospects for optimization (not taken up in PDF).             */
/*                                                                           */
/*****************************************************************************/

void PDF_SaveTranslateDefineSave(OBJECT x, FULL_LENGTH xdist, FULL_LENGTH ydist)
{
  PDF_SaveGraphicState(x);
  PDF_CoordTranslate(xdist, ydist);
  PDF_DefineGraphicNames(x);
  PDF_SaveGraphicState(x);
} /* end PDF_SaveTranslateDefineSave */


/*****************************************************************************/
/*                                                                           */
/*  PDF_PrintGraphicInclude(x, colmark, rowmark)                             */
/*                                                                           */
/*  Print graphic include file, with appropriate surrounds.                  */
/*                                                                           */
/*****************************************************************************/

void PDF_PrintGraphicInclude(OBJECT x, FULL_LENGTH colmark, FULL_LENGTH rowmark)
{ OBJECT y;
  debug0(DPF, D, "PDF_PrintGraphicInclude(x)");
  Child(y, Down(x));
  Error(50, 4, "cannot include EPS file in PDF output; EPS file %s ignored",
      WARN, &fpos(x), string(y));
  debug0(DPF, D, "PDF_PrintGraphicInclude returning.");
} /* end PDF_PrintGraphicInclude */


/*****************************************************************************/
/*                                                                           */
/*  PDF_LinkSource(name, llx, lly, urx, ury)                                 */
/*                                                                           */
/*  Print a link source point.                                               */
/*                                                                           */
/*****************************************************************************/

static void PDF_LinkSource(OBJECT name, FULL_LENGTH llx, FULL_LENGTH lly,
  FULL_LENGTH urx, FULL_LENGTH ury)
{
  debug5(DPF, D, "PDF_LinkSource(%s, %d, %d, %d, %d)", EchoObject(name),
    llx, lly, urx, ury);

  /* still to do */

  debug0(DPF, D, "PDF_LinkSource returning.");
} /* end PDF_LinkSource */


/*****************************************************************************/
/*                                                                           */
/*  PDF_LinkDest(name, llx, lly, urx, ury)                                   */
/*                                                                           */
/*  Print a link dest point.                                                 */
/*                                                                           */
/*  Still to do: check that the name has not been used by a previous         */
/*  dest point.                                                              */
/*                                                                           */
/*****************************************************************************/

static void PDF_LinkDest(OBJECT name, FULL_LENGTH llx, FULL_LENGTH lly,
  FULL_LENGTH urx, FULL_LENGTH ury)
{
  debug5(DPF, D, "PDF_LinkDest(%s, %d, %d, %d, %d)", EchoObject(name),
    llx, lly, urx, ury);

  /* still to do */

  debug0(DPF, D, "PDF_LinkDest returning.");
} /* end PDF_LinkDest */


/*****************************************************************************/
/*                                                                           */
/*  PDF_LinkCheck()                                                          */
/*                                                                           */
/*  Called at end of run; will check that for every link source point there  */
/*  is a link dest point.                                                    */
/*                                                                           */
/*****************************************************************************/

static void PDF_LinkCheck()
{
  debug0(DPF, D, "PDF_LinkCheck()");

  /* still to do */

  debug0(DPF, D, "PDF_LinkCheck returning.");
} /* end PDF_LinkCheck */


/*****************************************************************************/
/*                                                                           */
/*  PDF_BackEnd                                                              */
/*                                                                           */
/*  The record into which all of these functions are packaged.               */
/*                                                                           */
/*****************************************************************************/

static struct back_end_rec pdf_back = {
  PDF,					/* the code number of the back end   */
  STR_PDF,				/* string name of the back end       */
  TRUE,					/* TRUE if @Scale is available       */
  TRUE,					/* TRUE if @Rotate is available      */
  TRUE,					/* TRUE if @Graphic is available     */
  TRUE,					/* TRUE if @IncludeGraphic is avail. */
  FALSE,				/* TRUE if @PlainGraphic is avail.   */
  TRUE,					/* TRUE if fractional spacing avail. */
  TRUE,					/* TRUE if actual font metrics used  */
  TRUE,					/* TRUE if colour is available       */
  PDF_PrintInitialize,
  PDF_PrintLength,
  PDF_PrintPageSetupForFont,
  PDF_PrintPageResourceForFont,
  PDF_PrintMapping,
  PDF_PrintBeforeFirstPage,
  PDF_PrintBetweenPages,
  PDF_PrintAfterLastPage,
  PDF_PrintWord,
  PDF_PrintPlainGraphic,
  PDF_PrintUnderline,
  PDF_CoordTranslate,
  PDF_CoordRotate,
  PDF_CoordScale,
  PDF_SaveGraphicState,
  PDF_RestoreGraphicState,
  PDF_PrintGraphicObject,
  PDF_DefineGraphicNames,
  PDF_SaveTranslateDefineSave,
  PDF_PrintGraphicInclude,
  PDF_LinkSource,
  PDF_LinkDest,
  PDF_LinkCheck,
};

BACK_END PDF_BackEnd = &pdf_back;
