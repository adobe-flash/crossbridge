/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* =============================== format.c ================================ */
/* Returns the registers associtiated with a FORMAT2 instruction (handles    */
/* BITS 8-15) and the Address or value associated with a FORMAT3/FORMAT4     */
/* instruction (handles BITS 6-23 or BITS 6-31).                             */

#include <math.h>
#include "boolean.h"
#include "memory.h"
#include "machine.h"
#include "convert.h"
#include "constants.h"
#include "instruct.h"
#include "interupt.h" 


/* ------------------------------ FORMAT2 ---------------------------------- */
/* Handles bits 8-15 of format 2 instruction. Puts bits 8-11 in REG1, bits   */
/* 12-15 in REG2 and updates PC.                                             */
void FORMAT2(int *REG1,int *REG2)
{
  (*REG1) = INT(MEMORY[PC_0+1]) / 16;
  (*REG2) = INT(MEMORY[PC_0+1]) - (*REG1) * 16;
  PC_0 += 2;
}


/* ----------------------------- FORMAT3_4 --------------------------------- */
/* Handles bits 6-23 for format 3 instructions, and bits 5-31 for format 4   */
/* instructions. Updates PC and returns ADDRESS or VALUE of the operand      */
/* Depending on demand.                                                      */
int FORMAT3_4(int DEMAND,int BYTES)
{
  int INSTR[4*BITS_PER_BYTE_2];   /* BIT for of the instruction.             */

  BOOLEAN RANGE_ERROR = FALSE_1;  /* TRUE iff an illegal address is found.   */
  int FORMAT = 3;                 /* Format 3 or format 4 instruction.       */
  int ADDRESS;                    /* Address specified by instruction.       */
  int VALUE;                      /* Value specified by instruction          */

  if (PC_0+2 >= MEM_SIZE_1) RANGE_ERROR = TRUE_1;
    else {
      BYTES_TO_BITS(INT(MEMORY[PC_0]),1,INSTR);
      BYTES_TO_BITS(INT(MEMORY[PC_0+1]),1,&(INSTR[8]));
      BYTES_TO_BITS(INT(MEMORY[PC_0+2]),1,&(INSTR[16]));

/* ------------------ if expended format bit on and not plane SIC -----------*/
      if ((INSTR[11] == 1) && !((INSTR[6] == 0) && (INSTR[7] == 0)))
	if (PC_0+3 >= MEM_SIZE_1) RANGE_ERROR = TRUE_1;
	else {
	  BYTES_TO_BITS(INT(MEMORY[PC_0+3]),1,&(INSTR[24]));
	  FORMAT = 4;
	}
    }

/* ------------ Check for Address out of range error                         */
  if (RANGE_ERROR) {
    SIGNAL_INTERUPT(2,02);
    return (0);
  }

  PC_0 += FORMAT;
/* ------- if plane SIC ---------------------------------------------------- */
  if ((INSTR[6] == 0) && (INSTR[7] == 0)) 
    ADDRESS = BITS_TO_BYTE(&(INSTR[12]),12);
  else if (FORMAT == 3) {
    ADDRESS = BITS_TO_BYTE(&(INSTR[12]),12);
/* ------------ Check for Illegal instruction                                */
    if ((INSTR[9] == 1) && (INSTR[10] == 1)) {
      SIGNAL_INTERUPT(2,00);
      return (0);
    }
    if (INSTR[8] == 1) ADDRESS += X_0;
    if (INSTR[9] == 1) ADDRESS += B_0;
    if (INSTR[10] == 1) {
      if (ADDRESS >= MAX_PC_RELATIVE_1 / 2)
	ADDRESS = (ADDRESS - MAX_PC_RELATIVE_1) + PC_0;
	else ADDRESS += PC_0;
    }
  } else {
/* ~~~~~~~~~~~~~~~~~~~~~~ FORMAT 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ------------ Check for Illegal instruction                                */
      if ((INSTR[9] == 1) || (INSTR[10] == 1)) {
	SIGNAL_INTERUPT(2,00); 
	return (0);
      }
      ADDRESS = BITS_TO_BYTE(&(INSTR[12]),20);
      if (INSTR[8] == 1) ADDRESS += X_0;
    }
  
/* ~~~~~~~~~~~~~~~~~~~~~~~ DIRECT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  if ((INSTR[6] == 1) && (INSTR[7] == 1)) {

    if (DEMAND == VALUE_1) {
      int I;

/* ------------ Check for Illegal instruction                                */
      if ((ADDRESS < 0) || (ADDRESS+2 >= MEM_SIZE_1)) {
	SIGNAL_INTERUPT(2,00);
	return (0);
      }

      VALUE = 0;
      for (I = 0; I < BYTES; I ++)
	VALUE = VALUE * 256 + INT(MEMORY[ADDRESS + I]);

/* -------------- If unitialized value that will not fit in SIC/XE make it 0 */
      if (VALUE >= (int) pow(2.0,BITS_PER_WORD_1*1.0)) VALUE = 0;
      return VALUE;
    } else return ADDRESS;
  }

/* ~~~~~~~~~~~~~~~~~~~~~~~ INDIRECT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  if ((INSTR[6] == 1) && (INSTR[7] == 0)) {
/* ------------ Check for Illegal instruction                                */
    if ((ADDRESS < 0) || (ADDRESS+2 >= MEM_SIZE_1)) {
      SIGNAL_INTERUPT(2,00);
      return (0);
    }
    ADDRESS = (((INT(MEMORY[ADDRESS]) * 256) + INT(MEMORY[ADDRESS+1]))*256 +
	    INT(MEMORY[ADDRESS+2]));

    if (DEMAND == VALUE_1) {
      int I;

/* ------------ Check for Address out of range error                         */
      if (ADDRESS > MEM_SIZE_1) {
	SIGNAL_INTERUPT(2,02);
	return (0);
      }

      VALUE = 0;
      for (I = 0; I < BYTES; I ++)
	VALUE = VALUE * 256 + INT(MEMORY[ADDRESS + I]);

/* -------------- If unitialized value that will not fit in SIC/XE make it 0 */
      if (VALUE >= (int) pow(2.0,BITS_PER_WORD_1*1.0)) VALUE = 0;
      return VALUE;
    } else return ADDRESS;
  }

/* ~~~~~~~~~~~~~~~~~~~~~~~ Immediate ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  if ((INSTR[6] == 0) && (INSTR[7] == 1)) {
/* ------------ Check for Illegal instruction                                */
    if (ADDRESS >= (int) pow(2.0,BITS_PER_WORD_1*1.0)) {
      SIGNAL_INTERUPT(2,00);
      return (0);
    }
    return (ADDRESS);
  }
  return(0); /* This will never be executed, but to keep lint happy          */
}
