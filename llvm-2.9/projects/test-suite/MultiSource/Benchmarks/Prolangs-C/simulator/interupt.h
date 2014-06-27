/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== interupt.h =============================== */
/* Handles interupts, rather simply.                                         */

/* ------------------------------ SIGNAL_INTERUPT -------------------------- */
/* 2 parameters:                                                             */
/*   1) int TYPE;                       TYPE/CLASS of interupt (1,2,3, or 4) */
/*   2) int CODE;                       Interupt code.                       */
/* Handles a signal interupt.                                                */
extern void SIGNAL_INTERUPT();
