/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== scan_line.h ============================= */
/* Contains routines for doing simple parsing of source program (eg. return  */
/* label, operator and everthing after the operator).                        */

/* pass_type               Which pass the assembler is currently in. Used to */
/*                         surpress redisplaying pass1 error messages in     */
/*                         pass2.                                            */
enum pass_type {pass1,pass2};

/* ------------------------------ IS_BLANK_OR_TAB -------------------------- */
/* 1 parameter:                                                              */
/*    1) char CH;            Character in question.                          */
/* True if CH is a blank or a tab, otherwise false.                          */
extern int IS_BLANK_OR_TAB();

/* ----------------------------- eoln -------------------------------------- */
/* 1 parameter:                                                              */
/*    1) char CH;            Character in question.                          */
/* true if CH is the End Of LiNe character, otherwise false.                 */
extern int eoln();

/* -------------------------------- GET_LABEL ------------------------------ */
/* 4 parameters:                                                             */
/*  1) char *LABEL;           Where to put the label if found.               */
/*  2) char *INPUT;           String to get the label from.                  */
/*  3) enum pass_type PASS;   pass1 = buffers error messages in ERROR_REC_BUF*/
/*                            pass2 = don't buffer error messages.           */
/*  4) char **NEXT_CHAR;      where to start looking for the label in INPUT  */
/* Get a SIC/XE Label (Letter followeb by zero to seven letters or digits).  */
/* Return it in LABEL. Get it from INPUT starting from NEXT_CHAR.            */
extern void GET_LABEL();

/* ---------------------------- SCAN_LINE ---------------------------------- */
/* 9 parameters:                                                             */
/*  1) int LOCATION_COUNTER;         SIC/XE location counter at start of line*/
/*  2) char **INPUT_LINE;            Where to return the source line         */
/*  3) char *LABEL;                  Where to return the label if one exists */
/*  4) int *EXTENDED;                Boolean: Is there a '+' before the      */
/*                                     operator. i.e. is this extended format*/
/*  5) char *OPERATOR;               Where to return the operator.           */
/*  6) char **REST;                  Where to return everything after the    */
/*                                     operator. Ignoring leading white-space*/
/*  7) enum pass_type PASS;          pass1 = do print error messages and     */
/*                                     source; pass2 = do not print error    */
/*                                     messages.                             */
/*  8) FILE *STREAM;                 Stream for outputing error messages and */
/*                                     the source code                       */
/*  9) FILE *INPUT_STREAM;           Where to get the source line from       */
/* Reads in a sourse program line and does simple parsing.                   */
extern void SCAN_LINE();
