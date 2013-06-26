/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== memory.c ================================ */
/* Contains all the routines that manage the main memory of the SIC/XE       */
/* machine.                                                                  */

#include "boolean.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "convert.h"
#include "load.h"

/* MEM_SPACE                       The type of a main memory in this program.*/
typedef char *MEM_SPACE;

/* ========================== Printing memory constants ==================== */
/* BYTES_PER_GROUP                 How many bytes to print out at a time.    */
#define BYTES_PER_GROUP                      4
/* GROUPS_PER_LINE                 How many groups of bytes to put on one    */
/*                                    line*/
#define GROUPS_PER_LINE                      4
/* LINES_OF_GAP_TO_ELIPSE          How many consequative lines of unchanged  */
/*                                    bytes must present to elipse out the   */
/*                                    lines from the output.                 */
#define LINES_OF_GAP_TO_ELIPSE               3

/* ========== Keeping Track of which locations have been modified ========== */
/* Keep a linked list of blocks of memory that have been modified. ONLY      */
/* locations that where actually loaded into are included in any block and   */
/* locations that where loaded are in some block.                            */

/* BUFFER_ELEMENT                 Type of one block. START of block and it's */
/*                                   length. Plus a pointer because a linked */
/*                                   list.                                   */
struct BUFFER_ELEMENT {
  int START;                   /* Start of the block.                        */
  int LENGTH;                  /* Length of the block.                       */
  struct BUFFER_ELEMENT *NEXT; /* Next block in the linked list.             */
};

/* BUFFER                        A record with the first and last element of */
/*                                  a lined list.                            */
/* LOCATIONS_USED                A BUFFER that contains the modifed locations*/
struct BUFFER {
  struct BUFFER_ELEMENT *HEAD;/* First block of the linked list.             */
  struct BUFFER_ELEMENT *END; /* Last block of the linked list.              */
} LOCATIONS_USED = {NULL,NULL};

/* ---------------------------- ADD_TO_BUFFER (local) ---------------------- */
/* Adds a block to the END of the linked list of blocks.                     */
void ADD_TO_BUFFER(int LOCATION,int LEN)
{
  if (LOCATIONS_USED.HEAD == NULL) {
/* ------------ Add to an empty list.                                        */
    LOCATIONS_USED.HEAD = (struct BUFFER_ELEMENT *) malloc(sizeof(struct BUFFER_ELEMENT));
    LOCATIONS_USED.END = LOCATIONS_USED.HEAD;
  } else {
/* ------------ Add to a non-empty list.                                     */
    (*LOCATIONS_USED.END).NEXT = 
	(struct BUFFER_ELEMENT *) malloc(sizeof(struct BUFFER_ELEMENT));
    LOCATIONS_USED.END = (*LOCATIONS_USED.END).NEXT;
  }
  (*LOCATIONS_USED.END).START = LOCATION;
  (*LOCATIONS_USED.END).LENGTH = LEN;
  (*LOCATIONS_USED.END).NEXT = NULL;
}

/* ============================= Memory Routines =========================== */

/* --------------------------------- INT  ---------------------------------- */
/* Return the integer representation for a character. Had to be written      */
/* Because the character 255 (for example) who be treated as the integer -1, */
/* but I needed it to be 255.                                                */
int INT(char CH)
{
  int I;
  I = CH;

  if (I<0) return (256+I);
  return (I);
}

/* ----------------------------- Create Memory ----------------------------- */
/* Create a megabyte of memory and point MEMORY to it. It does not initialize*/
/* the memory as this took about 5 minuites. Since the memory was            */
/* uninitized I needed another way to know when locations where modified,    */
/* thus the above lined list of blocks.                                      */
void CREATE_MEMORY(MEM_SPACE *MEMORY)
{
  if ((*MEMORY) != NULL)
    (void) printf("CREATE_MEMORY called illegally.\n");
  else (*MEMORY) = malloc((unsigned int) MEM_SIZE_1);
}

/* -------------------------- DO_STORE (local) ----------------------------- */
/* Place BYTES consequative bytes into MEMORY, starting at ADDRESS.          */
void DO_STORE(char *VALUE,int BYTES,int ADDRESS,MEM_SPACE MEMORY,
              BOOLEAN *ERROR,BOOLEAN BUFFER_Q)
{
  BOOLEAN LOCAL_ERROR = FALSE_1; /* Was an error detected during the store.  */
  int TEMP;                 /* A loop counter variable.                      */
  int INT_VAL;              /* Integer representation of one byte of the     */
                            /*    block.                                     */

/* ---------------------- Check if fits into memory                          */
  if ( (ADDRESS + BYTES <= MEM_SIZE_1) && (ADDRESS + BYTES >= 0) ) {

/* ---------------------- Remember loaded into this block (if necessary)     */
    if (BUFFER_Q) ADD_TO_BUFFER(ADDRESS,BYTES);

/* ---------------------- Load each byte into memory                         */
    for (TEMP = 0; ((TEMP < BYTES) && !eoln(VALUE[TEMP]) && !LOCAL_ERROR);
	 TEMP++) {
      STR_TO_NUM(&(VALUE[TEMP*HEX_CHAR_PER_BYTE_1]),HEX_CHAR_PER_BYTE_1,
		 16,&INT_VAL,&LOCAL_ERROR); 
      MEMORY[ADDRESS+TEMP] = (char) INT_VAL;
    }

    if (LOCAL_ERROR) {
      (void) printf("ERROR: Illegal store VALUE = %s.\n", VALUE);
      (*ERROR) = TRUE_1;
    }
  } else {
    (void) printf("ERROR: Illegal store[1] ADDRESS = %d, BYTES = %d.\n",
		  ADDRESS,BYTES);
    (*ERROR) = TRUE_1;
  }
}

/* ------------------------------ STORE_AT --------------------------------- */
/* Place BYTES consequative bytes into MEMORY, starting at ADDRESS.          */
void STORE_AT(char *VALUE,int BYTES,int ADDRESS,MEM_SPACE MEMORY,
              BOOLEAN *ERROR)
{
/*                                          vvvvvv Remember loaded this block*/
  DO_STORE(VALUE,BYTES,ADDRESS,MEMORY,ERROR,TRUE_1);
}

/* ---------------------------- ADD_INT_TO_LOC ----------------------------- */
/* Add the integer NUM to what is store at LOCATION in MEMORY.               */
void ADD_INT_TO_LOC(int NUM,int LOCATION,int HALF_BYTES,MEM_SPACE MEMORY,
                    BOOLEAN *ERROR)
{
  int INT_MEM_VAL = 0;      /* Integer stored at the relavant location.      */
  char CHAR_MEM_VAL[BITS_PER_WORD_1/BITS_PER_HALFBYTE_1+2]; 
                            /* CHAR_MEM_VAL is the string representation of  */
                            /*   the number stored at a location of size     */
                            /*   HALF_BYTES.                                 */

  if ( ((int) HALF_BYTES/2)*2 != HALF_BYTES)
    HALF_BYTES ++;          /* If odd number of half_bytes include an extra  */
                            /*   one so that have an even number always.     */

/* ----------------- Check if store is legal (fits in memory)                */
  if ( (LOCATION + HALF_BYTES/HEX_CHAR_PER_BYTE_1 <= MEM_SIZE_1) 
      && (LOCATION + HALF_BYTES/HEX_CHAR_PER_BYTE_1 >= 0)) {
    int I;

/* ------ Get what is stored there                                           */
    for (I = 0; I < HALF_BYTES/HEX_CHAR_PER_BYTE_1; I ++)
      INT_MEM_VAL = INT_MEM_VAL*256 + INT(MEMORY[LOCATION+I]);

/* ------ Add the NUMber to what was there                                   */
    INT_MEM_VAL += NUM;

/* ------ Put the result back                                                */
    NUM_TO_STR(INT_MEM_VAL,16,HALF_BYTES,CHAR_MEM_VAL);
    DO_STORE(CHAR_MEM_VAL,HALF_BYTES/HEX_CHAR_PER_BYTE_1, LOCATION,
	     MEMORY, ERROR,FALSE_1);
  } else {
    (void) printf("ERROR: Illegal store[2] ADDRESS = %d, BYTES = %d.\n",
		  LOCATION,HALF_BYTES/HEX_CHAR_PER_BYTE_1);
    (*ERROR) = TRUE_1;
  }
}

/* ============================ Outputting routines ======================== */

/* ------------------------ START_OF_LINE_ADDR (local) --------------------- */
/* Returns smallest address that is output on the same line as ADDRESS.      */
int START_OF_LINE_ADDR(int ADDRESS)
{
  return ( ((int) ADDRESS / (BYTES_PER_GROUP * GROUPS_PER_LINE)) *
	  BYTES_PER_GROUP * GROUPS_PER_LINE);
}

/* ------------------------- END_OF_LINE_ADDR (local) ---------------------- */
/* Returns largest address that is output on the same line as ADDRESS.       */
int END_OF_LINE_ADDR(int ADDRESS)
{
  return (START_OF_LINE_ADDR(ADDRESS) + BYTES_PER_GROUP * GROUPS_PER_LINE - 1);
}

/* --------------------------- LINES_OF_GAP (local) ------------------------ */
/* Number of lines (of output) of bytes that seperate the two blocks.        */
int LINES_OF_GAP(struct BUFFER_ELEMENT *REC1,struct BUFFER_ELEMENT *REC2)
{
  int REC1_END;                 /* Smallest address displayed on an output   */
                                /* line that contains last byte of REC1.     */
  int REC2_START;               /* Smallest address displayed on an output   */
                                /* line that contains first byte of REC2.    */

  if (REC1 == NULL) REC1_END = 0;
  else REC1_END = START_OF_LINE_ADDR( (*REC1).START + (*REC1).LENGTH - 1);

  if (REC2 == NULL) REC2_START = START_OF_LINE_ADDR(MEM_SIZE_1 - 1);
  else REC2_START = START_OF_LINE_ADDR( (*REC2).START);

  return ( (REC2_START - REC1_END) / (BYTES_PER_GROUP * GROUPS_PER_LINE) );
}

/* -------------------------- PRINT_ELIPSE (local) ------------------------- */
/* Elipse out portion of the output, if the gap between this block of memory */
/* and previous block is large enough.                                       */
void PRINT_ELIPSE(struct BUFFER_ELEMENT *PREV,struct BUFFER_ELEMENT *CURR,
                  int PREV_ADDR,FILE *OUTPUT)
{
  int LOOP_COUNTER;
/* ---------- STR_ADDR               String rep of addr.                     */
  char STR_ADDR[MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1 + 1];
  

/* -------------- Check if gap is large enough                               */
  if ( LINES_OF_GAP(PREV,CURR) >= LINES_OF_GAP_TO_ELIPSE ) {

    NUM_TO_STR(PREV_ADDR,16,MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1, STR_ADDR);
    (void) fprintf(OUTPUT,
		   " %s    xxxxxxxx  xxxxxxxx  xxxxxxxx  xxxxxxxx\n",
		   STR_ADDR);

    for (LOOP_COUNTER = 1;LOOP_COUNTER <= 3; LOOP_COUNTER++)
      (void) fprintf(OUTPUT,
		     "   .          .         .         .         .\n");
    
    if (CURR != NULL) 
      NUM_TO_STR(START_OF_LINE_ADDR( (*CURR).START)-
		 BYTES_PER_GROUP*GROUPS_PER_LINE, 
		 16,MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1, STR_ADDR);
    else 
      NUM_TO_STR(START_OF_LINE_ADDR( MEM_SIZE_1) -
		 BYTES_PER_GROUP*GROUPS_PER_LINE, 
		 16,MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1, STR_ADDR);

    (void) fprintf(OUTPUT,
		   " %s    xxxxxxxx  xxxxxxxx  xxxxxxxx  xxxxxxxx\n",
		   STR_ADDR);
  }
}

/* ----------------------------- PRINT_MEM --------------------------------- */
/* Output the loaded areas of memory in a nice user readable way.            */
void PRINT_MEM(MEM_SPACE MEMORY,FILE *OUTPUT)
{
  struct BUFFER_ELEMENT *STEP;     /* Current block of memory outputing.     */
  struct BUFFER_ELEMENT *PREVIOUS = NULL; /* Last block output               */
  int PREVIOUS_ADDR = 0;           /* Start address of the last output block */
  int ADDRESS;                     /* Next address of memory to output.      */
  char STR_ADDR[MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1 + 1];/* String version of*/
                                   /*                     address            */

  (void) fprintf(OUTPUT,"MEMORY\n");
  (void) fprintf(OUTPUT,"ADDRESS                  Contents\n");
  (void) fprintf(OUTPUT,"-------   --------------------------------------\n");
/*                        12345    12345678  12345678  12345678  12345678 */
/*                       123456789012345678901234567890123456789012345678 */

  if (LOCATIONS_USED.HEAD == NULL) 
    (void) fprintf(OUTPUT,
		   "=========> NOTHING LOADED INTO MEMORY <=========\n");
  else {

/* --------------- STEP through the list of loaded blocks                    */
    for (STEP = LOCATIONS_USED.HEAD; STEP != NULL; STEP = (*STEP).NEXT) {

      PRINT_ELIPSE(PREVIOUS,STEP,PREVIOUS_ADDR,OUTPUT);
      ADDRESS = START_OF_LINE_ADDR((*STEP).START);

/* ------ repeat until consequtive blocks are output on seperate lines.      */
      do {
	BOOLEAN REST_EMPTY = FALSE_1;
	int I,J;
	char STR_BYTE[HEX_CHAR_PER_BYTE_1 + 1];

	NUM_TO_STR(ADDRESS,16,MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1,STR_ADDR);
	(void) fprintf(OUTPUT," %s    ",STR_ADDR);
	
/* --------- OUTPUT 1 line                                                   */
	for (I=0;I<GROUPS_PER_LINE;I++) {
	  for (J=0;J<BYTES_PER_GROUP;J++) {
	    int LOC;

	    LOC = ADDRESS+I*BYTES_PER_GROUP+J;
	    if (!REST_EMPTY && (LOC >= ((*STEP).START + (*STEP).LENGTH)))
	      {
		if ( ((*STEP).NEXT == NULL) ||
		    (LINES_OF_GAP(STEP,(*STEP).NEXT) > 0)) REST_EMPTY = TRUE_1;
		else STEP = (*STEP).NEXT;
	      }

	    if ( REST_EMPTY || (LOC < (*STEP).START))
	      (void) fprintf(OUTPUT,"xx");
	    else {
	      NUM_TO_STR(INT(MEMORY[LOC]),16,HEX_CHAR_PER_BYTE_1,STR_BYTE);
	      (void) fprintf(OUTPUT,"%s",STR_BYTE);
	    }
	  }
	  (void) fprintf(OUTPUT,"  ");
	}
	(void) fprintf(OUTPUT,"\n");

	ADDRESS = END_OF_LINE_ADDR(ADDRESS) + 1;
      } while (ADDRESS <= 
	       END_OF_LINE_ADDR( (*STEP).START + (*STEP).LENGTH - 1) );
      PREVIOUS_ADDR = ADDRESS;
      PREVIOUS = STEP;
    }
    PRINT_ELIPSE(PREVIOUS,STEP,PREVIOUS_ADDR,OUTPUT);
  }
}

/* ---------------------------- OUTPUT_MEM --------------------------------- */
/* Prints out the loaded areas of memory in a machine friendly format. Very  */
/* to the text records of the object stream except relocated and modified.   */
void OUTPUT_MEM(MEM_SPACE MEMORY,FILE *OUTPUT)
{ 
  struct BUFFER_ELEMENT *CURRENT; /* Current block outputing                 */
  int COUNT;                   /* Simple loop counter                        */
  char ADDRESS[MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1 + 2];/* String version of */
                               /*                        address             */
  char STR_BYTE[HEX_CHAR_PER_BYTE_1 + 1]; /* String version of byte          */

  CURRENT = LOCATIONS_USED.HEAD;
  while (CURRENT != NULL) {
    NUM_TO_STR((*CURRENT).START,16,MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1+1,
	       ADDRESS);
    NUM_TO_STR((*CURRENT).LENGTH,16,HEX_CHAR_PER_BYTE_1,STR_BYTE);
    (void) fprintf(OUTPUT,"T%s%s",ADDRESS,STR_BYTE);

    for (COUNT = (*CURRENT).START; 
	 COUNT < ((*CURRENT).START + (*CURRENT).LENGTH);
	 COUNT ++) {
      NUM_TO_STR(INT(MEMORY[COUNT]),16,HEX_CHAR_PER_BYTE_1,STR_BYTE);
      (void) fprintf(OUTPUT,"%s",STR_BYTE);
    }
    (void) fprintf(OUTPUT,"\n");
    CURRENT = (*CURRENT).NEXT;
  }
}
