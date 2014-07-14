/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================= load.c ==================================== */
/* Main (driving) routine of the loader.                                     */

/* to compile (the loader) use:                                              */
/*     cc -o load load.o pass1.o pass2.o convert.o sym_tab.o memory.o        */
/*                stringI.o print.o -g -lm                                   */
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "sym_tab.h"
#include "boolean.h"
#include "convert.h"
#include "memory.h"
#include "pass1.h"
#include "pass2.h"
#include "print.h"

/* --------------------------------- Globals ------------------------------ */
/* START_ADDRESS                     Start Address for program execution.   */
int START_ADDRESS = -1;

/* MAIN_ROUTINE                      Name of (program) module declared with */
/*                                      a START (main routine).             */
char MAIN_ROUTINE[LABEL_SIZE_1+1];

/* -------------------------------- main ---------------------------------- */
int main(int argc,char **argv)
{
  FILE *INPUT_STREAM;             /* Input file stream                      */
  SYMBOL_TABLE SYM_TAB;           /* The symbol table                       */
  BOOLEAN ERROR = FALSE_1;        /* FALSE no errors seen. TRUE seen errors */
  int LOCATION = 0;               /* LOCATION in memory to load the next    */
                                  /*    module into.                        */
  int START_LOCATION;             /* LOCATION in memory to load the FIRST   */
                                  /*    module.                             */
  char *EXECUTABLE_FILE;          /* Name of file to put the executable into*/
  FILE *EXECUTABLE_STREAM;        /* Stream for executable file             */
  char *MEM_DUMP_FILE;            /* Name of file to put memory dump into   */
  FILE *MEM_DUMP_STREAM;          /* Stream for memory dump file            */
  char *SYM_TAB_DUMP_FILE;        /* Name of file to put sym. table into    */
  FILE *SYM_TAB_DUMP_STREAM;      /* Stream for symbol table dump file      */
  int LOOP_COUNTER;               /* a loop counter                         */
  MEM_SPACE MEMORY = NULL;        /* The Main Memory.                       */
  int FILES_START;                /* Where in the list of arguments do the  */
                                  /*    files start.                        */

  if (argc == 1) (void) printf("usage:  load [-l location] file ...\n");
  else {
    int ARGUMENT = 1;             /* Which argument of the command line     */
                                  /* currently processing.                  */
    BOOLEAN FLAG;                 /* Just a temporary boolean.              */

/* --------------------------------- Process command directives             */
    if (ARGUMENT < argc) FLAG = (argv[ARGUMENT][0] == '-');
    while ( (ARGUMENT < argc) && FLAG) {
      if (!strcmp(argv[ARGUMENT],"-l")) {
	BOOLEAN NOT_LEGAL_NUM = FALSE_1;

	ARGUMENT ++; 
	if (ARGUMENT < argc) {
	  char *NEXT;

	  NEXT = argv[ARGUMENT];
	  LOCATION = GET_NUM(&NEXT,MEM_ADDR_SIZE_1,10,
			     &NOT_LEGAL_NUM);
	  if (NOT_LEGAL_NUM) 
    (void) printf("load: %s is not a legal starting address. Starting at %d\n",
		  argv[ARGUMENT], LOCATION);
	} else 
   (void) printf("load: -l must be followed by a location. Starting at %d\n",
		 LOCATION);

      } else
	  (void) printf("Illegal command directive, '%s'. Ignoring.\n",
			argv[ARGUMENT]);
      if (ARGUMENT < argc) ARGUMENT ++;
      if (ARGUMENT < argc) FLAG = (argv[ARGUMENT][0] == '-');
    }

    if (ARGUMENT < argc) {
/* --------------------------------- Get name for output files               */
      EXECUTABLE_FILE = malloc((unsigned int) (strlen(argv[ARGUMENT])+5));
      MEM_DUMP_FILE = malloc((unsigned int) (strlen(argv[ARGUMENT])+5));
      SYM_TAB_DUMP_FILE = malloc((unsigned int) (strlen(argv[ARGUMENT])+5));
      (void) strcpy(EXECUTABLE_FILE,argv[ARGUMENT]);
      (void) strcpy(MEM_DUMP_FILE,argv[ARGUMENT]);
      (void) strcpy(SYM_TAB_DUMP_FILE,argv[ARGUMENT]);
      for ((LOOP_COUNTER = strlen(argv[ARGUMENT]));
	   ( (LOOP_COUNTER >= 0) && (EXECUTABLE_FILE[LOOP_COUNTER] != '.'));
	   LOOP_COUNTER --);
      if (LOOP_COUNTER <= 0) LOOP_COUNTER = strlen(argv[ARGUMENT]);
      (void) strcpy(&(EXECUTABLE_FILE[LOOP_COUNTER]),".exe");
      (void) strcpy(&(MEM_DUMP_FILE[LOOP_COUNTER]),".mem");
      (void) strcpy(&(SYM_TAB_DUMP_FILE[LOOP_COUNTER]),".sym");

    } else
      (void) printf("load: requires at least one file name.\n");


    INIT_SYM_TAB(&SYM_TAB);         /* Initialize the symbol table           */
    CREATE_MEMORY(&MEMORY);         /* Create/initialize the main memory.    */
/* ============================== PASS1 ==================================== */
    FILES_START = ARGUMENT;
    START_LOCATION = LOCATION;
    while ( (ARGUMENT < argc) && (LOCATION <= MEM_SIZE_1)) {
      if ( (INPUT_STREAM = fopen(argv[ARGUMENT],"r")) == NULL) {
	(void) printf("%s: No such file or directory\n",argv[ARGUMENT]);
      } else {
/* --------------------------------- Have a valid file: load it              */
	PASS1(&SYM_TAB,&LOCATION,&ERROR,INPUT_STREAM);
	(void) fclose(INPUT_STREAM);
      }
      ARGUMENT ++;
    }

/* ============================== PASS2 ==================================== */
    ARGUMENT = FILES_START;
    LOCATION = START_LOCATION;

    while ( (ARGUMENT < argc) && (LOCATION <= MEM_SIZE_1)) {
      if ( (INPUT_STREAM = fopen(argv[ARGUMENT],"r")) != NULL) 
	{
/* --------------------------------- Have a valid file: load it              */
	  PASS2(MEMORY,&SYM_TAB,&LOCATION,&ERROR,INPUT_STREAM);
	  (void) fclose(INPUT_STREAM);
	}
      ARGUMENT ++;
    }


    if (START_ADDRESS == -1) {
      (void) printf(
          "ERROR: Expected a main routine. None found. Program not loaded.\n");
      ERROR = TRUE_1;
    }

/* ================== Output the memory and symbol table to files ========== */
    SYM_TAB_DUMP_STREAM = fopen(SYM_TAB_DUMP_FILE,"w");
    PRT_SYM_TAB(SYM_TAB,SYM_TAB_DUMP_STREAM);
    (void) fclose(SYM_TAB_DUMP_STREAM);

    if (!ERROR || DEBUG_FLAG_1) {
      EXECUTABLE_STREAM = fopen(EXECUTABLE_FILE,"w");
      MEM_DUMP_STREAM = fopen(MEM_DUMP_FILE,"w");
      PRINT_MEM(MEMORY,MEM_DUMP_STREAM);
      PRINT_EXEC(MEMORY,SYM_TAB,EXECUTABLE_STREAM);
      (void) fclose(EXECUTABLE_STREAM);
      (void) fclose(MEM_DUMP_STREAM);
    } else
      (void) printf("Errors detected. No executable created.\n");

  }
  return 0;
}
