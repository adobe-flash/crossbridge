/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================= convert.c ================================= */
/* Contains routines that convert one thing to another. Most of the routines */
/* deal with converting a number to a string of that number in some base, or */
/* converting a string (assuming some base) into an integer. Basically       */
/* just a file of miscellaneous 'useful' routines.                           */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "boolean.h"
#include "constants.h"

/* ----------------------------- eoln -------------------------------------- */
/* true if CH is the End Of LiNe character, otherwise false.                 */
BOOLEAN eoln(char CH)
{
  return (CH == '\0');
}

/* ------------------------ CHAR_TO_DIGIT ---------------------------------- */
/* Convert a character (CH) into its integer value give base NUM_BASE. Return*/
/* -1 if it is not a valid digit in that base.                               */
int CHAR_TO_DIGIT(char CH,int NUM_BASE)
{
  int DIGIT = -1;
  if ( (CH >= '0') && (CH <= '9') ) DIGIT = (int) (CH - '0');
  if ( (CH >= 'A') && (CH <= 'Z') ) DIGIT = ((int) (CH - 'A'))+10;
  if (DIGIT >= NUM_BASE) DIGIT = -1;
  return DIGIT;
}


/* ------------------------- NUM_TO_STR ------------------------------------ */
/* Put into STR the string with LEN digits that represents the number NUM    */
/* in base BASE (eg. NUM_TO_STR(10,16,3,STR) puts "00A" in STR).             */
void  NUM_TO_STR(int NUM,int BASE,int LEN,char *STR)
{
  int I;                        /* loop counter                              */
  int DIGIT;                    /* one digit in base specified of NUM        */

  STR[LEN] = '\0';
  for (I=(LEN-1); I>=0; I--) {
    DIGIT = NUM - (NUM/BASE) * BASE;
    NUM /= BASE;
    if ((DIGIT >= 0) && (DIGIT <= 9))
      STR[I] = (char) DIGIT + '0';
    else STR[I] = (char) (DIGIT-10) + 'A';
  }
  if (NUM != 0)
    (void) printf("NUM_TO_STR called illegally.\n");
}

/* ------------------------------ GET_NUM ---------------------------------- */
/* CURRENT_CHAR points to a string. Starting with the character              */
/* **CURRENT_CHAR and get the biggest possible integer in BASE NUM_BASE. If  */
/* that number is too large/small to fit in BITS bits (2's complement),      */
/* Put an error message into ERROR_REC_BUF. Returns 0 if this or any error is*/
/* detected, otherwise it returns the integer representation of the number.  */
/* NOTE: If number is to large/small this routine stops as soon as it        */
/*   realizes this w/o (with out) looking at the rest of the input.          */
int GET_NUM(char **CURRENT_CHAR,int BITS,int NUM_BASE,BOOLEAN *ERROR)
{
  int CONVERT = 0;                /* CONVERT is the converted integer of the */
                                  /*    string */
  int MAX_UNSIGNED_INT;           /* MAX_UNSIGNED_INT = 2^BITS               */
                                  /*    - Biggest biggest number + 1         */
				  /*      (w/o sign) that fits               */
  int DIGIT;                      /* One digit of the number.                */

  MAX_UNSIGNED_INT = (int) pow(2.0,BITS*1.0);

/* ----------------------------- Get the biggest [legal] number you can      */
  while ( ( (DIGIT = CHAR_TO_DIGIT(**CURRENT_CHAR,NUM_BASE)) != -1) &&
	 !eoln(**CURRENT_CHAR) ) {
    if (CONVERT <= MAX_UNSIGNED_INT)
      CONVERT = CONVERT*NUM_BASE + DIGIT;
    (*CURRENT_CHAR) ++;
  }

  if ( (CONVERT > (MAX_UNSIGNED_INT - 1)) ||
      (!eoln(**CURRENT_CHAR)) ) (*ERROR) = TRUE_1;
  
  return CONVERT;
}

/* -------------------------------- STR_TO_NUM ----------------------------- */
/* Convert the first DIGITS characters of STR to an integer of base BASE.    */
void STR_TO_NUM(char *STR,int DIGITS,int BASE,int *NUM,BOOLEAN *ERROR)
{
  int I;
  int ONE_DIGIT;
  BOOLEAN LOCAL_ERROR = FALSE_1;

  (*NUM) = 0;
  for (I = 0; ((I < DIGITS) && !LOCAL_ERROR); I ++)
    if ( (ONE_DIGIT = CHAR_TO_DIGIT(STR[I],BASE)) == -1)
      LOCAL_ERROR = TRUE_1;               /* Not a valid number.             */
    else
      (*NUM) = (*NUM)*BASE + ONE_DIGIT;

  if (LOCAL_ERROR) (*ERROR) = TRUE_1;
}
