/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== pass1.c ================================= */
/* Process Header, define, and end records of oen object file.               */
#include <string.h>
#include <stdio.h>
#include "boolean.h"
#include "constants.h"
#include "sym_tab.h"
#include "stringI.h"
#include "memory.h"
#include "convert.h"
#include "load.h"

/* ------------------------------- PASS1 ----------------------------------- */
/* Process Header, define, and end records of oen object file.               */
void PASS1(SYMBOL_TABLE *SYM_TAB,int *LOCATION,int *ERROR,FILE *INPUT)
{
  char MODULE_NAME[LABEL_SIZE_1+1];  /* Name of (program) module currently   */
                                     /* assembling.                          */

  char *RECORD;                   /* One record from the input stream        */
  int UNREL_TO_REL_SHIFT = 0;     /* signed number of bytes to shift the     */
                                  /*    object code due to relocation        */


  MODULE_NAME[LABEL_SIZE_1] = '\0';

  while (!feof(INPUT) && ((*LOCATION) <= MEM_SIZE_1)) {
    BOOLEAN LOCAL_ERROR;

    LOCAL_ERROR = FALSE_1;

    GET_LINE(&RECORD,INPUT);
    switch (RECORD[0]) {

    case 'H':
/* ---------------------- HEADER RECORD ------------------------------------ */
      if (strlen(RECORD) != (13 + LABEL_SIZE_1)) 
	LOCAL_ERROR = TRUE_1;
       else {
	int TEMP_LOC;

	(void) strncpy(MODULE_NAME,&(RECORD[1]),LABEL_SIZE_1);
	if (!INSERT_IN_SYM_TAB(GLOBAL_1,MODULE_NAME,*LOCATION,MODULE,
			       SYM_TAB)) {
	  int I;

	  for (I=LABEL_SIZE_1-1; ( (I<1) || (MODULE_NAME[I] == ' ')); I--);
	  MODULE_NAME[I+1] = '\0';
	  (void) printf(
		"ERROR: Multiply defined global %s. Program not loaded.\n",
			MODULE_NAME);
	  (*ERROR) = TRUE_1;
	} else {

	  STR_TO_NUM(&(RECORD[9]),6,16,&TEMP_LOC,&LOCAL_ERROR);
	  UNREL_TO_REL_SHIFT = (*LOCATION) - TEMP_LOC;
	  STR_TO_NUM(&(RECORD[15]),6,16,&TEMP_LOC,&LOCAL_ERROR);
	  (*LOCATION) += TEMP_LOC;
	  (*LOOK_UP_SYMBOL(GLOBAL_1,MODULE_NAME,SYM_TAB)).LENGTH =
	    TEMP_LOC;
	}
      }

      if (LOCAL_ERROR)
	(void) printf("ERROR: Illegal header record = %s\n",RECORD);

      break;
	    
    case 'D':
/* ------------------------ DEFINE RECORD ---------------------------------- */
      if (strlen(RECORD) > 71) LOCAL_ERROR = TRUE_1;
	else {
	  int NEXT = 1;
	  char TEMP_NAME[LABEL_SIZE_1+1];
	  int TEMP_LOC;

	  TEMP_NAME[LABEL_SIZE_1] = '\0';
	  while (NEXT + 14 <= strlen(RECORD)) {
	    (void) strncpy(TEMP_NAME,&(RECORD[NEXT]),LABEL_SIZE_1);
	    STR_TO_NUM(&(RECORD[NEXT+LABEL_SIZE_1]),6,16,&TEMP_LOC,
		       &LOCAL_ERROR);
	    if (!INSERT_IN_SYM_TAB(GLOBAL_1,TEMP_NAME
				   ,TEMP_LOC + UNREL_TO_REL_SHIFT,GLOBAL,
				   SYM_TAB)) {
	      int I;

	      for (I=LABEL_SIZE_1-1; ( (I<1) || (TEMP_NAME[I] == ' ')); I--);
	      TEMP_NAME[I+1] = '\0';
	      (void) printf(
		   "ERROR: Multiply defined global %s. Program not loaded.\n",
			    TEMP_NAME);
	      (*ERROR) = TRUE_1;
	    }
	    NEXT += 14;
	  }
	  if (NEXT != strlen(RECORD)) LOCAL_ERROR = TRUE_1;
	}

      if (LOCAL_ERROR)
	(void) printf("ERROR: Illegal define record = %s\n",RECORD);

      break;

/* ------------------------- END RECORD ------------------------------------ */
    case 'E':
      if (strlen(RECORD) != 1)
	if (strlen(RECORD) != 7)
	  LOCAL_ERROR = TRUE_1;
	else {
	  if (START_ADDRESS != -1) 
	    (void) printf("%s%s\n",
			  "WARNING: Found multiple start addresses.",
			  " Expected only one. Using the first.");
	  else {
	    STR_TO_NUM(&(RECORD[1]),6,16,&START_ADDRESS, &LOCAL_ERROR);
	    START_ADDRESS += UNREL_TO_REL_SHIFT;
	    (void) strcpy(MAIN_ROUTINE,MODULE_NAME);
	  }
	}

      if (LOCAL_ERROR)
	(void) printf("ERROR: Illegal end record = %s\n",RECORD);

      break;
    case 'M':
    case 'R':
    case 'T':
    case '\0':
      break;

    default:
      (void) printf("ERROR: Illegal RECORD = %s\n",RECORD);
      LOCAL_ERROR = TRUE_1;
    }
    (*ERROR) = (*ERROR) || LOCAL_ERROR;
  }
}
