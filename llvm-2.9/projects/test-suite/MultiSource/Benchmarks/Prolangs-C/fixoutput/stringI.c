/* To compile use:
cc stringI.c lex.o -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"  /* Useful Macro Definitions */

#define LEX_LEN_INCR          256
#define PERROR_1                 0

char CH;                         /* Current input Character */
char *LEXEME;                    /* Input String */

char *FRONT,*BACK;               /* FRONT and BACK characters of Input String*/
unsigned LEX_LEN = 0;            /* Size of the Input String Buffer */

/* FORWORD function definitions */
/* extern char *calloc();       */
 
void FLUSH(void)
/* Clear input string (LEXEME), reset buffer to correct size, and reset
   pointers */
{
/* reset buffer to correst size if necessary */
  if (LEX_LEN > LEX_LEN_INCR)
    {
      free(LEXEME);
      LEX_LEN = LEX_LEN_INCR;
      LEXEME = calloc(LEX_LEN,sizeof(CH));
    }
/* Clear input string */
  *LEXEME = 0;
/* reset pointers */
  BACK = LEXEME;
  FRONT = BACK - 1;
}

void GETCHR(void)
/* gets the next character form input file and expands buffer if needed */
{
  char *TEMP;
  int I;

/* expand buffer if needed */
  if (FRONT-BACK >= LEX_LEN-3)
    {
      LEX_LEN = LEX_LEN + LEX_LEN_INCR;
      TEMP = calloc(LEX_LEN,sizeof(CH));
      for (I = 0; I < LEX_LEN - LEX_LEN_INCR; I ++)
	TEMP[I] = LEXEME[I];
    }
  
/* get next character */
  CH = getc(stdin);
/* update input string appropriately */
  FRONT ++;
  *FRONT = CH;
  *(FRONT+1) = 0;
}

void BACKUP(void)
/* backups input string to last character of input and writes current 
   character back on input file */
{
  if (FRONT < BACK)
    (void) printf(
	  "%D SCANNER ERROR: Tried to BACKUP past beginning of a Token\n",
	   PERROR_1);
  else
    {
/* put current char back on input file */
      (void) ungetc(CH,stdin);
/* backup input string one character */
      *FRONT = 0;
      FRONT --;
      CH = *FRONT;
    }
}
