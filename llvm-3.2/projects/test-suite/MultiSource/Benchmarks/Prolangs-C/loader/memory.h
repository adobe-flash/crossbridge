/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== memory.h ================================= */
/* Contains all the routines that manage the main memory of the SIC/XE       */
/* machine.                                                                  */

/* MEM_SPACE                     The type of a main memory in this program.  */
typedef char *MEM_SPACE;

/* --------------------------------- INT  ---------------------------------- */
/* 1 parameter:                                                              */
/*   1)  char CH;               Character to convert.                        */
/* Return the integer representation for a character. Had to be written      */
/* Because the character 255 (for example) who be treated as the integer -1, */
/* but I needed it to be 255.                                                */
extern int INT();

/* ----------------------------- Create Memory ----------------------------- */
/* 1 parameter:                                                              */
/*    1) MEM_SPACE *MEMORY;          name of the main memory to create.      */
/* Create a megabyte of memory and point MEMORY to it. It does not initialize*/
/* the memory as this took about 5 minuites. Since the memory was            */
/* uninitized I needed another way to know when locations where modified,    */
/* thus the above lined list of blocks.                                      */
extern void CREATE_MEMORY();

/* ------------------------------ STORE_AT --------------------------------- */
/* 5 parameters:                                                             */
/*    1) char *VALUE;            Character representation (2 chars/byte) of  */
/*                                 what the block of memory should contain.  */
/*    2) int BYTES;              bytes in the block to load into memory.     */
/*    3) int ADDRESS;            Where the block begins in memory.           */
/*    4) MEM_SPACE MEMORY;       The main memory.                            */
/*    5) BOOLEAN *ERROR;         Was an error detected during the store.     */
/* Place BYTES consequative bytes into MEMORY, starting at ADDRESS.          */
extern void STORE_AT();

/* ---------------------------- ADD_INT_TO_LOC ----------------------------- */
/* 5 parameters:                                                             */
/*    1) int NUM;                NUMber to add to the value already in memory*/
/*    2) int LOCATION;           Address of the integer in memory            */
/*    3) int HALF_BYTES;         How many HALF_BYTES does the number occupy  */
/*    4) MEM_SPACE MEMORY;       The memory.                                 */
/*    5) BOOLEAN *ERROR;         Has an error been detected.                 */
/* Add the integer NUM to what is store at LOCATION in MEMORY.               */
extern void ADD_INT_TO_LOC();

/* ----------------------------- PRINT_MEM --------------------------------- */
/* 2 parameters:                                                             */
/*    1) MEM_SPACE MEMORY;              Memory to output.                    */
/*    2) FILE *OUTPUT;                  Output stream.                       */
/* Output the loaded areas of memory in a nice user readable way.            */
extern void PRINT_MEM();

/* ---------------------------- OUTPUT_MEM --------------------------------- */
/* 2 parameters:                                                             */
/*    1) MEM_SPACE MEMORY;              Memory to output.                    */
/*    2) FILE *OUTPUT;                  Output stream.                       */
/* Prints out the loaded areas of memory in a machine friendly format. Very  */
/* to the text records of the object stream except relocated and modified.   */
extern void OUTPUT_MEM();
