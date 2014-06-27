/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== print.c ================================== */
/* Contains routines for output the symbol table and executable files.       */
#include <string.h>
#include <stdio.h>
#include "boolean.h"
#include "constants.h"
#include "sym_tab.h"
#include "convert.h"
#include "memory.h"
#include "load.h"

/* ============================== Symbol Table ============================= */
/* -------------------------- PRINT_TABLE (local) -------------------------- */
/* Outputs the Symbol Table. Recursive                                       */
void PRINT_TABLE(SYMBOL_TABLE SYM_TAB,FILE *OUTPUT)
{
  char TEMP[(int) MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1 + 1];

/* ------------------- Print out the linked list backwards                   */
  if (SYM_TAB != NULL) {

    PRINT_TABLE((*SYM_TAB).NEXT,OUTPUT);

    if ((*SYM_TAB).TYPE == MODULE) 
      (void) fprintf(OUTPUT,   "%s            ",(*SYM_TAB).LABEL);
    else (void) fprintf(OUTPUT,"          %s  ",(*SYM_TAB).LABEL);

    NUM_TO_STR((*SYM_TAB).LOCATION, 16, 
	       (int) MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1,TEMP);

    if ((*SYM_TAB).TYPE == UNDEFINED)
      (void) fprintf(OUTPUT,"Undefined ");
    else (void) fprintf(OUTPUT,"%s    ",TEMP);

    if ((*SYM_TAB).TYPE == MODULE) {
      NUM_TO_STR((*SYM_TAB).LENGTH, 16, 
		 (int)MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1+1,TEMP);
      (void) fprintf(OUTPUT,"%s\n",TEMP);
    } else (void) fprintf(OUTPUT,"\n");
  }
}

/* --------------------------------- PRT_SYM_TAB --------------------------- */
/* Prints header for the symbol table file and lets PRINT_TABLE print it.    */
void PRT_SYM_TAB(SYMBOL_TABLE SYM_TAB,FILE *OUTPUT)
{
  (void) fprintf(OUTPUT,"CONTROL   SYMBOL\n");
  (void) fprintf(OUTPUT,"SECTION   NAME     ADDRESS   LENGTH\n");  
  (void) fprintf(OUTPUT,"-----------------------------------\n");
  PRINT_TABLE(SYM_TAB,OUTPUT);
}
/* ============================= Executable ================================ */
/* ----------------------------- OUTPUT_TABLE ------------------------------ */
/* Prints out the symbol table into the executable file. Again is recursive  */
void OUTPUT_TABLE(SYMBOL_TABLE SYM_TAB,FILE *OUTPUT)
{
  char TEMP[(int) MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1 + 2];

/* ---------------- Prints linked list out backwards                         */
  if (SYM_TAB != NULL) {
    OUTPUT_TABLE((*SYM_TAB).NEXT,OUTPUT);

    if ((*SYM_TAB).TYPE == MODULE) 
      (void) fprintf(OUTPUT,"M%s",(*SYM_TAB).LABEL);
    else if (!strcmp((*SYM_TAB).MODULE,GLOBAL_1)) 
      (void) fprintf(OUTPUT,"G%s",(*SYM_TAB).LABEL);
    else (void) fprintf(OUTPUT,"L%s%s",(*SYM_TAB).MODULE,(*SYM_TAB).LABEL);

    NUM_TO_STR((*SYM_TAB).LOCATION, 16, 
	       (int) MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1+1,TEMP);
    (void) fprintf(OUTPUT,"%s",TEMP);

    if ((*SYM_TAB).TYPE == MODULE) {
      NUM_TO_STR((*SYM_TAB).LENGTH, 16,
		 (int) MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1+1, TEMP);
      (void) fprintf(OUTPUT,"%s",TEMP);
    }
    (void) fprintf(OUTPUT,"\n");
  }
}

/* ------------------------------ PRINT_EXEC ------------------------------- */
/* Drives the outputing of the executable file                               */
void PRINT_EXEC(MEM_SPACE MEMORY,SYMBOL_TABLE SYM_TAB,FILE *OUTPUT)
{
  char ADDRESS[MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1 + 1];
  
  NUM_TO_STR(START_ADDRESS,16,MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1,ADDRESS);
  (void) fprintf(OUTPUT,"S%s%s\n",MAIN_ROUTINE,ADDRESS);

  OUTPUT_TABLE(SYM_TAB,OUTPUT);
  OUTPUT_MEM(MEMORY,OUTPUT);
}
