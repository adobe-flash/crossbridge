/* +++Date last modified: 05-Jul-1997 */

/*
**  Macros and prototypes for bit operations
**
**  public domain for SNIPPETS by:
**    Scott Dudley
**    Auke Reitsma
**    Ratko Tomic
**    Aare Tali
**    J. Blauth
**    Bruce Wedding
**    Bob Stout
*/

#ifndef BITOPS__H
#define BITOPS__H

#include <stdio.h>
#include <stdlib.h>                             /* For size_t           */
#include <limits.h>                             /* For CHAR_BIT         */
#include "sniptype.h"                           /* For TOBOOL()         */
#include "extkword.h"                           /* For CDECL            */

/*
**  Macros to manipulate bits in any integral data type.
*/

#define BitSet(arg,posn) ((arg) | (1L << (posn)))
#define BitClr(arg,posn) ((arg) & ~(1L << (posn)))
#define BitFlp(arg,posn) ((arg) ^ (1L << (posn)))
#define BitTst(arg,posn) TOBOOL((arg) & (1L << (posn)))

/*
**  Macros to manipulate bits in an array of char.
**  These macros assume CHAR_BIT is one of either 8, 16, or 32.
*/

#define MASK  CHAR_BIT-1
#define SHIFT ((CHAR_BIT==8)?3:(CHAR_BIT==16)?4:8)

#define BitOff(a,x)  ((void)((a)[(x)>>SHIFT] &= ~(1 << ((x)&MASK))))
#define BitOn(a,x)   ((void)((a)[(x)>>SHIFT] |=  (1 << ((x)&MASK))))
#define BitFlip(a,x) ((void)((a)[(x)>>SHIFT] ^=  (1 << ((x)&MASK))))
#define IsBit(a,x)   ((a)[(x)>>SHIFT]        &   (1 << ((x)&MASK)))

/*
**  BITARRAY.C
*/

char *alloc_bit_array(size_t bits);
int   getbit(char *set, int number);
void  setbit(char *set, int number, int value);
void  flipbit(char *set, int number);

/*
**  BITFILES.C
*/

typedef struct  {
      FILE *  file;       /* for stream I/O   */
      char    rbuf;       /* read bit buffer  */
      char    rcnt;       /* read bit count   */
      char    wbuf;       /* write bit buffer */
      char    wcnt;       /* write bit count  */
} bfile;

bfile * bfopen(char *name, char *mode);
int     bfread(bfile *bf);
void    bfwrite(int bit, bfile *bf);
void    bfclose(bfile *bf);

/*
** BITSTRNG.C
*/

void bitstring(char *str, long byze, int biz, int strwid);

/*
**  BSTR_I.C
*/

unsigned int bstr_i(char *cptr);

/*
**  BITCNT_1.C
*/

int CDECL bit_count(long x);

/*
**  BITCNT_2.C
*/

int CDECL bitcount(long i);

/*
**  BITCNT_3.C
*/

int CDECL ntbl_bitcount(long int x);
int CDECL BW_btbl_bitcount(long int x);
int CDECL AR_btbl_bitcount(long int x);

/*
**  BITCNT_4.C
*/

int CDECL ntbl_bitcnt(long x);
int CDECL btbl_bitcnt(long x);

#endif /*  BITOPS__H */
