/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* =========================== sim_debug.c ================================= */
/* Main (driving) routine of the Simulator/Debugger.                         */

/* to compile (the simulator/debugger) use:                                  */
/*    cc -o run sim_debug.c sym_tab.c stringI.c convert.c memory.c           */
/*              machine.c debugger.c loadexe.c print.c instruct.c format.c   */
/*              interupt.c instruct2.c -g -lm                                */

#include <stdio.h>
#include "constants.h"
#include "boolean.h"
#include "memory.h"
#include "machine.h"
#include "debugger.h"
#include "loadexe.h"
#include "print.h"

/* --------------------------------- Globals ------------------------------ */
/* MAIN_ROUTINE                      Name of (program) module declared with */
/*                                      a START (main routine).             */
char MAIN_ROUTINE[LABEL_SIZE_1+1];

/* -------------------------------- main ---------------------------------- */
int main(int argc,char **argv)
{
  FILE *INPUT_STREAM;             /* Input file stream                      */
  BOOLEAN DEBUG_MODE = TRUE_1;    /* Flag that can turn off the debugging   */
                                  /* feature.                               */
  BOOLEAN ERROR = FALSE_1;        /* Was there an error loading the         */
				  /* executable?                            */

  if (argc == 1) (void) printf("usage:  run [-n] file\n");
  else {
    int ARGUMENT = 1;             /* Which argument of the command line     */
                                  /* currently processing.                  */
    BOOLEAN FLAG;                 /* Just a temporary boolean.              */

/* --------------------------------- Process command directives             */
    if (ARGUMENT < argc) FLAG = (argv[ARGUMENT][0] == '-');
    while ( (ARGUMENT < argc) && FLAG) {
      if (!strcmp(argv[ARGUMENT],"-n")) DEBUG_MODE = FALSE_1;
      else
	(void) printf("Illegal command directive, '%s'. Ignoring.\n",
		      argv[ARGUMENT]);

      if (ARGUMENT < argc) ARGUMENT ++;
      if (ARGUMENT < argc) FLAG = (argv[ARGUMENT][0] == '-');
    }

    if (ARGUMENT >= argc) 
      (void) printf("run: requires a file name.\n");


    INIT_SYM_TAB(&SYM_TAB);         /* Initialize the symbol table           */
    CREATE_MEMORY(&MEMORY);         /* Create/initialize the main memory.    */
    if ( (INPUT_STREAM = fopen(argv[ARGUMENT],"r")) == NULL) {
      (void) printf("%s: No such file or directory\n",argv[ARGUMENT]);
    } else {
/* --------------------------------- Have a valid file: run it               */
      LOAD(DEBUG_MODE,&ERROR,INPUT_STREAM);
      if (!ERROR) DEBUGGER(DEBUG_MODE);
      (void) fclose(INPUT_STREAM);
    }

  }
  return 0;
}
