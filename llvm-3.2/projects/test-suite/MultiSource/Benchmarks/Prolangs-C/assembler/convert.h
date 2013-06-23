/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== convert.h =============================== */
/* Contains routines that convert one thing to another. Most of the routines */
/* deal with converting a number to a string of that number in some base, or */
/* converting a string (assuming some base) into an integer. Basically       */
/* just a file of miscellaneous 'useful' routines.                           */

/* ------------------------ RESET_MISSING_LABEL_NAME ----------------------- */
/* No parmeters:                                                             */
/* Between pass1 and pass2, MISSING_LABEL_NAME must be reset to its          */
/* orginal value. This procedure does this.                                  */
extern void RESET_MISSING_LABEL_NAME();

/* ------------------------ GET_NEXT_MISSING_LABEL ------------------------- */
/* 2 parameters:                                                             */
/*     1) char *LABEL;                    String to put label into           */
/*     2) SYMBOL_TABLE *SYM_TAB;          symbol table to insert the label in*/
/* Puts MISSING_LABEL_NAME into string LABEL, and puts it into the symbol    */
/* table. It then changes MISSING_LABEL_NAME to some other unique name.      */
/* NOTE: this will only return about 90 unique names, after that it returns  */
/*       the same name repeadily. This would result only some additional     */
/*       previously defined label error messages. Never called when executed */
/*       on a source with no errors.                                         */
extern void GET_NEXT_MISSING_LABEL();

/* ---------------------------- BLANK_STR ---------------------------------- */
/* 1 paramter:                                                               */
/*     char *STRING;                       String to blank out               */
/* Set characters 0-7 of STRING to blank and make the 8th character NULL     */
/* ie. a 8 character string of blanks.                                       */
extern void BLANK_STR();

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

/* -------------------------- PRT_NUM ------------------------------------- */
/* 4 parameters:                                                            */
/*   1) int NUM;                Number to print out                         */
/*   2) int NUM_BASE;           Base to print it out in                     */
/*   3) int LEN;                Number of digits to print out.              */
/*   4) FILE *OUTPUT;           Stream to output the number to              */
/* Output (to stream OUTPUT) the string with LEN digits that represents the */
/* number NUM in base BASE (eg. NUM_TO_STR(20,16,2,STR) puts "14" in STR).  */
extern void PRT_NUM();

/* ------------------------------ GET_NUM ---------------------------------- */
/* 3 parameters:                                                             */
/*   1) char **CURRENT_CHAR;         pointer to start of string to interpret */
/*                                   as an integer (in NUM_BASE)             */
/*   2) int BITS;                    Number must fit in this may bits 2's    */
/*                                   complement.                             */
/*   3) int NUM_BASE;              * Base the number is in.                  */
/* CURRENT_CHAR points to a string. Starting with the character              */
/* **CURRENT_CHAR and get the biggest possible integer in BASE NUM_BASE. If  */
/* that number is too large/small to fit in BITS bits (2's complement),      */
/* Put an error message into ERROR_REC_BUF. Returns 0 if this or any error is*/
/* detected, otherwise it returns the integer representation of the number.  */
/* NOTE: If number is to large/small this routine stops as soon as it        */
/*   realizes this w/o (with out) looking at the rest of the input.          */
extern int GET_NUM();

/* ---------------------------- GET_EXPRESSION ----------------------------- */
/* 5 parameters:                                                             */
/*   1) char **CURRENT_CHAR;             Where to look for the expression    */
/*   2) int BITS;                        value of the expression must fit in */
/*                                       BITS 2's complement                 */
/*   3) int LOCATION;                    LOCATION this number is to go into  */
/*                                        this is needed for modification    */
/*                                        records when referencing an EXTREF */
/*   4) SYMBOL_TABLE *SYM_TAB;        The symbol table                       */
/*   5) int *WHAT_KIND;               Initially whether the caller wants     */
/*                                       Relative or absolute or either type */
/*                                       of expression. At termination its   */
/*                                       the kind found.                     */
/* Is used to find an expression (well, for now actually it can have no      */
/* operations, just single terms). Caller can specify what kind of expression*/
/* is needed (ABSOLUTE or RELATIVE) or can specify that either kind is okay. */
/* Sets WHAT_KIND to they type (kind) of expression found.                   */
extern int GET_EXPRESSION();
