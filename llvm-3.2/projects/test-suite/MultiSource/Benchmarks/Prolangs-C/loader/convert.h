/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== convert.h =============================== */
/* Contains routines that convert one thing to another. Most of the routines */
/* deal with converting a number to a string of that number in some base, or */
/* converting a string (assuming some base) into an integer. Basically       */
/* just a file of miscellaneous 'useful' routines.                           */

/* ----------------------------- eoln -------------------------------------- */
/* true if CH is the End Of LiNe character, otherwise false.                 */
/* 1 parameter:                                                              */
/*    1) char CH;            Character in question.                          */
extern int eoln();

/* ------------------------ CHAR_TO_DIGIT ---------------------------------- */
/* 2 parameters:                                                             */
/*    1) char CH;                       character to convert                 */
/*    2) int NUM_BASE;                  base of number                       */
/* Convert a character (CH) into its integer value give base NUM_BASE. Return*/
/* -1 if it is not a valid digit in that base.                               */
extern int CHAR_TO_DIGIT();

/* ------------------------- NUM_TO_STR ------------------------------------ */
/* 4 parameters:                                                             */
/*    1) int NUM;                  Number to convert                         */
/*    2) int BASE;                 Base to convert into                      */
/*    3) int LEN;                  Number of digits in final string          */
/*    4) char *STR;                where to put it.                          */
/* Put into STR the string with LEN digits that represents the number NUM    */
/* in base BASE (eg. NUM_TO_STR(10,16,3,STR) puts "00A" in STR).             */
extern void NUM_TO_STR();

/* ------------------------------ GET_NUM ---------------------------------- */
/* 4 parameters:                                                             */
/*   1) char **CURRENT_CHAR;         pointer to start of string to interpret */
/*                                   as an integer (in NUM_BASE)             */
/*   2) int BITS;                    Number must fit in this may bits 2's    */
/*                                   complement.                             */
/*   3) int NUM_BASE;                Base the number is in.                  */
/*   4) BOOLEAN *ERROR;              Was an error detected.                  */
/* CURRENT_CHAR points to a string. Starting with the character              */
/* **CURRENT_CHAR and get the biggest possible integer in BASE NUM_BASE. If  */
/* that number is too large/small to fit in BITS bits (2's complement),      */
/* Put an error message into ERROR_REC_BUF. Returns 0 if this or any error is*/
/* detected, otherwise it returns the integer representation of the number.  */
/* NOTE: If number is to large/small this routine stops as soon as it        */
/*   realizes this w/o (with out) looking at the rest of the input.          */
extern int GET_NUM();

/* -------------------------------- STR_TO_NUM ----------------------------- */
/* Convert the first DIGITS characters of STR to an integer of base BASE.    */
/* 5 paramaters:                                                             */
/*      1) char *STR;             STRing to convert.                         */
/*      2) int DIGITS;            number of DIGITS of string to convert.     */
/*      3) int BASE;              BASE of the number.                        */
/*      4) int *NUM;              Place to return the result                 */
/*      5) BOOLEAN *ERROR;        Has an error been detected.                */
extern void STR_TO_NUM();

