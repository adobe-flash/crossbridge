/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== pass1.c ================================== */
/* Drives pass1 of the assembler, but lets other routines do the real work   */

#include <stdio.h>
#include "assem.h"
#include "scan_line.h"
#include "sym_tab.h"
#include "op_tab.h"
#include "convert.h"
#include "buffer.h"
#include "constants.h"
#include "pseudo.h"

#define START_LEGAL_1            0 /* = May see a START legally              */
#define SEEN_START_1             1 /* = Have seen a START                    */
/* SEND_START_OP                      Global that contains whether or not    */
/*                                    a START operator has been seen         */
int SEEN_START_OP = START_LEGAL_1;


#define NO_END_SEEN_1          0 /* = Haven't seen an END                    */
#define END_SEEN_1             1 /* = Have seen an END but haven't           */
/*                                  found any code after it                  */
#define END_SEEN_WITH_CODE_1   2 /* = Have seen an END and code after it     */
#define START_OF_CODE_1        3 /* = Start of code                          */
/* SEEN_END_OP                       Flag to determine if seen an end since  */
/*                                   start of last module or not. If so it   */
/*                                   also indicated if any code seen since   */
/*                                   end (so only one 'code after end' error */
/*                                   is generated.                           */
int SEEN_END_OP = START_OF_CODE_1;

/* LOCATION_EXCEEDS_MEM_SIZE         Indicates if location counter is greater*/
/*                                   then the memory size.                   */
int LOCATION_EXCEEDS_MEM_SIZE = FALSE_1;

/* -------------------------- CHANGE_LOCATION ------------------------------ */
/* LOCATION = LOCATION + INCREMENT unless this puts LOCATION past MEM_SIZE,  */
/* or if the location has already exceeded the memory size.                  */
void CHANGE_LOCATION(int *LOCATION,int INCREMENT,FILE *OUTPUT)
{
  if (!LOCATION_EXCEEDS_MEM_SIZE)
    if ( ((*LOCATION + INCREMENT) > MEM_SIZE_1) || (*LOCATION == MEM_SIZE_1))
      {
	LOCATION_EXCEEDS_MEM_SIZE = TRUE_1;
	(void) fprintf(OUTPUT,
		       "eERROR[12]: Location exceeds the memory size (%d)\n",
		MEM_SIZE_1);
	(*LOCATION) = MEM_SIZE_1 - 1;
      } else (*LOCATION) += INCREMENT;
}

/* ---------------------- CAPITALIZE_STRING -------------------------------- */
/* Make all lower case letters in the string STR upper case letters          */
void CAPITALIZE_STRING(char STR[])
{
  int I;
  for (I=0; I<= (strlen(STR)); I++)
    if ((STR[I] >= 'a') && (STR[I] <= 'z'))
      STR[I] = STR[I] - 'a' + 'A';
}

/* --------------------------- PASS1 --------------------------------------- */
/* Drives the pass1 process, letting others do the real work.                */
void PASS1(FILE *INPUT_FILE,SYMBOL_TABLE *SYM_TABLE,FILE *TEMP_OUTPUT_STREAM)
{
  char LABEL_NAME[LABEL_SIZE_1+1];       /* Place to store a label.          */
  char OPCODE[LABEL_SIZE_1+1];           /* Place to store an opcode.        */
  char *ARGUMENTS;                       /* Pointer to arguments/comments    */
  char *INPUT_LINE;                      /* Pointer to whole source line     */
  int EXTENDED_CODE;                     /* Boolean: Is this an extended     */
                                         /*   format instruction.            */

  int LOCATION_COUNTER =0;
  MODULE_NAME[0] = '_';
  MODULE_NAME[1] = '\0';

  (void) INSERT_IN_SYM_TAB(MODULE_NAME,MODULE_NAME,0,RELATIVE,SYM_TABLE);

  MAIN_ROUTINE[0] = '\0';
  LABEL_NAME[0] = '\0';

  while (!feof(INPUT_FILE)){
/* --------------------- Get the source line and do some parsing             */
    SCAN_LINE(LOCATION_COUNTER,&INPUT_LINE,LABEL_NAME,
	      &EXTENDED_CODE,OPCODE,&ARGUMENTS,pass1,
	      TEMP_OUTPUT_STREAM,INPUT_FILE);

    CAPITALIZE_STRING(LABEL_NAME);
    CAPITALIZE_STRING(OPCODE);
    CAPITALIZE_STRING(ARGUMENTS);

    if (( strcmp(LABEL_NAME,"") || strcmp(OPCODE,"") || EXTENDED_CODE)
      && (SEEN_END_OP == END_SEEN_1) && strcmp(OPCODE,"CSECT")
	&& strcmp(OPCODE,"START")) {
      (void) fprintf(TEMP_OUTPUT_STREAM,
		     "eERROR[30]: Statements following END.\n");
      SEEN_END_OP = END_SEEN_WITH_CODE_1;
    }

    if (strcmp(LABEL_NAME,""))
      if ( LOOK_UP_SYMBOL(MODULE_NAME,LABEL_NAME,SYM_TABLE) == NULL) {
	CHANGE_LOCATION(&LOCATION_COUNTER,0,TEMP_OUTPUT_STREAM);
	if (!LOCATION_EXCEEDS_MEM_SIZE) 
	  (void) INSERT_IN_SYM_TAB(MODULE_NAME,LABEL_NAME,LOCATION_COUNTER,
				   RELATIVE,SYM_TABLE);
      }
      else (void) fprintf(TEMP_OUTPUT_STREAM,
		   "eERROR[6]: %s is a multipy defined label.\n",
		   LABEL_NAME);

    if (strcmp(OPCODE,""))
      {
/* --------- Have an OPERATOR. Change location counter depending on format.  */
	struct OP_ENTRY *OPCODE_INFO;
	switch ( (*(OPCODE_INFO = LOOK_UP_OP(OPCODE))).FORMAT) 
	  {
	  case NOT_FOUND: (void) fprintf(TEMP_OUTPUT_STREAM,
			     "eERROR[9]: %s is not a legal OPCODE.\n",OPCODE);
	    break;
	  case ONE: CHANGE_LOCATION(&LOCATION_COUNTER,1,TEMP_OUTPUT_STREAM);
	    if (EXTENDED_CODE)
	      (void) fprintf(TEMP_OUTPUT_STREAM,
		      "eERROR[8]: + is an illegal prefix to %s.",OPCODE);
	    break;
	  case TWO: CHANGE_LOCATION(&LOCATION_COUNTER,2,TEMP_OUTPUT_STREAM);
	    if (EXTENDED_CODE)
	      (void) fprintf(TEMP_OUTPUT_STREAM,
		      "eERROR[8]: + is an illegal prefix to %s.",OPCODE);
	    break;
	  case THREE_FOUR: if (EXTENDED_CODE)	
	    CHANGE_LOCATION(&LOCATION_COUNTER,4,TEMP_OUTPUT_STREAM);
	    else CHANGE_LOCATION(&LOCATION_COUNTER,3,TEMP_OUTPUT_STREAM);
	    break;
	  case PSEUDO: if (EXTENDED_CODE)
	    (void) fprintf(TEMP_OUTPUT_STREAM,
			   "eERROR[8]: + is an illegal prefix to %s.",OPCODE);

/* ************************************************************************* */
/* The following instruction calls the correct procedure to do what needs to */
/* be done for a particular pseudo operator. The operator table contains     */
/* the pointer to the correct routine.                                       */
	    DO_PSEUDO((*OPCODE_INFO).FUNCTION,LABEL_NAME,ARGUMENTS,
		      &LOCATION_COUNTER,SYM_TABLE,TEMP_OUTPUT_STREAM);
	    break;
	  }
      }

    if ((SEEN_END_OP == START_OF_CODE_1) && 
	(strcmp(OPCODE,"") || strcmp(LABEL_NAME,""))) {
      SEEN_END_OP = NO_END_SEEN_1;
      (void) fprintf(TEMP_OUTPUT_STREAM,
		     "eERROR[35]: No START/CSECT found before statements.\n");
    }
  }
  OUTPUT_BUFFER(&MOD_REC_BUF,TEMP_OUTPUT_STREAM,1);
  if (!SEEN_END_OP) 
    (void) fprintf(TEMP_OUTPUT_STREAM,
	       "eERROR[36]: End of File detected without an END statement.\n");

}
