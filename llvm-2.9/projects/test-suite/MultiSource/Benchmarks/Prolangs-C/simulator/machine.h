/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================= machine.h ================================= */
/* Maintains all the information associated with the machine. Can be used to */
/* drive complete simulation of a program or just execute one instruction.   */
#include <stdio.h>
#include "sim_debug.h"


/* ---------------------------------- Globals ------------------------------ */
/* MEMORY                            The Main Memory.                        */
extern MEM_SPACE MEMORY;

/* INTERVAL_TIMER                    Used for timing out a process and      */
extern int INTERVAL_TIMER;        /* Returning control to the OS            */

/* ---------------------------- The Registers ----------------------------- */
extern int REGISTER[NUM_REGISTERS];
#define A_0       REGISTER[0]          /* 0     A       Accumulator         */
#define X_0       REGISTER[1]          /* 1     X       Index Register      */
#define L_0       REGISTER[2]          /* 2     L       Linkage Register    */
#define B_0       REGISTER[3]          /* 3     B       Base Register       */
#define S_0       REGISTER[4]          /* 4     S       General Register    */
#define T_0       REGISTER[5]          /* 5     T       General Register    */
#define F_0       REGISTER[6]          /* 6     F       Floating Point - NOT*/
                                       /*                   IMPLEMENTED     */
                                       /* 7             NO REGISTER         */
#define PC_0      REGISTER[8]          /* 8     PC      Program Counter     */
#define SW_0      REGISTER[9]          /* 9     SW      Status Word         */

  
/* ------------------------------ The Devices ------------------------------ */
#define NOT_IN_USE_1    0
#define IN_USE_1        1
/* The devices:                                                              */
/*    0                         Keyboard (Input)                             */
/*    80                        Terminal (Output)                            */
/*    i (00 <= i <= FF)         File named DEVi                              */
/*    I (00 <= I < 80) Input Devices.      I (80 <= I <= FF) Output Devices. */
extern struct DEVICE_ENTRY {
    int STATUS;
    FILE *STREAM;
  } DEVICE[256];

/*---------------------------------- Procedures ---------------------------- */

/* --------------------------- RESET_DEVICES ------------------------------- */
/* 0 parameters:                                                             */
/* Reset all the devices. This is needed so that if you run program twice    */
/* in one debugging section, you can be sure the devices will be okay.       */
extern void RESET_DEVICES();

/* --------------------------- PRINT_REG_NAME ------------------------------ */
/* 3 parameters:                                                             */
/*   1) char *STR1;                                                          */
/*   2) int REG_NUM;              The number of the register.                */
/*   3) char *STR2;                                                          */
/* Prints out STR1, followed by the name of a register, followed by STR2.    */
extern void PRINT_REG_NAME();

/* ----------------------------- BYTES_TO_BITS ----------------------------- */
/* 3 parameters:                                                             */
/*   1) int VAL;                  VALue to convert to bits.                  */
/*   2) int NUM_BYTES;            How many BYTES to convert.                 */
/*   3) int *BINARY;              Where to put the bits.                     */
/* Convert BYTES bytes store in VAL (the variable VAL) and set BINARY to be  */
/* the corresponding binary array of BITS.                                   */
extern void BYTES_TO_BITS();

/* -------------------------- BITS_TO_BYTE --------------------------------- */
/* 2 parameters:                                                             */
/*   1) int *BINARY;             array of bits to convert.                   */
/*   2) int BITS;                number of BITS in the array.                */
/* Convert an array of BITS bits to an integer (BYTE) and return result.     */
extern int BITS_TO_BYTE();

/* --------------------------- SET_CC -------------------------------------- */
/* 1 parameter:                                                              */
/*   1) int CODE;                Set the CC to CODE.                         */
/* SET the CC (Condition Code) to CODE                                       */
extern void SET_CC();

/* ---------------------------------- CC ----------------------------------- */
/* 0 parameters:                                                             */
/* Return the value stored in the condition code.                            */
extern int CC();

/* ---------------------------- SUPERVISOR_MODE ---------------------------- */
/* 0 parameters:                                                             */
/* Return TRUE iff in SUPERVISOR MODE.                                       */
extern BOOLEAN SUPERVISOR_MODE();

/* --------------------------- OPEN_DEVICE --------------------------------- */
/* 1 parameter:                                                              */
/*   1) int DEV;                                                             */
/* Mark device DEV as IN USE and, if needed, open it.                        */
extern void OPEN_DEVICE();

/* ---------------------------- PRINT_ADDRESS ------------------------------ */
/* 3 parameters:                                                             */
/*   1) char *STR1;                                                          */
/*   2) int ADDRESS;                   ADDRESS to output.                    */
/*   3) char *STR2;                                                          */
/* Output PREFIX, followed by the ADDRESS, followed by SUFFIX. Print the     */
/* Address in hexidecimal, decimal or by a LABEL name.                       */
extern void PRINT_ADDRESS();

/* ---------------------------- PRINT_CONSTANT ----------------------------- */
/* 3 parameters:                                                             */
/*   1) char *STR1;                                                          */
/*   2) int CONST;                                                           */
/*   3) char *STR2;                                                          */
/* Output PREFIX, followed by the CONSTANT in a base depending on            */
/* TYPE_OUT_MODE, followed by SUFFIX                                         */
extern void PRINT_CONSTANT();

/* ------------------------- PRINT_INSTRUCTION ----------------------------- */
/* 3 parameters:                                                             */
/*   1) int LOCATION;               Where the word in located in memory.     */
/*   2) BOOLEAN PRINT;              Do you really want to print out          */
/*                                    instruction or just want BYTES?        */
/*   3) int *BYTES;                 Number of bytes this instruction has     */
/* Output a number as a SIC/XE instruction.                                  */
extern void PRINT_INSTRUCTION();

/* ---------------------------------- EXEC --------------------------------- */
/* 2 parameters:                                                             */
/*   1) BOOLEAN DEBUG_MODE;     TRUE, debugging/simulating; FALSE simulating */
/*   2) BOOLEAN PRINT_EFFECT;   Print the effect of the commands (TRUE/FALSE)*/
/* if DEBUG_MODE is FALSE, drives the whole simulation process, otherwise    */
/* Just does one instruction and returns.                                    */
extern void EXEC();

