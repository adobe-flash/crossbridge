/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== record.c ================================ */
/* This is another buffering module, but unlike buffer.c that buffers whole  */
/* lines of code and whole records, this buffers pieces of one record. A     */
/* object record has an maximal size and contains as much information on     */
/* one record as it possibly can (it tries to use 1 record instead of 2, if  */
/* possible). This routine allows other routines to say, 'This belongs in a  */
/* record, take care of it'. And it descides when to start a new record and  */
/* when not put it on the current record.                                    */
#include <stdio.h>
#include <string.h>
#include "convert.h"
#include "constants.h"

/* MAX_RECORD_SIZE_1                      Largest size of ANY record.        */
#define MAX_RECORD_SIZE_1                 80

char RECORD[MAX_RECORD_SIZE_1 + 1];      /* The current record creating      */
char INIT_RECORD[MAX_RECORD_SIZE_1 + 1]; /* The intial configuration         */
int NEXT_COL;                            /* Where the next thing will go into*/
                                         /* the record.                      */
int MAX_SIZE;                            /* The maximum size of THIS record  */
int LOCATION = 0;                        /* Where in SICs main memory does   */
                                         /* this record start. Only TEXT     */
                                         /* records.                         */

/* IS_INITIALIZED                         Has the record been initialized?   */
#define NOT_INIT_2          0
#define INITIALIZED_2              1
int IS_INITIALIZED = NOT_INIT_2;

/* --------------------------- INITIALIZE_RECORD --------------------------- */
/* Set up all the global variables needed to handle records.                 */
void INITIALIZE_RECORD(char *VAL,int SIZE)
{
  if ((SIZE > MAX_RECORD_SIZE_1) || (strlen(VAL) > SIZE) || IS_INITIALIZED)
    (void) printf("INITIALIZE_RECORD called illegally.\n");
  else {
    (void) strcpy(INIT_RECORD,VAL);
    (void) strcpy(RECORD,VAL);
    MAX_SIZE = SIZE;
    NEXT_COL = strlen(RECORD);
    IS_INITIALIZED = INITIALIZED_2;
  }
}

/* ------------------------- PRT_RECORD ------------------------------------ */
/* If anything is in the record, output it. Set the record to unitialized.   */
void PRT_RECORD(FILE *OUTPUT)
{
  if (!IS_INITIALIZED)
    (void) printf("PRT_RECORD called illegally.\n");
  else {
    if (strcmp(RECORD,INIT_RECORD))
      (void) fprintf(OUTPUT,"%s\n",RECORD);
    IS_INITIALIZED = NOT_INIT_2;
  }
}

/* ------------------------- ADD_TO_RECORD --------------------------------- */
/* Put Value into the record. If the record needs to be output put it into   */
/* stream OUTPUT.                                                            */
void ADD_TO_RECORD(char *VAL,FILE *OUTPUT)
{
  int LENGTH;                  /* Stores legth of the record                 */
  int ERROR = FALSE_1;         /* Has an error been detected.                */

  if (!IS_INITIALIZED) ERROR = 1;
  else 
    if ((NEXT_COL + (LENGTH = strlen(VAL))) > MAX_SIZE) {
/* ---------- Output current record, but VAL onto next record.               */
      PRT_RECORD(OUTPUT);
      INITIALIZE_RECORD(INIT_RECORD,MAX_SIZE);
      if ((NEXT_COL + LENGTH) > MAX_SIZE) 
	ERROR = 1;
    }
  if (!ERROR) {
    (void) strcpy(&(RECORD[NEXT_COL]),VAL);
    NEXT_COL += LENGTH;
  } else
    (void) printf("ADD_TO_RECORD called illegally.\n");
}

/* --------------------- INITITIALIZE_TEXT_RECORD -------------------------- */
/* Initialize a special type of record. A text record.                       */
void INITIALIZE_TEXT_RECORD(void)
{
/* Normal record contains col. 10-69 of text record. */
  INITIALIZE_RECORD("",60);
}

/* ---------------------- PRT_TEXT_RECORD ---------------------------------- */
/* If anything is in the text record, output it. Set the record to           */
/* unitialized.                                                              */
void PRT_TEXT_RECORD(FILE *OUTPUT)
{
  if (!IS_INITIALIZED)
    (void) printf("PRT_TEXT_RECORD called illegally.\n");
  else {
    if (strcmp(RECORD,INIT_RECORD)) {
      (void) fprintf(OUTPUT,"T");
      PRT_NUM(LOCATION,16,6,OUTPUT);
      PRT_NUM((NEXT_COL/HEX_CHAR_PER_BYTE_1),16,2,OUTPUT);
      (void) fprintf(OUTPUT,"%s\n",RECORD);

    }
    IS_INITIALIZED = NOT_INIT_2;
  }
}

/* ------------------------- ADD_TO_TEXT_RECORD ---------------------------- */
/* Put Value into the text record at SIC addres PUT_AT. If the record needs  */
/* to be output put it into stream OUTPUT. Record must be output if the      */
/* current record is full, or where the VALUE belongs isn't the next spot    */
/* on the text record.                                                       */
void ADD_TO_TEXT_RECORD(char *VAL,int PUT_AT,FILE *OUTPUT)
{
  int ERROR = FALSE_1;
  int LEN;

  LEN = strlen(VAL);

  if (!IS_INITIALIZED || (LEN > MAX_SIZE)) ERROR = 1;
  else {

    if (!ERROR && ((LOCATION + NEXT_COL/HEX_CHAR_PER_BYTE_1) != PUT_AT) ) {
      PRT_TEXT_RECORD(OUTPUT);
      LOCATION = PUT_AT;
      INITIALIZE_TEXT_RECORD();
    }

    if (!ERROR && ((NEXT_COL + LEN) > MAX_SIZE) ) {
      PRT_TEXT_RECORD(OUTPUT);
      LOCATION += strlen(RECORD)/HEX_CHAR_PER_BYTE_1;
      INITIALIZE_TEXT_RECORD();
    }
  }
  if (!ERROR) {
    (void) strcpy(&(RECORD[NEXT_COL]),VAL);
    NEXT_COL += LEN;
  } else
    (void) printf("ADD_TO_TEXT_RECORD called illegally.\n");
}
