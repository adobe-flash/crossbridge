/*@z38.c:Character Mappings:Declarations@*************************************/
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
/*  FILE:         z38.c                                                      */
/*  MODULE:       Character Mappings                                         */
/*  EXTERNS:      MapLoad(), MapCharEncoding(), MapEncodingName(),           */
/*                MapPrintEncodings(), MapPrintResources(), MapSmallCaps()   */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"
#define MAX_MAP		 20		/* max number of lcm files           */

/*****************************************************************************/
/*                                                                           */
/*  Should really be private but have been placed in externs because for     */
/*  efficiency they are used by z37.c and z34.c                              */
/*                                                                           */
/*  #define   MAX_CHASH      353                                             */
/*  #define   MAP_UPPERCASE    0                                             */
/*  #define   MAP_LOWERCASE    1                                             */
/*  #define   MAP_UNACCENTED   2                                             */
/*  #define   MAP_ACCENT       3                                             */
/*  #define   MAPS             4                                             */
/*                                                                           */
/*  typedef struct mapvec {                                                  */
/*    OBJECT      file_name;                                                 */
/*    FILE_NUM    fnum;                                                      */
/*    BOOLEAN     seen_recoded;                                              */
/*    int	  last_page_printed;                                         */
/*    OBJECT      name;                                                      */
/*    OBJECT      vector[MAX_CHARS];                                         */
/*    FULL_CHAR   hash_table[MAX_CHASH];                                     */
/*    FULL_CHAR   map[MAPS][MAX_CHARS];                                      */
/*  } *MAP_VEC;                                                              */
/*                                                                           */
/*****************************************************************************/

MAP_VEC	MapTable[MAX_MAP];		/* the mappings                      */

static	OBJECT	notdef_word = nilobj;	/* notdef word                       */
static	int	maptop = 1;		/* first free slot in MapTable[]     */
					/* save 0 for "no mapping"           */

/*****************************************************************************/
/*                                                                           */
/*  static int NameInsert(cname)                                             */
/*  static FULL_CHAR NameRetrieve(cname)                                     */
/*                                                                           */
/*****************************************************************************/

#define hash(str, pos)							\
{ FULL_CHAR *p = str;							\
  for( pos = 2 * *p++;  *p;  pos += *p++);				\
  pos = pos % MAX_CHASH;						\
}

static void NameInsert(FULL_CHAR *cname, int ccode, MAP_VEC map)
{ int pos;
  hash(cname, pos);
  while( map->hash_table[pos] != (FULL_CHAR) '\0' )
    pos = (pos + 1) % MAX_CHASH;
  map->vector[ccode] = MakeWord(WORD, cname, no_fpos);
  map->hash_table[pos] = ccode;
} /* end NameInsert */

static FULL_CHAR NameRetrieve(FULL_CHAR *cname, MAP_VEC map)
{ int pos;  FULL_CHAR ch;
  hash(cname, pos);
  while( (ch = map->hash_table[pos]) != (FULL_CHAR) '\0' )
  {
    if( StringEqual(string(map->vector[ch]), cname) )
      return ch;
    pos = (pos + 1) % MAX_CHASH;
  }
  return ch;
} /* end NameRetrieve */


/*@::MapLoad()@***************************************************************/
/*                                                                           */
/*  MAPPING MapLoad(file_name, recoded)                                      */
/*                                                                           */
/*  Declare file_name to be a character mapping (LCM) file.  A file may be   */
/*  so declared more than once.  Parameter recoded is true if the font that  */
/*  uses this mapping declares that it needs to be recoded, which in turn    */
/*  means that this mapping might have to be printed out.  Whether or not it */
/*  is actually printed depends upon whether we print a font that uses it    */
/*  and that requires recoding.                                              */
/*                                                                           */
/*****************************************************************************/

MAPPING MapLoad(OBJECT file_name, BOOLEAN recoded)
{ FILE *fp;  MAP_VEC map;  MAPPING res;
  int i, m, curr_line_num, line_pos, prev_code, dc, oc, count;
  FULL_CHAR buff[MAX_BUFF], cn[MAX_BUFF], ch, mapname[MAX_BUFF],
  mapval[MAX_BUFF];
  debug2(DCM,D, "MapLoad(%s, %s)", EchoObject(file_name), bool(recoded));

  /* if the file name is "-", it means no mapping file is supplied */
  if( StringEqual(string(file_name), AsciiToFull("-")) )
  { debug1(DCM, D, "MapLoad returning 0 (file name is %s)",
      string(file_name));
    return (MAPPING) 0;
  }

  /* if seen this file name before, just update seen_recoded and return prev */
  for( res = 1;  res < maptop;  res++ )
  {
    if( StringEqual(string(MapTable[res]->file_name), string(file_name)) )
    { Dispose(file_name);
      MapTable[res]->seen_recoded = MapTable[res]->seen_recoded || recoded;
      debug1(DCM, D, "MapLoad returning %d (not new)", res);
      return res;
    }
  }

  /* initialize PostScript name of all undefined characters */
  if( notdef_word == nilobj )
    notdef_word = MakeWord(WORD, AsciiToFull(".notdef"), no_fpos);

  /* new, so allocate a new slot in MapTable for this new mapping */
  if( maptop == MAX_MAP )
    Error(38, 1, "too many character mappings", FATAL, &fpos(file_name));
  ifdebug(DMA, D, DebugRegisterUsage(MEM_CMAPS, 1, sizeof(struct mapvec)));
  MapTable[res = maptop++] = map = (MAP_VEC) malloc( sizeof(struct mapvec) );
  if( map == (MAP_VEC) NULL )
    Error(38, 2, "run out of memory when loading character mapping",
      FATAL, &fpos(file_name));

  /* initialize all the fields */
  map->file_name = file_name;
  debug0(DFS, D, "  calling DefineFile from MapLoad");
  map->fnum = DefineFile(string(file_name), STR_EMPTY, &fpos(file_name),
    MAPPING_FILE, MAPPING_PATH);
  fp = OpenFile(map->fnum, FALSE, FALSE);
  if( fp == NULL )  Error(38, 3, "cannot open character mapping file %s",
      FATAL, PosOfFile(map->fnum), FileName(map->fnum));
  map->seen_recoded = recoded;
  map->last_page_printed = 0;
  StringCopy(buff, AsciiToFull("vec"));
  StringCat(buff, StringInt(maptop));
  map->name = MakeWord(WORD, buff, no_fpos);
  for( m = 0;  m < MAPS;  m++ )
  { for( i = 0;  i < MAX_CHARS;  i++ )
      map->map[m][i] = '\0';
  }

  /* unaccented map is defined to be self as default */
  for( i = 0;  i < MAX_CHARS;  i++ )
    map->map[MAP_UNACCENTED][i] = i;

  for( i = 0;  i < MAX_CHARS; i++ )  map->vector[i] = notdef_word;
  for( i = 0;  i < MAX_CHASH; i++ )  map->hash_table[i] = 0;

  /* first pass through the file; read character codes and names only */
  prev_code = -1;  curr_line_num = 0;
  while( fgets( (char *) buff, MAX_BUFF, fp) == (char *) buff )
  { 
    /* skip comment lines and blank lines */
    curr_line_num++;
    for( i = 0;  buff[i] == ' ' || buff[i] == '\t';  i++ );
    if( buff[i] == '#' || buff[i] == '\n' || buff[i] == '\0' )  continue;

    /* parse line and check validity of decimal and octal character codes */
    count = sscanf( (char *) buff, "%d %o %s", &dc, &oc, cn);
    if( count < 2 )
      Error(38, 4, "character code(s) missing in mapping file (line %d)",
	FATAL, &fpos(file_name));
    if( dc != oc )
      Error(38, 5, "decimal and octal codes disagree in mapping file (line %d)",
	FATAL, &fpos(file_name));
    if( dc < 1 && !StringEqual(cn, STR_NOCHAR) )
      Error(38, 6, "code %d too small (min is 1) in mapping file (line %d)",
	FATAL, &fpos(file_name), dc, curr_line_num);
    if( dc < prev_code )
      Error(38, 7, "code %d out of order in mapping file (line %d)",
	FATAL, &fpos(file_name), dc, curr_line_num);
    if( dc == prev_code )
      Error(38, 8, "code %d repeated in mapping file (line %d)",
	FATAL, &fpos(file_name), dc, curr_line_num);
    if( dc > MAX_CHARS )
      Error(38, 9, "code %d too large (max is %d) in mapping file (line %d)",
	FATAL, &fpos(file_name), dc, MAX_CHARS, curr_line_num);
    prev_code = dc;

    /* insert character name, if any */
    debug2(DCM, DD, "  line %d: %s", curr_line_num, cn);
    if( count >= 3 && !StringEqual(cn, STR_NOCHAR) )
    {
      /* insert (cn, dc) pair into hash table; name may be repeated */
      if( (ch = NameRetrieve(cn, map)) != 0 )
	map->vector[dc] = map->vector[ch];
      else
	NameInsert(cn, dc, map);
    }
  }

  /* second pass through the file: read mappings */
  rewind(fp);
  curr_line_num = 0;
  while( fgets( (char *) buff, MAX_BUFF, fp) == (char *) buff )
  { 
    /* skip comment lines and blank lines */
    curr_line_num++;
    for( i = 0;  buff[i] == ' ' || buff[i] == '\t';  i++ );
    if( buff[i] == '#' || buff[i] == '\n' || buff[i] == '\0' )  continue;

    /* parse line */
    count = sscanf( (char *) buff, "%d %o %s%n",
      &dc, &oc, cn, &line_pos);

    /* find and insert the maps */
    while( sscanf( (char *) &buff[line_pos], "%s %[^;];%n",
      mapname, mapval, &i) == 2 )
    {
      debug3(DCM, DD, "  line %d: %s %s", curr_line_num, mapname, mapval);
      line_pos += i;
      if( StringEqual(mapname, AsciiToFull("UC")) )
	m = MAP_UPPERCASE;
      else if( StringEqual(mapname, AsciiToFull("LC")) )
	m = MAP_LOWERCASE;
      else if( StringEqual(mapname, AsciiToFull("UA")) )
	m = MAP_UNACCENTED;
      else if( StringEqual(mapname, AsciiToFull("AC")) )
	m = MAP_ACCENT;
      else
	Error(38, 10, "unknown mapping name %s in mapping file %s (line %d)",
	  FATAL, &fpos(file_name), mapname, FileName(map->fnum), curr_line_num);
      ch = NameRetrieve(mapval, map);
      if( ch == (FULL_CHAR) '\0' )
	Error(38, 11, "unknown character %s in mapping file %s (line %d)",
	  FATAL, &fpos(file_name), mapval, FileName(map->fnum), curr_line_num);
      map->map[m][dc] = ch;
    }
  }
  fclose(fp);
  debug1(DCM, D, "MapLoad returning %d (new mapping)", res);
  return res;
} /* end MapLoad */


/*@::MapCharEncoding(), MapEncodingName(), MapPrintEncodings()@***************/
/*                                                                           */
/*  FULL_CHAR MapCharEncoding(str, map)                                      */
/*                                                                           */
/*  Returns the character code corresponding to character name str in        */
/*  MAPPING enc, or 0 if not found.                                          */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR MapCharEncoding(FULL_CHAR *str, MAPPING m)
{ MAP_VEC map;
  map = MapTable[m];
  return (FULL_CHAR) NameRetrieve(str, map);
} /* end MapCharEncoding */


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *MapEncodingName(m)                                            */
/*                                                                           */
/*  Returns the PostScript name of the encoding vector of mapping m          */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *MapEncodingName(MAPPING m)
{ assert( m < maptop, "MapEncodingName: m out of range!" );
  return string(MapTable[m]->name);
} /* end MapEncodingName */


/*****************************************************************************/
/*                                                                           */
/*  void MapEnsurePrinted(MAPPING m, int curr_page)                          */
/*                                                                           */
/*  Ensure that MAPPING m is printed on page curr_page, if required.         */
/*  It's required if it has neither been printed on the current page         */
/*  already, nor on page 1 (page 1 is really the entire document setup).     */
/*                                                                           */
/*****************************************************************************/

void MapEnsurePrinted(MAPPING m, int curr_page)
{ MAP_VEC map = MapTable[m];
  assert( map->seen_recoded, "MapEnsurePrinted: not seen_recoded!" );
  if( map->last_page_printed < curr_page && map->last_page_printed != 1 )
  { map->last_page_printed = curr_page;
    BackEnd->PrintMapping(m);
  }
}


/*****************************************************************************/
/*                                                                           */
/*  MapPrintEncodings()                                                      */
/*                                                                           */
/*  Print all encoding vectors in existence so far; this counts as printing  */
/*  them on "page 1", but in fact they will appear in the document setup     */
/*  section.                                                                 */
/*                                                                           */
/*****************************************************************************/

void MapPrintEncodings()
{ MAPPING m;  MAP_VEC map;
  for( m = 1;  m < maptop;  m++ )
  { if( MapTable[m]->seen_recoded )
    { BackEnd->PrintMapping(m);
      map = MapTable[m];
      map->last_page_printed = 1;
    }
  }
} /* end MapPrintEncodings */


/*****************************************************************************/
/*                                                                           */
/*  MapPrintPSResources(fp)                                                  */
/*                                                                           */
/*  Print PostScript resource entries for all encoding vectors on file fp.   */
/*                                                                           */
/*****************************************************************************/

void MapPrintPSResources(FILE *fp)
{ MAPPING m;  MAP_VEC map;
  for( m = 1;  m < maptop;  m++ )  if( MapTable[m]->seen_recoded )
  { map = MapTable[m];
    fprintf(fp, "%%%%+ encoding %s\n", string(map->name));
  }
} /* end MapPrintPSResources */


/*@@**************************************************************************/
/*                                                                           */
/*  OBJECT DoWord(buff, q, x, fnum)                                          */
/*                                                                           */
/*  Replace WORD or QWORD x by a small caps version, based on word_font(x).  */
/*                                                                           */
/*****************************************************************************/

static OBJECT DoWord(FULL_CHAR *buff, FULL_CHAR *q, OBJECT x, FONT_NUM fnum)
{ OBJECT res;
  *q++ = '\0';
  res = MakeWord(type(x), buff, &fpos(x));
  word_font(res) = fnum;
  word_colour(res) = word_colour(x);
  word_outline(res) = word_outline(x);
  word_language(res) = word_language(x);
  word_hyph(res) = word_hyph(x);
  underline(res) = UNDER_OFF;
  return res;
} /* end DoWord */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT DoVShift(x, vshift, chld)                                         */
/*                                                                           */
/*  Make an new VSHIFT object with the given shift and child.                */
/*                                                                           */
/*****************************************************************************/

static OBJECT DoVShift(OBJECT x, FULL_LENGTH vshift, OBJECT chld)
{ OBJECT res;
  New(res, VSHIFT);
  FposCopy(fpos(res), fpos(x));
  shift_type(res) = GAP_DEC;
  units(shift_gap(res)) = FIXED_UNIT;
  mode(shift_gap(res)) = EDGE_MODE;
  width(shift_gap(res)) = vshift;
  underline(res) = UNDER_OFF;
  Link(res, chld);
  return res;
}

/*****************************************************************************/
/*                                                                           */
/*  void DoAddGap(new_acat)                                                  */
/*                                                                           */
/*  Add a new 0i gap object to new_acat.                                     */
/*                                                                           */
/*****************************************************************************/

static void DoAddGap(OBJECT new_acat)
{ OBJECT new_g;
  New(new_g, GAP_OBJ);
  FposCopy(fpos(new_g), fpos(new_acat));
  hspace(new_g) = vspace(new_g) = 0;
  SetGap(gap(new_g), TRUE, FALSE, TRUE, FIXED_UNIT, EDGE_MODE, 0*IN);
  underline(new_g) = UNDER_OFF;
  Link(new_acat, new_g);
}

/*@::MapSmallCaps()@**********************************************************/
/*                                                                           */
/*  OBJECT MapSmallCaps(x, style)                                            */
/*                                                                           */
/*  Replace WORD or QWORD x by a small caps version, based on word_font(x).  */
/*                                                                           */
/*****************************************************************************/
#define	INIT		0
#define	ALL_NON		1
#define	ALL_TRANS	2
#define	MIXED_NON	3
#define	MIXED_TRANS	4
#define transformable(ch)	(uc[ch] != '\0')

OBJECT MapSmallCaps(OBJECT x, STYLE *style)
{ MAPPING m;  int i;  OBJECT new_y, new_x, new_acat, tmp;
  FULL_CHAR buff[MAX_BUFF], *uc, *p, *q;
  FONT_NUM small_font;  FULL_LENGTH vshift;  int state;  STYLE new_style;
  static OBJECT font_change_word = nilobj;
  assert( is_word(type(x)), "MapSmallCaps: !is_word(type(x))" );
  debug2(DCM, D, "MapSmallCaps(%s %s)", Image(type(x)), string(x));

  /* get the mapping and return if there isn't one for this font */
  m = FontMapping(font_num(x), &fpos(x));
  if( m == 0 )
  { debug0(DCM, D, "MapSmallCaps returning unchanged (mapping is 0)");
    return x;
  }
  assert( 1 <= m && m < maptop, "MapSmallCaps: mapping out of range!" );
  uc = MapTable[m]->map[MAP_UPPERCASE];

  /* if plain text, apply the mapping and exit */
  if( !(BackEnd->scale_avail) )
  {
    for( i = 0;  string(x)[i] != '\0';  i++ )
      if( uc[string(x)[i]] != '\0' )
        string(x)[i] = uc[string(x)[i]];
    debug1(DCM, D, "MapSmallCaps returning (plain text) %s", EchoObject(x));
    return x;
  }

  /* set up the font change word if not already done */
  if( font_change_word == nilobj )
  { font_change_word = MakeWord(WORD, AsciiToFull("0.7f"), no_fpos);
  }

  state = INIT;  q = buff;
  for( p = string(x);  *p != '\0';  p++ )
  {
    debug2(DCM, DD, " examining %c (%s)", *p,
      transformable(*p) ? "transformable" : "not transformable");
    switch( state )
    {
      case INIT:

        /* this state is for when we are at the first character */
        if( transformable(*p) )
        { *q++ = uc[*p];

	  /* work out what the smaller font is going to be, and the vshift */
	  StyleCopy(new_style, *style);
	  FontChange(&new_style, font_change_word);
	  small_font = font(new_style);
	  vshift = FontHalfXHeight(word_font(x)) - FontHalfXHeight(small_font);

          state = ALL_TRANS;
        }
        else
        { *q++ = *p;
          state = ALL_NON;
        }
        break;


      case ALL_NON:

        /* in this state, all characters so far are non-transformable */
        if( transformable(*p) )
        { 
	  /* work out what the smaller font is going to be */
	  StyleCopy(new_style, *style);
	  FontChange(&new_style, font_change_word);
	  small_font = font(new_style);
	  vshift = FontHalfXHeight(word_font(x)) - FontHalfXHeight(small_font);

	  /* make a new WORD out of the current contents of buff */
	  new_y = DoWord(buff, q, x, word_font(x));

	  /* construct the skeleton of the result to replace x */
	  New(new_x, ONE_COL);
	  FposCopy(fpos(new_x), fpos(x));
	  New(new_acat, ACAT);
	  FposCopy(fpos(new_acat), fpos(x));
	  Link(new_x, new_acat);
	  Link(new_acat, new_y);
	  DoAddGap(new_acat);

	  /* start off a new buffer with *p */
	  q = buff;
	  *q++ = uc[*p];
	  state = MIXED_TRANS;
        }
        else *q++ = *p;
        break;


      case ALL_TRANS:

        /* in this state, all characters so far are transformable */
        if( transformable(*p) ) *q++ = uc[*p];
        else
        {
	  /* make a new @VShift WORD out of the current contents of buff */
	  tmp = DoWord(buff, q, x, small_font);
	  new_y = DoVShift(x, vshift, tmp);

	  /* construct the skeleton of the result to replace x */
	  New(new_x, ONE_COL);
	  FposCopy(fpos(new_x), fpos(x));
	  New(new_acat, ACAT);
	  FposCopy(fpos(new_acat), fpos(x));
	  Link(new_x, new_acat);
	  Link(new_acat, new_y);
	  DoAddGap(new_acat);

	  /* start off a new buffer with *p */
	  q = buff;
	  *q++ = *p;
	  state = MIXED_NON;
        }
        break;


      case MIXED_NON:

        /* in this state the previous char was non-transformable, but */
        /* there have been characters before that that were transformable */
        if( transformable(*p) )
        {
	  /* make a new WORD out of the current contents of buff */
	  new_y = DoWord(buff, q, x, word_font(x));

	  /* link the new word into the growing structure that replaces x */
	  Link(new_acat, new_y);
	  DoAddGap(new_acat);

	  /* start off a new buffer with *p */
	  q = buff;
	  *q++ = uc[*p];
	  state = MIXED_TRANS;
        }
        else *q++ = *p;
        break;


      case MIXED_TRANS:

        /* in this state the previous char was transformable, but there */
        /* have been characters before that that were non-transformable */
        if( transformable(*p) ) *q++ = uc[*p];
        else
        {
	  /* make a new @VShift WORD out of the current contents of buff */
	  tmp = DoWord(buff, q, x, small_font);
	  new_y = DoVShift(x, vshift, tmp);

	  /* link the new word into the growing structure that replaces x */
	  Link(new_acat, new_y);
	  DoAddGap(new_acat);

	  /* start off a new buffer with *p */
	  q = buff;
	  *q++ = *p;
	  state = MIXED_NON;
        }
        break;

    }
  }

  /* now at termination, clean up the structure */
  switch( state )
  {
    case INIT:
    case ALL_NON:

      /* original x is OK as is: either empty or all non-transformable */
      break;


    case ALL_TRANS:

      /* make a new @VShift WORD and replace x with it */
      tmp = DoWord(buff, q, x, small_font);
      new_x = DoVShift(x, vshift, tmp);
      ReplaceNode(new_x, x);
      Dispose(x);
      x = new_x;
      break;


    case MIXED_NON:

      /* make a new WORD, add to new_acat, and replace x */
      new_y = DoWord(buff, q, x, word_font(x));
      Link(new_acat, new_y);
      ReplaceNode(new_x, x);
      Dispose(x);
      x = new_x;
      break;


    case MIXED_TRANS:

      /* make a new @VShift WORD, add to new_acat, and replace x */
      tmp = DoWord(buff, q, x, small_font);
      new_y = DoVShift(x, vshift, tmp);
      Link(new_acat, new_y);
      ReplaceNode(new_x, x);
      Dispose(x);
      x = new_x;
      break;
  }
  debug1(DCM, D, "MapSmallCaps returning %s", EchoObject(x));
  return x;
} /* end MapSmallCaps */


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN MapIsLowerCase(FULL_CHAR ch, MAPPING m)                          */
/*                                                                           */
/*  Returns TRUE if ch is a lower-case character in mapping m; i.e. if it    */
/*  has a corresponding upper-case character.                                */
/*                                                                           */
/*****************************************************************************/

BOOLEAN MapIsLowerCase(FULL_CHAR ch, MAPPING m)
{ BOOLEAN res;
  debug2(DCM, D, "MapIsLowerCase(%c, %d)", ch, m);
  res = (MapTable[m]->map[MAP_UPPERCASE][ch] != '\0');
  debug1(DCM, D, "MapIsLowerCase returning %s", bool(res));
  return res;
} /* end MapIsLowerCase */
