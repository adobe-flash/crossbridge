/*@z51.c:Plain Text Back End:Plain_BackEnd@***********************************/
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
/*  MODULE:       PostScript Back End                                        */
/*  EXTERNS:      PS_BackEnd                                                 */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  PlainCharWidth    the width of each character                            */
/*  PlainCharHeight   the height of each character                           */
/*  PlainFormFeed     TRUE if components to be separated by \f.              */
/*                                                                           */
/*****************************************************************************/

FULL_LENGTH PlainCharWidth, PlainCharHeight;
BOOLEAN PlainFormFeed;

/*****************************************************************************/
/*                                                                           */
/*  State variables for this module                                          */
/*                                                                           */
/*****************************************************************************/

static FILE		*out_fp;	/* file to print output on           */
static int		hsize;		/* horizontal size of page in chars  */
static int		vsize;		/* vertical size of page in chars    */
static FULL_CHAR	*page;		/* the page (two-dim array of chars) */
static BOOLEAN		prologue_done;	/* TRUE after prologue is printed    */


/*****************************************************************************/
/*                                                                           */
/*  void Plain_PrintInitialize(fp)                                           */
/*                                                                           */
/*  Initialize this module; fp is the output file.                           */
/*                                                                           */
/*****************************************************************************/

void Plain_PrintInitialize(FILE *fp)
{
  debug0(DPT, DD, "Plain_PrintInitialize(fp)");
  out_fp = fp;
  prologue_done = FALSE;
  debug0(DPT, DD, "Plain_PrintInitialize returning.");
} /* end Plain_PrintInitialize */


/*****************************************************************************/
/*                                                                           */
/*  void Plain_PrintLength(FULL_CHAR *buff, int length, int length_dim)      */
/*                                                                           */
/*  Print a length (debugging only)                                          */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintLength(FULL_CHAR *buff, int length, int length_dim)
{
  if( length_dim == COLM )
  {
    sprintf( (char *) buff, "%.2fs", (float) length/PlainCharWidth);
  }
  else
  {
    sprintf( (char *) buff, "%.2ff", (float) length/PlainCharHeight);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  void Plain_PrintPageSetupForFont(OBJECT face, int font_curr_page,        */
/*    FULL_CHAR *font_name, FULL_CHAR *first_size_str)                       */
/*                                                                           */
/*  Print the page setup commands required to use a font on some page:       */
/*                                                                           */
/*    face            The font face record, defining which font we need      */
/*    font_curr_page  The current page number                                */
/*    font_name       The name of the font                                   */
/*    first_size_str  No idea, have to check                                 */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintPageSetupForFont(OBJECT face, int font_curr_page,
  FULL_CHAR *font_name, FULL_CHAR *first_size_str)
{
  /* nothing to do here */

} /* end Plain_PrintPageSetupForFont */


/*****************************************************************************/
/*                                                                           */
/*  void Plain_PrintPageResourceForFont(FULL_CHAR *font_name, BOOLEAN first) */
/*    BOOLEAN first)                                                         */
/*                                                                           */
/*  Print page resource info on file fp for font font_name; first is true    */
/*  if this is the first resource on this page.                              */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintPageResourceForFont(FULL_CHAR *font_name, BOOLEAN first)
{
  /* nothing to do here */

} /* end Plain_PrintPageResourceForFont */


/*****************************************************************************/
/*                                                                           */
/*  static void Plain_PrintMapping(MAPPING m)                                */
/*                                                                           */
/*  Print mapping m.                                                         */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintMapping(MAPPING m)
{
  /* nothing to do here */

} /* end Plain_PrintMapping */


/*****************************************************************************/
/*                                                                           */
/*  void Plain_PrintBeforeFirstPage(h, v, label)                             */
/*                                                                           */
/*  This procedure is called just before starting to print the first         */
/*  component of the output.  Its size is h, v, and label is the page label. */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintBeforeFirstPage(FULL_LENGTH h, FULL_LENGTH v,
  FULL_CHAR *label)
{ int i, j;
  debug2(DPT, DD, "PrintBeforeFirst(%d, %d)", h, v);

  /* get a new page[] and clear it */
  hsize = ceiling(h, PlainCharWidth);
  vsize = ceiling(v, PlainCharHeight);
  debug2(DPT, DD, "  PlainCharWidth: %d;  PlainCharHeight: %d",
    PlainCharWidth, PlainCharHeight);
  ifdebug(DMA, D, DebugRegisterUsage(MEM_PAGES, 1,
    hsize * vsize * sizeof(FULL_CHAR)));
  debug2(DPT, DD, "  PrintBeforeFirst allocating %d by %d", hsize, vsize);
  page = (FULL_CHAR *) malloc(hsize * vsize * sizeof(FULL_CHAR));
  for( i = 0;  i < vsize;  i++ )
    for( j = 0;  j < hsize;  j++ )
      page[i*hsize + j] = ' ';
  prologue_done = TRUE;
} /* end Plain_PrintBeforeFirstPage */


/*****************************************************************************/
/*                                                                           */
/*  void Plain_PrintBetweenPages(h, v, label)                                */
/*                                                                           */
/*  Start a new output component, of size h by v; label is the page label    */
/*  to attach to the %%Page comment.                                         */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintBetweenPages(FULL_LENGTH h, FULL_LENGTH v,
  FULL_CHAR *label)
{ int new_hsize, new_vsize, i, j, jmax;
  debug2(DPT, DD, "PrintBetween(%d, %d)", h, v);

  /* print the page that has just ended */
  ifdebug(DPT, D,
    putc('+', out_fp);
    for( j = 0;  j < hsize;  j++ )  putc('-', out_fp);
    putc('+', out_fp);
    putc('\n', out_fp);
  );
  for( i = vsize - 1;  i >= 0;  i-- )
  { ifdebug(DPT, D, putc('|', out_fp));
    for( jmax = hsize-1;  jmax >= 0 && page[i*hsize+jmax] == ' ';  jmax--);
    ifdebug(DPT, D, jmax = hsize - 1);
    for( j = 0;  j <= jmax;  j++ )
      putc(page[i*hsize + j], out_fp);
    ifdebug(DPT, D, putc('|', out_fp));
    putc('\n', out_fp);
  }
  ifdebug(DPT, D,
    putc('+', out_fp);
    for( j = 0;  j < hsize;  j++ )  putc('-', out_fp);
    putc('+', out_fp);
    putc('\n', out_fp);
  );

  /* separate the page from the next one with a form-feed if required */
  if( PlainFormFeed ) putc('\f', out_fp);

  /* if page size has changed, get a new page[] array */
  new_hsize = ceiling(h, PlainCharWidth);
  new_vsize = ceiling(v, PlainCharHeight);
  if( new_hsize != hsize || new_vsize != vsize )
  {
    ifdebug(DMA, D, DebugRegisterUsage(MEM_PAGES, -1,
      -hsize * vsize * sizeof(FULL_CHAR)));
    free(page);
    hsize = new_hsize;
    vsize = new_vsize;
    debug2(DPT, DD, "  PrintBetween allocating %d by %d", hsize, vsize);
    ifdebug(DPT, D, DebugRegisterUsage(MEM_PAGES, 1,
      hsize * vsize * sizeof(FULL_CHAR)));
    page = (FULL_CHAR *) malloc(hsize * vsize * sizeof(FULL_CHAR));
  }

  /* clear page[] for the new page just beginning */
  for( i = 0;  i < vsize;  i++ )
    for( j = 0;  j < hsize;  j++ )
      page[i*hsize + j] = ' ';
} /* end Plain_PrintBetweenPges */


/*****************************************************************************/
/*                                                                           */
/*  Plain_PrintWord(x, hpos, vpos)                                           */
/*                                                                           */
/*  Print non-empty word x; its marks cross at the point (hpos, vpos).       */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintWord(OBJECT x, int hpos, int vpos)
{ FULL_CHAR *p;  int i, h, v;

  debug6(DPT, DD, "Plain_PrintWord( %s, %d, %d ) font %d colour %d%s",
    string(x), hpos, vpos, word_font(x), word_colour(x),
    word_outline(x) ? " outline" : "");
  TotalWordCount++;

  h = ((float) hpos / PlainCharWidth) + 0.5;
  v = ((float) vpos / PlainCharHeight);
  debug3(DPT, DD, "PrintWord(%s at h = %d, v = %d)", string(x), h, v);
  if( h >= 0 && h + StringLength(string(x)) < hsize && v >= 0 && v < vsize )
  {
    assert( h >= 0,     "PrintWord:  h < 0!" );
    assert( h <  hsize, "PrintWord:  h >= hsize!" );
    assert( v >= 0,     "PrintWord:  v < 0!" );
    assert( v <  vsize, "PrintWord:  v >= vsize!" );
    p = &page[v*hsize + h];
    for( i = 0;  string(x)[i] != '\0';  i++ )
      *p++ = string(x)[i];
  }
  else
    Error(51, 1, "word %s deleted (internal error, off page at %d,%d)",
      WARN, &fpos(x), string(x), h, v);
  debug0(DPT, DDD, "PrintWord returning");
} /* end Plain_PrintWord */


/*****************************************************************************/
/*                                                                           */
/*  Plain_PrintPlainGraphic(x, xmk, ymk, z)                                  */
/*                                                                           */
/*  Print plain graphic object x at xmk, ymk with the size of z.             */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintPlainGraphic(OBJECT x, FULL_LENGTH xmk,
  FULL_LENGTH ymk, OBJECT z)
{ int i, len, starth, startv, stoph, stopv, h, v;
  debug2(DPT, D, "Plain_PrintPlainGraphic(x, xmk %s, ymk %s)",
    EchoLength(xmk), EchoLength(ymk));

  if( type(x) != WORD && type(x) != QWORD )
  {
    Error(51, 2, "left parameter of %s must be a simple word",
      WARN, &fpos(x), KW_PLAINGRAPHIC);
    return;
  }
  len = StringLength(string(x));
  if( StringLength(string(x)) == 0 )
  {
    Error(51, 3, "left parameter of %s must be a non-empty word",
      WARN, &fpos(x), KW_PLAINGRAPHIC);
    return;
  }
  starth = (((float) xmk ) / PlainCharWidth) + 0.5;
  startv = (((float) ymk ) / PlainCharHeight);
  stoph = (((float) xmk + size(z, COLM)) / PlainCharWidth) + 0.5;
  stopv = (((float) ymk - size(z, ROWM)) / PlainCharHeight); /* NB - not + */
  SetLengthDim(COLM);
  debug5(DPT, D, "  xmk %s bk %s fwd %s -> %d,%d",
    EchoLength(xmk), EchoLength(back(z, COLM)), EchoLength(fwd(z, COLM)),
    starth, stoph);
  SetLengthDim(ROWM);
  debug5(DPT, D, "  ymk %s bk %s fwd %s -> %d,%d",
    EchoLength(ymk), EchoLength(back(z, ROWM)), EchoLength(fwd(z, ROWM)),
    startv, stopv);
  if( starth >= 0 && stoph < hsize && startv >= 0 && stopv < vsize )
  { i = 0;
    for( v = startv-1;  v >= stopv;  v-- )
    {
      for( h = starth;  h < stoph;  h++ )
      {
        if( i == len )  i = 0;
        page[v*hsize + h] = string(x)[i++];
      }
    }
  }
  else
  {
    Error(51, 4, "fill %s deleted (internal error, off page at %d,%d)",
      WARN, &fpos(x), string(x), h, v);
  }
} /* end Plain_PrintPlainGraphic */


/*****************************************************************************/
/*                                                                           */
/*  Plain_PrintUnderline(fnum, col, xstart, xstop, ymk)                      */
/*                                                                           */
/*  Draw an underline suitable for font fnum, in colour col, from xstart to  */
/*  xstop at the appropriate distance below mark ymk.                        */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintUnderline(FONT_NUM fnum, COLOUR_NUM col,
  FULL_LENGTH xstart, FULL_LENGTH xstop, FULL_LENGTH ymk)
{

  debug5(DPT, DD, "Plain_PrintUnderline(fnum %d, col %d, xstart %s, xstop %s, ymk %s )",
    fnum, col, EchoLength(xstart), EchoLength(xstop), EchoLength(ymk));

  /* do nothing */

  debug0(DPT, DD, "PrintUnderline returning.");
} /* end Plain_PrintUnderline */


/*****************************************************************************/
/*                                                                           */
/*  Plain_PrintAfterLastPage()                                               */
/*                                                                           */
/*  Clean up this module and close output stream.                            */
/*                                                                           */
/*****************************************************************************/

static void Plain_PrintAfterLastPage(void)
{ int i, j, jmax;
  if( prologue_done )
  { 
    /* print the page that has just ended (exists since prologue_done) */
    ifdebug(DPT, D,
      putc('+', out_fp);
      for( j = 0;  j < hsize;  j++ )  putc('-', out_fp);
      putc('+', out_fp);
      putc('\n', out_fp);
    );
    for( i = vsize - 1;  i >= 0;  i-- )
    { ifdebug(DPT, D, putc('|', out_fp));
      for( jmax = hsize-1;  jmax >= 0 && page[i*hsize+jmax] == ' ';  jmax--);
      ifdebug(DPT, D, jmax = hsize - 1);
      for( j = 0;  j <= jmax;  j++ )
        putc(page[i*hsize + j], out_fp);
      ifdebug(DPT, D, putc('|', out_fp));
      putc('\n', out_fp);
    }
    ifdebug(DPT, D,
      putc('+', out_fp);
      for( j = 0;  j < hsize;  j++ )  putc('-', out_fp);
      putc('+', out_fp);
      putc('\n', out_fp);
    );
  }
} /* end Plain_PrintAfterLastPage */


/*****************************************************************************/
/*                                                                           */
/*  Plain_CoordTranslate(xdist, ydist)                                       */
/*                                                                           */
/*  Translate coordinate system by the given x and y distances.              */
/*                                                                           */
/*****************************************************************************/

void Plain_CoordTranslate(FULL_LENGTH xdist, FULL_LENGTH ydist)
{ debug2(DPT, D, "Plain_CoordTranslate(%s, %s)",
    EchoLength(xdist), EchoLength(ydist));
  assert(FALSE, "Plain_CoordTranslate: should never be called!");
  debug0(DPT, D, "Plain_CoordTranslate returning.");
} /* end Plain_CoordTranslate */


/*****************************************************************************/
/*                                                                           */
/*  Plain_CoordRotate(amount)                                                */
/*                                                                           */
/*  Rotate coordinate system by given amount (in internal DG units)          */
/*                                                                           */
/*****************************************************************************/

static void Plain_CoordRotate(FULL_LENGTH amount)
{ debug1(DPT, D, "Plain_CoordRotate(%.1f degrees)", (float) amount / DG);
  assert(FALSE, "Plain_CoordRotate: should never be called!");
  debug0(DPT, D, "Plain_CoordRotate returning.");
} /* end Plain_CoordRotate */


/*****************************************************************************/
/*                                                                           */
/*  Plain_CoordScale(ratio, dim)                                             */
/*                                                                           */
/*  Scale coordinate system by ratio in the given dimension.                 */
/*                                                                           */
/*****************************************************************************/

void Plain_CoordScale(float hfactor, float vfactor)
{
  assert(FALSE, "Plain_CoordScale: should never be called!");
} /* end Plain_CoordScale */


/*****************************************************************************/
/*                                                                           */
/*  Plain_SaveGraphicState(x)                                                */
/*                                                                           */
/*  Save current coord system on stack for later restoration.                */
/*  Object x is just for error reporting, not really used at all.            */
/*                                                                           */
/*****************************************************************************/

void Plain_SaveGraphicState(OBJECT x)
{ debug0(DPT, D, "Plain_SaveGraphicState()");
  assert(FALSE, "Plain_SaveGraphicState: should never be called!" );
  debug0(DPT, D, "Plain_SaveGraphicState returning.");
} /* end Plain_SaveGraphicState */


/*****************************************************************************/
/*                                                                           */
/*  Plain_RestoreGraphicState()                                              */
/*                                                                           */
/*  Restore previously saved coordinate system.                              */
/*                                                                           */
/*****************************************************************************/

void Plain_RestoreGraphicState(void)
{ debug0(DPT, D, "Plain_RestoreGraphicState()");
  assert(FALSE, "Plain_RestoreGraphicState: should never be called!" );
  debug0(DPT, D, "Plain_RestoreGraphicState returning.");
} /* end Plain_RestoreGraphicState */


/*****************************************************************************/
/*                                                                           */
/*  Plain_PrintGraphicObject(x)                                              */
/*                                                                           */
/*  Print object x on out_fp                                                 */
/*                                                                           */
/*****************************************************************************/

void Plain_PrintGraphicObject(OBJECT x)
{
  debug3(DPT, D, "Plain_PrintGraphicObject(%s %s %s)",
    EchoFilePos(&fpos(x)), Image(type(x)), EchoObject(x));
  assert(FALSE, "Plain_PrintGraphicObject: should never be called!" );
  debug0(DPT, D, "Plain_PrintGraphicObject returning");
} /* end Plain_PrintGraphicObject */


/*****************************************************************************/
/*                                                                           */
/*  Plain_DefineGraphicNames(x)                                              */
/*                                                                           */
/*  Generate PostScript for xsize, ysize etc. names of graphic object.       */
/*                                                                           */
/*****************************************************************************/

void Plain_DefineGraphicNames(OBJECT x)
{
  debug1(DPT, D, "Plain_DefineGraphicNames( %s )", EchoObject(x));
  debug1(DPT, DD, "  style = %s", EchoStyle(&save_style(x)));
  assert(FALSE, "Plain_DefineGraphicNames: should never be called!" );
  debug0(DPT, D, "Plain_DefineGraphicNames returning.");
} /* end Plain_DefineGraphicNames */


/*****************************************************************************/
/*                                                                           */
/*  Plain_SaveTranslateDefineSave(x, xdist, ydist)                           */
/*                                                                           */
/*  Equivalent to the sequence of calls                                      */
/*                                                                           */
/*      SaveGraphicState(x)                                                  */
/*      CoordTranslate(xdist, ydist)                                         */
/*      DefineGraphicNames(x)                                                */
/*      SaveGraphicState(x)                                                  */
/*                                                                           */
/*  but offers opportunities for optimization.                               */
/*                                                                           */
/*****************************************************************************/

void Plain_SaveTranslateDefineSave(OBJECT x, FULL_LENGTH xdist,
  FULL_LENGTH ydist)
{
  assert(FALSE, "Plain_SaveTranslateDefineSave: should never be called!" );
} /* end Plain_SaveTranslateDefineSave */


/*****************************************************************************/
/*                                                                           */
/*  Plain_PrintGraphicInclude(x, colmark, rowmark)                           */
/*                                                                           */
/*  Print graphic include file, with appropriate surrounds.                  */
/*                                                                           */
/*****************************************************************************/

void Plain_PrintGraphicInclude(OBJECT x, FULL_LENGTH colmark,
  FULL_LENGTH rowmark)
{
  debug0(DPT, D, "Plain_PrintGraphicInclude(x)");
  assert(FALSE, "Plain_PrintGraphicInclude: should never be called!" );
  debug0(DPT, D, "Plain_PrintGraphicInclude returning.");
} /* end Plain_PrintGraphicInclude */


/*****************************************************************************/
/*                                                                           */
/*  Plain_LinkSource(name, llx, lly, urx, ury)                               */
/*                                                                           */
/*  Print a link source point.                                               */
/*                                                                           */
/*****************************************************************************/

static void Plain_LinkSource(OBJECT name, FULL_LENGTH llx, FULL_LENGTH lly,
  FULL_LENGTH urx, FULL_LENGTH ury)
{
  debug5(DPT, D, "Plain_LinkSource(%s, %d, %d, %d, %d)", EchoObject(name),
    llx, lly, urx, ury);
  /* do nothing; no links in plain text output */
  debug0(DPT, D, "Plain_LinkSource returning.");
} /* end Plain_LinkSource */


/*****************************************************************************/
/*                                                                           */
/*  Plain_LinkDest(name, llx, lly, urx, ury)                                 */
/*                                                                           */
/*  Print a link dest point.                                                 */
/*                                                                           */
/*  Still to do: check that the name has not been used by a previous         */
/*  dest point.                                                              */
/*                                                                           */
/*****************************************************************************/

static void Plain_LinkDest(OBJECT name, FULL_LENGTH llx, FULL_LENGTH lly,
  FULL_LENGTH urx, FULL_LENGTH ury)
{
  debug5(DPT, D, "Plain_LinkDest(%s, %d, %d, %d, %d)", EchoObject(name),
    llx, lly, urx, ury);
  /* do nothing; no links in plain text output */
  debug0(DPT, D, "Plain_LinkDest returning.");
} /* end Plain_LinkDest */


/*****************************************************************************/
/*                                                                           */
/*  Plain_LinkCheck()                                                        */
/*                                                                           */
/*  Called at end of run; will check that for every link source point there  */
/*  is a link dest point.                                                    */
/*                                                                           */
/*****************************************************************************/

static void Plain_LinkCheck()
{
  debug0(DPT, D, "Plain_LinkCheck()");
  /* do nothing; no links in plain text output */
  debug0(DPT, D, "Plain_LinkCheck returning.");
} /* end Plain_LinkCheck */


/*****************************************************************************/
/*                                                                           */
/*  Plain_BackEnd                                                            */
/*                                                                           */
/*  The record into which all of these functions are packaged.               */
/*                                                                           */
/*****************************************************************************/

static struct back_end_rec plain_back = {
  PLAINTEXT,				/* the code number of the back end   */
  STR_PLAINTEXT,			/* string name of the back end       */
  FALSE,				/* TRUE if @Scale is available       */
  FALSE,				/* TRUE if @Rotate is available      */
  FALSE,				/* TRUE if @Graphic is available     */
  FALSE,				/* TRUE if @IncludeGraphic is avail. */
  TRUE,					/* TRUE if @PlainGraphic is avail.   */
  FALSE,				/* TRUE if fractional spacing avail. */
  FALSE,				/* TRUE if actual font metrics used  */
  FALSE,				/* TRUE if colour is available       */
  Plain_PrintInitialize,
  Plain_PrintLength,
  Plain_PrintPageSetupForFont,
  Plain_PrintPageResourceForFont,
  Plain_PrintMapping,
  Plain_PrintBeforeFirstPage,
  Plain_PrintBetweenPages,
  Plain_PrintAfterLastPage,
  Plain_PrintWord,
  Plain_PrintPlainGraphic,
  Plain_PrintUnderline,
  Plain_CoordTranslate,
  Plain_CoordRotate,
  Plain_CoordScale,
  Plain_SaveGraphicState,
  Plain_RestoreGraphicState,
  Plain_PrintGraphicObject,
  Plain_DefineGraphicNames,
  Plain_SaveTranslateDefineSave,
  Plain_PrintGraphicInclude,
  Plain_LinkSource,
  Plain_LinkDest,
  Plain_LinkCheck,
};

BACK_END Plain_BackEnd = &plain_back;
