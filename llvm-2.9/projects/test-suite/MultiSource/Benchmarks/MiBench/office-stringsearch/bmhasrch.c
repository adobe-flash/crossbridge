/* +++Date last modified: 05-Jul-1997 */

/*
**  Boyer-Moore-Horspool pattern match
**  Case-insensitive with accented character translation
**
**  public domain by Raymond Gardner 7/92
**
**  limitation: pattern length + subject length must be less than 32767
**
**  10/21/93 rdg  Fixed bug found by Jeff Dunlop
*/
#include <limits.h>                                         /* rdg 10/93 */
#include <stddef.h>
#include <string.h>
typedef unsigned char uchar;

#define LOWER_ACCENTED_CHARS

unsigned char lowervec[UCHAR_MAX+1] = {                     /* rdg 10/93 */
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
 32,'!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
'0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
'@','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','[','\\',']','^','_',
'`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','{','|','}','~',127,
#ifdef LOWER_ACCENTED_CHARS
'c','u','e','a','a','a','a','c','e','e','e','i','i','i','a','a',
'e',145,146,'o','o','o','u','u','y','o','u',155,156,157,158,159,
'a','i','o','u','n','n',166,167,168,169,170,171,172,173,174,175,
#else
128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
#endif
176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

#define lowerc(c) lowervec[(uchar)(c)]

#define LARGE 32767

static int patlen;
static int skip[UCHAR_MAX+1];                               /* rdg 10/93 */
static int skip2;
static uchar *pat;

void bmha_init(const char *pattern)
{
      int i, j;
      pat = (uchar *)pattern;
      patlen = strlen(pattern);
      for (i = 0; i <= UCHAR_MAX; ++i)                      /* rdg 10/93 */
      {
            skip[i] = patlen;
            for (j = patlen - 1; j >= 0; --j)
            {
                  if (lowerc(i) == lowerc(pat[j]))
                        break;
            }
            if (j >= 0)
                  skip[i] = patlen - j - 1;
            if (j == patlen - 1)
                  skip[i] = LARGE;
      }
      skip2 = patlen;
      for (i = 0; i < patlen - 1; ++i)
      {
            if ( lowerc(pat[i]) == lowerc(pat[patlen - 1]) )
                  skip2 = patlen - i - 1;
      }
}

char *bmha_search(const char *string, const int stringlen)
{
      int i, j;
      char *s;

      i = patlen - 1 - stringlen;
      if (i >= 0)
            return NULL;
      string += stringlen;
      for ( ;; )
      {
            while ((i += skip[((uchar *)string)[i]]) < 0)
                  ;                           /* mighty fast inner loop */
            if (i < (LARGE - stringlen))
                  return NULL;
            i -= LARGE;
            j = patlen - 1;
            s = (char *)string + (i - j);
            while (--j >= 0 && lowerc(s[j]) == lowerc(pat[j]))
                  ;
            if ( j < 0 )                                    /* rdg 10/93 */
                  return s;                                 /* rdg 10/93 */
            if ( (i += skip2) >= 0 )                        /* rdg 10/93 */
                  return NULL;                              /* rdg 10/93 */
      }
}
