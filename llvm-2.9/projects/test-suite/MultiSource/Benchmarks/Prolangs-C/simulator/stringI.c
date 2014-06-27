/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ================================ stringI.c ============================== */
/* Reads in one line of input gauranteeing that it will return a pointer to  */
/* the whole line, no matter how long it is (it uses CALLOC a version of     */
/* MALLOC).                                                                  */
#include <stdio.h>

/* CARRAGIAGE_RETURN            <cr> is ascii 10                             */
#define CARRIAGE_RETURN         10

/* LEX_LEN_INCREMENT            For long strings, number of characters to    */
/*                              increase the string buffer by on overflow.   */
#define LEX_LEN_INCR            128

char CH = ' ';                 /* Current input Character                    */
char LEXEME[LEX_LEN_INCR+1];   /* Input String                               */

char *FRONT,*BACK;             /* FRONT and BACK characters of Input String  */
unsigned LEX_LEN = LEX_LEN_INCR;/* Size of the Input String Buffer           */


/* ------------------------------- GETCHR ---------------------------------- */
/* gets the next character form input file and expands buffer if needed      */
void GETCHR(FILE *STREAM)
{
/*  int TEMP; */

/* --------- get next character                                              */
  CH = getc(STREAM);

/*  TEMP = (int) (FRONT-BACK); */
  if ((FRONT-BACK) < ((int)LEX_LEN-3)) {
/* --------- update input string appropriately                               */
      FRONT ++;
      *FRONT = CH;
      *(FRONT+1) = '\0';
    }
}

/* --------------------------------- GET_LINE ------------------------------ */
/* Read in a line from the input stream.                                     */
void GET_LINE(char **LINE,FILE *STREAM)
{
/* -------- Clear input string                                               */
  LEXEME[0] = '\0';
/* -------- reset pointers                                                   */
  BACK = LEXEME;
  FRONT = BACK - 1;

  GETCHR(STREAM);
  while ((CH != CARRIAGE_RETURN) && !feof(STREAM))
    GETCHR(STREAM);
  *FRONT = '\0';
  *LINE = LEXEME;
}
