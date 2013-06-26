/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== pass1.h ================================= */

/* ------------------------------- PASS1 ----------------------------------- */
/* 4 parameters:                                                             */
/*    1) SYMBOL_TABLE *SYM_TAB;      loaders [global] symbol table           */
/*    2) int *LOCATION;              Location to load next csect             */
/*    3) int *ERROR;                 any errors in the object code?          */
/*    4) FILE *INPUT;                stream with the object code             */
/* Process Header, define, and end records of oen object file.               */
extern void PASS1();
