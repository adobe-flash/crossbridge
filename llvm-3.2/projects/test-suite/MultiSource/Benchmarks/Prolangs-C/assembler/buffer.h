/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ================================ buffer.h =============================== */
/* This module was created for 2 purposes:                                   */
/*         1) Store Modify records (of object code) till after all text      */
/*             of a module (so that a modify record is always after the text */
/*             record for the same location)                                 */
/*         2) Store Error messages. Sometimes errors are detected that can   */
/*             not be output immediately (in the middle of writting out      */
/*             another record). This allows you to save error messages till  */
/*             able to output them.                                          */

/* type BUFFER                          Linked list of strings               */
struct BUFFER {
  char *LINE;
  struct BUFFER *NEXT;
};

/* type BUFFER_TYPE                     Pointers to both ends of the linked  */
/*                                      list and whether contains            */
/*                                      modification records or errors       */
struct BUFFER_TYPE {
  struct BUFFER *HEAD_OF_BUFFER;
  struct BUFFER *TAIL_OF_BUFFER;
  int KIND;
};


/* MOD_REC_BUF                         GLOBAL buffer for modification records*/
extern struct BUFFER_TYPE MOD_REC_BUF;

/* ERROR_REC_BUF                       GLOBAL buffer for error messages      */
extern struct BUFFER_TYPE ERROR_REC_BUF;

/* ------------------------- OUTPUT_BUFFER --------------------------------- */
/* 3 parameters:                                                             */
/*    1) struct BUFFER_TYPE *BUF;    the buffer to output                    */
/*    2) FILE *OUTPUT;               the stream to output to                 */
/*    3) int PASS;                   which pass called from 1 = pass1,       */
/* Puts everything in buffer BUF into output stream OUTPUT. PASS (1 or 2) is */
/* used as a fix to a problem I. In pass2, I call a procedure used by pass1, */
/* which outputs errors in a different format. This is used to, in pass2,    */
/* convert the pass1 errors. Upon completion the buffer is empty.            */
extern void OUTPUT_BUFFER();

/* ----------------------- ADD_TO_END_OF_BUFFER ---------------------------- */
/* 2 parameters:                                                             */
/*     1) struct BUFFER_TYPE *BUF;   the buffer to add onto                  */
/*     2) char *INPUT_STR;           what to add onto the buffer             */
/* Puts the string INPUT_STR, on the end of the buffer pointed to by BUF.    */
extern void ADD_TO_END_OF_BUFFER();


