/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== code.c =================================== */
/* Does the 'real' work of PASS2. Produces one listing file line for a       */
/* assembly instruction and all object file records, except those from pass1 */
/* EVERY line of listing file is produced in this module (except pass1       */
/* error messages).                                                          */
/* Before this is called everything the assembler needs to know is figured   */
/* and this does (most of) the code generation.                              */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pass1.h"
#include "stringI.h"
#include "scan_line.h"
#include "convert.h"
#include "sym_tab.h"
#include "record.h"
#include "assem.h"
#include "constants.h"
#include "buffer.h"
#include "op_tab.h"

/* ------------------------------- PSEUDO_CODE (local) --------------------- */
/* Generates listing line (except location counter which is handled          */
/* elsewhere) for source lines dealing with PSEUDO OPs. Produces object      */
/* records for pseudo ops that could not be figured at pass1 (line Header    */
/* records; do not know length until pass2).                                 */

void PSEUDO_CODE(char *OPERATOR,FILE *INPUT_STREAM,FILE *OBJECT_STREAM,
                 FILE *LISTING_STREAM)
{
  char CH;                       /* a character for looking ahead in the     */
                                 /* input stream.                            */

/* -------------------------------- Output any generated machine code        */
  if ( (strcmp(OPERATOR,"WORD")) && (strcmp(OPERATOR,"BYTE")))
/* -------------------------------- Only BYTE and WORD generate code         */
/*                                  (leave code part of listing blank)       */
    /* COLUMNS 8 - 17              8901234567                                */
    (void) fprintf(LISTING_STREAM,"          ");
  else {
/* ---------------- Must look into pass1 output to see if any code generated */
    CH = fgetc(INPUT_STREAM);
    if (CH == 't') {
/* -------------- Code in text record. Put into text record and listing file */
/* -------------- put code into object file text records, and print on       */
/* -------------- listing line.                                              */
      int LOCATION;
      char TEMP_CH;
      char *REST_OF_LINE;

      (void) fscanf(INPUT_STREAM,"%d%c",&LOCATION,&TEMP_CH);
      GET_LINE(&REST_OF_LINE,INPUT_STREAM);

      (void) fprintf(LISTING_STREAM,"%s",REST_OF_LINE);
      if (strlen(REST_OF_LINE) <= 8) {
	int I;
	for (I=(strlen(REST_OF_LINE)+1);I<=10;I++) 
	  (void) fprintf(LISTING_STREAM," ");
      } else 
	(void) fprintf(LISTING_STREAM,
		       /* COLUMNS 1 - 8         123456789012345678 */
		       " {source on next line}\n                  ");

      if ( TEMP_CH == 'W')
	ADD_TO_TEXT_RECORD(REST_OF_LINE,LOCATION,OBJECT_STREAM);
      else {
	char TEMP[HEX_CHAR_PER_BYTE_1+1];
	int DELTA = 0;
	
	for (TEMP[HEX_CHAR_PER_BYTE_1]='\0';!eoln(*REST_OF_LINE);
	     REST_OF_LINE+=HEX_CHAR_PER_BYTE_1) {
	  (void) strncpy(TEMP,REST_OF_LINE,HEX_CHAR_PER_BYTE_1);
	  ADD_TO_TEXT_RECORD(TEMP,(LOCATION+DELTA),OBJECT_STREAM);
	  DELTA ++;
	}
      }
    } /* END if (CH == 't')*/
    else {
/* --------------- No code was generated for the BYTE/WORD because of errors */
      (void) ungetc(CH,INPUT_STREAM);
      /* COLUMNS 8 - 17              8901234567 */
      (void) fprintf(LISTING_STREAM,"          ");
    }
  }
}

/* ------------------------ GET_REG (local) -------------------------------- */
/* Converts the next thing in a string (pointed to by REG) into the number   */
/* that corresponds to the register with that name. Returns -1 if next thing */
/* is not a register. when done, *REG will point to the character immediately*/
/* following the register name.                                              */

int GET_REG(char **REG)
{
  int VAL;
  switch (**REG) {
  case 'A':
    VAL = 0;
    break;
  case 'X':
    VAL = 1;
    break;
  case 'L':
    VAL = 2;
    break;
  case 'B':
    VAL = 3;
    break;
  case 'S':
    if (*((*REG)+1) == 'W') {
      (*REG) ++;
    VAL = 9;
    } else VAL = 4;
    break;
  case 'T':
    VAL = 5;
    break;
  case 'P':
    if (*((*REG)+1) == 'C') {
      (*REG) ++;
    VAL = 8;
    } else VAL = -1;
    break;
  default:
    VAL = -1;
    break;
  }
  (*REG)++;
  return VAL;
}



/* ------------------------- SET_BITS_TO (local) --------------------------- */
/* CODE is a 32 character string. Each element in that string is either a '0'*/
/* or a '1' and represents the machine code for an instruction. SET_BITS_TO  */
/* sets the bits between LOW_BIT and HIGH_BIT (inclusive) so that they are   */
/* the binary equivalen to VALUE (i.e. make is so that:                      */
/*     SUM i=0 to (HIGH_BIT-LOW_BIT) {CODE[HIGH_BIT-i]*2^i} = VALUE.         */

void SET_BITS_TO(int LOW_BIT,int HIGH_BIT,int VALUE,char *CODE)
{
  int I;                            /* Loop counter variable                 */
  int BIT;                          /* BIT of code to set next               */
  int VALUE_BIT;                    /* one BIT from VALUE to be moved to CODE*/

  if ((HIGH_BIT < LOW_BIT) || (HIGH_BIT >= LARGEST_INSTRUCT_SIZE_1))
    (void) printf("SET_BITS_TO called improperly.\n");
  else {
    BIT = HIGH_BIT;
    for (I= ((int) pow(2.0,1.0*(HIGH_BIT-LOW_BIT))); I > 0; I /= 2) {
      VALUE_BIT = VALUE - (VALUE/2)*2;
      VALUE /= 2;
      CODE[BIT] = ((char) VALUE_BIT) + '0';
      BIT --;
    }
    if (VALUE != 0)
      (void) printf("SET_BITS_TO called improperly, VALUE too large.\n");
  }
}

/* ----------------------- PRT_CODE (local) -------------------------------- */
/* Output to OBJECT_STREAM, the number represented by the first BYTES bytes  */
/* of the [binary] string CODE. Also add the number to the TEXT Record of the*/
/* object file (stream name is OBJECT_STREAM) at LOCATION. Output is         */
/* hexidecimal.                                                              */
void PRT_CODE(char *CODE,int BYTES,int LOCATION,FILE *LISTING_STREAM,
              FILE *OBJECT_STREAM)
{
  int BYTE_COUNTER;                /* byte to output next                    */
  int BIT_COUNTER;                 /* bit (of byte) to process next          */
  int CH;
  char NUM_STR[LARGEST_INSTRUCT_SIZE_1/4];

  if (BYTES*4 > LARGEST_INSTRUCT_SIZE_1)
    (void) printf("PRT_CODE called improperly.\n");
  else {
/* ------- Make a hex string representing the number.                       */
    for (BYTE_COUNTER=0;BYTE_COUNTER < BYTES; BYTE_COUNTER ++) {
      CH = 0;
      for (BIT_COUNTER=0; BIT_COUNTER < 4; BIT_COUNTER ++)
	CH = CH * 2 + ((int) 
		       CHAR_TO_DIGIT(CODE[BYTE_COUNTER*4+BIT_COUNTER],2));
      if ( (CH >= 0) && (CH <= 9))
	NUM_STR[BYTE_COUNTER] = (char) CH + '0';
      else NUM_STR[BYTE_COUNTER] = (char) (CH - 10) + 'A';
    }
    NUM_STR[BYTES] = '\0';

/* ----------------------- Put Number into listing stream and object stream. */
    (void) fprintf(LISTING_STREAM,"%s",NUM_STR);
    ADD_TO_TEXT_RECORD(NUM_STR,LOCATION,OBJECT_STREAM);

    for (BYTE_COUNTER = BYTES; BYTE_COUNTER < (LARGEST_INSTRUCT_SIZE_1/4);
	 BYTE_COUNTER ++)
      (void) fprintf(LISTING_STREAM," ");
  }
}
/* ------------------------------- REAL_CODE (local) ----------------------- */
/* Generates listing line (except location counter which is handled          */
/* elsewhere) for source lines dealing with NON-PSEUDO OPs ('real' ops).     */
/* Produces object records for real ops. It also produces the text and       */
/* modification records (of object file) assocated with these source lines.  */

void REAL_CODE(struct OP_ENTRY *OP_CODE,int EXTENDED,char *ARGUMENTS,
               int LOCATION,SYMBOL_TABLE *SYM_TAB,FILE *OBJECT_STREAM,
               FILE *LISTING_STREAM,int *ERROR)
{
  char INSTRUCT_CODE[LARGEST_INSTRUCT_SIZE_1+1];
  int I;                                 /* loop counter                     */
  int REG_NUMB;                          /* REGister NUMBer. Temporay storage*/

  INSTRUCT_CODE[LARGEST_INSTRUCT_SIZE_1] = '\0';
  for (I=0;I<LARGEST_INSTRUCT_SIZE_1;I++) INSTRUCT_CODE[I] = '0';
  
  SET_BITS_TO(0,7,(CHAR_TO_DIGIT((*OP_CODE).OPCODE[0],16)*16 +
	                  CHAR_TO_DIGIT((*OP_CODE).OPCODE[1],16)),
	      INSTRUCT_CODE);

  if ((*OP_CODE).FORMAT == ONE) {
/* ********************* Process FORMAT 1 instructions (easy) ************** */
    PRT_CODE(INSTRUCT_CODE,2,LOCATION,LISTING_STREAM,OBJECT_STREAM);
  } /* END format ONE */

    if ((*OP_CODE).FORMAT == TWO) {
/* ********************* Process FORMAT 2 instructions ********************* */
      if ((*OP_CODE).OPERAND == NUM) {
/* --------------------- One operatand which is a number 0-4                 */
	if ((*ARGUMENTS >= '0') && (*ARGUMENTS <= '4')) 
	  SET_BITS_TO(8,11,(int) ((*ARGUMENTS) - '0'),INSTRUCT_CODE);
	else {
	      ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
				   "ERROR[43]: Illegal argument to SVC.");
	      (*ERROR) = TRUE_1;
	    }
	ARGUMENTS ++;
      } else 
/* --------------------- First (maybe only) argument must be a register.     */
/*                       get it.                                             */
	if ((REG_NUMB = GET_REG(&ARGUMENTS)) < 0) {
	  ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
			       "ERROR[45]: Expected a valid register.");
	  (*ERROR) = TRUE_1;
	  ARGUMENTS ++;
	} else {
	  SET_BITS_TO(8,11,REG_NUMB,INSTRUCT_CODE);
	  if ((*OP_CODE).OPERAND != REG) 
/* -------------------- Two argument command, Get second after skipping comma*/
	    if ((*ARGUMENTS) != ',') {
	      ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
            "ERROR[46]: Expected a comma after the first register.");
	      (*ERROR) = TRUE_1;
	      ARGUMENTS ++;
	    } else {
	      ARGUMENTS ++;
	      if ((*OP_CODE).OPERAND == REG_NUM) {
/* -------------------- Second argument must be a number 1-16                */
		REG_NUMB = CHAR_TO_DIGIT(*ARGUMENTS,10);
		ARGUMENTS ++;

		if (REG_NUMB < 0) {
		  ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
			   "ERROR[49]: Expected a number 1-16 after comma.");
		  (*ERROR) = TRUE_1;
		  ARGUMENTS ++;
		} else {
		  if (CHAR_TO_DIGIT(*ARGUMENTS,10) >= 0) {
		    REG_NUMB = REG_NUMB*10+CHAR_TO_DIGIT(*ARGUMENTS,10);
		    ARGUMENTS ++;
		    REG_NUMB --;
		  } else REG_NUMB --;
		  if ((REG_NUMB < 0) || (REG_NUMB > 15)) {
		    ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
			 "ERROR[49]: Expected a number 1-16 after comma.");
		    (*ERROR) = TRUE_1;
		  }
		  else SET_BITS_TO(12,15,REG_NUMB,INSTRUCT_CODE);
		}
	      } else {
/* ---------------------- Second argument must be a register.                */
		if ((REG_NUMB = GET_REG(&ARGUMENTS)) < 0) {
		  ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
	       "ERROR[47]: Expected a valid register after the comma.");
		  (*ERROR) = TRUE_1;
		} else 
		  SET_BITS_TO(12,15,REG_NUMB,INSTRUCT_CODE);
	      }
	    }
	}

/* ---------------------- make sure nothing illegal after argument[s]        */
      if (!IS_BLANK_OR_TAB(*ARGUMENTS) && !eoln(*ARGUMENTS)) {
	ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
		  "ERROR[44]: Expected a <space> after the operand.");
	(*ERROR) = TRUE_1;
      }
     
      PRT_CODE(INSTRUCT_CODE,4,LOCATION,LISTING_STREAM,OBJECT_STREAM);
    } /* END format TWO */


  if ((*OP_CODE).FORMAT == THREE_FOUR) {
/* ********************** Process FORMAT 3/4 instructions ****************** */
    int VALUE;                                /* value of argument           */
    int WHAT_KIND = ABSOLUTE_OR_RELATIVE_1;   /* type of argument            */
    
/* ------ set extended bit correctly                                         */
    SET_BITS_TO(11,11,EXTENDED,INSTRUCT_CODE);
    if ((*OP_CODE).OPERAND == NONE) {
/* ------- RSUB has no arguments... easy                                     */
	SET_BITS_TO(6,7,3,INSTRUCT_CODE);
	if (EXTENDED)  
	  PRT_CODE(INSTRUCT_CODE,8,LOCATION,LISTING_STREAM,OBJECT_STREAM);
	else 
	  PRT_CODE(INSTRUCT_CODE,6,LOCATION,LISTING_STREAM,OBJECT_STREAM);
      } else {
/* ------ check for immediate or indirect mode, and set the bits accordingly */
      switch (*ARGUMENTS) {
      case '@': 
	SET_BITS_TO(6,6,1,INSTRUCT_CODE);
	ARGUMENTS ++;
	break;
      case '#': 
	SET_BITS_TO(7,7,1,INSTRUCT_CODE);
	ARGUMENTS ++;
	break;
      default:
	SET_BITS_TO(6,7,3,INSTRUCT_CODE);
      }
/* ------- get the value of the argument and the type (kind) of that argument*/
      VALUE = GET_EXPRESSION(&ARGUMENTS,MEM_ADDR_SIZE_1+1,
			     LOCATION+1,SYM_TAB,&WHAT_KIND);

      if ( ERROR_REC_BUF.HEAD_OF_BUFFER != NULL) (*ERROR) = TRUE_1;
      if ( ((*ARGUMENTS) == ',') && (*(ARGUMENTS+1) == 'X')) {
	ARGUMENTS += 2;
	SET_BITS_TO(8,8,1,INSTRUCT_CODE);
      }

      if (EXTENDED)  {
	char MOD_REC[9];
/* ------ 2's complement negative numbers                                    */
	if (VALUE < 0) VALUE = MEM_SIZE_1 + VALUE;
	if (WHAT_KIND == RELATIVE_VALUE_1) {
	  NUM_TO_STR(LOCATION+1,16,6,MOD_REC);
	  (void) strcat(MOD_REC,"05");
	  ADD_TO_END_OF_BUFFER(&MOD_REC_BUF,MOD_REC);
	}
	SET_BITS_TO(12,31,VALUE,INSTRUCT_CODE);
	PRT_CODE(INSTRUCT_CODE,8,LOCATION,LISTING_STREAM,OBJECT_STREAM);
      } else {
/* ------- format 3's                                                        */
	if (WHAT_KIND == EXTERN_VALUE_1) {
	  ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
    "ERROR[53]: EXTREF label can not be used in a format THREE instruction.");
	  (*ERROR) = TRUE_1;
	  VALUE = 0;
	}
	if (WHAT_KIND == RELATIVE_VALUE_1) 
	  VALUE = VALUE - (LOCATION + 3);
	if (    ( (WHAT_KIND == RELATIVE_VALUE_1) && 
		 (VALUE >= (MAX_PC_RELATIVE_1/2 ))) ||
	    ( (WHAT_KIND != RELATIVE_VALUE_1) && 
		 (VALUE >= (MAX_PC_RELATIVE_1)))) {

	  if (WHAT_KIND == RELATIVE_VALUE_1) 
	    ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
		  "ERROR[51]: Location is not within PC relative range.");
	  else
	    ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
  	     "ERROR[52]: Constant too large for a format THREE instruction.");
	  (*ERROR) = TRUE_1;
	  VALUE = 0;
	}


	if (VALUE < 0) {
/* --------- 2's complement negative values                                  */
	  VALUE = MAX_PC_RELATIVE_1 + VALUE;
	  if ( (VALUE >= MAX_PC_RELATIVE_1) || 
	      (VALUE < MAX_PC_RELATIVE_1 /2) ) {
	    if (WHAT_KIND == RELATIVE_VALUE_1) 
	      ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
		    "ERROR[51]: Location is not within PC relative range.");
	    else
	      ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
  	     "ERROR[52]: Constant too large for a format THREE instruction.");
	    (*ERROR) = TRUE_1;
	    VALUE = 0;
	  }
	}

	SET_BITS_TO(12,23,VALUE,INSTRUCT_CODE);
	if (WHAT_KIND == RELATIVE_VALUE_1) 
	  SET_BITS_TO(10,10,1,INSTRUCT_CODE);
	PRT_CODE(INSTRUCT_CODE,6,LOCATION,LISTING_STREAM,OBJECT_STREAM);
      }
      if (!IS_BLANK_OR_TAB(*ARGUMENTS) && !eoln(*ARGUMENTS)) {
	ADD_TO_END_OF_BUFFER(&ERROR_REC_BUF,
		  "ERROR[44]: Expected a <space> after the operand.");
	(*ERROR) = 1;
      }
    }
  } /* END format THREE_FOUR */

  (void) fprintf(LISTING_STREAM,"  ");
}

/* ================================= CODE ================================== */
/* Determines the location counter value for a line, puts it into the        */
/* listing stream and calls the correct procedures to output the code        */
void CODE(FILE *INPUT_STREAM,SYMBOL_TABLE *SYM_TAB,FILE *OBJECT_STREAM,
          FILE *LISTING_STREAM,int *ERROR)
{
  char LABEL[LABEL_SIZE_1+1];           /* label on the source line          */
  char OPERATOR[LABEL_SIZE_1+1];        /* operator on the source line       */
  char *ARGUMENTS;                      /* source line after operator        */
  char *INPUT_LINE;
  int LOCATION_COUNTER;
  int EXTENDED;                         /* is this an extended instruction?  */
  struct OP_ENTRY *OP_CODE;
  struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;

  (void) fscanf(INPUT_STREAM,"%d",&LOCATION_COUNTER);
  (void) getc(INPUT_STREAM); /* remove space after s */

  SCAN_LINE(LOCATION_COUNTER,&INPUT_LINE,LABEL,&EXTENDED,OPERATOR,&ARGUMENTS,
	    pass2,(FILE *) NULL,INPUT_STREAM);
  
  CAPITALIZE_STRING(LABEL);
  CAPITALIZE_STRING(OPERATOR);

  OP_CODE = LOOK_UP_OP(OPERATOR);
  if (!strcmp(LABEL,"") && !strcmp(OPERATOR,"") && (EXTENDED == 0))
/* ----- blank source line                                                   */
                                /* 123456789012345678 */
    (void) fprintf(LISTING_STREAM,"                  %s\n",INPUT_LINE);
  else {
    if (!strcmp(OPERATOR,"")) {
/* --------- a no-op                                                         */
      PRT_NUM(LOCATION_COUNTER,16,6,LISTING_STREAM);
      (void) fprintf(LISTING_STREAM,"  ");
    } 

/* OUTPUT blanks for address, if PSEUDO OP that doesn't display a location   */
    else if ((*OP_CODE).OPERAND == PSEUDO_NOLOC)
                                  /* 1234567 */
      (void) fprintf(LISTING_STREAM,"       ");


    else if ((*OP_CODE).OPERAND == PSEUDO_ADDR) {
/* -------- OUTPUT value as address if START,CSECT, or EQU                   */
      if (!strcmp(OPERATOR,"EQU")) {
	TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,LABEL,SYM_TAB);
	if (TABLE_ENTRY == NULL) 
	  LOCATION_COUNTER = 0;
	else LOCATION_COUNTER = (*TABLE_ENTRY).LOCATION;
      } else {
	if (!strcmp(LABEL,"")) 
	  GET_NEXT_MISSING_LABEL(LABEL,SYM_TAB);
	TABLE_ENTRY = LOOK_UP_SYMBOL(LABEL,LABEL,SYM_TAB);
	if (TABLE_ENTRY == NULL) {
	  (void) printf(
	"CODE: Something is wrong with code. This shouldn't be executed.\n");
/* ------ In all other case output the location counter and the address      */
	} else LOCATION_COUNTER = (*TABLE_ENTRY).LOCATION;
      }

      PRT_NUM(LOCATION_COUNTER,16,6,LISTING_STREAM);
      (void) fprintf(LISTING_STREAM,"  ");
    }
    else {
      PRT_NUM(LOCATION_COUNTER,16,6,LISTING_STREAM);
      (void) fprintf(LISTING_STREAM,"  ");
    }

    {
      char *SAVE_LINE;

/* SAVE the source line so that other routines can mess it up without        */
/* worrying. This was necessary because PSEUDO_CODE might have to look a     */
/* whole line ahead in the input                                             */
      SAVE_LINE = malloc((unsigned int) (strlen(INPUT_LINE) + 1));
      (void) strcpy(SAVE_LINE,INPUT_LINE);
      CAPITALIZE_STRING(ARGUMENTS);

/* call correct procedure to produce the code.                               */
      if ((*OP_CODE).FORMAT == NOT_FOUND) 
	/* COLUMNS 8 - 17                8901234567 */
	(void) fprintf(LISTING_STREAM,"          ");
      else if ((*OP_CODE).FORMAT == PSEUDO) 
	PSEUDO_CODE(OPERATOR,INPUT_STREAM,OBJECT_STREAM,LISTING_STREAM);
      else
	REAL_CODE(OP_CODE,EXTENDED,ARGUMENTS,LOCATION_COUNTER,
		  SYM_TAB,OBJECT_STREAM,LISTING_STREAM,ERROR);

      (void) fprintf(LISTING_STREAM,"%s\n",SAVE_LINE);
      free(SAVE_LINE);
    }
  }
}
