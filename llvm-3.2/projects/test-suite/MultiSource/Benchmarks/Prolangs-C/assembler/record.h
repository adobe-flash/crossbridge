/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== record.h ================================= */
/* This is another buffering module, but unlike buffer.c that buffers whole  */
/* lines of code and whole records, this buffers pieces of one record. A     */
/* object record has an maximal size and contains as much information on     */
/* one record as it possibly can (it tries to use 1 record instead of 2, if  */
/* possible). This routine allows other routines to say, 'This belongs in a  */
/* record, take care of it'. And it descides when to start a new record and  */
/* when not put it on the current record.                                    */

/* --------------------------- INITIALIZE_RECORD --------------------------- */
/* 2 parameters:                                                             */
/*   1) char *VAL;                     Intial value/header of a record       */
                                    /* eg. "R" for a reference record.       */
/*   2) int SIZE;                      Biggest this type of record can ever  */
                                    /* get.                                  */
/* Set up all the global variables needed to handle records.                 */
extern void INITIALIZE_RECORD();

/* ------------------------- PRT_RECORD ------------------------------------ */
/* 1 parameters:                                                             */
/*   1)  FILE *OUTPUT;            Stream to output the record to             */
/* If anything is in the record, output it. Set the record to unitialized.   */
extern void PRT_RECORD();

/* ------------------------- ADD_TO_RECORD --------------------------------- */
/* Put Value into the record. If the record needs to be output put it into   */
/* 2 parameters:                                                             */
/*     1) char *VAL;                /* Value to add to the record            */
/*     2) FILE *OUTPUT;             /* If need be, where to output the record*/
/* stream OUTPUT.                                                            */

extern void ADD_TO_RECORD();

/* --------------------- INITITIALIZE_TEXT_RECORD -------------------------- */
/* No parameters                                                             */
/* Initialize a special type of record. A text record.                       */
extern void INITIALIZE_TEXT_RECORD();

/* ---------------------- PRT_TEXT_RECORD ---------------------------------- */
/* 1 parameter:
/*    1) FILE *OUTPUT;                /* Stream to output the record to      */
/* If anything is in the text record, output it. Set the record to           */
/* unitialized.                                                              */
extern void PRT_TEXT_RECORD();

/* ------------------------- ADD_TO_TEXT_RECORD ---------------------------- */
/*    1) char *VAL;               /* Value to add to text record             */
/*    2) int PUT_AT;              /* Location in SIC the value will go into  */
/*    3) FILE *OUTPUT;            /* If need to OUTPUT, put it into stream   */
/* Put Value into the text record at SIC addres PUT_AT. If the record needs  */
/* to be output put it into stream OUTPUT. Record must be output if the      */
/* current record is full, or where the VALUE belongs isn't the next spot    */
/* on the text record.                                                       */
extern void ADD_TO_TEXT_RECORD();
