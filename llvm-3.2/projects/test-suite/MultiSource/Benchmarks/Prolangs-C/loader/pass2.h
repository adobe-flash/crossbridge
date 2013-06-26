/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== pass2.h ================================== */

/* --------------------------------- pass2 --------------------------------- */
/* 5 parameters:                                                             */
/*    1) MEM_SPACE MEMORY;           The SIC/XE's main memory                */
/*    2) SYMBOL_TABLE *SYM_TAB;      loaders [global] symbol table           */
/*    3) int *LOCATION;              Location to load next csect             */
/*    4) int *ERROR;                 any errors in the object code?          */
/*    5) FILE *INPUT;                stream with the object code             */
/* Process Text, Modify, and Reference records of one object file. Also      */
/* does minimal recomputation of the number of bytes needs to be shifted to  */
/* accomidate the relocation.                                                */
extern void PASS2();
