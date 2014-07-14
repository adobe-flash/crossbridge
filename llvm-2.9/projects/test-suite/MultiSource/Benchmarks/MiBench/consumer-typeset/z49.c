/*@z49.c:PostScript Back End:PS_BackEnd@**************************************/
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
/*  State variables for this module                                          */
/*                                                                           */
/*****************************************************************************/
#define StartUpResource "LoutStartUp"
#define NO_FONT		0		/* actually stolen from z37.c        */
#define NO_COLOUR	0
#define MAX_GS		50		/* maximum depth of graphics states  */

BOOLEAN	Encapsulated;			/* TRUE if EPS file is wanted	     */

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
static FILE		*out_fp;	/* file to print PostScript on       */


/*****************************************************************************/
/*                                                                           */
/*  Data structures for checking links                                       */
/*                                                                           */
/*  We keep a hash table of all dest points, and an ordinary list of all     */
/*  source points.  To check that no dest point appears twice, we consult    */
/*  the hash table once for each dest point to ensure it is not already      */
/*  there.  To check that every source point has an dest, we run through     */
/*  the list of source points at end of run and look each one up in the      */
/*  dest point hash table.                                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  LINK_DEST_TABLE                                                          */
/*                                                                           */
/*  A symbol table permitting access to link dest name objects.              */
/*  The table will automatically enlarge to accept any number of entries.    */
/*                                                                           */
/*     ltab_new(newsize)         New empty table, newsize capacity           */
/*     ltab_insert(x, &S)        Insert new link dest name object x into S   */
/*     ltab_retrieve(str, S)     Retrieve link dest name object named str    */
/*     ltab_debug(S, fp)         Debug print of table S to file fp           */
/*                                                                           */
/*****************************************************************************/

typedef struct
{ int linktab_size;				/* size of table             */
  int linktab_count;				/* number of objects held    */
  OBJECT linktab_item[1];
} *LINK_DEST_TABLE;

#define	ltab_size(S)	(S)->linktab_size
#define	ltab_count(S)	(S)->linktab_count
#define	ltab_item(S, i)	(S)->linktab_item[i]

#define hash(pos, str, S)						\
{ FULL_CHAR *p = str;							\
  pos = *p++;								\
  while( *p ) pos += *p++;						\
  pos = pos % ltab_size(S);						\
}

static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

static LINK_DEST_TABLE ltab_new(int newsize)
{ LINK_DEST_TABLE S;  int i;
  /* ifdebug(DMA, D, DebugRegisterUsage(MEM_LINK_TAB, 1,
    2*sizeof(int) + newsize * sizeof(OBJECT))); */
  S = (LINK_DEST_TABLE)
	  malloc(2*sizeof(int) + newsize * sizeof(OBJECT));
  if( S == (LINK_DEST_TABLE) NULL )
    Error(43, 1, "run out of memory enlarging link dest table", FATAL, no_fpos);
  ltab_size(S) = newsize;
  ltab_count(S) = 0;
  for( i = 0;  i < newsize;  i++ )  ltab_item(S, i) = nilobj;
  return S;
} /* end ltab_new */

static void ltab_insert(OBJECT x, LINK_DEST_TABLE *S);

static LINK_DEST_TABLE ltab_rehash(LINK_DEST_TABLE S, int newsize)
{ LINK_DEST_TABLE NewS;  int i;
  NewS = ltab_new(newsize);
  for( i = 1;  i <= ltab_size(S);  i++ )
  { if( ltab_item(S, i) != nilobj )
      ltab_insert(ltab_item(S, i), &NewS);
  }
  free(S);
  return NewS;
} /* end ltab_rehash */

static void ltab_insert(OBJECT x, LINK_DEST_TABLE *S)
{ int pos;  OBJECT z, link, y;
  if( ltab_count(*S) == ltab_size(*S) - 1 )	/* one less since 0 unused */
    *S = ltab_rehash(*S, 2*ltab_size(*S));
  hash(pos, string(x), *S);
  if( ltab_item(*S, pos) == nilobj )  New(ltab_item(*S, pos), ACAT);
  z = ltab_item(*S, pos);
  for( link = Down(z);  link != z;  link = NextDown(link) )
  { Child(y, link);
    if( StringEqual(string(x), string(y)) )
    { Error(43, 2, "link name %s used twice (first at%s)",
	WARN, &fpos(x), string(x), EchoFilePos(&fpos(y)));
    }
  }
  Link(ltab_item(*S, pos), x);
} /* end ltab_insert */

static OBJECT ltab_retrieve(FULL_CHAR *str, LINK_DEST_TABLE S)
{ OBJECT x, link, y;  int pos;
  hash(pos, str, S);
  x = ltab_item(S, pos);
  if( x == nilobj )  return nilobj;
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( StringEqual(str, string(y)) )  return y;
  }
  return nilobj;
} /* end ltab_retrieve */

#if DEBUG_ON
static void ltab_debug(LINK_DEST_TABLE S, FILE *fp)
{ int i;  OBJECT x, link, y;
  fprintf(fp, "  table size: %d;  current number of keys: %d\n",
    ltab_size(S), ltab_count(S));
  for( i = 0;  i < ltab_size(S);  i++ )
  { x = ltab_item(S, i);
    fprintf(fp, "ltab_item(S, %d) =", i);
    if( x == nilobj )
      fprintf(fp, " <nilobj>");
    else if( type(x) != ACAT )
      fprintf(fp, " not ACAT!");
    else for( link = Down(x);  link != x;  link = NextDown(link) )
    { Child(y, link);
      fprintf(fp, " %s",
	is_word(type(y)) ? string(y) : AsciiToFull("not-WORD!"));
    }
    fprintf(fp, "\n");
  }
} /* end ltab_debug */
#endif

static	LINK_DEST_TABLE	link_dest_tab;		/* the link dest names       */
static	OBJECT		link_source_list;	/* the link source names     */

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
/*  PS_PrintInitialize(FILE *fp)                                             */
/*                                                                           */
/*  Initialize this module; fp is the output file.                           */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintInitialize(FILE *fp)
{
  debug0(DPO, DD, "PS_PrintInitialize(fp)");
  out_fp = fp;
  prologue_done = FALSE;
  gs_stack_top = -1;
  currentfont = NO_FONT;
  currentcolour = NO_COLOUR;
  cpexists = FALSE;
  wordcount = pagecount = 0;
  New(needs, ACAT);
  New(supplied, ACAT);
  debug0(DPO, DD, "PS_PrintInitialize returning.");
  link_dest_tab = ltab_new(200);
  New(link_source_list, ACAT);
} /* end PS_PrintInitialize */


/*****************************************************************************/
/*                                                                           */
/*  void PS_PrintLength(FULL_CHAR *buff, int length, int length_dim)         */
/*                                                                           */
/*  Print a length (debugging only)                                          */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintLength(FULL_CHAR *buff, int length, int length_dim)
{
  sprintf( (char *) buff, "%.3fc", (float) length/CM);
}


/*****************************************************************************/
/*                                                                           */
/*  void PS_PrintPageSetupForFont(OBJECT face, int font_curr_page,           */
/*    FULL_CHAR *font_name, FULL_CHAR *short_name)                           */
/*                                                                           */
/*  Print the page setup commands required to use a font on some page:       */
/*                                                                           */
/*    face            The font face record, defining which font we need      */
/*    font_curr_page  The current page number                                */
/*    font_name       The name of the font                                   */
/*    short_name      Internal short name for the font                       */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintPageSetupForFont(OBJECT face, int font_curr_page,
  FULL_CHAR *font_name, FULL_CHAR *short_name)
{
  fprintf(out_fp, "%%%%IncludeResource: font %s\n", font_name);
  if( font_recoded(face) )
  {
    MapEnsurePrinted(font_mapping(face), font_curr_page);
    fprintf(out_fp, "/%s%s %s /%s LoutRecode\n",
      font_name, short_name,
      MapEncodingName(font_mapping(face)), font_name);
    fprintf(out_fp, "/%s { /%s%s LoutFont } def\n", short_name,
      font_name, short_name);
  }
  else fprintf(out_fp, "/%s { /%s LoutFont } def\n", short_name, font_name);
} /* end PrintPageSetupForFont */


/*****************************************************************************/
/*                                                                           */
/*  void PS_PrintPageResourceForFont(FULL_CHAR *font_name, BOOLEAN first)    */
/*                                                                           */
/*  Print page resource info on file fp for font font_name; first is true    */
/*  if this is the first resource on this page.                              */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintPageResourceForFont(FULL_CHAR *font_name, BOOLEAN first)
{
  fprintf(out_fp, "%s font %s\n", first ? "%%PageResources:" : "%%+",font_name);
} /* end PS_PrintPageResourceForFont */


/*****************************************************************************/
/*                                                                           */
/*  static void PS_PrintMapping(MAPPING m)                                   */
/*                                                                           */
/*  Print mapping m onto out_fp.                                             */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintMapping(MAPPING m)
{ MAP_VEC map = MapTable[m]; int i;
  fprintf(out_fp, "%%%%BeginResource: encoding %s\n", string(map->name));
  fprintf(out_fp, "/%s [\n", string(map->name));
  for( i = 0;  i < MAX_CHARS;  i++ )
    fprintf(out_fp, "/%s%c", string(map->vector[i]), (i+1)%8 != 0 ? ' ' : '\n');
  fprintf(out_fp, "] def\n");
  fprintf(out_fp, "%%%%EndResource\n\n");
} /* end PrintMapping */


/*****************************************************************************/
/*                                                                           */
/*  char *MediaName(int h, int v)                                            */
/*                                                                           */
/*  Return the PostScript MediaName attribute appropriate to a page of       */
/*  width h and height v.                                                    */
/*                                                                           */
/*  Communicated by Valeriy E. Ushakov, who wrote:                           */
/*                                                                           */
/*  "Attached is a patch to recognize known paper sizes and emit them as     */
/*  media name in DocumentMedia comment.  GhostView and other PostScript     */
/*  viewers recognize these names and display them to the user.  Thus user   */
/*  knows what paper size document uses without having to know the magic     */
/*  numbers."                                                                */
/*                                                                           */
/*****************************************************************************/

static const char *MediaName(int h, int v)
{
    struct paper {
       const char *name;
       FULL_LENGTH width, height;
    };

    /* names for known paper sizes */
    static const struct paper paper_map[] = {
       { "Letter",     612*PT,  792*PT },
       { "Tabloid",    792*PT, 1224*PT },
       { "Ledger",    1224*PT,  792*PT },
       { "Legal",      612*PT, 1008*PT },
       { "Statement",  396*PT,  612*PT },
       { "Executive",  540*PT,  720*PT },
       { "A3",         842*PT, 1190*PT },
       { "A4",         595*PT,  842*PT },
       { "A5",         420*PT,  595*PT },
       { "B4",         729*PT, 1032*PT },
       { "B5",         516*PT,  729*PT },
       { "Folio",      612*PT,  936*PT },
       { "Quarto",     610*PT,  780*PT },
       { "10x14",      720*PT, 1008*PT },
       { NULL,              0,       0 }
    };

    /* default media name */
    static const char *user_defined = "Plain";

    const struct paper *p;
    for (p = paper_map; p->name; ++p) {
       if ((h == p->width) && (v == p->height)) {
           return p->name;
       }
    }
    return user_defined;
}


/*****************************************************************************/
/*                                                                           */
/*  static void PrintBeforeFirstPage(FULL_LENGTH h, FULL_LENGTH v,           */
/*    FULL_CHAR *label)                                                      */
/*                                                                           */
/*  Print whatever is needed before the start of the first page: the         */
/*  PostScript prologue, augmented with any @PrependGraphic or               */
/*  @SysPrependGraphic files specified by the user.  The following           */
/*  PostScript operators are defined:                                        */
/*                                                                           */
/*    scale_factor  fnt       scale and set font                             */
/*    x_coordinate  x         move to x_coordinate, current y coordinate     */
/*    string        s         show string                                    */
/*    number        in        result is number inches                        */
/*    number        cm        result is number centimetres                   */
/*    number        pt        result is number points                        */
/*    number        sp        result is number spaces                        */
/*    number        vs        result is number vspaces                       */
/*    number        ft        result is number font-sizes                    */
/*                                                                           */
/*  as well as LoutGraphic, for use with the @Graphic operator:              */
/*                                                                           */
/*    xsize ysize xmark ymark fr vs sp LoutGraphic -                         */
/*                                                                           */
/*  Define xmark, ymark, xsize, ysize to be the positions of                 */
/*  these features of x, and define symbols ft, vs and sp                    */
/*  to be the current font size, line separation, and space width.           */
/*                                                                           */
/*****************************************************************************/

#define p0(str) fputs(str, out_fp)
#define p1(str, arg1) fprintf(out_fp, str, arg1)
#define p2(str, arg1, arg2) fprintf(out_fp, str, arg1, arg2)
#define p3(str, arg1, arg2, arg3) fprintf(out_fp, str, arg1, arg2, arg3)

static void PS_PrintBeforeFirstPage(FULL_LENGTH h, FULL_LENGTH v,
  FULL_CHAR *label)
{ FILE_NUM fnum;  FULL_CHAR *p;
  debug2(DPO, DD, "PrintBeforeFirst(%d, %d)", h, v);

  /* print header comments for PostScript DSC 3.0 output */
  if( Encapsulated )
    p0("%!PS-Adobe-3.0 EPSF-3.0\n");
  else
    p0("%!PS-Adobe-3.0\n");
  p1("%%%%Creator: %s\n", LOUT_VERSION);
  p0("%%%%CreationDate: Sometime Today\n");
  p0("%%DocumentData: Binary\n");
  p0("%%DocumentNeededResources: (atend)\n");
  p0("%%DocumentSuppliedResources: (atend)\n");
  p3("%%%%DocumentMedia: %s %d %d 0 white ()\n", MediaName(h, v), h/PT, v/PT);
  p0("%%PageOrder: Ascend\n");
  p0("%%Pages: (atend)\n");
  p2("%%%%BoundingBox: 0 0 %d %d\n", h/PT, v/PT);
  p0("%%EndComments\n\n");

  /* print procedure definitions part of header */
  p0("%%BeginProlog\n");
  p1("%%%%BeginResource: procset %s\n", StartUpResource);
  p0("/save_cp { currentpoint /cp_y exch def /cp_x exch def } def\n");
  p0("/restore_cp { cp_x cp_y moveto } def\n");
  p0("/outline { gsave 1 1 1 setrgbcolor dup show save_cp\n");
  p0("  grestore true charpath stroke restore_cp } bind def\n");
  p0("/m  { 3 1 roll moveto show } bind def\n");
  p0("/mo { 3 1 roll moveto outline } bind def\n");
  p0("/s  { exch currentpoint exch pop moveto show } bind def\n");
  p0("/so { exch currentpoint exch pop moveto outline } bind def\n");
  p0("/k  { exch neg 0 rmoveto show } bind def\n");
  p0("/ko { exch neg 0 rmoveto outline } bind def\n");
  p0("/r  { exch 0 rmoveto show } bind def\n");
  p0("/ro { exch 0 rmoveto outline } bind def\n");
  p0("/c  { gsave 3 1 roll rmoveto show grestore } bind def\n");
  p0("/co { gsave 3 1 roll rmoveto outline grestore } bind def\n");
  p0("/ul { gsave setlinewidth dup 3 1 roll\n");
  p0("      moveto lineto stroke grestore } bind def\n");
  p1("/in { %d mul } def\n", IN);
  p1("/cm { %d mul } def\n", CM);
  p1("/pt { %d mul } def\n", PT);
  p1("/em { %d mul } def\n", EM);
  p0("/sp { louts mul } def\n");
  p0("/vs { loutv mul } def\n");
  p0("/ft { loutf mul } def\n");
  p0("/dg {           } def\n\n");

  p0("/LoutGraphic {\n");
  p0("  /louts exch def\n");
  p0("  /loutv exch def\n");
  p0("  /loutf exch def\n");
  p0("  /ymark exch def\n");
  p0("  /xmark exch def\n");
  p0("  /ysize exch def\n");
  p0("  /xsize exch def\n} def\n\n");

  p0("/LoutGr2 { gsave translate LoutGraphic gsave } def\n\n");

  /* print definition used by Lout output to recode fonts                */
  /* adapted from PostScript Language Reference Manual (2nd Ed), p. 275  */
  /* usage: /<fullname> <encodingvector> /<originalname> LoutRecode -    */

  p0("/LoutFont\n");
  p0("{ findfont exch scalefont setfont\n");
  p0("} bind def\n\n");

  p0("/LoutRecode {\n");
  p0("  { findfont dup length dict begin\n");
  p0("    {1 index /FID ne {def} {pop pop} ifelse} forall\n");
  p0("    /Encoding exch def\n");
  p0("    currentdict end definefont pop\n");
  p0("  }\n");
  p0("  stopped pop\n");
  p0("} bind def\n\n");

  /* print definitions used by Lout output when including EPSF files     */
  /* copied from PostScript Language Reference Manual (2nd Ed.), p. 726  */

  p0("/BeginEPSF {\n");
  p0("  /LoutEPSFState save def\n");
  p0("  /dict_count countdictstack def\n");
  p0("  /op_count count 1 sub def\n");
  p0("  userdict begin\n");
  p0("  /showpage { } def\n");
  p0("  0 setgray 0 setlinecap\n");
  p0("  1 setlinewidth 0 setlinejoin\n");
  p0("  10 setmiterlimit [] 0 setdash newpath\n");
  p0("  /languagelevel where\n");
  p0("  { pop languagelevel\n");
  p0("    1 ne\n");
  p0("    { false setstrokeadjust false setoverprint\n");
  p0("    } if\n");
  p0("  } if\n");
  p0("} bind def\n\n");

  p0("/EndEPSF {\n");
  p0("  count op_count sub { pop } repeat\n");
  p0("  countdictstack dict_count sub { end } repeat\n");
  p0("  LoutEPSFState restore\n");
  p0("} bind def\n");

  p0("%%EndResource\n\n");

  /* print prepend files (assumed to be organized as DSC 3.0 Resources) */
  for( fnum = FirstFile(PREPEND_FILE);  fnum != NO_FILE;  fnum=NextFile(fnum) )
  { FULL_CHAR buff[MAX_BUFF];  FILE *fp;
    if( (fp = OpenFile(fnum, FALSE, FALSE)) == null )
      Error(49, 3, "cannot open %s file %s", WARN, PosOfFile(fnum),
	KW_PREPEND, FileName(fnum));
    else if( StringFGets(buff, MAX_BUFF, fp) == NULL )
      Error(49, 4, "%s file %s is empty", WARN, PosOfFile(fnum),
	KW_PREPEND, FileName(fnum));
    else
    {
      if( StringBeginsWith(buff, AsciiToFull("%%BeginResource:")) )
      { OBJECT tmp;
	tmp = MakeWord(WORD, &buff[strlen("%%BeginResource:")], no_fpos);
	Link(supplied, tmp);
      }
      else
	Error(49, 5, "%s file %s lacks PostScript BeginResource comment",
	  WARN, PosOfFile(fnum), KW_PREPEND, FileName(fnum));
      StringFPuts(buff, out_fp);
      p2("%% %s file %s\n", KW_PREPEND, mybasename(FileName(fnum)));
      while( StringFGets(buff, MAX_BUFF, fp) != NULL )
	StringFPuts(buff, out_fp);
      p0("\n");
      fclose(fp);
    }
  }

  fputs("%%EndProlog\n\n", out_fp);
  fputs("%%BeginSetup\n", out_fp);
  MapPrintEncodings();

  /* pdfmark compatibility code, as in the pdfmark Reference Manual p10 */
  p0("/pdfmark where {pop} {userdict /pdfmark /cleartomark load put} ifelse\n");

  /* FontPrintPageSetup(out_fp); */
  fputs("%%EndSetup\n\n", out_fp);
  fprintf(out_fp, "%%%%Page: ");
  for( p = label;  *p != '\0';  p++ )
    fputs(EightBitToPrintForm[*p], out_fp);
  fprintf(out_fp, " %d\n", ++pagecount);
  fprintf(out_fp, "%%%%BeginPageSetup\n");
  FontPrintPageResources(out_fp);
  fprintf(out_fp, "/pgsave save def\n");
  FontPrintPageSetup(out_fp);
  FontAdvanceCurrentPage();
  fprintf(out_fp, "%.4f dup scale %d setlinewidth\n", 1.0 / PT, PT/2);
  fprintf(out_fp, "%%%%EndPageSetup\n\n");
  prologue_done = TRUE;
} /* end PS_PrintBeforeFirstPage */


/*****************************************************************************/
/*                                                                           */
/*  PS_PrintAfterLastPage()                                                  */
/*                                                                           */
/*  Clean up this module and close output stream.                            */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintAfterLastPage(void)
{ OBJECT x, link;  BOOLEAN first_need;
  if( prologue_done )
  { 
    fprintf(out_fp, "\npgsave restore\nshowpage\n");
    fprintf(out_fp, "\n%%%%Trailer\n");

    /* print resource requirements (DSC 3.0 version) - fonts */
    first_need = FontNeeded(out_fp);

    /* print resource requirements (DSC 3.0 version) - included EPSFs  */
    for( link = Down(needs); link != needs; link = NextDown(link) )
    { Child(x, link);
      assert(is_word(type(x)), "PrintAfterLast: needs!" );
      fprintf(out_fp, "%s %s",
	first_need ? "%%DocumentNeededResources:" : "%%+", string(x));
      first_need = FALSE;
    }

    /* print resources supplied */
    fprintf(out_fp, "%%%%DocumentSuppliedResources: procset %s\n",
      StartUpResource);
    for( link = Down(supplied);  link != supplied;  link = NextDown(link) )
    { Child(x, link);
      fprintf(out_fp, "%%%%+ %s", string(x));
    }
    MapPrintPSResources(out_fp);

    fprintf(out_fp, "%%%%Pages: %d\n", pagecount);
    fprintf(out_fp, "%%%%EOF\n");
  }
} /* end PS_PrintAfterLastPage */


/*****************************************************************************/
/*                                                                           */
/*  PS_PrintBetweenPages(h, v, label)                                        */
/*                                                                           */
/*  Start a new output component, of size h by v; label is the page label    */
/*  to attach to the %%Page comment.                                         */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintBetweenPages(FULL_LENGTH h, FULL_LENGTH v, FULL_CHAR *label)
{ FULL_CHAR *p;
  debug2(DPO, DD, "PrintBetweenPages(%d, %d)", h, v);

  fprintf(out_fp, "\npgsave restore\nshowpage\n");
  gs_stack_top = 0;
  cpexists = FALSE;
  currentfont = NO_FONT;
  currentcolour = NO_COLOUR;
  if( Encapsulated )
  { PS_PrintAfterLastPage();
    Error(49, 6, "truncating -EPS document at end of first page",
      FATAL, no_fpos);
  }
  fprintf(out_fp, "\n%%%%Page: ");
  for( p = label;  *p != '\0';  p++ )
    fputs(EightBitToPrintForm[*p], out_fp);
  fprintf(out_fp, " %d\n", ++pagecount);
  fprintf(out_fp, "%%%%BeginPageSetup\n");
  FontPrintPageResources(out_fp);
  fprintf(out_fp, "/pgsave save def\n");
  FontPrintPageSetup(out_fp);
  FontAdvanceCurrentPage();
  fprintf(out_fp, "%.4f dup scale %d setlinewidth\n", 1.0 / PT, PT/2);
  fprintf(out_fp, "%%%%EndPageSetup\n");
  wordcount = 0;
} /* end PS_PrintBetweenPages */


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
/*  Print composite character cp, assuming that the current point is         */
/*  set to the correct origin.  If outline is true, we want to print the     */
/*  composite character in outline.                                          */
/*                                                                           */
/*****************************************************************************/

static void PrintComposite(COMPOSITE *cp, BOOLEAN outline, FILE *fp)
{ debug1(DPO, D, "PrintComposite(cp, %s, fp)", bool(outline));
  while( cp->char_code != '\0' )
  {
    debug4(DPO, D, "  cp = %d printing code %d (%d, %d)", (int) cp,
      cp->char_code, cp->x_offset, cp->y_offset);
    fprintf(fp, "%d %d (%c)%s ", cp->x_offset, cp->y_offset,
      cp->char_code, outline ? "co" : "c");
    cp++;
  }
} /* end PrintComposite */


/*****************************************************************************/
/*                                                                           */
/*  PS_PrintWord(x, hpos, vpos)                                              */
/*                                                                           */
/*  Print non-empty word x; its marks cross at the point (hpos, vpos).       */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintWord(OBJECT x, int hpos, int vpos)
{ FULL_CHAR *p, *q, *a, *b, *lig, *unacc;
  int ksize;  char *command;  MAPPING m;
  unsigned short *composite; COMPOSITE *cmp;

  debug6(DPO, DD, "PrintWord( %s, %d, %d ) font %d colour %d%s", string(x),
    hpos, vpos, word_font(x), word_colour(x), word_outline(x) ? " outline":"");
  TotalWordCount++;

  /* if font is different to previous word then print change */
  if( word_font(x) != currentfont )
  { currentfont = word_font(x);
    currentxheight2 = FontHalfXHeight(currentfont);
    fprintf(out_fp, "%hd %s", FontSize(currentfont, x), FontName(currentfont));
    if( ++wordcount >= 5 )
    { putc('\n', out_fp);
      wordcount = 0;
    }
    else putc(' ', out_fp);
  }

  /* if colour is different to previous word then print change */
  if( word_colour(x) != currentcolour )
  { currentcolour = word_colour(x);
    if( currentcolour > 0 )
    { fprintf(out_fp, "%s", ColourCommand(currentcolour));
      if( ++wordcount >= 5 )
      {	putc('\n', out_fp);
	wordcount = 0;
      }
      else putc(' ', out_fp);
    }
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

  /* move to coordinate of x */
  cmp = finfo[word_font(x)].cmp_table;
  composite = finfo[word_font(x)].composite;
  debug1(DPO, DDD, "  currentxheight2 = %d", currentxheight2);
  vpos = vpos - currentxheight2;
  if( cpexists && currenty == vpos )
  { printnum(hpos, out_fp);
    command = word_outline(x) ? "so" : "s";
  }
  else
  { currenty = vpos;
    printnum(hpos, out_fp);
    putc(' ', out_fp);
    printnum(currenty, out_fp);
    command = word_outline(x) ? "mo" : "m";
    cpexists = TRUE;
  }

  /* show string(x) */
  putc('(', out_fp);
  p = string(x);
  if( composite[*p] )
  {
    fprintf(out_fp, ")%s ", command);
    debug3(DPO, D,
      "  calling PrintComposite(&cmp[composite[%d] = %d]); cmp_top = %d",
      (int) *p, composite[*p], finfo[word_font(x)].cmp_top);
    PrintComposite(&cmp[composite[*p]], word_outline(x), out_fp);
    printnum(finfo[word_font(x)].size_table[*p].right, out_fp);
    putc('(', out_fp);
    command = word_outline(x) ? "ro" : "r";
  }
  else fputs(EightBitToPrintForm[*p], out_fp);
  m = font_mapping(finfo[word_font(x)].font_table);
  unacc = MapTable[m]->map[MAP_UNACCENTED];
  /* acc   = MapTable[m]->map[MAP_ACCENT]; */
  for( p++;  *p;  p++ )
  { KernLength(word_font(x), unacc, *(p-1), *p, ksize);
    if( ksize != 0 )
    { fprintf(out_fp, ")%s %d(", command, -ksize);
      ++wordcount;
      command = word_outline(x) ? "ko" : "k";
    }
    if( composite[*p] )
    { fprintf(out_fp, ")%s ", command);
      debug3(DPO, D,
	"  calling PrintComposite(&cmp[composite[%d] = %d]); cmp_top = %d",
	(int) *p, composite[*p], finfo[word_font(x)].cmp_top);
      PrintComposite(&cmp[composite[*p]], word_outline(x), out_fp);
      printnum(finfo[word_font(x)].size_table[*p].right, out_fp);
      putc('(', out_fp);
      command = word_outline(x) ? "ro" : "r";
    }
    else fputs(EightBitToPrintForm[*p], out_fp);
  }
  if( ++wordcount >= 5 )
  { fprintf(out_fp, ")%s\n", command);
    wordcount = 0;
  }
  else fprintf(out_fp, ")%s ", command);
  debug0(DPO, DDD, "PrintWord returning");
} /* end PS_PrintWord */


/*****************************************************************************/
/*                                                                           */
/*  PS_PrintPlainGraphic(OBJECT x, FULL_LENGTH xmk, ymk, OBJECT z)           */
/*                                                                           */
/*  Print a plain graphic object                                             */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintPlainGraphic(OBJECT x, FULL_LENGTH xmk,
  FULL_LENGTH ymk, OBJECT z)
{
  assert(FALSE, "PS_PrintPlainGraphic: this routine should never be called!");
} /* end PS_PrintPlainGraphic */


/*****************************************************************************/
/*                                                                           */
/*  PS_PrintUnderline(fnum, col, xstart, xstop, ymk)                         */
/*                                                                           */
/*  Draw an underline suitable for font fnum, in colour col, from xstart to  */
/*  xstop at the appropriate distance below mark ymk.                        */
/*                                                                           */
/*****************************************************************************/

static void PS_PrintUnderline(FONT_NUM fnum, COLOUR_NUM col,
  FULL_LENGTH xstart, FULL_LENGTH xstop, FULL_LENGTH ymk)
{
  debug5(DPO, DD, "PrintUnderline(fnt %d, col %d, xstart %s, xstop %s, ymk %s)",
    fnum, col, EchoLength(xstart), EchoLength(xstop), EchoLength(ymk));

  /* if colour is different to previously then print change */
  if( col != currentcolour )
  { currentcolour = col;
    if( currentcolour > 0 )
    { fprintf(out_fp, "%s", ColourCommand(currentcolour));
      if( ++wordcount >= 5 )
      {	putc('\n', out_fp);
	wordcount = 0;
      }
      else putc(' ', out_fp);
    }
  }

  /* now print the underline command */
  fprintf(out_fp, "%d %d %d %d ul\n", xstart, xstop,
    ymk - finfo[fnum].underline_pos, finfo[fnum].underline_thick);
  debug0(DPO, DD, "PrintUnderline returning.");
} /* end PS_PrintUnderline */


/*****************************************************************************/
/*                                                                           */
/*  PS_CoordTranslate(xdist, ydist)                                          */
/*                                                                           */
/*  Translate coordinate system by the given x and y distances.              */
/*                                                                           */
/*****************************************************************************/

static void PS_CoordTranslate(FULL_LENGTH xdist, FULL_LENGTH ydist)
{ debug2(DPO, D, "PS_CoordTranslate(%s, %s)",
    EchoLength(xdist), EchoLength(ydist));
  fprintf(out_fp, "%d %d translate\n", xdist, ydist);
  cpexists = FALSE;
  debug0(DPO, D, "PS_CoordTranslate returning.");
} /* end PS_CoordTranslate */


/*****************************************************************************/
/*                                                                           */
/*  PS_CoordRotate(amount)                                                   */
/*                                                                           */
/*  Rotate coordinate system by given amount (in internal DG units)          */
/*                                                                           */
/*****************************************************************************/

static void PS_CoordRotate(FULL_LENGTH amount)
{ debug1(DPO, D, "PS_CoordRotate(%.1f degrees)", (float) amount / DG);
    fprintf(out_fp, "%.4f rotate\n", (float) amount / DG);
  cpexists = FALSE;
  debug0(DPO, D, "CoordRotate returning.");
} /* end PS_CoordRotate */


/*****************************************************************************/
/*                                                                           */
/*  PS_CoordScale(ratio, dim)                                                */
/*                                                                           */
/*  Scale coordinate system by ratio in the given dimension.                 */
/*                                                                           */
/*****************************************************************************/

static void PS_CoordScale(float hfactor, float vfactor)
{
#if DEBUG_ON
  char buff[20];
#endif
  ifdebug(DPO, D, sprintf(buff, "%.3f, %.3f", hfactor, vfactor));
  debug1(DPO, D, "CoordScale(%s)", buff);
    fprintf(out_fp, "%.4f %.4f scale\n", hfactor, vfactor);
  cpexists = FALSE;
  debug0(DPO, D, "CoordScale returning.");
} /* end PS_CoordScale */


/*****************************************************************************/
/*                                                                           */
/*  PS_SaveGraphicState(x)                                                   */
/*                                                                           */
/*  Save current coord system on stack for later restoration.                */
/*  Object x is just for error reporting, not really used at all.            */
/*                                                                           */
/*****************************************************************************/

static void PS_SaveGraphicState(OBJECT x)
{ debug0(DPO, D, "SaveGraphicState()");
  fprintf(out_fp, "gsave\n");
  gs_stack_top++;
  if( gs_stack_top >= MAX_GS )
    Error(49, 7, "rotations, graphics etc. too deeply nested (max is %d)",
      FATAL, &fpos(x), MAX_GS);
  gs_stack[gs_stack_top].gs_font	= currentfont;
  gs_stack[gs_stack_top].gs_colour	= currentcolour;
  gs_stack[gs_stack_top].gs_cpexists	= cpexists;
  gs_stack[gs_stack_top].gs_currenty	= currenty;
  gs_stack[gs_stack_top].gs_xheight2	= currentxheight2;
  debug0(DPO, D, "PS_SaveGraphicState returning.");
} /* end PS_SaveGraphicState */


/*****************************************************************************/
/*                                                                           */
/*  PS_RestoreGraphicState()                                                 */
/*                                                                           */
/*  Restore previously saved coordinate system.  NB we normally assume that  */
/*  no white space is needed before any item of output, but since this       */
/*  procedure is sometimes called immediately after PrintGraphicObject(),    */
/*  which does not append a concluding space, we prepend one here.           */
/*                                                                           */
/*****************************************************************************/

void PS_RestoreGraphicState(void)
{ debug0(DPO, D, "PS_RestoreGraphicState()");
  fprintf(out_fp, "\ngrestore\n");
  currentfont	  = gs_stack[gs_stack_top].gs_font;
  currentcolour	  = gs_stack[gs_stack_top].gs_colour;
  cpexists	  = gs_stack[gs_stack_top].gs_cpexists;
  currenty	  = gs_stack[gs_stack_top].gs_currenty;
  currentxheight2 = gs_stack[gs_stack_top].gs_xheight2;
  gs_stack_top--;
  debug0(DPO, D, "PS_RestoreGraphicState returning.");
} /* end PS_RestoreGraphicState */


/*****************************************************************************/
/*                                                                           */
/*  PS_PrintGraphicObject(x)                                                 */
/*                                                                           */
/*  Print object x on out_fp                                                 */
/*                                                                           */
/*****************************************************************************/

void PS_PrintGraphicObject(OBJECT x)
{ OBJECT y, link;
  debug3(DPO, D, "PS_PrintGraphicObject(%s %s %s)",
    EchoFilePos(&fpos(x)), Image(type(x)), EchoObject(x));
  switch( type(x) )
  {
    case WORD:
    case QWORD:

      StringFPuts(string(x), out_fp);
      break;
	

    case ACAT:
    
      for( link = Down(x);  link != x;  link = NextDown(link) )
      {	Child(y, link);
	if( type(y) == GAP_OBJ )
	{
	  if( vspace(y) > 0 )  fputs("\n", out_fp);
	  else if( hspace(y) > 0 ) fputs(" ", out_fp);
	}
	else if( is_word(type(y)) || type(y) == ACAT )
	  PS_PrintGraphicObject(y);
	else if( type(y) == WIDE || is_index(type(y)) )
	{
	  /* ignore: @Wide, indexes are sometimes inserted by Manifest */
	}
	else
	{ Error(49, 8, "error in left parameter of %s",
	    WARN, &fpos(x), KW_GRAPHIC);
	  debug1(DPO, D, "  type(y) = %s, y =", Image(type(y)));
	  ifdebug(DPO, D, DebugObject(y));
	}
      }
      break;


    default:
    
      Error(49, 9, "error in left parameter of %s", WARN, &fpos(x), KW_GRAPHIC);
      debug1(DPO, D, "  type(x) = %s, x =", Image(type(x)));
      ifdebug(DPO, D, DebugObject(x));
      break;

  }
  debug0(DPO, D, "PS_PrintGraphicObject returning");
} /* end PS_PrintGraphicObject */


/*****************************************************************************/
/*                                                                           */
/*  PS_DefineGraphicNames(x)                                                 */
/*                                                                           */
/*  Generate PostScript for xsize, ysize etc. names of graphic object.       */
/*                                                                           */
/*****************************************************************************/

void PS_DefineGraphicNames(OBJECT x)
{ assert( type(x) == GRAPHIC, "PrintGraphic: type(x) != GRAPHIC!" );
  debug1(DPO, D, "DefineGraphicNames( %s )", EchoObject(x));
  debug1(DPO, DD, "  style = %s", EchoStyle(&save_style(x)));

  /* if font is different to previous word then print change */
  if( font(save_style(x)) != currentfont )
  { currentfont = font(save_style(x));
    if( currentfont > 0 )
    { currentxheight2 = FontHalfXHeight(currentfont);
      fprintf(out_fp, "%hd %s ", FontSize(currentfont, x),
	FontName(currentfont));
    }
  }

  /* if colour is different to previous word then print change */
  if( colour(save_style(x)) != currentcolour )
  { currentcolour = colour(save_style(x));
    if( currentcolour > 0 )
      fprintf(out_fp, "%s ", ColourCommand(currentcolour));
  }

  /* now print the actual command that defines the names */
  fprintf(out_fp, "%d %d %d %d %d %d %d LoutGraphic\n",
    size(x, COLM), size(x, ROWM), back(x, COLM), fwd(x, ROWM),
    currentfont <= 0 ? 12*PT : FontSize(currentfont, x),
    width(line_gap(save_style(x))), width(space_gap(save_style(x))));
  debug0(DPO, D, "PS_DefineGraphicNames returning.");
} /* end PS_DefineGraphicNames */


/*****************************************************************************/
/*                                                                           */
/*  PS_SaveTranslateDefineSave(x, xdist, ydist)                              */
/*                                                                           */
/*  Equivalent to the sequence of calls                                      */
/*                                                                           */
/*      SaveGraphicState(x)                                                  */
/*      CoordTranslate(xdist, ydist)                                         */
/*      DefineGraphicNames(x)                                                */
/*      SaveGraphicState(x)                                                  */
/*                                                                           */
/*  but generates less PostScript.                                           */
/*                                                                           */
/*****************************************************************************/

void PS_SaveTranslateDefineSave(OBJECT x, FULL_LENGTH xdist, FULL_LENGTH ydist)
{
  if( gs_stack_top >= MAX_GS - 1 || font(save_style(x)) != currentfont ||
      colour(save_style(x))!=currentcolour )
  {
    /* do it bit by bit, will be rare anyway */
    PS_SaveGraphicState(x);
    PS_CoordTranslate(xdist, ydist);
    PS_DefineGraphicNames(x);
    PS_SaveGraphicState(x);
  }
  else
  {
    /* no font or colour changes, no stack overflow, so can optimize */

    /* from Save */
    gs_stack_top++;
    gs_stack[gs_stack_top].gs_font	= currentfont;
    gs_stack[gs_stack_top].gs_colour	= currentcolour;
    gs_stack[gs_stack_top].gs_cpexists	= cpexists;
    gs_stack[gs_stack_top].gs_currenty	= currenty;
    gs_stack[gs_stack_top].gs_xheight2	= currentxheight2;

    /* from CoordTranslate */
    cpexists = FALSE;

    /* from Save */
    gs_stack_top++;
    gs_stack[gs_stack_top].gs_font	= currentfont;
    gs_stack[gs_stack_top].gs_colour	= currentcolour;
    gs_stack[gs_stack_top].gs_cpexists	= cpexists;
    gs_stack[gs_stack_top].gs_currenty	= currenty;
    gs_stack[gs_stack_top].gs_xheight2	= currentxheight2;

    /* accumulated output from all four calls, repackaged */
    fprintf(out_fp, "%d %d %d %d %d %d %d %d %d LoutGr2\n",
      size(x, COLM), size(x, ROWM), back(x, COLM), fwd(x, ROWM),
      currentfont <= 0 ? 12*PT : FontSize(currentfont, x),
      width(line_gap(save_style(x))), width(space_gap(save_style(x))),
      xdist, ydist);
      
  }
} /* end PS_SaveTranslateDefineSave */


/*****************************************************************************/
/*                                                                           */
/*  PS_PrintGraphicInclude(x, colmark, rowmark)                              */
/*                                                                           */
/*  Print graphic include file, with appropriate surrounds.  This code       */
/*  closely follows the PostScript Language Reference Manual, 2n ed.,        */
/*  pages 733-5, except we do not clip the included EPSF.                    */
/*                                                                           */
/*  Note to porters: Version 3.0 of the EPSF standard is not compatible      */
/*  with previous versions.  Thus, this output may crash your system.        */
/*  If you can find out which comment line(s) are causing the trouble,       */
/*  you can add to procedure strip_out to strip them out during the          */
/*  file inclusion step.  e.g. on my system %%EOF causes problems, so I      */
/*  strip it out.                                                            */
/*                                                                           */
/*  May 1994: I've just discovered that %%Trailer causes problems for        */
/*  the mpage Unix utility, so now I'm stripping it out as well.             */
/*                                                                           */
/*****************************************************************************/
#define	SKIPPING	0
#define	READING_DNR	1
#define FINISHED	2

static BOOLEAN strip_out(FULL_CHAR *buff)
{ if( StringBeginsWith(buff, AsciiToFull("%%EOF"))     )  return TRUE;
  if( StringBeginsWith(buff, AsciiToFull("%%Trailer")) )  return TRUE;
  return FALSE;
} /* end strip_out */

void PS_PrintGraphicInclude(OBJECT x, FULL_LENGTH colmark, FULL_LENGTH rowmark)
{ OBJECT y, full_name;  FULL_CHAR buff[MAX_BUFF];
  FILE *fp;  int state;  BOOLEAN compressed;
  debug0(DPO, D, "PS_PrintGraphicInclude(x)");

  assert(type(x)==INCGRAPHIC || type(x)==SINCGRAPHIC, "PrintGraphicInclude!");
  assert(incgraphic_ok(x), "PrintGraphicInclude: !incgraphic_ok(x)!");

  /* open the include file and get its full path name */
  Child(y, Down(x));
  fp = OpenIncGraphicFile(string(y), type(x), &full_name,&fpos(y),&compressed);
  assert( fp != NULL, "PrintGraphicInclude: fp!" );

  /* if font is different to previous word then print change */
  if( font(save_style(x)) != currentfont )
  { currentfont = font(save_style(x));
    currentxheight2 = FontHalfXHeight(currentfont);
    fprintf(out_fp, "%hd %s\n", FontSize(currentfont,x), FontName(currentfont));
  }

  /* if colour is different to previous word then print change */
  if( colour(save_style(x)) != currentcolour )
  { currentcolour = colour(save_style(x));
    if( currentcolour > 0 )
    {
      fprintf(out_fp, "%s\n", ColourCommand(currentcolour));
    }
  }

  /* generate appropriate header code */
  fprintf(out_fp, "BeginEPSF\n");
  PS_CoordTranslate(colmark - back(x, COLM), rowmark - fwd(x, ROWM));
  PS_CoordScale( (float) PT, (float) PT );
  PS_CoordTranslate(-back(y, COLM), -back(y, ROWM));
  fprintf(out_fp, "%%%%BeginDocument: %s\n", string(full_name));

  /* copy through the include file, except divert resources lines to needs */
  /* and strip out some comment lines that cause problems                  */
  state = (StringFGets(buff, MAX_BUFF, fp) == NULL) ? FINISHED : SKIPPING;
  while( state != FINISHED ) switch(state)
  {
    case SKIPPING:

      if( StringBeginsWith(buff, AsciiToFull("%%DocumentNeededResources:")) &&
	  !StringContains(buff, AsciiToFull("(atend)")) )
      { y = MakeWord(WORD, &buff[StringLength("%%DocumentNeededResources:")],
	      no_fpos);
        Link(needs, y);
	state = (StringFGets(buff,MAX_BUFF,fp)==NULL) ? FINISHED : READING_DNR;
      }
      else
      { if( StringBeginsWith(buff, AsciiToFull("%%LanguageLevel:")) )
	  Error(49, 10, "ignoring LanguageLevel comment in %s file %s",
	    WARN, &fpos(x), KW_INCGRAPHIC, string(full_name));
	if( StringBeginsWith(buff, AsciiToFull("%%Extensions:")) )
	  Error(49, 11, "ignoring Extensions comment in %s file %s",
	    WARN, &fpos(x), KW_INCGRAPHIC, string(full_name));
	if( !strip_out(buff) )  StringFPuts(buff, out_fp);
	state = (StringFGets(buff, MAX_BUFF, fp) == NULL) ? FINISHED : SKIPPING;
      }
      break;

    case READING_DNR:

      if( StringBeginsWith(buff, AsciiToFull("%%+")) )
      {	x = MakeWord(WORD, &buff[StringLength(AsciiToFull("%%+"))], no_fpos);
	Link(needs, x);
	state = (StringFGets(buff,MAX_BUFF,fp)==NULL) ? FINISHED : READING_DNR;
      }
      else
      { if( !strip_out(buff) )  StringFPuts(buff, out_fp);
	state = (StringFGets(buff, MAX_BUFF, fp) == NULL) ? FINISHED : SKIPPING;
      }
      break;
  }

  /* wrapup */
  DisposeObject(full_name);
  fclose(fp);
  if( compressed )  StringRemove(AsciiToFull(LOUT_EPS));
  fprintf(out_fp, "\n%%%%EndDocument\nEndEPSF\n");
  wordcount = 0;
  debug0(DPO, D, "PS_PrintGraphicInclude returning.");
} /* end PS_PrintGraphicInclude */
/*****************************************************************************/
/*                                                                           */
/*  char *ConvertToPDFName(name)                                             */
/*                                                                           */
/*  Convert string(name) to a suitable PDF label.  The result is in static   */
/*  memory and must be copied before the next call to ConvertToPDFName.      */
/*                                                                           */
/*  At present our algorithm is to prefix the label with "LOUT" and to       */
/*  replace all non-alphanumerics by one underscore.                         */
/*                                                                           */
/*****************************************************************************/
#define in_range(ch, a, b)	( (ch) >= (a) && (ch) <= (b) )
#define is_lower(ch)		in_range(ch, 'a', 'z')
#define is_upper(ch)		in_range(ch, 'A', 'Z')
#define is_digit(ch)		in_range(ch, '0', '9')
#define is_alphanum(ch)		(is_lower(ch) || is_upper(ch) || is_digit(ch))

char *ConvertToPDFName(OBJECT name)
{ static char buff[200];
  char *q;
  FULL_CHAR *p;
  strcpy(buff, "LOUT");
  q = &buff[strlen(buff)];
  for( p = string(name);  *p != '\0';  p++ )
  {
    if( q >= &buff[199] )
      Error(49, 12, "tag %s is too long", FATAL, &fpos(name), string(name));
    if( is_alphanum(*p) )
      *q++ = (char) *p;
    else
      *q++ = '_';
  }
  *q++ = '\0';
  return buff;
}


/*****************************************************************************/
/*                                                                           */
/*  PS_LinkSource(name, llx, lly, urx, ury)                                  */
/*                                                                           */
/*  Print a link source point.                                               */
/*                                                                           */
/*****************************************************************************/

static void PS_LinkSource(OBJECT name, FULL_LENGTH llx, FULL_LENGTH lly,
  FULL_LENGTH urx, FULL_LENGTH ury)
{ debug5(DPO, D, "PS_LinkSource(%s, %d, %d, %d, %d)", EchoObject(name),
    llx, lly, urx, ury);

  /* print the link source point */
  fprintf(out_fp,
    "\n[ /Rect [%d %d %d %d] /Subtype /Link /Dest /%s /ANN pdfmark\n",
    llx, lly, urx, ury, ConvertToPDFName(name));

  /* remember it so that at end of run can check if it has an dest point */
  Link(link_source_list, name);
  debug0(DPO, D, "PS_LinkSource returning.");
} /* end PS_LinkSource */


/*****************************************************************************/
/*                                                                           */
/*  PS_LinkDest(name, llx, lly, urx, ury)                                    */
/*                                                                           */
/*  Print a link dest point (note llx etc are not used), after making sure   */
/*  that no previously printed dest point has the same name.                 */
/*                                                                           */
/*****************************************************************************/

static void PS_LinkDest(OBJECT name, FULL_LENGTH llx, FULL_LENGTH lly,
  FULL_LENGTH urx, FULL_LENGTH ury)
{ OBJECT prev;
  debug5(DPO, D, "PS_LinkDest(%s, %d, %d, %d, %d)", EchoObject(name),
    llx, lly, urx, ury);

  prev = ltab_retrieve(string(name), link_dest_tab);
  if( prev == nilobj )
  {
    /* not used previously, so print it and remember it */
    fprintf(out_fp, "\n[ /Dest /%s /DEST pdfmark\n", "IGNORED");
    ltab_insert(name, &link_dest_tab);
  }
  else
  {
    /* used previously, so don't print it, and warn the user */
    Error(49, 13, "link destination %s ignored (there is already one at%s)",
      WARN, &fpos(name), string(name), EchoFilePos(&fpos(prev)));
  }
  debug0(DPO, D, "PS_LinkDest returning.");
} /* end PS_LinkDest */


/*****************************************************************************/
/*                                                                           */
/*  PS_LinkCheck()                                                           */
/*                                                                           */
/*  Called at end of run; will check that for every link source point there  */
/*  is a link dest point.                                                    */
/*                                                                           */
/*****************************************************************************/

static void PS_LinkCheck()
{ OBJECT y, link;
  debug0(DPO, D, "PS_LinkCheck()");

  for( link=Down(link_source_list); link!=link_source_list; link=NextDown(link) )
  { Child(y, link);
    assert( is_word(type(y)), " PS_LinkCheck: !is_word(type(y))!");
    if( ltab_retrieve(string(y), link_dest_tab) == nilobj )
      Error(49, 14, "link name %s has no destination point", WARN, &fpos(y),
	string(y));
  }

  debug0(DPO, D, "PS_LinkCheck returning.");
} /* end PS_LinkCheck */


/*****************************************************************************/
/*                                                                           */
/*  PS_BackEnd                                                               */
/*                                                                           */
/*  The record into which all of these functions are packaged.               */
/*                                                                           */
/*****************************************************************************/

static struct back_end_rec ps_back = {
  POSTSCRIPT,				/* the code number of the back end   */
  STR_POSTSCRIPT,			/* string name of the back end       */
  TRUE,					/* TRUE if @Scale is available       */
  TRUE,					/* TRUE if @Rotate is available      */
  TRUE,					/* TRUE if @Graphic is available     */
  TRUE,					/* TRUE if @IncludeGraphic is avail. */
  FALSE,				/* TRUE if @PlainGraphic is avail.   */
  TRUE,					/* TRUE if fractional spacing avail. */
  TRUE,					/* TRUE if actual font metrics used  */
  TRUE,					/* TRUE if colour is available       */
  PS_PrintInitialize,
  PS_PrintLength,
  PS_PrintPageSetupForFont,
  PS_PrintPageResourceForFont,
  PS_PrintMapping,
  PS_PrintBeforeFirstPage,
  PS_PrintBetweenPages,
  PS_PrintAfterLastPage,
  PS_PrintWord,
  PS_PrintPlainGraphic,
  PS_PrintUnderline,
  PS_CoordTranslate,
  PS_CoordRotate,
  PS_CoordScale,
  PS_SaveGraphicState,
  PS_RestoreGraphicState,
  PS_PrintGraphicObject,
  PS_DefineGraphicNames,
  PS_SaveTranslateDefineSave,
  PS_PrintGraphicInclude,
  PS_LinkSource,
  PS_LinkDest,
  PS_LinkCheck,
};

BACK_END PS_BackEnd = &ps_back;
