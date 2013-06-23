/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================= pseudo.c ================================== */
/* Does most of the 'real' work of pass1. Contains the routines for          */
/* processing each of the pseudo operators.                                  */
/* Each of the procedures have the same paramters because they are all called*/
/* by the same invocation (by use of a function pointer). Thus may of the    */
/* procedures have parameters they never use.                                */


#include <string.h>
#include <stdio.h>
#include "assem.h"
#include "convert.h"
#include "sym_tab.h"
#include "pass1.h"
#include "scan_line.h"
#include "record.h"
#include "buffer.h"
#include "constants.h"
#include "pseudo.h"

/* ------------------------------- PSEUDO_BYTE ----------------------------- */
/* Process the BYTE pseudo operator.                                         */
void PSEUDO_BYTE(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                 SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  char *START;                      /* Remember where arguments started.     */
  int START_LOC;                    /* Where the location counter started at */
  int IS_HEX_Q;                     /* Is this a HEX (TRUE) or char (FALSE)  */
                                    /* Initial value.                        */

  START = ARGUMENTS;
  START_LOC = *LOCATION;

  if ( (IS_HEX_Q = ( (*ARGUMENTS == 'X') && ( *(ARGUMENTS + 1) = '\'')) ) ||
      ( (*ARGUMENTS == 'C') && ( *(ARGUMENTS + 1) = '\'')) )
    {
      int ERROR = FALSE_1;          /* Has an error been found.              */
      
      ARGUMENTS += 2;
      if (*ARGUMENTS == '\'')
	(void) fprintf(OUTPUT_STREAM,
		"eERROR[18]: Null hexidecimal/character constant.\n");
      else {
	int SIZE = 0;

/*  ------- Step through argument till find end or string or end of constant */
	while ( (!eoln(*ARGUMENTS)) && (*ARGUMENTS != '\'')) {
	  if (IS_HEX_Q) ERROR = (ERROR ||
				 (CHAR_TO_DIGIT(*ARGUMENTS,16) == -1));
	  ARGUMENTS ++;
	  if (!ERROR) SIZE ++;
	}

	if (ERROR) 
	  (void) fprintf(OUTPUT_STREAM,
		  "eERROR[20]: Illegal hexidecimal.\n");

	if (*ARGUMENTS == '\'') ARGUMENTS ++;
	else {
	  (void) fprintf(OUTPUT_STREAM,
 "eERROR[21]: Illegal hexidecimal/character constant. Missing close quote.\n");
	  ERROR = TRUE_1;
	}

	if ( IS_HEX_Q && ( (((int) SIZE / 2) * 2) != SIZE)) {
	  (void) fprintf(OUTPUT_STREAM,
    "eERROR[22]: Hexidecimal number is not divisible into whole bytes.\n");
	  ERROR = TRUE_1;
	}

	if (!ERROR) {
	  if (IS_HEX_Q) 
/* -------- Change location counter to reflect size of BYTE block            */
	    CHANGE_LOCATION(LOCATION,(int) SIZE / 2, OUTPUT_STREAM);
	  else CHANGE_LOCATION(LOCATION,SIZE, OUTPUT_STREAM);

	  if (IS_HEX_Q) {
	    char TEMP;
/* -------- Output text record for BYTE block (hexidecimal)                  */
	    TEMP = *(ARGUMENTS-1);
	    (*(ARGUMENTS-1)) = '\0'; 
	    (void) fprintf(OUTPUT_STREAM,"t%dB%s\n",START_LOC,(START+2));
	    (*(ARGUMENTS-1)) = TEMP;
	  } else {
/* -------- Output text record for BYTE block (character)                    */
	    char *LOOP;

	    (void) fprintf(OUTPUT_STREAM,"t%dB",START_LOC);
	    for (LOOP = START + 2; LOOP <= ARGUMENTS - 2; LOOP ++)
	      PRT_NUM((int) *LOOP,16,2,OUTPUT_STREAM);
	    (void) fprintf(OUTPUT_STREAM,"\n");
	  }
	}
	if (!IS_BLANK_OR_TAB(*ARGUMENTS) && !eoln(*ARGUMENTS))
	  (void) fprintf(OUTPUT_STREAM,
	    "eERROR[13]: Expected a <space> after the operand, found %c.\n",
	    *ARGUMENTS);
      }
    } else
      (void) fprintf(OUTPUT_STREAM,
	  "eERROR[19]: Expected hexidecimal or character constant.\n");
}

/* --------------------------- PSEUDO_CSECT -------------------------------- */
/* Process the CSECT pseudo operator.                                        */
void PSEUDO_CSECT(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                  SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;

  if (!SEEN_END_OP) {
/* --------- If haven't seen an end, then pretend you did.                   */
    (void) fprintf(OUTPUT_STREAM,"eERROR[27]: Section %s has no 'end'.\n",
	    MODULE_NAME);
    OUTPUT_BUFFER(&MOD_REC_BUF,OUTPUT_STREAM,1);
    LOCATION_EXCEEDS_MEM_SIZE = 0;
    (void) fprintf(OUTPUT_STREAM,"E\n");
    TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,MODULE_NAME,SYM_TAB);
    (*TABLE_ENTRY).LENGTH = (*LOCATION) - (*TABLE_ENTRY).LOCATION;
  }

  SEEN_END_OP = NO_END_SEEN_1;

  if (!strcmp(LABEL_NAME,"")) {
    (void) fprintf(OUTPUT_STREAM,"eERROR[26]: CSECT requires a label.\n");
    GET_NEXT_MISSING_LABEL(LABEL_NAME,SYM_TAB);
  } 

  if (LOOK_UP_SYMBOL(LABEL_NAME,LABEL_NAME,SYM_TAB) != NULL) {
   (void) fprintf(OUTPUT_STREAM,"eERROR[55]: Multiply defined module name.\n");
   (void) strcpy(MODULE_NAME,LABEL_NAME);
  } else {
/* --------------- Update symbol to reflect fact that LABEL_NAME is a module */
/*                 name.                                                     */
    TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,LABEL_NAME,SYM_TAB);
    (*TABLE_ENTRY).LOCATION = 0;
    (void) strcpy(  (*TABLE_ENTRY).MODULE, LABEL_NAME);
    (void) strcpy(MODULE_NAME,LABEL_NAME);

/* ---------------- Reset location counter to zero                           */
    (*LOCATION) = 0;
    (void) fprintf(OUTPUT_STREAM,"s%s\n",MODULE_NAME);
  }
}

/* --------------------------- PSEUDO_END ---------------------------------- */
/* Process the END pseudo operator.                                          */
void PSEUDO_END(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;

  SEEN_END_OP = END_SEEN_1; 

  if (strcmp(ARGUMENTS,""))
    {
      char OPERAND_LAB[LABEL_SIZE_1+1];
      char *START;
      START = ARGUMENTS;
  
      GET_LABEL(OPERAND_LAB,ARGUMENTS,&START,pass1);
      OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);

      if (strcmp(OPERAND_LAB,"")) {
/* ----------- End specifies a start adress.                                 */
	struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;

	TABLE_ENTRY = LOOK_UP_SYMBOL(MAIN_ROUTINE,OPERAND_LAB,SYM_TAB);

	if (TABLE_ENTRY == NULL)
	  (void) fprintf(OUTPUT_STREAM,"eERROR[28]: Label %s not defined.\n",
		  OPERAND_LAB);
	else if ((*TABLE_ENTRY).TYPE == ABSOLUTE)
	  (void) fprintf(OUTPUT_STREAM,
		  "eERROR[28]: Expected label, found constant %s.\n",
		  OPERAND_LAB);
	else {
/* ---------------- Found an valid end statement. Clear modify buffer.       */
	  OUTPUT_BUFFER(&MOD_REC_BUF,OUTPUT_STREAM,1);
	  LOCATION_EXCEEDS_MEM_SIZE = 0;
	  (void) fprintf(OUTPUT_STREAM,"E\n");
	  if (START_ADDRESS == -1)
	    START_ADDRESS = (*TABLE_ENTRY).LOCATION;
	  else
	    (void) fprintf(OUTPUT_STREAM,
		   "eERROR[54]: Multiple starting addresses. Using first.\n");
	}

	if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) &&
	    (START != ARGUMENTS))
	  (void) fprintf(OUTPUT_STREAM,
	  "eERROR[13]: Expected a <space> after the operand, found %c.\n",
		  *START);
      } else (void) fprintf(OUTPUT_STREAM,
	  "eERROR[31]: Expected a Symbol, found %s.\n",
		  ARGUMENTS);

    } else {
/* ------------------ End doesn't specify a start address                    */
/*                    Clear modify buffer                                    */
      OUTPUT_BUFFER(&MOD_REC_BUF,OUTPUT_STREAM,1);
      LOCATION_EXCEEDS_MEM_SIZE = 0;
      (void) fprintf(OUTPUT_STREAM,"E\n");
    }
  TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,MODULE_NAME,SYM_TAB);
  (*TABLE_ENTRY).LENGTH = (*LOCATION) - (*TABLE_ENTRY).LOCATION;
  (void) strcpy(MODULE_NAME,"_");
  (*LOCATION) = 0;
}

/* --------------------------- PSEUDO_EQU ---------------------------------- */
/* Process the EQU pseudo operator.                                          */
void PSEUDO_EQU(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  if (!strcmp(LABEL_NAME,""))
    (void) fprintf(OUTPUT_STREAM,"eERROR[23]: EQU requires a label.\n");
  else {
    char *START;
    struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;
    int ABS_VAL = ABSOLUTE_VALUE_1;

    START = ARGUMENTS;

    TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,LABEL_NAME,SYM_TAB);

    (*TABLE_ENTRY).LOCATION = 
      GET_EXPRESSION(&START,MEM_ADDR_SIZE_1+1,*LOCATION,SYM_TAB,&ABS_VAL);
    OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);
    (*TABLE_ENTRY).TYPE = ABSOLUTE;
 
    if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) &&
	(START != ARGUMENTS))
      (void) fprintf(OUTPUT_STREAM,
	      "eERROR[13]: Expected a <space> after the operand, found %c.\n",
	      *START);
  }

}

/* --------------------------- PSEUDO_EXTDEF ------------------------------- */
/* Process the EXTDEF pseudo operator.                                       */
void PSEUDO_EXTDEF(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                   SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  char *START;
  char DEF_LAB[LABEL_SIZE_1+1];
  int ERROR = FALSE_1;
  char TEMP[LABEL_SIZE_1+1];

  if (!strcmp(ARGUMENTS,""))
    (void) fprintf(OUTPUT_STREAM,"eERROR[40]: EXTDEF requires arguments.\n");
  else {
    START = ARGUMENTS;
    INITIALIZE_RECORD("d",LABEL_SIZE_1*5+1);

/* ------------------ Get the list of label names put in a record buffer    */
    do {
      ARGUMENTS = START;
      GET_LABEL(DEF_LAB,START,&START,pass1);
      OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);

      if (!strcmp(DEF_LAB,"")) ERROR = TRUE_1;
      else {
	BLANK_STR(TEMP);
	(void) strncpy(TEMP,DEF_LAB,strlen(DEF_LAB));
	ADD_TO_RECORD(TEMP,OUTPUT_STREAM);
      }
      START ++;
    } while (!ERROR && ((*(START-1)) == ','));

/* ------------------- Output the define record                              */
    PRT_RECORD(OUTPUT_STREAM);

    if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) && !ERROR &&
	(START != ARGUMENTS))
      (void) fprintf(OUTPUT_STREAM,
	      "eERROR[13]: Expected a <space> after the operand, found %c.\n",
	      *(START-1));

    if (ERROR == 1)
      (void) fprintf(OUTPUT_STREAM,
     "eERROR[39]: Invalid list of symbols. Expected symbol found %s.\n",
		     ARGUMENTS);
  }
}

/* --------------------------- PSEUDO_EXTREF ------------------------------- */
/* Process the EXTREF pseudo operator.                                       */
void PSEUDO_EXTREF(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                   SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  char *START;
  char DEF_LAB[LABEL_SIZE_1+1];
  int ERROR = FALSE_1;
  char TEMP[LABEL_SIZE_1+1];

  if (!strcmp(ARGUMENTS,""))
    (void) fprintf(OUTPUT_STREAM,"eERROR[37]: EXTREF requires arguments.\n");
  else {
    START = ARGUMENTS;
    INITIALIZE_RECORD("R",LABEL_SIZE_1*9+1);

/* ----------------------------- Get the list of labels that are externally */
/*                               defined. Buffer them.                      */
    do {
      ARGUMENTS = START;
      GET_LABEL(DEF_LAB,START,&START,pass1);
      OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);
      if (!strcmp(DEF_LAB,"")) ERROR = 1;
      else if (!INSERT_IN_SYM_TAB(MODULE_NAME,DEF_LAB,0,EXTERN_REF,SYM_TAB))
	ERROR = 2;
      else {
	BLANK_STR(TEMP);
	(void) strncpy(TEMP,DEF_LAB,strlen(DEF_LAB));
	ADD_TO_RECORD(TEMP,OUTPUT_STREAM);
      }
      START ++;
    } while (!ERROR && ((*(START-1)) == ','));

    PRT_RECORD(OUTPUT_STREAM);

    if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) && !ERROR &&
	(START != ARGUMENTS))
      (void) fprintf(OUTPUT_STREAM,
	      "eERROR[13]: Expected a <space> after the operand, found %c.\n",
	      *(START-1));

    if (ERROR == 1)
      (void) fprintf(OUTPUT_STREAM,
     "eERROR[39]: Invalid list of symbols. Expected symbol found %s.\n",
		     ARGUMENTS);
    if (ERROR == 2) {
      *(START-1) = '\0';
      (void) fprintf(OUTPUT_STREAM,"eERROR[38]: %s is already defined.\n",
		     ARGUMENTS);
    }

  }
}

/* --------------------------- PSEUDO_RESB --------------------------------- */
/* Process the RESB pseudo operator.                                         */
void PSEUDO_RESB(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                 SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  char *START;
  int ABS_VAL = ABSOLUTE_VALUE_1;

  START = ARGUMENTS;

  CHANGE_LOCATION(LOCATION,
    GET_EXPRESSION(&START,MEM_ADDR_SIZE_1+1,*LOCATION,SYM_TAB,&ABS_VAL),
		  OUTPUT_STREAM);
  OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);
  if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) &&
      (START != ARGUMENTS))
    (void) fprintf(OUTPUT_STREAM,
	    "eERROR[13]: Expected a <space> after the operand, found %c.\n",
	    *START);
}

/* --------------------------- PSEUDO_RESW --------------------------------- */
/* Process the RESW pseudo operator.                                         */
void PSEUDO_RESW(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                 SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  char *START;
  int ABS_VAL = ABSOLUTE_VALUE_1;

  START = ARGUMENTS;

  CHANGE_LOCATION(LOCATION,
	  GET_EXPRESSION(&START,MEM_ADDR_SIZE_1,*LOCATION,SYM_TAB,&ABS_VAL)*3,
		  OUTPUT_STREAM);
  OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);

  if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) &&
      (START != ARGUMENTS))
    (void) fprintf(OUTPUT_STREAM,
	    "eERROR[13]: Expected a <space> after the operand, found %c.\n",
	    *START);
}

/* --------------------------- PSEUDO_START =------------------------------- */
/* Process the START pseudo operator.                                        */
void PSEUDO_START(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                  SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;
  int VALUE;
  char *START;

  if (!SEEN_END_OP) {
    (void) fprintf(OUTPUT_STREAM,"eERROR[27]: Section %s has no 'end'.\n",
	    MODULE_NAME);
    OUTPUT_BUFFER(&MOD_REC_BUF,OUTPUT_STREAM,1);
    LOCATION_EXCEEDS_MEM_SIZE = 0;
    (void) fprintf(OUTPUT_STREAM,"E\n");
    TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,MODULE_NAME,SYM_TAB);
    (*TABLE_ENTRY).LENGTH = (*LOCATION) - (*TABLE_ENTRY).LOCATION;
  }
  if (SEEN_START_OP == SEEN_START_1) 
    (void) fprintf(OUTPUT_STREAM,
		   "eERROR[32]: Multiple STARTs in this file.\n");

  SEEN_START_OP = SEEN_START_1;
  SEEN_END_OP = NO_END_SEEN_1;

  if (!strcmp(LABEL_NAME,"")) {
    (void) fprintf(OUTPUT_STREAM,"eERROR[33]: START requires a label.\n");
    GET_NEXT_MISSING_LABEL(LABEL_NAME,SYM_TAB);
  } 

  if (LOOK_UP_SYMBOL(LABEL_NAME,LABEL_NAME,SYM_TAB) != NULL) {
   (void) fprintf(OUTPUT_STREAM,"eERROR[55]: Multiply defined module name.\n");
   (void) strcpy(MODULE_NAME,LABEL_NAME);
  } else {
/* --------------- Modify LABEL_NAMEs symbol table entry to reflect that it  */
/*                 is a module name.                                         */
    TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,LABEL_NAME,SYM_TAB);
    START = ARGUMENTS;
    VALUE = GET_NUM(&START,MEM_ADDR_SIZE_1+1,10);
    OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);

    if (VALUE < 0)
      (void) fprintf(OUTPUT_STREAM,"eERROR[34]: Negative starting address.\n");
    else (*TABLE_ENTRY).LOCATION = VALUE;

    if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) &&
	(START != ARGUMENTS))
      (void) fprintf(OUTPUT_STREAM,
	     "eERROR[13]: Expected a <space> after the operand, found %c.\n",
		     *START);

/* ---------------- Indicate that is a module and the MAIN module.           */
    (void) strcpy(  (*TABLE_ENTRY).MODULE, LABEL_NAME);
    (void) strcpy(MODULE_NAME,LABEL_NAME);
    (void) strcpy(MAIN_ROUTINE,LABEL_NAME);
    
    (*LOCATION) = (*TABLE_ENTRY).LOCATION;
    (void) fprintf(OUTPUT_STREAM,"s%s\n",MODULE_NAME);
  }
}

/* --------------------------- PSEUDO_WORD --------------------------------- */
/* Process the WORD pseudo operator.                                         */
void PSEUDO_WORD(char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
                 SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
  int VALUE;
  char *START;
  int ABS_VAL = ABSOLUTE_VALUE_1;

  START = ARGUMENTS;

  VALUE = GET_EXPRESSION(&START,BITS_PER_WORD_1,*LOCATION,
			 SYM_TAB,&ABS_VAL);
  OUTPUT_BUFFER(&ERROR_REC_BUF,OUTPUT_STREAM,1);

  (void) fprintf(OUTPUT_STREAM,"t%dW",*LOCATION);
  PRT_NUM(VALUE,16,6,OUTPUT_STREAM);
  (void) fprintf(OUTPUT_STREAM,"\n");

  CHANGE_LOCATION(LOCATION,3, OUTPUT_STREAM);

  if (!IS_BLANK_OR_TAB(*START) && !eoln(*START) &&
      (START != ARGUMENTS))
    (void) fprintf(OUTPUT_STREAM,
	    "eERROR[13]: Expected a <space> after the operand, found %c.\n",
	    *START);
}

/* ---------------------------- DO_PSEUDO ---------------------------------- */
/* execute a pseudo function                                                 */
void DO_PSEUDO(int WHICH_PSEUDO,char *LABEL_NAME,char *ARGUMENTS,int *LOCATION,
	       SYMBOL_TABLE *SYM_TAB,FILE *OUTPUT_STREAM)
{
    switch (WHICH_PSEUDO) {
    case PSEUDO_BYTE0:
	PSEUDO_BYTE(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_CSECT0:
	PSEUDO_CSECT(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_END0:
	PSEUDO_END(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_EQU0:
	PSEUDO_EQU(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_EXTDEF0:
	PSEUDO_EXTDEF(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_EXTREF0:
	PSEUDO_EXTREF(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_RESB0:
	PSEUDO_RESB(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_RESW0:
	PSEUDO_RESW(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_START0:
	PSEUDO_START(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    case PSEUDO_WORD0:
	PSEUDO_WORD(LABEL_NAME,ARGUMENTS,LOCATION,SYM_TAB,OUTPUT_STREAM);
	break;
    default: break;
    }
}
