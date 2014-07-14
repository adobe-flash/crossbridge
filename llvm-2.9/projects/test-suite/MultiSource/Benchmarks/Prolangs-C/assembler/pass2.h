/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================ pass2.h =================================== */
/* Drives the pass2 process, but lets other routines do the real work.      */

/* -------------------------- PASS2 ---------------------------------------- */
/* 5 parameters:                                                             */
/*  1) FILE *INPUT_STREAM;              Input for PASS2 (NOT the source).    */
/*  2) FILE *OBJECT_STREAM;             Stream for the object file.          */
/*  3) FILE *LISTING_STREAM;            Stream for the listing file.         */
/*  4) SYMBOL_TABLE *SYM_TAB;           Symbol table build in pass1          */
/*  5) int *ERROR;                      ERROR flag. Has an error been seen.  */
/* Drives pass2                                                              */
extern void PASS2();
