/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =========================== buffer.c ==================================== */
/* This module was created for 2 purposes:                                   */
/*         1) Store Modify records (of object code) till after all text      */
/*             of a module (so that a modify record is always after the text */
/*             record for the same location)                                 */
/*         2) Store Error messages. Sometimes errors are detected that can   */
/*             not be output immediately (in the middle of writting out      */
/*             another record). This allows you to save error messages till  */
/*             able to output them.                                          */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Constants used to determine if being used to store modify records or      */
/* error messages.                                                           */
#define MOD_REC_2              0
#define ERROR_REC_2            1


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
struct BUFFER_TYPE MOD_REC_BUF = {NULL,NULL,MOD_REC_2};

/* ERROR_REC_BUF                       GLOBAL buffer for error messages      */
struct BUFFER_TYPE ERROR_REC_BUF = {NULL,NULL,ERROR_REC_2};




/* ------------------------- OUTPUT_BUFFER --------------------------------- */
/* Puts everything in buffer BUF into output stream OUTPUT. PASS (1 or 2) is */
/* used as a fix to a problem I. In pass2, I call a procedure used by pass1, */
/* which outputs errors in a different format. This is used to, in pass2,    */
/* convert the pass1 errors. Upon completion the buffer is empty.            */

void OUTPUT_BUFFER(struct BUFFER_TYPE *BUF,FILE *OUTPUT,int PASS)
{
  struct BUFFER *NEXT;

/* --------------------------------- Step through the linked list, putting   */
/* --------------------------------- each string on a different line.        */

  while ((*BUF).HEAD_OF_BUFFER != NULL) {
    NEXT =(* (*BUF).HEAD_OF_BUFFER).NEXT;
    if ((PASS == 2) && !strncmp("eERROR",(*(*BUF).HEAD_OF_BUFFER).LINE,6))
      (void) fprintf(OUTPUT,"%s\n",&((*(*BUF).HEAD_OF_BUFFER).LINE[1]));
    else
      (void) fprintf(OUTPUT,"%s\n",(*(*BUF).HEAD_OF_BUFFER).LINE);
    free((*(*BUF).HEAD_OF_BUFFER).LINE);
    free((char *) (*BUF).HEAD_OF_BUFFER);
    (*BUF).HEAD_OF_BUFFER = NEXT;
  }
  (*BUF).TAIL_OF_BUFFER = NULL;
}


/* ----------------------- ADD_TO_END_OF_BUFFER ---------------------------- */
/* Puts the string INPUT_STR, on the end of the buffer pointed to by BUF.    */

void ADD_TO_END_OF_BUFFER(struct BUFFER_TYPE *BUF,char *INPUT_STR)
{
  char *TEMP_LINE;
/* --------------------------------- Create new element, and put on buffr    */
  if ((*BUF).HEAD_OF_BUFFER == NULL) {
    (*BUF).HEAD_OF_BUFFER = (struct BUFFER *) malloc(sizeof(struct BUFFER));
    (*BUF).TAIL_OF_BUFFER = (*BUF).HEAD_OF_BUFFER;
  } else {
    (*(*BUF).TAIL_OF_BUFFER).NEXT = (struct BUFFER *) malloc(sizeof(struct BUFFER));
    (*BUF).TAIL_OF_BUFFER = (*(*BUF).TAIL_OF_BUFFER).NEXT;
  }

/* --------------------------------- Initialize the buffer element correctly */
  (*(*BUF).TAIL_OF_BUFFER).LINE = TEMP_LINE = 
    malloc((unsigned int) (strlen(INPUT_STR) + 2));
  if ((*BUF).KIND == MOD_REC_2) {
    TEMP_LINE[0] = 'M';
    (void) strcpy( &(TEMP_LINE[1]),INPUT_STR);
  } else
    (void) strcpy( TEMP_LINE,INPUT_STR);
  (*(*BUF).TAIL_OF_BUFFER).NEXT = NULL;
}
