/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* ================================ loadexe.c ============================== */
/* Sets up the machine from the executable file ('loads' program)            */

#include <stdio.h>
#include <string.h>
#include "boolean.h"
#include "stringI.h"
#include "memory.h"
#include "constants.h"
#include "convert.h"
#include "machine.h"
#include "debugger.h"

/* ------------------------------ LOAD ------------------------------------- */
/* Load the program from the executable file.                                */
void LOAD(BOOLEAN DEBUG_MODE,int *ERROR,FILE *INPUT)
{
  char *RECORD;                   /* One record from the input stream        */
  int LENGTH;                     /* Used to store length of blocks of       */
                                  /* memory affected by text and modification*/
                                  /* records                                 */
  int LOCATION = 0;               /* Location to load next csect             */
  char MODULE_NAME[LABEL_SIZE_1+1]; /* Name of a module of program.          */

  MODULE_NAME[LABEL_SIZE_1] = '\0';

  if (feof(INPUT)) (*ERROR) = TRUE_1;
  else {
    GET_LINE(&RECORD,INPUT);
    if ((RECORD[0] != 'S') || (strlen(RECORD) != 14)) (*ERROR) = TRUE_1;
    else {
/* ---------------------------- START RECORD ------------------------------- */
      if (DEBUG_MODE) {
	MAIN_ROUTINE[LABEL_SIZE_1] = '\0';
	(void) strncpy(MAIN_ROUTINE,&(RECORD[1]),LABEL_SIZE_1);
      }
      STR_TO_NUM(&(RECORD[LABEL_SIZE_1+1]), 5, 16,
		 &START_ADDRESS,ERROR);
    }
  }

  while (!feof(INPUT)  && (LOCATION <= MEM_SIZE_1) && !(*ERROR)) {

    GET_LINE(&RECORD,INPUT);
    switch (RECORD[0]) {

    case 'T':
/* ---------------------------- TEXT RECORD -------------------------------- */
      if (strlen(RECORD) < 9)
	(*ERROR) = TRUE_1;
      else {

	STR_TO_NUM(&(RECORD[1]),6,16,&LOCATION,ERROR);
	STR_TO_NUM(&(RECORD[7]),2,16,&LENGTH,ERROR);
	if (strlen(RECORD) != 9+LENGTH*2) (*ERROR) = TRUE_1;
	else STORE_AT(&(RECORD[9]), LENGTH, LOCATION, MEMORY,ERROR);
      }
      break;


    case 'M':
/* ---------------------- MODULE RECORD ------------------------------------ */
      if (strlen(RECORD) != (13 + LABEL_SIZE_1)) (*ERROR) = TRUE_1;
      else if (DEBUG_MODE) {
	
	(void) strncpy(MODULE_NAME,&(RECORD[1]),LABEL_SIZE_1);
	STR_TO_NUM(&(RECORD[9]),6,16,&LOCATION,ERROR);
	(*ERROR) = !INSERT_IN_SYM_TAB(GLOBAL_1,MODULE_NAME,LOCATION,MODULE,
				 &SYM_TAB);
	STR_TO_NUM(&(RECORD[15]),6,16,&LENGTH,ERROR);
	(*LOOK_UP_SYMBOL(GLOBAL_1,MODULE_NAME,&SYM_TAB)).LENGTH = LENGTH;
      }

      break;
         
    case 'G':
/* ----------------------- GLOBAL RECORD ----------------------------------- */
      if (strlen(RECORD) != (7 + LABEL_SIZE_1)) (*ERROR) = TRUE_1;
      else if (DEBUG_MODE) {

	(void) strncpy(MODULE_NAME,&(RECORD[1]),LABEL_SIZE_1);
	STR_TO_NUM(&(RECORD[9]),6,16,&LOCATION,ERROR);
	(*ERROR) = !INSERT_IN_SYM_TAB(GLOBAL_1,MODULE_NAME,LOCATION,GLOBAL,
				 &SYM_TAB);
      }

      break;

    case '\0':
    default:
      break;
    }
  }
  if (*ERROR) 
    (void) printf("ERROR: Input is not an executable file. Aborting.\n");
}
