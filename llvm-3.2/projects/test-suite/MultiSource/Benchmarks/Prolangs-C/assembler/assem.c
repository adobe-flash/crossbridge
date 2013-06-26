/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================ assem.c ==================================== */
/* Main (driving) routine of the assembler. Contains a few globals, opens    */
/* closes input, output, and temporary files. Calls Pass1 and Pass2          */

/* to compile (the assembler) use:                                           */
/*     cc -o assem assem.c pass1.c pass2.c scan_line.c stringI.c sym_tab.c   */
/*           op_tab.c pseudo.c code.c record.c buffer.c convert.c  -lm       */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "pass1.h"
#include "pass2.h"
#include "sym_tab.h"

/* --------------------------------- Globals ------------------------------ */
/* MODULE_NAME                       Name of (program) module currently     */
/*                                      assembling.                         */
char MODULE_NAME[LABEL_SIZE_1+1];

/* MAIN_ROUTINE                      Name of (program) module declared with */
/*                                      a START (main routine).             */
char MAIN_ROUTINE[LABEL_SIZE_1+1];

/* START_ADDRESS                     Start Address for program execution.   */
int START_ADDRESS = -1;

/* -------------------------------- main ---------------------------------- */
int main(int argc,char **argv)
{
  FILE *INPUT_STREAM;             /* Input file stream                      */
  SYMBOL_TABLE SYM_TAB;           /* The symbol table                       */
  FILE *TEMP_OUTPUT_STREAM;       /* pass1 creates a file that is used as   */
                                  /*    as the input to pass2.              */
  char *TEMP_OUTPUT_FILE_NAME     /* Name of the above mentioned file       */
    = "%pass1%";
  int ERROR = FALSE_1;            /* FALSE no errors seen. TRUE seen errors */

  INIT_SYM_TAB(&SYM_TAB);         /* Initialize the symbol table            */

  if (argc == 1) (void) printf("usage: assem file\n");
  else {
    if ( (INPUT_STREAM = fopen(argv[1],"r")) == NULL) {
      (void) printf("%s: No such file or directory\n",argv[1]);
    } else {
/* --------------------------------- Have a valid file: assemble it         */
      char *LISTING_FILE;         /* Name of file to put the listing into   */
      FILE *LISTING_STREAM;       /* Stream for listing file                */
      char *OBJECT_FILE;          /* Name of object code file               */
      FILE *OBJECT_STREAM;        /* Object code stream                     */
      int LOOP_COUNTER;           /* a loop counter                         */

      TEMP_OUTPUT_STREAM = fopen(TEMP_OUTPUT_FILE_NAME,"w");

      PASS1(INPUT_STREAM,&SYM_TAB,TEMP_OUTPUT_STREAM); 
      (void) fclose(INPUT_STREAM);
      (void) fclose(TEMP_OUTPUT_STREAM);

/* --------------------------------- Get name for LISTING FILE              */
      LISTING_FILE = (char *) malloc((unsigned int) (strlen(argv[1])+5));
      (void) strcpy(LISTING_FILE,argv[1]);
      for ((LOOP_COUNTER = strlen(argv[1]));
	   ( (LOOP_COUNTER >= 0) && (LISTING_FILE[LOOP_COUNTER] != '.'));
	    LOOP_COUNTER --);
      if (LOOP_COUNTER <= 0) LOOP_COUNTER = strlen(argv[1]);
      (void) strcpy(&(LISTING_FILE[LOOP_COUNTER]),".lst");

/* --------------------------------- Get name for OBJECT FILE               */
      OBJECT_FILE = (char *) malloc((unsigned int) (strlen(argv[1])+5));
      (void) strcpy(OBJECT_FILE,argv[1]);
      (void) strcpy(&(OBJECT_FILE[LOOP_COUNTER]),".obj");

      INPUT_STREAM = fopen(TEMP_OUTPUT_FILE_NAME,"r");
      LISTING_STREAM  = fopen(LISTING_FILE,"w");
      OBJECT_STREAM  = fopen(OBJECT_FILE,"w");

      PASS2(INPUT_STREAM,OBJECT_STREAM,LISTING_STREAM,&SYM_TAB,&ERROR);

      (void) fclose(OBJECT_STREAM);
      (void) fclose(LISTING_STREAM);
      (void) fclose(INPUT_STREAM);

/* -------------------------------- Delete the temporary output file        */
      if (!DEBUG_FLAG_1) {
	char *TEMP;
	TEMP = (char *) malloc((unsigned int) 
			       (4+strlen(TEMP_OUTPUT_FILE_NAME)));
	(void) sprintf(TEMP,"rm %s",TEMP_OUTPUT_FILE_NAME);
	(void) system(TEMP);
      }

/* -------------------------------- If errors, delete the object file       */
      if (ERROR) {
	char *TEMP;
	TEMP = (char *) malloc((unsigned int) 
			       (4+strlen(OBJECT_FILE)));
	(void) sprintf(TEMP,"rm %s",OBJECT_FILE); 
	(void) system(TEMP); 
	(void) printf("Errors detected. Deleted object file.\n");
      }
    }
  }
  return 0;
}
