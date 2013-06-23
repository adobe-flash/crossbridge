/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ================================ pass1.h =============================== */

#define START_LEGAL_1            0 /* = May see a START legally              */
#define SEEN_START_1             1 /* = Have seen a START                    */
/* SEND_START_OP                      Global that contains whether or not    */
/*                                    a START operator has been seen         */
extern int SEEN_START_OP;


#define NO_END_SEEN_1          0 /* = Haven't seen an END                    */
#define END_SEEN_1             1 /* = Have seen an END but haven't           */
/*	                                        found any code after it      */
#define END_SEEN_WITH_CODE_1   2 /* = Have seen an END and code after it     */
#define START_OF_CODE_1        3 /* = Start of code                          */
/* SEEN_END_OP                       Flag to determine if seen an end since  */
/*                                   start of last module or not. If so it   */
/*                                   also indicated if any code seen since   */
/*                                   end (so only one 'code after end' error */
/*                                   is generated.                           */
extern int SEEN_END_OP;

/* LOCATION_EXCEEDS_MEM_SIZE         Indicates if location counter is greater*/
/*                                   then the memory size.                   */
extern int LOCATION_EXCEEDS_MEM_SIZE;

/* -------------------------- CHANGE_LOCATION ------------------------------ */
/* 3 parameters:                                                             */
/*   1) int *LOCATION;           The location counter                        */
/*   2) int INCREMENT;           How much to increment it by.                */
/*   3) FILE *OUTPUT;            Where to put error messages, if need be.    */
/* LOCATION = LOCATION + INCREMENT unless this puts LOCATION past MEM_SIZE,  */
/* or if the location has already exceeded the memory size.                  */
extern void CHANGE_LOCATION();

/* ---------------------- CAPITALIZE_STRING -------------------------------- */
/* 1 parameter:                                                              */
/*   1) char STR[];             String to capitalize.                        */
/* Make all lower case letters in the string STR upper case letters          */
extern void CAPITALIZE_STRING();

/* --------------------------- PASS1 --------------------------------------- */
/* 3 parameters:
/*   1) FILE *INPUT_FILE;                   Stream with the source code      */
/*   2) SYMBOL_TABLE *SYM_TABLE;            The symbol table (already        */
                                         /* initialized).                    */
/*   3) FILE *TEMP_OUTPUT_STREAM;           Stream for the output of Pass1   */
/* Drives the pass1 process, letting others do the real work.                */
extern void PASS1();
