/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== code.h ================================== */
/* Does the 'real' work of PASS2. Produces one listing file line for a       */
/* assembly instruction and all object file records, except those from pass1 */
/* EVERY line of listing file is produced in this module (except pass1       */
/* error messages).                                                          */
/* Before this is called everything the assembler needs to know is figured   */
/* and this does (most of) the code generation.                              */


/* ================================= CODE ================================== */
/* 5 parameters:                                                             */
/*   1) FILE *INPUT_STREAM;                                                  */
/*   2) FILE *OBJECT_STREAM;                                                 */
/*   3) FILE *LISTING_STREAM;                                                */
/*   4) SYMBOL_TABLE *SYM_TAB;           symbol table to use                 */
/*   5) int *ERROR;                      has an error been found (true/false)*/
/* Determines the location counter value for a line, puts it into the        */
/* listing stream and calls the correct procedures to output the code        */
     extern void CODE();
