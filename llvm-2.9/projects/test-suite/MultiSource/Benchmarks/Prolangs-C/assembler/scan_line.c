/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ================================ scan_line.c ============================ */
/* Contains routines for doing simple parsing of source program (eg. return  */
/* label, operator and everthing after the operator).                        */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "stringI.h"
#include "assem.h"
#include "buffer.h"
#include "constants.h"

/* pass_type               Which pass the assembler is currently in. Used to */
/*                         surpress redisplaying pass1 error messages in     */
/*                         pass2.                                            */
enum pass_type {pass1,pass2};

/* ----------------------------- UPPER (local) ----------------------------- */
/* If CH is a lower case letter, return the upper case version, otherwise    */
/* Return the character.                                                     */
char UPPER(CH)
     char CH;             /* Character to convert to upper case (if needed). */
{
  if ((CH >= 'a') && (CH <= 'z')) return (CH - 'a' + 'A');
  else return CH;
}

/* ------------------------------ IS_ALPHA_NUM ----------------------------- */
/* True if CH is a capital letter or a digit. Otherwise false.               */
int IS_ALPHA_NUM(CH)
     char CH;              /* Character in question.                         */
{
  CH = UPPER(CH);
  return ( ((CH >= 'A') && (CH <= 'Z')) || ((CH >= '0') && (CH <= '9')) );
}

/* ------------------------------ IS_BLANK_OR_TAB -------------------------- */
/* True if CH is a blank or a tab, otherwise false.                          */
int IS_BLANK_OR_TAB(CH)
     char CH;             /* Character in question.                          */
{
  return ((CH == ' ' /* Blank */) || (CH == '	' /* TAB */));
}

/* ----------------------------- eoln -------------------------------------- */
/* true if CH is the End Of LiNe character, otherwise false.                 */
int eoln(CH)
     char CH;             /* Character in question.                          */
{
  return (CH == '\0');
}

/* -------------------------------- GET_LABEL ------------------------------ */
/* Get a SIC/XE Label (Letter followed by zero to seven letters or digits).  */
/* Return it in LABEL. Get it from INPUT starting from NEXT_CHAR.            */
void GET_LABEL (LABEL,INPUT,NEXT_CHAR,PASS)
     char *LABEL;          /* Where to put the label if found.               */
     char *INPUT;          /* String to get the label from.                  */
     enum pass_type PASS;  /* pass1 = buffers error messages in ERROR_REC_BUF*/
                           /* pass2 = don't buffer error messages.           */
     char **NEXT_CHAR;     /* where to start looking for the label in INPUT  */
{
  if ((UPPER(**NEXT_CHAR) >= 'A') && (UPPER(**NEXT_CHAR) <= 'Z')) {
    
    /* GET LABEL */
    int LABEL_LEN = 1;
    (*NEXT_CHAR) ++;
     while (IS_ALPHA_NUM(**NEXT_CHAR) && (!eoln(**NEXT_CHAR))) {
      LABEL_LEN ++;
      (*NEXT_CHAR) ++;
    };
    if (LABEL_LEN > LABEL_SIZE_1) {
       if (PASS == pass1) {
	char TEMP;
	char *ERROR_MSG;

	TEMP = INPUT[LABEL_LEN];
	INPUT[LABEL_LEN] = '\0';

	ERROR_MSG = (char *) malloc((unsigned int) 
	       (80+ ((int) log10((double) LABEL_SIZE_1)) +strlen(INPUT)));
	(void) sprintf(ERROR_MSG,
	       "eERROR[1]: Label '%s' is too long (MAX %d characters).",
		       INPUT,LABEL_SIZE_1);
	ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
	free(ERROR_MSG);

	INPUT[LABEL_LEN] = TEMP;
      };
/* ------------------- truncate to 8 characters.                             */
      LABEL_LEN = LABEL_SIZE_1;
     };
    (void) strncpy(LABEL,INPUT,LABEL_LEN);
    LABEL[LABEL_LEN] = '\0';
    /* End GET LABEL */
  } else {
/* ---------- Object Starting at NEXT_CHAR can not possible be a label       */
/*            Return the empty string ("")                                   */
    LABEL[0] = '\0';
  }
}

/* ------------------------- GET_OPCODE_STR -------------------------------- */
/* Get a SIC/XE OPERAND (string of letters, not more than 8).Return it in    */
/* OPCODE. Get it from INPUT starting from NEXT_CHAR.                        */
void GET_OPCODE_STR (OPCODE,NEXT_CHAR,PASS,STREAM)
     char *OPCODE;             /* Where to put the opcode that was found.    */
     enum pass_type PASS;      /* pass1 = print error messages; pass2 = don't*/
                               /* print error messages.                      */
     char **NEXT_CHAR;         /* Where to look for the OPERAND              */
     FILE *STREAM;             /* Stream to output error messages to.        */
{
  char *START_OF_OPCODE;
  START_OF_OPCODE = *NEXT_CHAR;
  if ((UPPER(**NEXT_CHAR) >= 'A') && (UPPER(**NEXT_CHAR) <= 'Z')) {
    
    int OP_LEN = 1;
    (*NEXT_CHAR) ++;
     while (((UPPER(**NEXT_CHAR) >= 'A') && (UPPER(**NEXT_CHAR) <= 'Z')) 
	    && (!eoln(**NEXT_CHAR))) {
      OP_LEN ++;
      (*NEXT_CHAR) ++;
    };
    if (OP_LEN > LABEL_SIZE_1) {
/* -------------- (maybe) print error message, and truncate to 8 characters. */
       if (PASS == pass1) {
	char TEMP;
	TEMP = START_OF_OPCODE[OP_LEN];
	START_OF_OPCODE[OP_LEN] = '\0';
	(void) fprintf(STREAM,
     "eERROR[3]: OpCode field '%s' is too long. Truncating to %d charaters.\n"
         ,START_OF_OPCODE,LABEL_SIZE_1);
	START_OF_OPCODE[OP_LEN] = TEMP;
      };
      OP_LEN = LABEL_SIZE_1;
     };
    (void) strncpy(OPCODE,START_OF_OPCODE,OP_LEN);
    OPCODE[OP_LEN] = '\0';
   } else {
    OPCODE[0] = 0;
  }
}

/* ---------------------------- SCAN_LINE ---------------------------------- */
/* Reads in a sourse program line and does simple parsing.                   */
void SCAN_LINE (LOCATION_COUNTER,INPUT_LINE,LABEL,EXTENDED,OPERATOR,
		REST,PASS,STREAM,INPUT_STREAM)
     int LOCATION_COUNTER;        /* SIC/XE location counter at start of line*/
     char **INPUT_LINE;           /* Where to return the source line         */
     char *LABEL;                 /* Where to return the label if one exists */
     int *EXTENDED;               /* Boolean: Is there a '+' before the      */
                                  /*   operator. i.e. is this extended format*/
     char *OPERATOR;              /* Where to return the operator.           */
     char **REST;                 /* Where to return everything after the    */
                                  /*   operator. Ignoring leading white-space*/
     enum pass_type PASS;         /* pass1 = do print error messages and     */
                                  /*   source; pass2 = do not print error    */
                                  /*   messages.                             */
     FILE *STREAM;                /* Stream for outputing error messages and */
                                  /*   the source code                       */
     FILE *INPUT_STREAM;          /* Where to get the source line from       */
{
  char *CH;

  GET_LINE(INPUT_LINE,INPUT_STREAM);
  (*REST) = (*INPUT_LINE);
  if (PASS == pass1)
    (void) fprintf(STREAM,"p%d %s\n",LOCATION_COUNTER,*REST);
  CH = *REST;
  if (*CH != '.') {
/* ---------------------- Not a comment line                                 */
    GET_LABEL(LABEL,*REST,&CH,PASS);
    OUTPUT_BUFFER(&ERROR_REC_BUF,STREAM,1);
/* ---------------------- Must be white space after a operator.              */
    if (!IS_BLANK_OR_TAB(*CH) && !eoln(*CH)) {
      if (PASS == pass1)
	(void) fprintf(STREAM,
"eERROR[2]: Illegal LABEL/OPCODE seperater('%c') expected <tab> or <blank>.\n",
		       *CH);
      CH ++;
    };
/* ---------------------- remove white space characters                      */
    while (IS_BLANK_OR_TAB(*CH) && !eoln(*CH)) CH ++;

    if (*CH == '+') {
      CH ++;
      (*EXTENDED) = 1;
    } else (*EXTENDED) = 0;

    GET_OPCODE_STR(OPERATOR,&CH,PASS,STREAM);

    if ((*EXTENDED == 1) && !strcmp(OPERATOR,"") && (PASS == pass1))
      (void) fprintf(STREAM,"eERROR[5]: Expected an OPCODE after the '+'.\n");

/* ---------------------- Must be white space after a label.                 */
    if (!IS_BLANK_OR_TAB(*CH) && !eoln(*CH)) {
      if (PASS == pass1)
	(void) fprintf(STREAM,
"eERROR[4]: Illegal OPCODE/OPERANDS seperater('%c') expected TAB or BLANK.\n",
		       *CH);
      CH ++;
    };
/* ---------------------- remove white space characters                      */
    while (IS_BLANK_OR_TAB(*CH) && !eoln(*CH)) CH ++;

    (*REST) = CH;
  } else {
/* ---------------------- comment line */
    LABEL[0] = '\0';
    (*EXTENDED) = 0;
    OPERATOR[0] = '\0';
  }
}

