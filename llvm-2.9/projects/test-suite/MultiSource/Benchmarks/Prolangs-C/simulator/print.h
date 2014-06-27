/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== print.h ================================== */
/* Contains routines for output the symbol table and executable files.       */

/* --------------------------------- PRT_SYM_TAB --------------------------- */
/* 2 parameters:                                                             */
/*    1) SYMBOL_TABLE SYM_TAB;          The symbol table.                    */
/*    2) FILE *OUTPUT;                  The output steam                     */
/* Prints header for the symbol table file and lets PRINT_TABLE print it.    */
extern void PRT_SYM_TAB();

/* ------------------------------ PRINT_EXEC ------------------------------- */
/* 3 parameters:                                                             */
/*    1) MEM_SPACE MEMORY;               The memory                          */
/*    2) SYMBOL_TABLE SYM_TAB;           The symbol table                    */
/*    3) FILE *OUTPUT;                   The output stream                   */
/* Drives the outputing of the executable file                               */
extern void PRINT_EXEC();
