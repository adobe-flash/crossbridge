/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== debugger.h ============================== */
/* Drives the whole simulatation/debugging process. Contains the debugging   */
/* Routines and calls the simulation routines (else where defined).          */
#include "sym_tab.h"
/* ---------------------------------- Globals ------------------------------ */
/* MAIN_ROUTINE                       Name of (program) module declared with */
/*                                       a START (main routine).             */
extern char MAIN_ROUTINE[LABEL_SIZE_1+1];

/* START_ADDRESS                      Location where execution begins        */
extern int START_ADDRESS;

/* SYM_TAB                            The symbol table                       */
extern SYMBOL_TABLE SYM_TAB;

/* RETURN_STATUS                      Status of machine when control returned*/
extern int RETURN_STATUS;          /* to debugger.                           */

/* TYPE_OUT_MODE                      Remembers the type out mode            */
char TYPE_OUT_MODE[6];

/* ------------------------------- Procedures ------------------------------ */
/* -------------------------------- DEBUGGER ------------------------------- */
/* 1 parameter:                                                              */
/*   1) BOOLEAN DEBUG_MODE;      TRUE => Debugging. FALSE => just simulating */
/* Drives the entire simulatation/debugging process.                         */
extern void DEBUGGER ();
     
