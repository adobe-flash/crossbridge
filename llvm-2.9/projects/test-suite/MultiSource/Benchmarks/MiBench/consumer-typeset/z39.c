/*@z39.c:String Handler:AsciiToFull(), StringEqual(), etc.@*******************/
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
/*  FILE:         z39.c                                                      */
/*  MODULE:       String Handler                                             */
/*  EXTERNS:      AsciiToFull(), StringEqual(),                              */
/*                StringCat(), StringCopy(), StringLength(),                 */
/*                StringFOpen(), StringFPuts(), StringFGets(),               */
/*                StringRemove(), StringRename(), StringBeginsWith(),        */
/*                StringContains(), StringInt(), StringFiveInt(),            */
/*                StringQuotedWord()                                         */
/*                                                                           */
/*****************************************************************************/
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*          AsciiToFull(str)          Returns ASCII string as FULL_CHARs.    */
/*  BOOLEAN StringEqual(a, b)         TRUE if strings a and b are equal      */
/*          StringCat(a, b)           Catenate string b onto end of a        */
/*          StringCopy(a, b)          Overwrite string a with string b       */
/*          StringLength(a)           Length of string a                     */
/*          StringFOpen(str, mode)    Equivalent to fopen(str, mode)         */
/*          StringFPuts(str, fp)      Equivalent to fputs(str, fp)           */
/*          StringFGets(str, n, fp)   Equivalent to fgets(str, n, fp)        */
/*          StringRemove(a)           Equivalent to remove(a)                */
/*          StringRename(a, b)        Equivalent to rename(a, b)             */
/*                                                                           */
/*  These procedures are defined as macros in file externs.                  */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*  int strcollcmp(char *a, char *b)                                         */
/*                                                                           */
/*  Written by Valery Ushakov (uwe).                                         */
/*                                                                           */
/*  Like strcoll, but returns 0 only iff strcmp returns 0.                   */
/*  This allow to test for equality using only strcmp. --uwe                 */
/*                                                                           */
/*  The new version of strcollcmp analyses a and b into three fields         */
/*  separated by \t, then does the comparison field by field.  This is       */
/*  to ensure that the collation order of \t has no effect on the result.    */
/*                                                                           */
/*****************************************************************************/

/* *** old version
int strcollcmp(char *a, char *b)
{
  int order = strcoll (a, b);
  if( order == 0 ) / * then disambiguate with strcmp * /
    order = strcmp (a, b);
  return order;
}
*** */

int strcollcmp(char *a, char *b)
{ char a1[100], b1[100];
  int order;
  sscanf(a, "%[^\t]", a1);
  sscanf(b, "%[^\t]", b1);
  order = strcoll(a1, b1);
  if( order == 0 )
    order = strcmp(a, b);  /* disambiguate with strcmp */
  debug3(DBS, D, "strcollcmp(\"%s<tab>\", \"%s<tab>\") = %d", a1, b1, order)
  return order;
}

/*@::StringBeginsWith(), StringContains(), StringInt(), StringFiveInt()@******/
/*                                                                           */
/*  BOOLEAN StringBeginsWith(str, pattern)                                   */
/*  BOOLEAN StringEndsWith(str, pattern)                                     */
/*                                                                           */
/*  Check whether str begins with or ends with pattern.                      */
/*                                                                           */
/*****************************************************************************/

BOOLEAN StringBeginsWith(FULL_CHAR *str, FULL_CHAR *pattern)
{ FULL_CHAR *sp, *pp;
  sp = str;  pp = pattern;
  while( *sp != '\0' && *pp != '\0' )
  { if( *sp++ != *pp++ )  return FALSE;
  }
  return (*pp == '\0');
} /* end StringBeginsWith */


BOOLEAN StringEndsWith(FULL_CHAR *str, FULL_CHAR *pattern)
{ FULL_CHAR *sp, *pp; int slen, plen;
  slen = StringLength(str);
  plen = StringLength(pattern);
  if( slen < plen )  return FALSE;
  sp = &str[slen - plen];  pp = pattern;
  while( *sp != '\0' && *pp != '\0' )
  { if( *sp++ != *pp++ )  return FALSE;
  }
  return (*pp == '\0');
} /* end StringBeginsWith */


/*****************************************************************************/
/*                                                                           */
/*  BOOLEAN StringContains(str, pattern)                                     */
/*                                                                           */
/*  Check whether str contains pattern.                                      */
/*                                                                           */
/*****************************************************************************/

BOOLEAN StringContains(FULL_CHAR *str, FULL_CHAR *pattern)
{ FULL_CHAR *sp;
  for( sp = str;  *sp != '\0';  sp++ )
  { if( StringBeginsWith(sp, pattern) )  return TRUE;
  }
  return FALSE;
} /* end StringContains */


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *StringInt(i)                                                  */
/*  FULL_CHAR *StringFiveInt(i)                                              */
/*                                                                           */
/*  Returns a string version of integer i.                                   */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *StringInt(int i)
{ static FULL_CHAR buff[20];
  sprintf( (char *) buff, "%d", i);
  return buff;
} /* end StringInt */

FULL_CHAR *StringFiveInt(int i)
{ static FULL_CHAR buff[20];
  sprintf( (char *) buff, "%.5d", i);
  return buff;
} /* end StringInt */


/*@::StringQuotedWord()@******************************************************/
/*                                                                           */
/*  static char *quoted_string[]                                             */
/*                                                                           */
/*  quoted_string[ch] is a string containing the representation of the       */
/*  8-bit character ch within a quoted string in a Lout source file.         */
/*                                                                           */
/*****************************************************************************/

static char *quoted_string[] = {
    "\\000", "\\001", "\\002", "\\003", "\\004", "\\005", "\\006", "\\007",
    "\\010", "\\011", "\\012", "\\013", "\\014", "\\015", "\\016", "\\017",
    "\\020", "\\021", "\\022", "\\023", "\\024", "\\025", "\\026", "\\027",
    "\\030", "\\031", "\\032", "\\033", "\\034", "\\035", "\\036", "\\037",
    " ",     "!",     "\\\"",  "#",     "$",     "%",     "&",     "'",
    "(",     ")",     "*",     "+",     ",",     "-",     ".",     "/",
    "0",     "1",     "2",     "3",     "4",     "5",     "6",     "7",
    "8",     "9",     ":",     ";",     "<",     "=",     ">",     "?",

    "@",     "A",     "B",     "C",     "D",     "E",     "F",     "G",
    "H",     "I",     "J",     "K",     "L",     "M",     "N",     "O",
    "P",     "Q",     "R",     "S",     "T",     "U",     "V",     "W",
    "X",     "Y",     "Z",     "[",     "\\\\",  "]",     "^",     "_",
    "`",     "a",     "b",     "c",     "d",     "e",     "f",     "g",
    "h",     "i",     "j",     "k",     "l",     "m",     "n",     "o",
    "p",     "q",     "r",     "s",     "t",     "u",     "v",     "w",
    "x",     "y",     "z",     "{",     "|",     "}",     "~",     "\\177",

    "\\200", "\\201", "\\202", "\\203", "\\204", "\\205", "\\206", "\\207",
    "\\210", "\\211", "\\212", "\\213", "\\214", "\\215", "\\216", "\\217",
    "\\220", "\\221", "\\222", "\\223", "\\224", "\\225", "\\226", "\\227",
    "\\230", "\\231", "\\232", "\\233", "\\234", "\\235", "\\236", "\\237",
    "\\240", "\\241", "\\242", "\\243", "\\244", "\\245", "\\246", "\\247",
    "\\250", "\\251", "\\252", "\\253", "\\254", "\\255", "\\256", "\\257",
    "\\260", "\\261", "\\262", "\\263", "\\264", "\\265", "\\266", "\\267",
    "\\270", "\\271", "\\272", "\\273", "\\274", "\\275", "\\276", "\\277",

    "\\300", "\\301", "\\302", "\\303", "\\304", "\\305", "\\306", "\\307",
    "\\310", "\\311", "\\312", "\\313", "\\314", "\\315", "\\316", "\\317",
    "\\320", "\\321", "\\322", "\\323", "\\324", "\\325", "\\326", "\\327",
    "\\330", "\\331", "\\332", "\\333", "\\334", "\\335", "\\336", "\\337",
    "\\340", "\\341", "\\342", "\\343", "\\344", "\\345", "\\346", "\\347",
    "\\350", "\\351", "\\352", "\\353", "\\354", "\\355", "\\356", "\\357",
    "\\360", "\\361", "\\362", "\\363", "\\364", "\\365", "\\366", "\\367",
    "\\370", "\\371", "\\372", "\\373", "\\374", "\\375", "\\376", "\\377",
};


/*****************************************************************************/
/*                                                                           */
/*  FULL_CHAR *StringQuotedWord(x)                                           */
/*                                                                           */
/*  Returns the string in QWORD x in the form it would need to take if it    */
/*  was a quoted word in a Lout source file.  Note that the result is        */
/*  returned in a static variable so it needs to be copied before a          */
/*  subsequent call to StringQuotedWord is made.                             */
/*                                                                           */
/*****************************************************************************/

FULL_CHAR *StringQuotedWord(OBJECT x)
{ FULL_CHAR *p, *q, *r;
  static FULL_CHAR buff[MAX_BUFF];
  assert( type(x) == QWORD, "StringQuotedWord: type(x) != QWORD!" );
  q = buff;
  *q++ = CH_QUOTE;
  for( p = string(x);  *p != '\0';  p++ )
  { 
    for( r = (FULL_CHAR *) quoted_string[*p];  *r != '\0';  *q++ = *r++ );
  }
  *q++ = CH_QUOTE;
  *q++ = '\0';
  return buff;
} /* StringQuotedWord */
