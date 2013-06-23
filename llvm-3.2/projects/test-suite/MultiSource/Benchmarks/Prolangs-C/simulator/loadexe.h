/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ================================ loadexe.h ============================== */
/* Sets up the machine from the executable file ('loads' program)            */

/* ------------------------------ LOAD ------------------------------------- */
/* 3 parameters:                                                             */
/*   1) BOOLEAN DEBUG_MODE;          False - Simulator; True - Sim/Debugger  */
/*   2) int *ERROR;                  any errors in the executable code?      */
/*   3) FILE *INPUT;                 stream with the executable code         */
/* Load the program from the executable file.                                */
extern void LOAD();
