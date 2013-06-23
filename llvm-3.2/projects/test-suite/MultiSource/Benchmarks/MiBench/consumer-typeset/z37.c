/*@z37.c:Font Service:Declarations@*******************************************/
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
/*  FILE:         z37.c                                                      */
/*  MODULE:       Font Service                                               */
/*  EXTERNS:      FontInit(), FontDefine(), FontChange(), FontWordSize(),    */
/*                FontSize(), FontHalfXHeight(), FontEncoding(),             */
/*                FontMapping(), FontFamilyAndFace(), FontNeeded()           */
/*                                                                           */
/*  This module implements fonts, using encoding vectors and Adobe font      */
/*  metrics files (.AFM files, version 2).                                   */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define DEFAULT_XHEIGHT 500	/* the default XHeight if font has none      */
#define	NO_FONT		  0	/* the not-a-font font number                */
#define SZ_DFT	       1000	/* default lout size is 50p                  */
#define	INIT_FINFO_SIZE	100	/* initial number of sized fonts set aside   */

/*****************************************************************************/
/*                                                                           */
/* These definitions have been moved to "externs.h" since z24.c needs them:  */
/*                                                                           */
/*  struct metrics {							     */
/*    SHORT_LENGTH up;							     */
/*    SHORT_LENGTH down;						     */
/*    SHORT_LENGTH left;						     */
/*    SHORT_LENGTH right;						     */
/*    SHORT_LENGTH last_adjust;						     */
/*  };							     		     */
/*                                                                           */
/*  typedef struc composite_rec {                                            */
/*    FULL_CHAR char_code;                                                   */
/*    SHORT_LENGTH x_offset;                                                 */
/*    SHORT_LENGTH y_offset;                                                 */
/*  } COMPOSITE;                                                             */
/*                                                                           */
/*  typedef struct font_rec {						     */
/*    struct metrics	*size_table;		   metrics of sized fonts    */
/*    FULL_CHAR		*lig_table;		   ligatures                 */
/*    unsigned short	*composite;		   non-zero means composite  */
/*    COMPOSITE		*cmp_table;		   composites to build       */
/*    int               cmp_top;                   length of cmp_table       */
/*    OBJECT		font_table;		   record of sized fonts     */
/*    OBJECT            original_face;             face object of font       */
/*    SHORT_LENGTH	underline_pos;             position of underline     */
/*    SHORT_LENGTH	underline_thick;           thickness of underline    */
/*    unsigned short	*kern_table;		   first kerning chars       */
/*    FULL_CHAR		*kern_chars;		   second kerning chars      */
/*    unsigned char	*kern_value;		   points into kern_lengths  */
/*    SHORT_LENGTH	*kern_sizes;		   sizes of kernings         */
/*  } FONT_INFO;							     */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  Private data structures of this module                                   */
/*                                                                           */
/*            +++++++++++++++++++++++++++                                    */
/*            +                         +                                    */
/*  root ->   +  ACAT                   +                                    */
/*            +                         +                                    */
/*            +                         +                                    */
/*            +++++++++++++++++++++++++++                                    */
/*                    |                                 font families...     */
/*                    |                                                      */
/*              +-----+-----------------------------------------------+ ...  */
/*              |                                                     |      */
/*              |                                                     |      */
/*            +++++++++++++++++++++++++++                                    */
/*            +                         +                                    */
/*  family -> + WORD                    +                                    */
/*            +   string (family name)  +                                    */
/*            +                         +                                    */
/*            +++++++++++++++++++++++++++                                    */
/*                    |                           faces of this family...    */
/*                    |                                                      */
/*              +-----+-----------------------------------------------+ ...  */
/*              |                                                     |      */
/*              |                                                     |      */
/*            +++++++++++++++++++++++++++++++++                              */
/*            +                               +                              */
/*  face ->   + WORD                          +                              */
/*            +   string (face name)          +                              */
/*            +   font_recoded                +                              */
/*            +   font_mapping                +                              */
/*            +   font_page                   +                              */
/*            +                               +                              */
/*            +++++++++++++++++++++++++++++++++                              */
/*                |                                 size records...          */
/*                |                                                          */
/*     +----------+---------+--------------------+-----------------------+   */
/*     |                    |                    |                       |   */
/*     |                    |                    |                       |   */
/*   +++++++++++++++++++  +++++++++++++++++++  +++++++++++++++++++++         */
/*   +                 +  +                 +  +                   +         */
/*   + WORD            +  + WORD            +  + WORD              +         */
/*   +   string (font  +  +   string (AFM   +  +   string (short   +         */
/*   +     name)       +  +     file name)  +  +     font name)    +         */
/*   +                 +  +                 +  +   font_num        +         */
/*   +++++++++++++++++++  +++++++++++++++++++  +   font_size       +         */
/*                          |                  +   font_xheight2   +         */
/*                          |                  +   font_recoded    +         */
/*                  ++++++++++++++++++++       +   font_mapping    +         */
/*                  +                  +       +   font_spacewidth +         */
/*       (optional) + WORD             +       +                   +         */
/*                  +   string (extra  +       +++++++++++++++++++++         */
/*                  +   AFM file name) +                                     */
/*                  +                  +                                     */
/*                  ++++++++++++++++++++                                     */
/*                                                                           */
/*****************************************************************************/

	int		font_curr_page;		/* current page number       */
	FONT_INFO	*finfo;			/* all the font table info   */
static	int		finfo_size;		/* current finfo array size  */
static	OBJECT		font_root;		/* root of tree of fonts     */
static	OBJECT		font_used;		/* fonts used on this page   */
static	FONT_NUM	font_count;		/* number of sized fonts     */
static	int		font_seqnum;		/* unique number for a font  */
static	OBJECT		FontDefSym;		/* symtab entry for @FontDef */
static	OBJECT		fd_tag;			/* @FontDef @Tag entry       */
static	OBJECT		fd_family;		/* @FontDef @Family entry    */
static	OBJECT		fd_face;		/* @FontDef @Face entry      */
static	OBJECT		fd_name;		/* @FontDef @Name entry      */
static	OBJECT		fd_metrics;		/* @FontDef @Metrics entry   */
static	OBJECT		fd_extra_metrics;	/* @FontDef @ExtraMetrics    */
static	OBJECT		fd_mapping;		/* @FontDef @Mapping entry   */
static	OBJECT		fd_recode;		/* @FontDef @Recode entry    */


/*@::FontInit(), FontDebug()@*************************************************/
/*                                                                           */
/*  FontInit()                                                               */
/*                                                                           */
/*  Initialise this module.                                                  */
/*                                                                           */
/*****************************************************************************/

static OBJECT load(FULL_CHAR *name, unsigned dtype, OBJECT encl, BOOLEAN compulsory)
{ OBJECT res;
  res = InsertSym(name, dtype, no_fpos, DEFAULT_PREC, FALSE, FALSE, 0, encl,
    MakeWord(WORD, STR_EMPTY, no_fpos));
  if( dtype == NPAR ) visible(res) = TRUE;
  if( compulsory )
  { has_compulsory(encl)++;
    is_compulsory(res) = TRUE;
  }
  return res;
}

void FontInit(void)
{ 
  debug0(DFT, D, "FontInit()");
  font_curr_page = 1;
  font_count	= 0;
  New(font_root, ACAT);
  New(font_used, ACAT);
  font_seqnum	= 0;
  finfo         = (FONT_INFO *) malloc(INIT_FINFO_SIZE * sizeof(FONT_INFO));
  finfo_size    = INIT_FINFO_SIZE;
  ifdebug(DMA, D,
    DebugRegisterUsage(MEM_FONTS, 1, INIT_FINFO_SIZE * sizeof(FONT_INFO)));

  /* set up FontDefSym */
  FontDefSym	   = load(KW_FONTDEF,       LOCAL, StartSym,   FALSE);
  fd_tag	   = load(KW_TAG,           NPAR,  FontDefSym, TRUE);
  fd_family	   = load(KW_FAMILY,        NPAR,  FontDefSym, TRUE);
  fd_face	   = load(KW_FACE,          NPAR,  FontDefSym, TRUE);
  fd_name	   = load(KW_NAME,          NPAR,  FontDefSym, TRUE);
  fd_metrics	   = load(KW_METRICS,       NPAR,  FontDefSym, TRUE);
  fd_extra_metrics = load(KW_EXTRA_METRICS, NPAR,  FontDefSym, FALSE);
  fd_mapping	   = load(KW_MAPPING,       NPAR,  FontDefSym, TRUE);
  fd_recode	   = load(KW_RECODE,        NPAR,  FontDefSym, FALSE);

  debug0(DFT, D, "FontInit returning.");
}


/*****************************************************************************/
/*                                                                           */
/*  FontDebug()                        	                                     */
/*                                                                           */
/*  Print out font tree (not currectly used).                                */
/*                                                                           */
/*****************************************************************************/

#if DEBUG_ON
static void FontDebug(void)
{ OBJECT family, face, link, flink, zlink, z;  int i;
  assert(font_root!=nilobj && type(font_root)==ACAT, "FontDebug: font_root!");
  for( link = Down(font_root);  link != font_root;  link = NextDown(link) )
  { Child(family, link);
    assert( is_word(type(family)), "FontDebug: family!" );
    debug1(DFS, D, "family %s:", string(family));
    for( flink = Down(family);  flink != family;  flink = NextDown(flink) )
    { Child(face, flink);
      assert( is_word(type(face)), "FontDebug: face!" );
      debug1(DFS, D, "   face %s:", string(face));
      for( zlink = Down(face);  zlink != face;  zlink = NextDown(zlink) )
      { Child(z, zlink);
	if( is_word(type(z)) )
	{ debug2(DFS, D, "      %s%s", string(z), Down(z) != z ? " child" : "");
	}
	else
	{ debug1(DFS, D, "      %s", Image(type(z)));
	}
      }
    }
  }
  for( i = 1;  i <= font_count;  i++ )
    fprintf(stderr, "  finfo[%d].font_table = %s\n", i,
      EchoObject(finfo[i].font_table));
} /* end FontDebug */


/*****************************************************************************/
/*                                                                           */
/*  DebugKernTable(fnum)                                                     */
/*                                                                           */
/*  Print debug output of kern table for font fnum.                          */
/*                                                                           */
/*****************************************************************************/

static void DebugKernTable(FONT_NUM fnum)
{ int i, j;
  unsigned short *kt = finfo[fnum].kern_table;
  FULL_CHAR      *kc = finfo[fnum].kern_chars;
  unsigned char  *kv = finfo[fnum].kern_value;
  SHORT_LENGTH   *ks = finfo[fnum].kern_sizes;
  debug1(DFT, DD, "DebugKernTable(%d)", fnum);
  for( i = 0;  i < MAX_CHARS;  i++ )
  { if( kt[i] != 0 )
    { debug1(DFT, DD, "kt[%d]:", i);
      for( j = kt[i];  kc[j] != '\0';  j++ )
      { debug3(DFT, DD, "KPX %c %c %d", i, kc[j], ks[kv[j]]);
      }
    }
  }
  debug1(DFT, DD, "DebugKernTable(%d) returning", fnum);
} /* DebugKernTable */
#endif


/*****************************************************************************/
/*                                                                           */
/*  ReadCharMetrics(face, fixed_pitch, xheight2,lig,ligtop,fnum,fnt,lnum,fp) */
/*                                                                           */
/*  Read a sequence of character metrics lines.  The font record is          */
/*  face, its ligatures are lig[0..ligtop], font number fnum, metrics fnt.   */
/*  The line number is lnum; input is to be read from file fp.               */
/*                                                                           */
/*****************************************************************************/

static void ReadCharMetrics(OBJECT face, BOOLEAN fixed_pitch, int xheight2,
  FULL_CHAR *lig, int *ligtop, FILE_NUM fnum, struct metrics *fnt,
  int *lnum, FILE *fp)
{ FULL_CHAR buff[MAX_BUFF], command[MAX_BUFF], ch, ligchar;
  int i, wx, llx, lly, urx, ury;
  float fl_wx, fl_llx, fl_lly, fl_urx, fl_ury;
  BOOLEAN wxfound, bfound;
  OBJECT AFMfilename;

  Child(AFMfilename, NextDown(Down(face)));
  while( StringFGets(buff, MAX_BUFF, fp) != NULL &&
	 !StringBeginsWith(buff, AsciiToFull("EndCharMetrics")) &&
	 !StringBeginsWith(buff, AsciiToFull("EndExtraCharMetrics")) )
  {
    /* read one line containing metric info for one character */
    debug1(DFT, DD, "  ReadCharMetrics: %s", buff);
    (*lnum)++;  ch = '\0';  
    wxfound = bfound = FALSE;
    i = 0;  while( buff[i] == ' ' )  i++;
    while( buff[i] != '\n' )
    {
      debug2(DFT, DDD, "  ch = %d, &buff[i] = %s", ch, &buff[i]);
      sscanf( (char *) &buff[i], "%s", command);
      if( StringEqual(command, "N") )
      { sscanf( (char *) &buff[i], "N %s", command);
	ch = MapCharEncoding(command, font_mapping(face));
      }
      else if( StringEqual(command, "WX") )
      {	sscanf( (char *) &buff[i], "WX %f", &fl_wx);
	wx = fl_wx;
	wxfound = TRUE;
      }
      else if( StringEqual(command, "B") )
      { sscanf( (char *) &buff[i], "B %f %f %f %f",
	  &fl_llx, &fl_lly, &fl_urx, &fl_ury);
	llx = fl_llx;
	lly = fl_lly;
	urx = fl_urx;
	ury = fl_ury;
	bfound = TRUE;
      }
      else if( StringEqual(command, "L") &&
	BackEnd->uses_font_metrics && ch != '\0' )
      { if( lig[ch] == 1 )  lig[ch] = (*ligtop) - MAX_CHARS;
	lig[(*ligtop)++] = ch;
	i++;  /* skip L */
	while( buff[i] == ' ' )  i++;
	while( buff[i] != ';' && buff[i] != '\n' )
	{ sscanf( (char *) &buff[i], "%s", command);
	  ligchar = MapCharEncoding(command, font_mapping(face));
	  if( ligchar != '\0' )  lig[(*ligtop)++] = ligchar;
	  else
	  { Error(37, 1, "ignoring unencoded ligature character %s in font file %s (line %d)",
	      WARN, &fpos(AFMfilename), command, FileName(fnum), *lnum);
	    lig[ch] = 1;
	  }
	  if( *ligtop > 2*MAX_CHARS - 5 )
	    Error(37, 2, "too many ligature characters in font file %s (line %d)",
	    FATAL, &fpos(AFMfilename), FileName(fnum), *lnum);
	  while( buff[i] != ' ' && buff[i] != ';' )  i++;
	  while( buff[i] == ' ' ) i++;
	}
	lig[(*ligtop)++] = '\0';
      }
      while( buff[i] != ';' && buff[i] != '\n' )  i++;
      if( buff[i] == ';' )
      { i++;  while( buff[i] == ' ' ) i++;
      }
    }
    if( ch > '\0' )
    { 
      if( !wxfound )
      { Error(37, 3, "WX missing in font file %s (line %d)",
	  FATAL, &fpos(AFMfilename), FileName(fnum), *lnum);
      }
      if( !bfound )
      { Error(37, 4, "B missing in font file %s (line %d)",
	  FATAL, &fpos(AFMfilename), FileName(fnum), *lnum);
      }
      if( lig[ch] == 1 )  lig[ch] = 0;	/* set to known if unknown */
      else if( lig[ch] > 1 )		/* add '\0' to end of ligs */
	lig[(*ligtop)++] = '\0';
      if( BackEnd->uses_font_metrics )
      {
	fnt[ch].left  = llx;
	fnt[ch].down  = lly - xheight2;
	fnt[ch].right = wx;
	fnt[ch].up    = ury - xheight2;
	fnt[ch].last_adjust = (urx==0 || wx==0 || fixed_pitch) ? 0 : urx - wx;
      }
      else
      {
	fnt[ch].left  = 0;
	fnt[ch].down  = - PlainCharHeight / 2;
	fnt[ch].right = PlainCharWidth;
	fnt[ch].up    = PlainCharHeight / 2;
	fnt[ch].last_adjust = 0;
      }
      debug6(DFT, DDD, "  fnt[%c] = (%d,%d,%d,%d,%d)",ch, fnt[ch].left,
	fnt[ch].down, fnt[ch].right, fnt[ch].up, fnt[ch].last_adjust);
    }
  }
} /* end ReadCharMetrics */


/*****************************************************************************/
/*                                                                           */
/*  ReadCompositeMetrics(face, Extrafilename, extra_fnum, lnum, composite,   */
/*    cmp, cmptop, fp)                                                       */
/*                                                                           */
/*  Read a sequence of composite metrics lines.  The font record is face.    */
/*  The line number is lnum; input is to be read from file fp.               */
/*                                                                           */
/*****************************************************************************/

static void ReadCompositeMetrics(OBJECT face, OBJECT Extrafilename,
  FILE_NUM extra_fnum, int *lnum, unsigned short composite[],
  COMPOSITE cmp[], int *cmptop, FILE *fp)
{ char *status;
  FULL_CHAR buff[MAX_BUFF], composite_name[100], name[100];
  int composite_num, x_offset, y_offset, i, count;
  FULL_CHAR composite_code, code;

  /* build composites */
  while( (status = StringFGets(buff, MAX_BUFF, fp)) != (char *) NULL
		&& StringBeginsWith(buff, AsciiToFull("CC")) )
  {
    (*lnum)++;
    debug1(DFT, D, "  composite: %s", buff);

    /* read CC <charname> <number_of_pieces> ; and move i to after it */
    if( sscanf((char *)buff, "CC %s %d ", composite_name, &composite_num) != 2 )
      Error(37, 5, "syntax error in extra font file %s (line %d)",
	FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
    for( i = 0;  buff[i] != ';' && buff[i] != '\n' && buff[i] != '\0'; i++ );
    if( buff[i] != ';' )
      Error(37, 5, "syntax error in extra font file %s (line %d)",
	FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
    i++;

    /* add entry for this character to composite */
    composite_code = MapCharEncoding(composite_name,font_mapping(face));
    if( composite_code == (FULL_CHAR) '\0' )
      Error(37, 6, "unknown character name %s in font file %s (line %d)",
	FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
    composite[composite_code] = *cmptop;

    for( count = 0; count < composite_num; count++ )
    {
      /* read one PCC <charname> <xoffset> <yoffset> ; and move i to after it */
      if( sscanf((char *)&buff[i]," PCC %s %d %d",name,&x_offset,&y_offset)!=3 )
	Error(37, 5, "syntax error in extra font file %s (line %d)",
	  FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
      for( ; buff[i] != ';' && buff[i] != '\n' && buff[i] != '\0'; i++ );
      if( buff[i] != ';' )
	Error(37, 5, "syntax error in extra font file %s (line %d)",
	  FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
      i++;

      /* load this piece into cmp */
      if( *cmptop >= MAX_CHARS )
	Error(37, 7, "too many composites in file %s (at line %d)",
	  FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
      code = MapCharEncoding(name, font_mapping(face));
      cmp[*cmptop].char_code = code;
      cmp[*cmptop].x_offset = x_offset;
      cmp[*cmptop].y_offset = y_offset;
      (*cmptop)++;
    }
    
    /* add null terminating component */
    if( *cmptop >= MAX_CHARS )
      Error(37, 8, "too many composites in file %s (at line %d)",
	FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
    cmp[*cmptop].char_code = (FULL_CHAR) '\0';
    (*cmptop)++;
  }
  if( status == (char *) NULL ||
	!StringBeginsWith(buff, AsciiToFull("EndBuildComposites")) )
    Error(37, 9, "missing EndBuildComposites in extra font file %s (line %d)",
      FATAL, &fpos(Extrafilename), FileName(extra_fnum), *lnum);
} /* end ReadCompositeMetrics */


/*@::FontRead()@**************************************************************/
/*                                                                           */
/*  static OBJECT FontRead(FULL_CHAR *family_name, *face_name, OBJECT err)   */
/*                                                                           */
/*  Search the font databases for a font with this family and face name.     */
/*  If found, read the font and update this module's data structures, then   */
/*  return the face object.                                                  */
/*                                                                           */
/*  If an error occurs, use fpos(err) for reporting its location if nothing  */
/*  better suggests itself.                                                  */
/*                                                                           */
/*****************************************************************************/

static OBJECT FontRead(FULL_CHAR *family_name, FULL_CHAR *face_name, OBJECT err)
{
  OBJECT cs, link, db, fontdef_obj, y, ylink;
  FULL_CHAR tag[100], seq[100];
  FILE_NUM dfnum; long dfpos, cont; int dlnum;
  BOOLEAN font_name_found;
  OBJECT family, face, font_name, AFMfilename, Extrafilename, LCMfilename;
  OBJECT recode, first_size;
  FULL_CHAR buff[MAX_BUFF], command[MAX_BUFF], ch;
  char *status;
  int xheight2, i, lnum, ligtop, cmptop;
  float fl_xheight2, fl_under_pos, fl_under_thick;
  int under_pos, under_thick;
  BOOLEAN upfound, utfound, xhfound;
  BOOLEAN fixed_pitch = FALSE;
  FILE_NUM fnum, extra_fnum;  FILE *fp, *extra_fp;
  struct metrics *fnt;
  FULL_CHAR *lig;  unsigned short *composite;  COMPOSITE *cmp;
  unsigned short *kt;  FULL_CHAR *kc;  unsigned char *kv;  SHORT_LENGTH *ks;
  debug2(DFT, D, "FontRead(%s, %s)", family_name, face_name);


  /***************************************************************************/
  /*                                                                         */
  /*  Get the @FontDef object with tag family_name-face_name from databases  */
  /*                                                                         */
  /***************************************************************************/

  /* if no databases available, fatal error */
  cs = cross_sym(FontDefSym);
  if( cs == nilobj )
  { Error(37, 10, "unable to set font %s %s (no font databases loaded)",
      FATAL, no_fpos, family_name, face_name);
  }

  /* search the databases for @FontDef @Tag { family-face } */
  sprintf( (char *) tag, "%s-%s", family_name, face_name);
  for( link = NextUp(Up(cs));  link != cs;  link = NextUp(link) )
  { Parent(db, link);
    if( DbRetrieve(db, FALSE, FontDefSym,tag,seq,&dfnum,&dfpos,&dlnum,&cont) )
      break;
  }

  /* if not found, return nilobj */
  if( link == cs )
  { debug0(DFT, D, "FontRead returning nilobj (not in any database)");
    return nilobj;
  }

  /* found it; read @FontDef object from database file */
  SwitchScope(nilobj);
  fontdef_obj = ReadFromFile(dfnum, dfpos, dlnum);
  UnSwitchScope(nilobj);
  if( fontdef_obj == nilobj )
    Error(37, 11, "cannot read %s for %s", INTERN, no_fpos, KW_FONTDEF, tag);


  /***************************************************************************/
  /*                                                                         */
  /*  Extract the attributes of fontdef_obj, and check that they are OK.     */
  /*                                                                         */
  /***************************************************************************/

  /* extract the various attributes */
  family = face = font_name = AFMfilename = nilobj;
  Extrafilename = LCMfilename = recode = nilobj;
  for( ylink=Down(fontdef_obj);  ylink != fontdef_obj;  ylink=NextDown(ylink) )
  { Child(y, ylink);
    assert( type(y) == PAR, "FontRead: type(y) != PAR!" );
    if( actual(y) == fd_tag )
    {
      /* do nothing with this one */
    }
    else if( actual(y) == fd_family )
    { Child(family, Down(y));
      if( !is_word(type(family)) || !StringEqual(string(family), family_name) )
	Error(37, 12, "font family name %s incompatible with %s value %s",
	  FATAL, &fpos(fontdef_obj), string(family), KW_TAG, tag);
    }
    else if( actual(y) == fd_face )
    { Child(face, Down(y));
      if( !is_word(type(face)) || !StringEqual(string(face), face_name) )
	Error(37, 13, "font face name %s incompatible with %s value %s",
	  FATAL, &fpos(fontdef_obj), string(face), KW_TAG, tag);
    }
    else if( actual(y) == fd_name )
    { Child(font_name, Down(y));
      font_name = ReplaceWithTidy(font_name, TRUE);
      if( !is_word(type(font_name)) )
	Error(37, 14, "illegal font name (quotes needed?)",
	    FATAL, &fpos(font_name));
    }
    else if( actual(y) == fd_metrics )
    { Child(AFMfilename, Down(y));
      AFMfilename = ReplaceWithTidy(AFMfilename, TRUE);
      if( !is_word(type(AFMfilename)) )
	Error(37, 15, "illegal font metrics file name (quotes needed?)",
	  FATAL, &fpos(AFMfilename));
    }
    else if( actual(y) == fd_extra_metrics )
    { Child(Extrafilename, Down(y));
      Extrafilename = ReplaceWithTidy(Extrafilename, TRUE);
      if( !is_word(type(Extrafilename)) )
	Error(37, 16, "illegal font extra metrics file name (quotes needed?)",
	  FATAL, &fpos(Extrafilename));
    }
    else if( actual(y) == fd_mapping )
    { Child(LCMfilename, Down(y));
      LCMfilename = ReplaceWithTidy(LCMfilename, TRUE);
      if( !is_word(type(LCMfilename)) )
	Error(37, 17, "illegal mapping file name (quotes needed?)",
	  FATAL, &fpos(LCMfilename));
    }
    else if( actual(y) == fd_recode )
    { Child(recode, Down(y));
      recode = ReplaceWithTidy(recode, TRUE);
      if( !is_word(type(recode)) )
	Error(37, 18, "illegal value of %s", FATAL, &fpos(recode),
	  SymName(fd_recode));
    }
    else
    { assert(FALSE, "FontRead: cannot identify component of FontDef")
    }

  }

  /* check that all the compulsory ones were found */
  /* a warning message will have already been given if not */
  if( family == nilobj || face == nilobj || font_name == nilobj ||
      AFMfilename  == nilobj || LCMfilename == nilobj )
  {
    debug0(DFT, D, "FontRead returning nilobj (missing compulsory)");
    return nilobj;
  }


  /***************************************************************************/
  /*                                                                         */
  /*  Update font tree to have this family, face and first_size.             */
  /*                                                                         */
  /***************************************************************************/

  /* insert family into font tree if not already present */
  for( link = Down(font_root);  link != font_root;  link = NextDown(link) )
  { Child(y, link);
    if( StringEqual(string(y), string(family)) )
    { family = y;
      break;
    }
  }
  if( link == font_root )
    MoveLink(Up(family), font_root, PARENT);

  /* insert face into family, or error if already present */
  for( link = Down(family);  link != family;  link = NextDown(link) )
  { Child(y, link);
    if( StringEqual(string(y), string(face)) )
    { Error(37, 19, "font %s %s already defined, at%s", WARN, &fpos(face),
	string(family), string(face), EchoFilePos(&fpos(y)));
      debug0(DFT, D, "FontRead returning: font already defined");
      DisposeObject(fontdef_obj);
      return y;
    }
  }
  MoveLink(Up(face), family, PARENT);

  /* PostScript name and AFM file name are first two children of face */
  Link(face, font_name);
  Link(face, AFMfilename);

  /* AFM file name has extra file name as optional child */
  if( Extrafilename != nilobj )
    Link(AFMfilename, Extrafilename);

  /* load character mapping file */
  if( recode != nilobj && StringEqual(string(recode), AsciiToFull("No")) )
  { font_recoded(face) = FALSE;
    font_mapping(face) = MapLoad(LCMfilename, FALSE);
  }
  else if( recode == nilobj || StringEqual(string(recode), AsciiToFull("Yes")) )
  { font_recoded(face) = TRUE;
    font_mapping(face) = MapLoad(LCMfilename, TRUE);
  }
  else Error(37, 20, "expecting either Yes or No here", FATAL, &fpos(recode));

  /* say that this font is currently unused on any page */
  font_page(face) = 0;

  /* get a new number for this (default) font size */
  if( ++font_count >= finfo_size )
  { if( font_count > MAX_FONT )
      Error(37, 21, "too many different fonts and sizes (maximum is %d)",
	FATAL, &fpos(err),MAX_FONT);
    ifdebug(DMA, D,
      DebugRegisterUsage(MEM_FONTS, -1, -finfo_size * sizeof(FONT_INFO)));
    finfo_size *= 2;
    ifdebug(DMA, D,
      DebugRegisterUsage(MEM_FONTS, 1, finfo_size * sizeof(FONT_INFO)));
    finfo = (FONT_INFO *) realloc(finfo, finfo_size * sizeof(FONT_INFO));
    if( finfo == (FONT_INFO *) NULL )
      Error(37, 22, "run out of memory when increasing font table size",
	FATAL, &fpos(err));
  }

  /* build the first size record, and initialize it with what we know now */
  first_size = MakeWordTwo(WORD, AsciiToFull("fnt"), StringInt(++font_seqnum),
    no_fpos);
  Link(face, first_size);
  font_num(first_size) = font_count;
  font_size(first_size) = BackEnd->uses_font_metrics ? SZ_DFT : PlainCharHeight;
  font_recoded(first_size) = font_recoded(face);
  font_mapping(first_size) = font_mapping(face);
  font_num(face) = font_num(first_size); /* Uwe's suggestion, helps PDF */
  /* leaves font_xheight2 and font_spacewidth still to do */


  /***************************************************************************/
  /*                                                                         */
  /*  Read the Adobe font metrics file, and record what's in it.             */
  /*                                                                         */
  /***************************************************************************/

  /* open the Adobe font metrics (AFM) file of the font */
  debug0(DFS, D, "  calling DefineFile from FontRead");
  fnum = DefineFile(string(AFMfilename), STR_EMPTY, &fpos(AFMfilename),
    FONT_FILE, FONT_PATH);
  fp = OpenFile(fnum, FALSE, FALSE);
  if( fp == NULL )
    Error(37, 23, "cannot open font file %s", FATAL, &fpos(AFMfilename),
      FileName(fnum));

  /* check that the AFM file begins, as it should, with "StartFontMetrics" */
  if( StringFGets(buff, MAX_BUFF, fp) == NULL ||
	sscanf( (char *) buff, "%s", command) != 1 ||
	!StringEqual(command, "StartFontMetrics")  )
  { debug1(DFT, DD, "first line of AFM file:%s", buff);
    debug1(DFT, DD, "command:%s", command);
    Error(37, 24, "font file %s does not begin with StartFontMetrics",
      FATAL, &fpos(AFMfilename), FileName(fnum));
  }

  /* initialise font metrics table for the new font */
  ifdebug(DMA, D,
      DebugRegisterUsage(MEM_FONTS, 1, MAX_CHARS * sizeof(struct metrics)));
  fnt = (struct metrics *) malloc(MAX_CHARS * sizeof(struct metrics));
  if( fnt == (struct metrics *) NULL )
    Error(37, 25, "run out of memory while reading font file %s",
      FATAL, &fpos(err), FileName(fnum));
  ifdebug(DMA, D,
      DebugRegisterUsage(MEM_FONTS, 0, 2*MAX_CHARS*sizeof(FULL_CHAR)));

  /* initialise ligature table for the new font */
  lig = (FULL_CHAR *) malloc(2*MAX_CHARS*sizeof(FULL_CHAR));
  if( lig == (FULL_CHAR *) NULL )
    Error(37, 25, "run out of memory while reading font file %s",
      FATAL, &fpos(err), FileName(fnum));
  for( i = 0;  i < MAX_CHARS;  i++ )  lig[i] = 1;	/* i.e. char unknown */
  ligtop = MAX_CHARS+2;		/* must avoid ligtop - MAX_CHARS == 0 or 1 */

  /* initialise composites table for the new font */
  composite = (unsigned short *) malloc(MAX_CHARS * sizeof(unsigned short));
  if( composite == (unsigned short *) NULL )
    Error(37, 25, "run out of memory while reading font file %s",
      FATAL, &fpos(err), FileName(fnum));
  cmp = (COMPOSITE *) malloc(MAX_CHARS * sizeof(COMPOSITE));
  if( cmp == (COMPOSITE *) NULL )
    Error(37, 25, "run out of memory while reading font file %s",
      FATAL, &fpos(err), FileName(fnum));
  for( i = 0;  i < MAX_CHARS;  i++ )  composite[i] = 0;	/* i.e. not composite */
  cmptop = 1;   /* must avoid cmptop == 0 */

  /* initialise kerning table for the new font */
  ifdebug(DMA, D,
      DebugRegisterUsage(MEM_FONTS, 0, MAX_CHARS * sizeof(unsigned short)));
  kt = (unsigned short *) malloc(MAX_CHARS * sizeof(unsigned short));
  if( kt == (unsigned short *) NULL )
    Error(37, 25, "run out of memory while reading font file %s",
      FATAL, &fpos(err), FileName(fnum));
  for( i = 0;  i < MAX_CHARS;  i++ )  kt[i] = 0;  /* i.e. no kerns */
  ks = (SHORT_LENGTH *) NULL;			  /* i.e. no kern sizes */

  /* read font metrics file fp */
  xhfound = upfound = utfound = FALSE;
  xheight2 = under_thick = under_pos = 0;
  kc = (FULL_CHAR *) NULL;
  kv = (unsigned char *) NULL;
  ks = (SHORT_LENGTH *) NULL;
  font_name_found = FALSE;  lnum = 1;
  while( (status = StringFGets(buff, MAX_BUFF, fp)) != (char *) NULL &&
    !(buff[0] == 'E' && StringEqual(buff, AsciiToFull("EndFontMetrics\n"))) )
  {
    lnum++;
    sscanf( (char *) buff, "%s", command);
    switch( command[0] )
    {

      case 'U':

	if( StringEqual(command, AsciiToFull("UnderlinePosition")) ) 
	{ if( upfound )
	  { Error(37, 26, "UnderlinePosition found twice in font file (line %d)",
	      FATAL, &fpos(AFMfilename), lnum);
	  }
	  sscanf( (char *) buff, "UnderlinePosition %f", &fl_under_pos);
	  under_pos = fl_under_pos;
	  upfound = TRUE;
	}
	else if( StringEqual(command, AsciiToFull("UnderlineThickness")) ) 
	{ if( utfound )
	  { Error(37, 27, "UnderlineThickness found twice in font file (line %d)",
	      FATAL, &fpos(AFMfilename), lnum);
	  }
	  sscanf( (char *) buff, "UnderlineThickness %f", &fl_under_thick);
	  under_thick = fl_under_thick;
	  utfound = TRUE;
	}
	break;


      case 'X':

	if( StringEqual(command, AsciiToFull("XHeight")) ) 
	{ if( xhfound )
	  { Error(37, 28, "XHeight found twice in font file (line %d)",
	      FATAL, &fpos(AFMfilename), lnum);
	  }
	  sscanf( (char *) buff, "XHeight %f", &fl_xheight2);
	  xheight2 = fl_xheight2 / 2;
	  xhfound = TRUE;
	}
	break;


      case 'F':

	if( StringEqual(command, AsciiToFull("FontName")) )
	{ if( font_name_found )
	  { Error(37, 29, "FontName found twice in font file %s (line %d)",
	      FATAL, &fpos(AFMfilename), FileName(fnum), lnum);
	  }
	  sscanf( (char *) buff, "FontName %s", command);
	  if( StringEqual(command, STR_EMPTY) )
	  { Error(37, 30, "FontName empty in font file %s (line %d)",
	      FATAL, &fpos(AFMfilename), FileName(fnum), lnum);
	  }
	  Child(y, Down(face));
	  if( !StringEqual(command, string(y)) )
	  Error(37, 31, "FontName in font file (%s) and %s (%s) disagree",
	    WARN, &fpos(AFMfilename), command, KW_FONTDEF, string(y));
	  font_name_found = TRUE;
	}
	break;


      case 'I':

	if( StringEqual(command, AsciiToFull("IsFixedPitch")) )
	{ 
	  sscanf( (char *) buff, "IsFixedPitch %s", command);
	  if( StringEqual(command, AsciiToFull("true")) )
	  { fixed_pitch = TRUE;
	  }
	}
	break;


      case 'S':

	if( StringEqual(command, AsciiToFull("StartCharMetrics")) )
	{
	  if( !font_name_found )
	    Error(37, 32, "FontName missing in file %s",
	      FATAL, &fpos(AFMfilename), FileName(fnum));
	  if( !xhfound )  xheight2 = DEFAULT_XHEIGHT / 2;
	  ReadCharMetrics(face, fixed_pitch, xheight2, lig, &ligtop,
	    fnum, fnt, &lnum, fp);
	}
	else if( BackEnd->uses_font_metrics && Kern &&
	  StringEqual(command, AsciiToFull("StartKernPairs")) )
	{ FULL_CHAR ch1, ch2, last_ch1;
	  FULL_CHAR name1[30], name2[30];
	  int kc_top, ks_top, pos, num_pairs, ksize;  float fl_ksize;

	  if( sscanf( (char *) buff, "StartKernPairs %d", &num_pairs) != 1 )
	    Error(37, 33, "syntax error on StartKernPairs line in font file %s (line %d)",
	      FATAL, &fpos(AFMfilename), FileName(fnum), lnum);
	  kc_top = 1;  ks_top = 1;
	  ifdebug(DMA, D,
	    DebugRegisterUsage(MEM_FONTS, 0, 2*num_pairs * sizeof(FULL_CHAR)));
	  kc = (FULL_CHAR *) malloc(2 * num_pairs * sizeof(FULL_CHAR));
	  ifdebug(DMA, D, DebugRegisterUsage(MEM_FONTS, 0,
	    2 * num_pairs * sizeof(unsigned char)));
	  kv = (unsigned char *) malloc(2 * num_pairs * sizeof(unsigned char));
	  ifdebug(DMA, D, DebugRegisterUsage(MEM_FONTS, 0,
	    num_pairs * sizeof(SHORT_LENGTH)));
	  ks = (SHORT_LENGTH *) malloc(num_pairs * sizeof(SHORT_LENGTH));
	  last_ch1 = '\0';
	  while( StringFGets(buff, MAX_BUFF, fp) == (char *) buff &&
	    !StringBeginsWith(buff, AsciiToFull("EndKernPairs")) )
	  {
	    debug1(DFT, DD, "FontRead reading %s", buff);
	    lnum++;
	    if( StringBeginsWith(buff, AsciiToFull("KPX")) )
	    {
	      /* get the two character names and kern size from buff */
	      if( sscanf((char *)buff, "KPX %s %s %f",name1,name2,&fl_ksize)!=3 )
		Error(37, 34, "syntax error in font file %s (line %d): %s",
		  FATAL, &fpos(AFMfilename), FileName(fnum), lnum, buff);

	      /* ignore size 0 kern pairs (they are frequent, why?) */
	      ksize = fl_ksize;
	      if( ksize == 0 )  continue;

	      /* check that both characters are encoded */
	      ch1 = MapCharEncoding(name1, font_mapping(face));
	      if( ch1 == '\0' )
	      {
		continue;
	      }
	      ch2 = MapCharEncoding(name2, font_mapping(face));
	      if( ch2 == '\0' )
	      {
		continue;
	      }

	      /* check that ch1 is contiguous with previous occurrences */
	      if( ch1 != last_ch1 && kt[ch1] != 0 )
	      { Error(37, 35, "non-contiguous kerning pair %s %s in font file %s (line %d)",
		  WARN, &fpos(AFMfilename), name1, name2, FileName(fnum), lnum);
		continue;
	      }
	      last_ch1 = ch1;

	      /* if ch1 never seen before, make new entry in kt[] and kc[] */
	      if( kt[ch1] == 0 )
	      { debug2(DFT, DD, "  kt[%d] = %d", ch1, kc_top);
		kt[ch1] = kc_top;
		kc[kc_top] = (FULL_CHAR) '\0';
		kv[kc_top] = 0;
		kc_top++;
	      }

	      /* find kerning size in ks[] or else add it to the end */
	      for( pos = 1;  pos < ks_top;  pos++ )
	      { if( ks[pos] == ksize )  break;
	      }
	      if( pos == ks_top )
	      { if( ks_top == num_pairs )
		  Error(37, 36, "too many kerning pairs in font file %s (line %d)",
		    FATAL, &fpos(AFMfilename), FileName(fnum), lnum);
		debug2(DFT, DD, "  ks[%d] = %d", pos, ksize);
		ks[pos] = ksize;
		ks_top++;
	      }

	      /* insert ch2 into the kc entries (sorted decreasing) for ch1 */
	      for( i = kc_top-1; i >= kt[ch1] && kc[i] < ch2;  i-- )
	      { kc[i+1] = kc[i];
		kv[i+1] = kv[i];
	      }
	      if( i >= kt[ch1] && kc[i] == ch2 )
		Error(37, 37, "kerning pair %s %s appears twice in font file %s (line %d)",
		  FATAL, &fpos(AFMfilename), name1, name2, FileName(fnum), lnum);
	      kc[i+1] = ch2;
	      kv[i+1] = pos;
	      kc_top++;
	    }
	  }
	  ks[0] = ks_top;
	}
	break;


      default:

	break;

    }
  }

  /* make sure we terminated the font metrics file gracefully */
  if( status == (char *) NULL )
    Error(37, 38, "EndFontMetrics missing from font file %s",
      FATAL, &fpos(AFMfilename), FileName(fnum));
  fclose(fp);
  fp = (FILE *) NULL;

  /* complete the initialization of first_size */
  font_xheight2(first_size) =
    BackEnd->uses_font_metrics ? xheight2 : PlainCharHeight / 4;
  ch = MapCharEncoding(STR_PS_SPACENAME, font_mapping(first_size));
  font_spacewidth(first_size) = ch == '\0' ? 0 : fnt[ch].right;


  /***************************************************************************/
  /*                                                                         */
  /*  Read the optional Extra font metrics file, and record what's in it.    */
  /*                                                                         */
  /***************************************************************************/

  if( Extrafilename != nilobj )
  { debug0(DFS, D, "  calling DefineFile from FontRead (extra_filename)");
    extra_fnum = DefineFile(string(Extrafilename), STR_EMPTY,
      &fpos(Extrafilename), FONT_FILE, FONT_PATH);
    extra_fp = OpenFile(extra_fnum, FALSE, FALSE);
    if( extra_fp == NULL )
      Error(37, 39, "cannot open extra font file %s", FATAL,
	&fpos(Extrafilename), FileName(extra_fnum));
    lnum = 0;

    while( StringFGets(buff, MAX_BUFF, extra_fp) != (char *) NULL )
    {
      debug1(DFT, D, "  Extra: %s", buff);
      lnum++;
      sscanf( (char *) buff, "%s", command);
      if( command[0] == 'S' )
      {
	if( StringEqual(command, AsciiToFull("StartExtraCharMetrics")) )
	{
	  /* get extra character metrics, just like the others */
	  debug0(DFT, D, "  StartExtraCharMetrics calling ReadCharMetrics");
	  ReadCharMetrics(face, fixed_pitch, xheight2, lig, &ligtop,
	    extra_fnum, fnt, &lnum, extra_fp);
	}
	else if( StringEqual(command, AsciiToFull("StartBuildComposites")) )
	{ 
	  /* build composites */
	  debug0(DFT, D, "  StartBuildComposites");
	  ReadCompositeMetrics(face, Extrafilename, extra_fnum, &lnum,
	    composite, cmp, &cmptop, extra_fp);
	}
      }
    }

    fclose(extra_fp);
    extra_fp = (FILE *) NULL;
  }


  /***************************************************************************/
  /*                                                                         */
  /*  Set finfo[fontcount] and exit.                                         */
  /*                                                                         */
  /***************************************************************************/

  finfo[font_count].font_table = first_size;
  finfo[font_count].original_face = face;
  finfo[font_count].underline_pos = xheight2 - under_pos;
  finfo[font_count].underline_thick = under_thick;
  finfo[font_count].size_table = fnt;
  finfo[font_count].lig_table = lig;
  finfo[font_count].composite = composite;
  finfo[font_count].cmp_table = cmp;
  finfo[font_count].cmp_top = cmptop;
  finfo[font_count].kern_table = kt;
  finfo[font_count].kern_chars = kc;
  finfo[font_count].kern_value = kv;
  finfo[font_count].kern_sizes = ks;

  ifdebug(DFT, DD, DebugKernTable(font_count));
  debug4(DFT, D, "FontRead returning: %d, name %s, fs %d, xh2 %d",
    font_count, string(first_size), font_size(first_size), xheight2);
  return face;

} /* end FontRead */


/*@::FontChange()@************************************************************/
/*                                                                           */
/*  FontChange(style, x)                                                     */
/*                                                                           */
/*  Returns an internal font number which is the current font changed        */
/*  according to word object x.  e.g. if current font is Roman 12p and x is  */
/*  "-3p", then FontChange returns the internal font number of Roman 9p.     */
/*                                                                           */
/*  FontChange permits empty and null objects within x; these have no        */
/*  effect.                                                                  */
/*                                                                           */
/*****************************************************************************/

void FontChange(STYLE *style, OBJECT x)
{ /* register */ int i;
  OBJECT requested_family, requested_face, requested_size;
  OBJECT par[3], family, face, fsize, y, link, new, old, tmpf;
  GAP gp;  SHORT_LENGTH flen;  int num, c;  unsigned inc;
  struct metrics *newfnt, *oldfnt;
  FULL_CHAR *lig;
  int cmptop;
  COMPOSITE *oldcmp, *newcmp;
  SHORT_LENGTH *oldks, *newks;  int klen;
  debug2(DFT, D, "FontChange( %s, %s )", EchoStyle(style), EchoObject(x));
  assert( font(*style) <= font_count, "FontChange: font_count!");
  ifdebug(DFT, DD, FontDebug());

  /***************************************************************************/
  /*                                                                         */
  /*  Analyse x, doing any small-caps style changes immediately, and putting */
  /*  all the other words of x into par[0 .. num-1] for further analysis.    */
  /*                                                                         */
  /***************************************************************************/

  num = 0;
  if( type(x) == NULL_CLOS )
  { /* acceptable, but do nothing */
  }
  else if( is_word(type(x)) )
  {
    if( StringEqual(string(x), STR_SMALL_CAPS_ON) )
      small_caps(*style) = SMALL_CAPS_ON;
    else if( StringEqual(string(x), STR_SMALL_CAPS_OFF) )
      small_caps(*style) = SMALL_CAPS_OFF;
    else if( !StringEqual(string(x), STR_EMPTY) )
      par[num++] = x; 
  }
  else if( type(x) == ACAT )
  { for( link = Down(x);  link != x;  link = NextDown(link) )
    { Child(y, link);
      debug1(DFT, DDD, "  pars examining y = %s", EchoObject(y));
      if( type(y) == GAP_OBJ || type(y)  == NULL_CLOS )  continue;
      if( is_word(type(y)) ) 
      {
	if( StringEqual(string(y), STR_SMALL_CAPS_ON) )
	  small_caps(*style) = SMALL_CAPS_ON;
	else if( StringEqual(string(y), STR_SMALL_CAPS_OFF) )
	  small_caps(*style) = SMALL_CAPS_OFF;
	else if( !StringEqual(string(y), STR_EMPTY) )
	{
	  if( num >= 3 )
	  { Error(37, 40, "error in left parameter of %s",
	      WARN, &fpos(x), KW_FONT);
	    debug0(DFT, D, "FontChange returning: ACAT children");
	    return;
	  }
	  par[num++] = y; 
	}
      }
      else
      {	Error(37, 41, "error in left parameter of %s",
	  WARN, &fpos(x), KW_FONT);
	debug0(DFT, D, "FontChange returning: ACAT children");
	return;
      }
    }
  }
  else
  { Error(37, 42, "error in left parameter of %s", WARN, &fpos(x), KW_FONT);
    debug0(DFT, D, "FontChange returning: wrong type");
    return;
  }
  debug1(DFT, DDD, " found pars, num = %d", num);
  if( num == 0 )
  { debug1(DFT, D, "FontChange returning %s", EchoStyle(style));
    return;
  }

  /***************************************************************************/
  /*                                                                         */
  /* Extract size, family, and face changes (if any) from par[0 .. num-1].   */
  /*                                                                         */
  /***************************************************************************/

  /* extract fsize parameter, if any */
  assert( num >= 1 && num <= 3, "FontChange: num!" );
  requested_size = nilobj;
  for( i = 0;  i < num;  i++ )
  {
    c = string(par[i])[0];
    if( c == CH_INCGAP || c == CH_DECGAP || decimaldigit(c) )
    {
      /* extract fsize, shuffle the rest down */
      requested_size = par[i];
      for( i = i + 1;  i < num;  i++ )
	par[i-1] = par[i];
      num--;
    }
  }

  /* what remains must be family and face */
  switch( num )
  {
    case 0:

      requested_family = requested_face = nilobj;
      break;


    case 1:

      requested_family = nilobj;
      requested_face = par[0];
      break;


    case 2:

      requested_family = par[0];
      requested_face = par[1];
      break;


    default:

      Error(37, 43, "error in left parameter of %s", WARN, &fpos(x), KW_FONT);
      debug0(DFT, D, "FontChange returning: too many parameters");
      return;
      break;
  }

  /* check for initial font case: must have family, face, and size */
  if( font(*style) == NO_FONT && (requested_size == nilobj ||
	requested_family == nilobj || requested_face == nilobj) )
    Error(37, 44, "initial font must have family, face and size",
      FATAL, &fpos(x));


  /***************************************************************************/
  /*                                                                         */
  /*  Either find the family and face already existing, or load them.        */
  /*                                                                         */
  /***************************************************************************/

  /* get font family */
  family = nilobj;
  if( requested_family != nilobj )
  {
    /* search for this family */
    for( link = Down(font_root);  link != font_root;  link = NextDown(link) )
    { Child(y, link);
      if( StringEqual(string(requested_family), string(y)) )  break;
    }
    if( link != font_root )
      family = y;
  }
  else
  {
    /* preserve current family */
    assert( Up(finfo[font(*style)].font_table)!=finfo[font(*style)].font_table,
      "FontChange: Up(finfo[font(*style)].font_table) !" );
    Parent(tmpf, Up(finfo[font(*style)].font_table));
    assert( is_word(type(tmpf)), "FontChange: type(tmpf)!" );
    assert( Up(tmpf) != tmpf, "FontChange: Up(tmpf)!" );
    Parent(family, Up(tmpf));
    assert( is_word(type(family)), "FontChange: type(family)!" );
  }

  /* get font face, if have family */
  face = nilobj;
  if( family != nilobj )
  {
    if( requested_face != nilobj )
    {
      /* search for this face in family */
      for( link = Down(family);  link != family;  link = NextDown(link) )
      {	Child(y, link);
	if( StringEqual(string(requested_face), string(y)) )  break;
      }
      if( link != family )
	face = y;
    }
    else
    {
      /* preserve current face */
      Parent(face, Up(finfo[font(*style)].font_table));
      assert( is_word(type(face)), "FontChange: type(face)!" );
      assert( Up(face) != face, "FontChange: Up(face)!" );
    }
  }

  if( face == nilobj )
  {
    /* face not loaded, try the font databases */
    assert( family != nilobj || requested_family != nilobj, "FontChange fr!" );
    assert( requested_face != nilobj, "FontChange requested_face!");
    if( family != nilobj )
      requested_family = family;
    face = FontRead(string(requested_family), string(requested_face), x);

    if( face == nilobj )
    {
      /* missing face name error; check whether a family name was intended */
      for( link = Down(font_root); link != font_root; link = NextDown(link) )
      { Child(y, link);
	if( StringEqual(string(y), string(requested_face)) )  break;
      }
      if( link != font_root )
	Error(37, 45, "font family name %s must be followed by a face name",
	  WARN, &fpos(requested_face), string(requested_face));
      else
	Error(37, 46, "there is no font with family name %s and face name %s",
	  WARN, &fpos(requested_face), string(requested_family),
	  string(requested_face));
      debug0(DFT, D, "FontChange returning (unable to set face)");
      return;
    }
  }

  assert( Down(face) != face, "FontChange: no children!" );
  assert( NextDown(Down(face)) != face, "FontChange: 1 child!" );
  assert( NextDown(NextDown(Down(face))) != face, "FontChange: 2 children!" );

  /***************************************************************************/
  /*                                                                         */
  /*  Now have family and face; search for size and return it if found.      */
  /*                                                                         */
  /***************************************************************************/

  /* get font size as integer flen */
  if( requested_size == nilobj )
    flen = font_size(finfo[font(*style)].font_table);
  else 
  { GetGap(requested_size, style, &gp, &inc);
    if( mode(gp) != EDGE_MODE || units(gp) != FIXED_UNIT )
    { Error(37, 47, "syntax error in font size %s; ignoring it",
	WARN, &fpos(requested_size), string(requested_size));
      flen = font_size(finfo[font(*style)].font_table);
    }
    else if( inc == GAP_ABS )
      flen = width(gp);
    else if( font(*style) == NO_FONT )
    { Error(37, 48, "no current font on which to base size change %s",
	FATAL, &fpos(requested_size), string(requested_size));
    }
    else if( inc == GAP_INC )
      flen = font_size(finfo[font(*style)].font_table) + width(gp);
    else if( inc == GAP_DEC )
      flen = font_size(finfo[font(*style)].font_table) - width(gp);
    else Error(37, 49, "FontChange: %d", INTERN, &fpos(x), inc);
  }

  if( flen <= 0 )
  { Error(37, 50, "%s %s ignored (result is not positive)",
      WARN, &fpos(requested_size), string(requested_size), KW_FONT);
    debug0(DFT, D,"FontChange returning (non-positive size)");
    return;
  }

  /* search fonts of face for desired size; return if already present */
  if( !(BackEnd->uses_font_metrics) )  flen = PlainCharHeight;
  for( link=NextDown(NextDown(Down(face))); link!=face; link = NextDown(link) )
  { Child(fsize, link);
    if( font_size(fsize) == flen )
    { font(*style) = font_num(fsize);
      SetGap(space_gap(*style), nobreak(space_gap(*style)), FALSE, TRUE,
	FIXED_UNIT, EDGE_MODE, font_spacewidth(fsize));
      debug2(DFT, D,"FontChange returning (old) %d (XHeight2 = %d)",
	font(*style), font_xheight2(finfo[font(*style)].font_table));
      return;
    }
  }

  /***************************************************************************/
  /*                                                                         */
  /*  No suitable size right now, so scale the original size and exit.       */
  /*                                                                         */
  /***************************************************************************/

  /* get a new number for this new size */
  if( ++font_count >= finfo_size )
  { if( font_count > MAX_FONT )
      Error(37, 51, "too many different fonts and sizes (max is %d)",
	FATAL, &fpos(x), MAX_FONT);
    ifdebug(DMA, D, DebugRegisterUsage(MEM_FONTS, -1,
      -finfo_size * sizeof(FONT_INFO)));
    finfo_size *= 2;
    ifdebug(DMA, D, DebugRegisterUsage(MEM_FONTS, 1,
      finfo_size * sizeof(FONT_INFO)));
    finfo = (FONT_INFO *) realloc(finfo, finfo_size * sizeof(FONT_INFO));
    if( finfo == (FONT_INFO *) NULL )
      Error(37, 52, "run out of memory when increasing font table size",
	FATAL, &fpos(x));
  }

  /* create a new sized font record */
  Child(old, NextDown(NextDown(Down(face))));
  assert( is_word(type(old)), "FontChange: old!" );
  new = MakeWord(WORD, string(old), no_fpos);
  Link(face, new);
  font_num(new)         = font_count;
  font_size(new)        = BackEnd->uses_font_metrics ? flen : font_size(old);
  font_xheight2(new)    = font_xheight2(old) * font_size(new) / font_size(old);
  font_recoded(new)	= font_recoded(old);
  font_mapping(new)	= font_mapping(old);
  font_spacewidth(new)	= font_spacewidth(old) * font_size(new)/font_size(old);
  finfo[font_count].font_table = new;
  finfo[font_count].original_face = face;
  finfo[font_count].underline_pos =
    (finfo[font_num(old)].underline_pos * font_size(new)) / font_size(old);
  finfo[font_count].underline_thick =
    (finfo[font_num(old)].underline_thick * font_size(new)) / font_size(old);
  ifdebug(DMA, D, DebugRegisterUsage(MEM_FONTS, 1,
      MAX_CHARS * sizeof(struct metrics)));
  finfo[font_count].size_table =
    (struct metrics *) malloc(MAX_CHARS * sizeof(struct metrics));
  if( finfo[font_count].size_table == (struct metrics *) NULL )
    Error(37, 53, "run out of memory when changing font or font size",
      FATAL, &fpos(x));
  finfo[font_count].lig_table  = lig = finfo[font_num(old)].lig_table;

  /* scale old font to new size */
  newfnt = finfo[font_num(new)].size_table;
  oldfnt = finfo[font_num(old)].size_table;
  for( i = 0;  i < MAX_CHARS;  i++ )  if( lig[i] != 1 )
  { newfnt[i].left  = (oldfnt[i].left  * font_size(new)) / font_size(old);
    newfnt[i].right = (oldfnt[i].right * font_size(new)) / font_size(old);
    newfnt[i].down  = (oldfnt[i].down  * font_size(new)) / font_size(old);
    newfnt[i].up    = (oldfnt[i].up    * font_size(new)) / font_size(old);
    newfnt[i].last_adjust = (oldfnt[i].last_adjust * font_size(new)) / font_size(old);
  }

  /* copy and scale composite table */
  finfo[font_count].composite = finfo[font_num(old)].composite;
  finfo[font_count].cmp_top = cmptop = finfo[font_num(old)].cmp_top;
  oldcmp = finfo[font_num(old)].cmp_table;
  newcmp = (COMPOSITE *) malloc(cmptop*sizeof(COMPOSITE));
  if( newcmp == (COMPOSITE *) NULL )
    Error(37, 54, "run out of memory when changing font or font size",
      FATAL, &fpos(x));
  for( i = 1;  i < cmptop;  i++ )  /* NB position 0 is unused */
  { newcmp[i].char_code = oldcmp[i].char_code;
    if( newcmp[i].char_code != (FULL_CHAR) '\0' )
    { newcmp[i].x_offset = (oldcmp[i].x_offset*font_size(new)) / font_size(old);
      newcmp[i].y_offset = (oldcmp[i].y_offset*font_size(new)) / font_size(old);
      debug5(DFT, D, "FontChange scales composite %d from (%d, %d) to (%d, %d)",
	(int) newcmp[i].char_code, oldcmp[i].x_offset, oldcmp[i].y_offset,
	newcmp[i].x_offset, newcmp[i].y_offset);
    }
  }
  finfo[font_count].cmp_table = newcmp;

  /* copy and scale kerning tables */
  finfo[font_count].kern_table = finfo[font_num(old)].kern_table;
  finfo[font_count].kern_chars = finfo[font_num(old)].kern_chars;
  finfo[font_count].kern_value = finfo[font_num(old)].kern_value;
  oldks = finfo[font_num(old)].kern_sizes;
  if( oldks != (SHORT_LENGTH *) NULL )
  { klen = oldks[0];
    ifdebug(DMA, D, DebugRegisterUsage(MEM_FONTS, 0, klen * sizeof(SHORT_LENGTH)));
    finfo[font_count].kern_sizes = newks =
      (SHORT_LENGTH *) malloc(klen * sizeof(SHORT_LENGTH));
    if( newks == (SHORT_LENGTH *) NULL )
      Error(37, 55, "run out of memory when changing font or font size",
	FATAL, &fpos(x));
    newks[0] = klen;
    for( i = 1;  i < klen;  i++ )
      newks[i] = (oldks[i] * font_size(new)) / font_size(old);
  }
  else finfo[font_count].kern_sizes = (SHORT_LENGTH *) NULL;

  /* return new font number and exit */
  font(*style) = font_count;
  SetGap(space_gap(*style), nobreak(space_gap(*style)), FALSE, TRUE,
    FIXED_UNIT, EDGE_MODE, font_spacewidth(new));
  debug2(DFT, D,"FontChange returning (scaled) %d (XHeight2 = %d)",
    font(*style), font_xheight2(finfo[font(*style)].font_table));
  /* FontDebug(); */
} /* end FontChange */


/*****************************************************************************/
/*                                                                           */
/*  KernLength(fnum, ch1, ch2, res)                                          */
/*                                                                           */
/*  Set res to the kern length between ch1 and ch2 in font fnum, or 0 if     */
/*  none.  Actually we first convert ch1 and ch2 to corresponding unaccented */
/*  characters, because metrics files don't seem to contain kerning pairs    */
/*  for accented characters.                                                 */
/*                                                                           */
/*****************************************************************************/

#define KernLength(fnum, mp, ch1, ch2, res)				\
{ int ua_ch1 = mp[ch1];							\
  int ua_ch2 = mp[ch2];							\
  int i = finfo[fnum].kern_table[ua_ch1], j;				\
  if( i == 0 )  res = 0;						\
  else									\
  { FULL_CHAR *kc = finfo[fnum].kern_chars;				\
    for( j = i;  kc[j] > ua_ch2;  j++ );				\
    res = (kc[j] == ua_ch2) ?						\
      finfo[fnum].kern_sizes[finfo[fnum].kern_value[j]] : 0;		\
  }									\
} /* end KernLength */


/*@::FontWordSize()@**********************************************************/
/*                                                                           */
/*  FontWordSize(x)                                                          */
/*                                                                           */
/*  Calculate the horizontal and vertical size of WORD or QWORD x, including */
/*  the effect of ligature sequences but not replacing them with ligatures.  */
/*                                                                           */
/*****************************************************************************/

void FontWordSize(OBJECT x)
{ FULL_CHAR *p, *q, *a, *b, *lig, *unacc, *acc;  OBJECT tmp;
  FULL_CHAR buff[MAX_BUFF];  MAPPING m;
  int r, u, d, ksize; struct metrics *fnt;
  debug2(DFT, D, "FontWordSize( %s ), font = %d", string(x), word_font(x));
  assert( is_word(type(x)), "FontWordSize: !is_word(type(x))!" );

  p = string(x);
  q = buff;
  if( *p )
  { if( word_font(x) < 1 || word_font(x) > font_count )
      Error(37, 56, "no current font at word %s", FATAL, &fpos(x), string(x));
    if( word_colour(x) == 0 && BackEnd->colour_avail )
      Error(37, 57, "no current colour at word %s", FATAL, &fpos(x), string(x));
    if( word_language(x) == 0 )
      Error(37, 58, "no current language at word %s", FATAL,&fpos(x),string(x));
    fnt = finfo[word_font(x)].size_table;
    lig = finfo[word_font(x)].lig_table;
    m = font_mapping(finfo[word_font(x)].font_table);
    unacc = MapTable[m]->map[MAP_UNACCENTED];
    acc   = MapTable[m]->map[MAP_ACCENT];
    d = u = r = 0;
    do
    { 
      /* check for missing glyph (lig[] == 1) or ligatures (lig[] > 1) */
      debug2(DFT, D, "  examining `%c' lig = %d", *p, lig[*p]);
      if( lig[*q = *p++] )
      {
	if( lig[*q] == 1 )
	{ tmp = MakeWord(QWORD, STR_SPACE, &fpos(x));
	  string(tmp)[0] = *q;
	  /* bug fix: unaccented version exists if unacc differs from self */
	  if( unacc[*q] != *q )
	  {
	    /* *** this is acceptable now, let this char through
	    Error(37, 59, "accent dropped from character %s (it has no glyph in font %s)",
	      WARN, &fpos(x),
	      StringQuotedWord(tmp), FontFamilyAndFace(word_font(x)));
	    *(p-1) = *q = unacc[*q];
	    *** */
	    debug2(DFT, D, "  unacc[%c] = `%c'", *q, unacc[*q]);
	    fnt[*q].up = fnt[unacc[*q]].up;
	    fnt[*q].down = fnt[unacc[*q]].down;
	    fnt[*q].left = fnt[unacc[*q]].left;
	    fnt[*q].right = fnt[unacc[*q]].right;
	    fnt[*q].last_adjust = fnt[unacc[*q]].last_adjust;
	    lig[*q] = 0;
	  }
	  else
	  {
	    debug1(DFT, D, "  unacc[%c] = 0, replacing by space", *q);
	    Error(37, 60, "character %s replaced by space (it has no glyph in font %s)",
	      WARN, &fpos(x),
	      StringQuotedWord(tmp), FontFamilyAndFace(word_font(x)));
	    *(p-1) = *q = CH_SPACE;
	  }
	  Dispose(tmp);
	}
	else
	{
	  debug1(DFT, D, "  processing ligature beginning at %c", *q);
	  a = &lig[ lig[*(p-1)] + MAX_CHARS ];
	  while( *a++ == *(p-1) )
	  { b = p;
	    while( *a == *b && *(a+1) != '\0' && *b != '\0' )  a++, b++;
	    if( *(a+1) == '\0' )
	    { *q = *a;
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

      /* accumulate size of *q */
      if( fnt[*q].up   > u )  u = fnt[*q].up;
      if( fnt[*q].down < d )  d = fnt[*q].down;
      r += fnt[*q++].right;
    } while( *p );
    *q = '\0';

    /* adjust for last character */
    r += fnt[*(q-1)].last_adjust;

    /* add kern lengths to r */
    for( p = buff, q = p+1;  *q;  p++, q++ )
    { KernLength(word_font(x), unacc, *p, *q, ksize);
      debugcond3(DFT, D, ksize != 0, "  KernLength(fnum, %c, %c) = %d",
	*p, *q, ksize);
      r += ksize;
    }
    /* set sizes of x */
    back(x, COLM) = 0;
    fwd(x, COLM)  = r;
    back(x, ROWM) = u;
    fwd(x, ROWM)  = -d;
  } 
  else back(x, COLM) = fwd(x, COLM) = back(x, ROWM) = fwd(x, ROWM) = 0;
  debug4(DFT, D, "FontWordSize returning %hd %hd %hd %hd",
	  back(x, COLM), fwd(x, COLM), back(x, ROWM), fwd(x, ROWM));
} /* end FontWordSize */


/*@::FontSize(), FontHalfXHeight(), FontEncoding(), FontName()@***************/
/*                                                                           */
/*  FULL_LENGTH FontSize(fnum, x)                                            */
/*                                                                           */
/*  Return the size of this font.  x is for error messages only.             */
/*                                                                           */
/*****************************************************************************/

FULL_LENGTH FontSize(FONT_NUM fnum, OBJECT x)
{ debug1(DFT, D, "FontSize( %d )", fnum);
  assert( fnum <= font_count, "FontSize!" );
  if( fnum <= 0 )
    Error(37, 61, "no current font at this point", FATAL, &fpos(x));
  debug1(DFT, D, "FontSize returning %d", font_size(finfo[fnum].font_table));
  return font_size(finfo[fnum].font_table);
} /* end FontSize */


/*****************************************************************************/
/*                                                                           */
/*  FULL_LENGTH FontHalfXHeight(fnum)                                        */
/*                                                                           */
/*  Return the xheight2 value of this font.                                  */
/*                                                                           */
/*****************************************************************************/

FULL_LENGTH FontHalfXHeight(FONT_NUM fnum)
{ debug1(DFT, DD, "FontHalfXHeight( %d )", fnum);
  assert( fnum <= font_count, "FontHalfXHeight!" );
  debug1(DFT, DD, "FontHalfXHeight returning %d",
    font_xheight2(finfo[fnum].font_table));
  return font_xheight2(finfo[fnum].font_table);
} /* end FontHalfXHeight */


/*****************************************************************************/
/*                                                                           */
/*  MAPPING FontMapping(fnum, xfpos)                                         */
/*                                                                           */
/*  Return the character mapping of this font, to use for small caps, etc.   */
/*  xfpos is the file position for error messages.                           */
/*                                                                           */
/*****************************************************************************/

MAPPING FontMapping(FONT_NUM fnum, FILE_POS *xfpos)
{ debug1(DFT, DD, "FontMapping( %d )", fnum);
  assert( fnum <= font_count, "FontMapping!" );
  if( fnum <= 0 )
    Error(37, 62, "no current font at this point", FATAL, xfpos);
  debug1(DFT, DD, "FontMapping returning %d",
    font_mapping(finfo[fnum].font_table));
  return font_mapping(finfo[fnum].font_table);
} /* end FontMapping */


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *FontName(fnum)                                                */
/*                                                                           */
/*  Return the short PostScript name of this font.                           */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *FontName(FONT_NUM fnum)
{ debug1(DFT, D, "FontName( %d )", fnum);
  assert( fnum <= font_count, "FontName!" );
  debug1(DFT, D, "FontName returning %s", string(finfo[fnum].font_table));
  return string(finfo[fnum].font_table);
} /* end FontName */


/*@::FontFamily(), FontFace@**************************************************/
/*                                                                           */
/*  FULL_CHAR *FontFamilyAndFace(fnum)                                       */
/*                                                                           */
/*  Return a static string of the current font family and face.              */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *FontFamily(FONT_NUM fnum)
{ OBJECT face, family;
  debug1(DFT, D, "FontFamily( %d )", fnum);
  assert( fnum <= font_count, "FontFamiliy!" );
  Parent(face, Up(finfo[fnum].font_table));
  Parent(family, Up(face));
  debug1(DFT, D, "FontFamily returning %s", string(family));
  return string(family);
} /* end FontFamilyAndFace */


FULL_CHAR *FontFace(FONT_NUM fnum)
{ OBJECT face, family;
  debug1(DFT, D, "FontFacec( %d )", fnum);
  assert( fnum <= font_count, "FontFamiliy!" );
  Parent(face, Up(finfo[fnum].font_table));
  Parent(family, Up(face));
  debug1(DFT, D, "FontFace returning %s", string(face));
  return string(face);
} /* end FontFamilyAndFace */


/*@::FontFamilyAndFace(), FontPrintAll()@*************************************/
/*                                                                           */
/*  FULL_CHAR *FontFamilyAndFace(fnum)                                       */
/*                                                                           */
/*  Return a static string of the current font family and face.              */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *FontFamilyAndFace(FONT_NUM fnum)
{ OBJECT face, family; static FULL_CHAR buff[80];
  debug1(DFT, D, "FontFamilyAndFace( %d )", fnum);
  assert( fnum <= font_count, "FontName!" );
  Parent(face, Up(finfo[fnum].font_table));
  Parent(family, Up(face));
  if( StringLength(string(family)) + StringLength(string(face)) + 1 > 80 )
    Error(37, 63, "family and face names %s %s are too long",
      FATAL, no_fpos, string(family), string(face));
  StringCopy(buff, string(family));
  StringCat(buff, STR_SPACE);
  StringCat(buff, string(face));
  debug1(DFT, D, "FontName returning %s", buff);
  return buff;
} /* end FontFamilyAndFace */


/*****************************************************************************/
/*                                                                           */
/*  FontPrintAll(fp)                   	                                     */
/*                                                                           */
/*  Print all font encoding commands on output file fp                       */
/*                                                                           */
/*****************************************************************************/

void FontPrintAll(FILE *fp)
{ OBJECT family, face, first_size, ps_name, link, flink;
  assert(font_root!=nilobj && type(font_root)==ACAT, "FontDebug: font_root!");
  debug0(DFT, DD, "FontPrintAll(fp)");
  for( link = Down(font_root);  link != font_root;  link = NextDown(link) )
  { Child(family, link);
    assert( is_word(type(family)), "FontPrintAll: family!" );
    for( flink = Down(family);  flink != family;  flink = NextDown(flink) )
    { Child(face, flink);
      assert( is_word(type(face)), "FontPrintAll: face!" );
      assert( Down(face) != face && NextDown(Down(face)) != face &&
	NextDown(NextDown(Down(face))) != face, "FontDebug: Down(face)!");
      Child(ps_name, Down(face));
      assert( is_word(type(ps_name)), "FontPrintAll: ps_name!" );
      Child(first_size, NextDown(NextDown(Down(face))));
      assert( is_word(type(first_size)), "FontPrintAll: first_size!" );
      if( font_recoded(face) )
      { fprintf(fp, "/%s%s %s /%s LoutRecode\n",
	  string(ps_name), string(first_size),
	  MapEncodingName(font_mapping(face)), string(ps_name));
	fprintf(fp, "/%s { /%s%s LoutFont } def\n", string(first_size),
	  string(ps_name), string(first_size));
      }
      else fprintf(fp, "/%s { /%s LoutFont } def\n", string(first_size),
	  string(ps_name));
    }
  }
  fputs("\n", fp);
  debug0(DFT, DD, "FontPrintAll returning.");
} /* end FontPrintAll */


/*@@**************************************************************************/
/*                                                                           */
/*  FontPrintPageSetup(fp)             	                                     */
/*                                                                           */
/*  Print all font encoding commands needed for the current page onto fp.    */
/*                                                                           */
/*****************************************************************************/

void FontPrintPageSetup(FILE *fp)
{ OBJECT face, first_size, ps_name, link;
  assert(font_root!=nilobj && type(font_root)==ACAT, "FontDebug: font_root!");
  assert(font_used!=nilobj && type(font_used)==ACAT, "FontDebug: font_used!");
  debug0(DFT, DD, "FontPrintPageSetup(fp)");
  for( link = Down(font_used);  link != font_used;  link = NextDown(link) )
  {
    Child(face, link);
    assert( is_word(type(face)), "FontPrintPageSetup: face!" );
    assert( Down(face) != face, "FontDebug: Down(face)!");

    /* print font encoding command */
    Child(first_size, NextDown(NextDown(Down(face))));
    assert( is_word(type(first_size)), "FontPrintPageSetup: first_size!" );
    Child(ps_name, Down(face));
    assert( is_word(type(ps_name)), "FontPrintPageSetup: ps_name!" );
    BackEnd->PrintPageSetupForFont(face, font_curr_page,
      string(ps_name), string(first_size));
  }
  debug0(DFT, DD, "FontPrintPageSetup returning.");
} /* end FontPrintPageSetup */


/*@@**************************************************************************/
/*                                                                           */
/*  FontPrintPageResources(fp)        	                                     */
/*                                                                           */
/*  Print all page resources (i.e. fonts needed or supplied) onto fp.        */
/*                                                                           */
/*****************************************************************************/

void FontPrintPageResources(FILE *fp)
{ OBJECT face, ps_name, link, pface, pname, plink;
  BOOLEAN first;
  assert(font_root!=nilobj && type(font_root)==ACAT, "FontDebug: font_root!");
  assert(font_used!=nilobj && type(font_used)==ACAT, "FontDebug: font_used!");
  debug0(DFT, DD, "FontPrintPageResources(fp)");
  first = TRUE;
  for( link = Down(font_used);  link != font_used;  link = NextDown(link) )
  {
    Child(face, link);
    assert( is_word(type(face)), "FontPrintPageResources: face!" );
    assert( Down(face) != face, "FontDebug: Down(face)!");
    Child(ps_name, Down(face));
    assert( is_word(type(ps_name)), "FontPrintPageResources: ps_name!" );

    /* make sure this ps_name has not been printed before (ugly, I know). */
    /* Repeats arise when the font appears twice in the database under    */
    /* different family-face names, perhaps because of sysnonyms like     */
    /* Italic and Slope, or perhaps because of different encoding vectors */
    for( plink = Down(font_used);  plink != link;  plink = NextDown(plink) )
    {
      Child(pface, plink);
      Child(pname, Down(pface));
      if( StringEqual(string(pname), string(ps_name)) )
	break;
    }
    if( plink == link )
    {
      /* not seen before, so print it */
      BackEnd->PrintPageResourceForFont(string(ps_name), first);
      first = FALSE;
    }
  }
  debug0(DFT, DD, "FontPrintPageResources returning.");
} /* end FontPrintPageResources */


/*@@**************************************************************************/
/*                                                                           */
/*  FontAdvanceCurrentPage()        	                                     */
/*                                                                           */
/*  Advance the current page.                                                */
/*                                                                           */
/*****************************************************************************/

void FontAdvanceCurrentPage(void)
{ debug0(DFT, DD, "FontAdvanceCurrentPage()");
  while( Down(font_used) != font_used )  DeleteLink(Down(font_used));
  font_curr_page++;
  debug0(DFT, DD, "FontAdvanceCurrentPage() returning.");
} /* end FontAdvanceCurrentPage */


/*@::FontPageUsed()@**********************************************************/
/*                                                                           */
/*  OBJECT FontPageUsed(face)                                                */
/*                                                                           */
/*  Declares that font face is used on the current page.                     */
/*                                                                           */
/*****************************************************************************/

void FontPageUsed(OBJECT face)
{ debug1(DFT, DD, "FontPageUsed(%d)", font_num(face));
  assert( font_page(face) < font_curr_page, "FontPageUsed!" );
  Link(font_used, face);
  font_page(face) = font_curr_page;
  debug0(DFT, DD, "FontPageUsed returning");
} /* end FontPageUsed */


/*@::FontNeeded()@************************************************************/
/*                                                                           */
/*  OBJECT FontNeeded(fp)                                                    */
/*                                                                           */
/*  Writes font needed resources onto file out_fp.  Returns TRUE if none.    */
/*  Now that we are using a database, every font that is actually loaded     */
/*  is really needed.                                                        */
/*                                                                           */
/*****************************************************************************/

BOOLEAN FontNeeded(FILE *fp)
{ BOOLEAN first_need = TRUE;
  OBJECT link, flink, family, face, ps_name;
  for( link = Down(font_root); link != font_root; link = NextDown(link) )
  { Child(family, link);
    for( flink = Down(family);  flink != family;  flink = NextDown(flink) )
    { Child(face, flink);
      Child(ps_name, Down(face));
      assert( is_word(type(ps_name)), "FontPrintPageResources: ps_name!" );
      fprintf(fp, "%s font %s\n",
	first_need ? "%%DocumentNeededResources:" : "%%+", string(ps_name));
      first_need = FALSE;
    }
  }
  return first_need;
} /* end FontNeeded */
