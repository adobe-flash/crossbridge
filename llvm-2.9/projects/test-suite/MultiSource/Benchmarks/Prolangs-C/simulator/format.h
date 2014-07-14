/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
 /* =============================== format.h ================================ */
/* Returns the registers associtiated with a FORMAT2 instruction (handles    */
/* BITS 8-15) and the Address or value associated with a FORMAT3/FORMAT4     */
/* instruction (handles BITS 6-23 or BITS 6-31).                             */


/* ------------------------------ FORMAT2 ---------------------------------- */
/* 2 parameters:                                                             */
/*   1) int *REG1;                   See bellow                              */
/*   2) int *REG2;                   See bellow                              */
/* Handles bits 8-15 of format 2 instruction. Puts bits 8-11 in REG1, bits   */
/* 12-15 in REG2 and updates PC.                                             */
extern int FORMAT2();

/* ----------------------------- FORMAT3_4 --------------------------------- */
/* 2 parameters:                                                             */
/*   1) int DEMAND;                  Do you want the VALUE_1 or ADDRESS_1?   */
/*   2) int BYTES;                   If want a value, how many bytes do you  */
/*                                      need?                                */
/* Handles bits 6-23 for format 3 instructions, and bits 5-31 for format 4   */
/* instructions. Updates PC and returns ADDRESS or VALUE of the operand      */
/* Depending on demand.                                                      */
extern int FORMAT3_4();
