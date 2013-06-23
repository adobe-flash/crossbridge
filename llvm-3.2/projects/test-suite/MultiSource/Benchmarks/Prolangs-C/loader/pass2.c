/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== pass2.h ================================= */
/* Process Text, Modify, and Reference records of one object file. Also      */
/* does minimal recomputation of the number of bytes needs to be shifted to  */
/* accomidate the relocation.                                                */
#include <string.h>
#include <stdio.h>
#include "boolean.h"
#include "constants.h"
#include "sym_tab.h"
#include "stringI.h"
#include "memory.h"
#include "convert.h"
#include "load.h"

/* --------------------------------- pass2 --------------------------------- */
/* Process Text, Modify, and Reference records of one object file. Also      */
/* does minimal recomputation of the number of bytes needs to be shifted to  */
/* accomidate the relocation.                                                */
void PASS2(MEM_SPACE MEMORY,SYMBOL_TABLE *SYM_TAB,int *LOCATION,int *ERROR,
           FILE *INPUT)
{
  char MODULE_NAME[LABEL_SIZE_1+1];  /* Name of (program) module currently   */
                                     /* assembling.                          */

  char *RECORD;                   /* One record from the input stream        */
  int UNREL_LOCATION;             /* Where in the unrelocated memory is it   */
                                  /* located.                                */
  int UNREL_TO_REL_SHIFT = 0;     /* signed number of bytes to shift the     */
                                  /*    object code due to relocation        */
  int LENGTH;                     /* Used to store length of blocks of       */
                                  /* memory affected by text and modification*/
                                  /* records                                 */

  MODULE_NAME[LABEL_SIZE_1] = '\0';

  while (!feof(INPUT)  && ((*LOCATION) <= MEM_SIZE_1)) {
    BOOLEAN LOCAL_ERROR;

    LOCAL_ERROR = FALSE_1;

    GET_LINE(&RECORD,INPUT);
    switch (RECORD[0]) {

    case 'T':
/* ---------------------------- TEXT RECORD -------------------------------- */
      if (strlen(RECORD) < 9)
	LOCAL_ERROR = TRUE_1;
      else {

	STR_TO_NUM(&(RECORD[1]),6,16,&UNREL_LOCATION,&LOCAL_ERROR);
	STR_TO_NUM(&(RECORD[7]),2,16,&LENGTH,&LOCAL_ERROR);
	if (strlen(RECORD) != 9+LENGTH*2) LOCAL_ERROR = TRUE_1;
	else STORE_AT(&(RECORD[9]), LENGTH, UNREL_LOCATION+UNREL_TO_REL_SHIFT,
		      MEMORY,&LOCAL_ERROR);
      }
      if (LOCAL_ERROR)
	(void) printf("ERROR: Illegal text record = %s\n",RECORD);

      break;

    case 'H':
/* ---------------------------- HEADER RECORD ------------------------------ */
/* ----- Just calcution of UNREL_TO_REL_SHIFT                                */
      if (strlen(RECORD) != (13 + LABEL_SIZE_1))
	LOCAL_ERROR = TRUE_1;
      else {
	int TEMP_LOC;
	
	(void) strncpy(MODULE_NAME,&(RECORD[1]),LABEL_SIZE_1);
	STR_TO_NUM(&(RECORD[9]),6,16,&TEMP_LOC,&LOCAL_ERROR);
	UNREL_TO_REL_SHIFT = (*LOCATION) - TEMP_LOC;
	STR_TO_NUM(&(RECORD[15]),6,16,&TEMP_LOC,&LOCAL_ERROR);
	(*LOCATION) += TEMP_LOC;
	if ((*LOCATION) > MEM_SIZE_1)
	  (void) printf("ERROR: Program does not fit in memory. Aborting.\n");
      }

      break;

/* ---------------------------- MODIFICATION RECORD ------------------------ */
    case 'M':
      if (strlen(RECORD) < 9) LOCAL_ERROR = TRUE_1;
      else if (strlen(RECORD) == 9) {	
/* -------------------------- Type 1 Modification record. ------------------ */
	STR_TO_NUM(&(RECORD[1]),6,16,&UNREL_LOCATION,&LOCAL_ERROR);
	STR_TO_NUM(&(RECORD[7]),2,16,&LENGTH,&LOCAL_ERROR);
	ADD_INT_TO_LOC(UNREL_TO_REL_SHIFT, UNREL_LOCATION + UNREL_TO_REL_SHIFT,
 		       LENGTH, MEMORY, &LOCAL_ERROR);
      } else 
/* -------------------------- Type 2 Modification record. ------------------ */
	if (strlen(RECORD) != 18) LOCAL_ERROR = TRUE_1;
	  else {
	    struct SYMBOL_TABLE_ENTRY *SYMBOL;
	    int SIGN;

	    STR_TO_NUM(&(RECORD[1]),6,16,&UNREL_LOCATION,&LOCAL_ERROR);
	    STR_TO_NUM(&(RECORD[7]),2,16,&LENGTH,&LOCAL_ERROR);

	    switch (RECORD[9]) {
	    case '-':
	      SIGN = -1;
	      break;
	    case '+':
	      SIGN = 1;
	      break;
	    default:
	      SIGN = 0;
	      LOCAL_ERROR = TRUE_1;
	      break;
	    }

	    SYMBOL = LOOK_UP_SYMBOL(GLOBAL_1,&(RECORD[10]),SYM_TAB);
	    if (SYMBOL == NULL) LOCAL_ERROR = TRUE_1;
	    else ADD_INT_TO_LOC((*SYMBOL).LOCATION * SIGN,
				UNREL_LOCATION + UNREL_TO_REL_SHIFT,
				LENGTH, MEMORY, &LOCAL_ERROR);
	  }

      if (LOCAL_ERROR)
	(void) printf("ERROR: Illegal text record = %s\n",RECORD);
      break;

    case 'R':
/* ------------------------------- REFERENCE RECORD ------------------------ */
      if (strlen(RECORD) > 73) LOCAL_ERROR = TRUE_1;
	else {
	  int NEXT = 1;
	  char TEMP_NAME[LABEL_SIZE_1+1];

	  TEMP_NAME[LABEL_SIZE_1] = '\0';
	  while (NEXT + LABEL_SIZE_1 <= strlen(RECORD)) {
	    (void) strncpy(TEMP_NAME,&(RECORD[NEXT]),LABEL_SIZE_1);
	    if (LOOK_UP_SYMBOL(GLOBAL_1,TEMP_NAME, SYM_TAB) == NULL) {
	      int I;

	      (void) INSERT_IN_SYM_TAB(GLOBAL_1,TEMP_NAME,0,UNDEFINED,SYM_TAB);
	      for (I=LABEL_SIZE_1-1; ( (I<1) || (TEMP_NAME[I] == ' ')); I--);
	      TEMP_NAME[I+1] = '\0';
	      (void) printf(
			    "ERROR: Undefined global %s. Program not loaded.\n"
			    ,TEMP_NAME);

	      (*ERROR) = TRUE_1;
	    }
	    NEXT += LABEL_SIZE_1;
	  }
	  if (NEXT != strlen(RECORD)) LOCAL_ERROR = TRUE_1;
	}

      if (LOCAL_ERROR)
	(void) printf("ERROR: Illegal define record = %s\n",RECORD);

      break;


    case 'E':
    case 'D':
    case '\0':
    default:
      break;
    }
    (*ERROR) = (*ERROR) || LOCAL_ERROR;
  }
}
