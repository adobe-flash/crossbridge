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
#include <stdlib.h>
#include "sym_tab.h"
#include "constants.h"
#include "scan_line.h"
#include "assem.h"
#include "buffer.h"


/* MISSING_LABEL_NAME            Sections need names because the symbol      */
/*                               requires module names. MISSING_LABEL_NAME   */
/*                               is a global that is the name to the next    */
/*                               section that needs a name.                  */
char MISSING_LABEL_NAME[3] = "_!";

/* ------------------------ RESET_MISSING_LABEL_NAME ----------------------- */
/* Between pass1 and pass2, MISSING_LABEL_NAME must be reset to its          */
/* orginal value. This procedure does this.                                  */
void RESET_MISSING_LABEL_NAME()
{
  (void) strcpy(MISSING_LABEL_NAME,"_!");
}

/* ------------------------ GET_NEXT_MISSING_LABEL ------------------------- */
/* Puts MISSING_LABEL_NAME into string LABEL, and puts it into the symbol    */
/* table. It then changes MISSING_LABEL_NAME to some other unique name.      */
/* NOTE: this will only return about 90 unique names, after that it returns  */
/*       the same name repeadily. This would result only some additional     */
/*       previously defined label error messages. Never called when executed */
/*       on a source with no errors.                                         */
void GET_NEXT_MISSING_LABEL(char *LABEL,SYMBOL_TABLE *SYM_TAB)
{
  char *CH;

  (void) strcpy(LABEL,MISSING_LABEL_NAME);
  CH = &(MISSING_LABEL_NAME[strlen(MISSING_LABEL_NAME)-1]);
  if ((*CH) < '\127') (*CH) = (*CH) + '\1';
  (void) INSERT_IN_SYM_TAB(MODULE_NAME,LABEL,0,RELATIVE,SYM_TAB);
}


/* ---------------------------- BLANK_STR ---------------------------------- */
/* Set characters 0-7 of STRING to blank and make the 8th character NULL     */
/* ie. a 8 character string of blanks.                                       */
void BLANK_STR(char *STRING)
{
  int I;
  for (I=0;I < LABEL_SIZE_1; I ++)
    STRING[I] = ' ';
  STRING[LABEL_SIZE_1] = '\0';
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

/* -------------------------- PRT_NUM ------------------------------------- */
/* Output (to stream OUTPUT) the string with LEN digits that represents the */
/* number NUM in base BASE (eg. NUM_TO_STR(20,16,2,STR) puts "14" in STR).  */
void PRT_NUM(int NUM,int NUM_BASE,int LEN,FILE *OUTPUT)
{
  int COUNT;                  /* loop counter                               */
  int MAX_NUM;                /* Biggest number can represent with LEN      */
                              /*    digits in base BASE                     */
  int DIGIT;                  /* One digit in base specified of NUM         */

  MAX_NUM = ((int) pow((NUM_BASE*1.0),(LEN-1)*1.0));

  if (NUM < 0) NUM = MAX_NUM*NUM_BASE + NUM;
  
  if (NUM >= MAX_NUM * NUM_BASE) 
    (void) printf("ASSEMBLER ERROR: PRT_NUM called illegally.\n");

  for (COUNT= MAX_NUM; COUNT >= 1; COUNT /= NUM_BASE) {
    DIGIT = NUM / COUNT;
    NUM = NUM - DIGIT*COUNT;
    if ((DIGIT >= 0) && (DIGIT <= 9))
      (void) fprintf(OUTPUT,"%c",(char) (DIGIT + '0'));
    else (void) fprintf(OUTPUT,"%c",(char) (DIGIT - 10 + 'A'));
  }

}


/* ------------------------------ GET_NUM ---------------------------------- */
/* CURRENT_CHAR points to a string. Starting with the character              */
/* **CURRENT_CHAR and get the biggest possible integer in BASE NUM_BASE. If  */
/* that number is too large/small to fit in BITS bits (2's complement),      */
/* Put an error message into ERROR_REC_BUF. Returns 0 if this or any error is*/
/* detected, otherwise it returns the integer representation of the number.  */
/* NOTE: If number is to large/small this routine stops as soon as it        */
/*   realizes this w/o (with out) looking at the rest of the input.          */
int GET_NUM(char **CURRENT_CHAR,int BITS,int NUM_BASE)
{
  int CONVERT = 0;                /* CONVERT is the converted integer of the */
                                  /*    string */
  int MAX_UNSIGNED_INT;           /* MAX_UNSIGNED_INT = 2^(BITS-1)           */
                                  /*    - Biggest negative number            */
				  /*      (w/o sign) that fits               */
                                  /*    - Biggest biggest number + 1         */
				  /*      (w/o sign) that fits               */
  int SIGN = 1;                   /* Sign of the number- assume positive.    */
  char *INPUT_START;              /* Keeps track of where string started     */
  int DIGIT;                      /* One digit of the number.                */

  MAX_UNSIGNED_INT = (int) pow(2.0,(BITS-1)*1.0);
  INPUT_START = *CURRENT_CHAR;

  if ((**CURRENT_CHAR) == '-')
    SIGN = -1;
  else SIGN = 1;

  if ( ((**CURRENT_CHAR) == '-') || ((**CURRENT_CHAR) == '+')) {
/*  ---------------------------- MUST be a valid digit after the sign       */
    (*CURRENT_CHAR) ++;
    if (CHAR_TO_DIGIT(**CURRENT_CHAR,NUM_BASE) == -1) {
      char *ERROR_MSG;
      ERROR_MSG = (char *) malloc((unsigned int) 80);
      if (**CURRENT_CHAR > 0)
	(void) sprintf(ERROR_MSG,
		       "eERROR[15]: Illegal Expression. Found '%c' after %c.",
		       **CURRENT_CHAR,*( (*CURRENT_CHAR) - 1));
      else
	(void) sprintf(ERROR_MSG,
		       "eERROR[15]: Illegal Expression. Found '' after %c.",
		       *( (*CURRENT_CHAR) - 1));
      ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
      free(ERROR_MSG);
    }
  }

/* ----------------------------- Get the biggest [legal] number you can      */
  while ( ( (DIGIT = CHAR_TO_DIGIT(**CURRENT_CHAR,NUM_BASE)) != -1) &&
	 !eoln(**CURRENT_CHAR) ) {
    if (CONVERT <= MAX_UNSIGNED_INT)
      CONVERT = CONVERT*NUM_BASE + DIGIT;
    (*CURRENT_CHAR) ++;
  }

/* ----------------------------- Make sure number is not to big or small     */
  if ( ((CONVERT > MAX_UNSIGNED_INT) && (SIGN == -1)) ||
      ((CONVERT > (MAX_UNSIGNED_INT - 1)) && (SIGN == 1)) ) {
    char SAV_CHAR;
    char *ERROR_MSG;

    SAV_CHAR = **CURRENT_CHAR;
    **CURRENT_CHAR = '\0';
    ERROR_MSG = (char *) malloc((unsigned int)
	   (80+ ((int) log10((double) NUM_BASE)) +
	    ((int) log10((double) MAX_UNSIGNED_INT)) +strlen(INPUT_START)));
      
    if (SIGN == 1)
      (void) sprintf(ERROR_MSG, 
       "eERROR[10]: %s[%d] is too large for it's intended use (MAX %d[10]).",
		     INPUT_START,NUM_BASE,MAX_UNSIGNED_INT - 1);
    else
      (void) sprintf(ERROR_MSG, 
       "eERROR[11]: %s[%d] is too small for it's intended use (MIN %d[10]).",
		     INPUT_START,NUM_BASE,-1*MAX_UNSIGNED_INT);
    ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
    free(ERROR_MSG);
    **CURRENT_CHAR = SAV_CHAR;
    CONVERT = 0;
  }
  
  return CONVERT*SIGN;
}


/* ---------------------------- GET_EXPRESSION ----------------------------- */
/* Is used to find an expression (well, for now actually it can have no      */
/* operations, just single terms). Caller can specify what kind of expression*/
/* is needed (ABSOLUTE or RELATIVE) or can specify that either kind is okay. */
/* Sets WHAT_KIND to they type (kind) of expression found.                   */
int GET_EXPRESSION(char **CURRENT_CHAR,int BITS,int LOCATION,
                   SYMBOL_TABLE *SYM_TAB,int *WHAT_KIND)
{
  char *START;                        /* remember where the expression       */
                                      /*    started in string.               */
  START = (*CURRENT_CHAR);

/* ------------------ Check if a decimal constant, if so get it              */
  if ((*WHAT_KIND != RELATIVE_VALUE_1) && 
      ( ( ((**CURRENT_CHAR) >= '0') && ((**CURRENT_CHAR) <= '9') ) ||
       ((**CURRENT_CHAR) == '-') || ((**CURRENT_CHAR) == '+') ))
 {
    (*WHAT_KIND) = ABSOLUTE_VALUE_1;
    return GET_NUM(CURRENT_CHAR,BITS,10);
  }

/* ------------------ Check if hexidecimal constant, if so, get it           */
  if ((*WHAT_KIND != RELATIVE_VALUE_1) && 
      ( ((**CURRENT_CHAR) == 'X') && (*((*CURRENT_CHAR)+1) == '\'')) ) {
    int RESULT;

    (*WHAT_KIND) = ABSOLUTE_VALUE_1;
    (*CURRENT_CHAR) += 2;
    RESULT = GET_NUM(CURRENT_CHAR,BITS,16);
/* --------------------------- Check for error messages                      */
    if ( (**CURRENT_CHAR) != '\'') {
      while ( (**CURRENT_CHAR != '\'') && (!eoln(**CURRENT_CHAR)) )
	(*CURRENT_CHAR) ++;
      if (eoln(**CURRENT_CHAR)) {
	char ERROR_MSG[80];
	(void) sprintf(ERROR_MSG,
	       "eERROR[16]: Illegal Hexidecimal. Expected close quote.");
	ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
      } else {
	char TEMP;
	char *ERROR_MSG;

	(*CURRENT_CHAR) ++;

	TEMP = **CURRENT_CHAR;
	**CURRENT_CHAR = '\0';

        ERROR_MSG = (char *) malloc((unsigned int)
		(80+ strlen(START)));
	 
	(void) sprintf(ERROR_MSG,
		       "eERROR[17]: Illegal Hexidecimal. %s",START);
	ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
	free(ERROR_MSG);
	**CURRENT_CHAR = TEMP;
      }
    }
    else (*CURRENT_CHAR) ++;
    return RESULT;
  }

/* ----------------------------- Must be a label, get it                     */
  {
    char LABEL_NAME[LABEL_SIZE_1 + 1];
    struct SYMBOL_TABLE_ENTRY *LABEL_INFO;

    GET_LABEL(LABEL_NAME,*CURRENT_CHAR,CURRENT_CHAR,pass1);

    if (!strcmp(LABEL_NAME,"")) {
/* ----------------------------- There was no label. ERROR                   */
   char *ERROR_MSG;
      
      ERROR_MSG = (char *) malloc((unsigned int)
				  (80+ strlen(*CURRENT_CHAR)));
      (void) sprintf(ERROR_MSG,
		     "eERROR[14]: Expected an expression, found '%s'.",
		     *CURRENT_CHAR);
      ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
      free(ERROR_MSG);
      }
    else {
/* ----------------------------- Get the value of the LABEL                  */
     LABEL_INFO = LOOK_UP_SYMBOL(MODULE_NAME,LABEL_NAME,SYM_TAB);
     if (LABEL_INFO == NULL) {
/* ----------------------------- has no value                                */
       char *ERROR_MSG;
      
       ERROR_MSG = (char *) malloc((unsigned int)
				  (80+ strlen(LABEL_NAME)));
       if ((*WHAT_KIND) != ABSOLUTE_OR_RELATIVE_1)
	 (void) sprintf(ERROR_MSG,
         "eERROR[24]: %s is not a defined symbol or is forwardly declared.",
			LABEL_NAME);
       else
	 (void) sprintf(ERROR_MSG,"eERROR[49]: %s is not a defined symbol.",
			LABEL_NAME);

       ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
       free(ERROR_MSG);
     } else
       if ( (*WHAT_KIND != RELATIVE_VALUE_1) &&
	   ((*LABEL_INFO).TYPE == ABSOLUTE)) {
	 (*WHAT_KIND) = ABSOLUTE_VALUE_1;
	 return (*LABEL_INFO).LOCATION;
       } else
	   if ( (*WHAT_KIND != ABSOLUTE_VALUE_1) &&
	       ((*LABEL_INFO).TYPE != ABSOLUTE)) {
	     if ((*LABEL_INFO).TYPE == EXTERN_REF) {
/* ----------------------- Value needs a modification record                 */
	       char OUTPUT_LINE[9+LABEL_SIZE_1];
	       char FIX_LEN_NAME[LABEL_SIZE_1+1];
	       char LEN_STR[3];
	       
	       NUM_TO_STR(LOCATION,16,6,OUTPUT_LINE);
	       NUM_TO_STR((BITS/4),16,2,LEN_STR);
	       (void) strcat(OUTPUT_LINE,LEN_STR);
	       (void) strcat(OUTPUT_LINE,"+");
	       BLANK_STR(FIX_LEN_NAME);
	       (void) strncpy(FIX_LEN_NAME,LABEL_NAME,strlen(LABEL_NAME));
	       (void) strcat(OUTPUT_LINE,FIX_LEN_NAME);
	       ADD_TO_END_OF_BUFFER(&MOD_REC_BUF,OUTPUT_LINE);
	       (*WHAT_KIND) = EXTERN_VALUE_1;
	     } else (*WHAT_KIND) = RELATIVE_VALUE_1;
	     return (*LABEL_INFO).LOCATION;
	   } else {
	     char *ERROR_MSG;
      
	     ERROR_MSG = (char *) malloc((unsigned int)
				     (80+ strlen(LABEL_NAME)));
	     if (*WHAT_KIND == ABSOLUTE_VALUE_1)
	       (void) sprintf(ERROR_MSG,
			"eERROR[25]: %s is a LABEL, expected a CONSTANT.",
			    LABEL_NAME);
	     else
	       (void) sprintf(ERROR_MSG,
			"eERROR[50]: %s is a CONSTANT, expected a LABEL.",
			    LABEL_NAME);
	     ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,ERROR_MSG);
	     free(ERROR_MSG);
	   } 
   }
    return 0;
  }
}
