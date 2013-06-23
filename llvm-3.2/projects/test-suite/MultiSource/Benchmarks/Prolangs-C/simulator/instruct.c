/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* =============================== intruction.c ============================ */
/* Contains all the procedures that know how to handle FORMAT3/4 instructions*/
/* Name of the procedure is the same as the SIC/XE instruction name but with */
/* a '_P' added to the end. See p. 410-411 of "System Software: An           */
/* Introduction to Systems Programming" by Leland L. Beck for detailed       */
/* Description to the function of each command.                              */

#include <math.h>
#include "boolean.h"
#include "debugger.h"
#include "memory.h"
#include "machine.h"
#include "convert.h"
#include "format.h"
#include "interupt.h"

#define MAX_BYTE_VAL_0            256

/* ---------------------------------- ADD_P -------------------------------- */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
void ADD_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;                  /* Value specified by the parameter.           */
  BOOLEAN OVERFLOW1 = FALSE_1; /* Has an overflow been detected.             */
  int ASIGN = 1;              /* Sign of Accumulator                         */
  int VALSIGN = 1;            /* Sign of VALUE                               */

  VALUE = FORMAT3_4(VALUE_1,3);

/* Check if VALUE if an impossible (in SIC/XE) unitialized value             */
  if ((VALUE < 0) || (VALUE >= MAX_INT_1)) VALUE = 0;

/* --------------- This is needed to correctly dectect overflow ------------ */
  if (VALUE >= MAX_INT_1 / 2) VALSIGN = -1;
  if (A_0 >= MAX_INT_1 / 2) ASIGN = -1;
    
  if ((ASIGN == 1) && (VALSIGN == 1)) {
    A_0 = A_0 + VALUE;
    if (A_0 >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
  }
  if ((ASIGN == -1) && (VALSIGN == 1)) {
    A_0 = A_0 + VALUE;
    A_0 = A_0 - ((int) A_0 / MAX_INT_1) * MAX_INT_1;
  }
  if ((ASIGN == 1) && (VALSIGN == -1)) {
    A_0 = A_0 + VALUE;
    A_0 = A_0 - ((int) A_0 / MAX_INT_1) * MAX_INT_1;
  }
  if ((ASIGN == -1) && (VALSIGN == -1)) {
    A_0 = A_0 + VALUE;
    A_0 = A_0 - ((int) A_0 / MAX_INT_1) * MAX_INT_1;
    if (A_0 < MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
  }

  if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");
  if (OVERFLOW1) SIGNAL_INTERUPT(2,04);
}

/* --------------------------------- AND_P --------------------------------- */
/* Bitwise AND of 2 words.                                                   */
void AND_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;                  /* Value specified by the parameter.           */
  int COUNT;                  /* Loop counter                                */

  int A_BITS[BITS_PER_WORD_1]; /* BIT form of A                              */
  int VAL_BITS[BITS_PER_WORD_1]; /* BIT form of VALUE                        */

  VALUE = FORMAT3_4(VALUE_1,3);

/* Check if VALUE if an impossible (in SIC/XE) unitialized value             */
  if ((VALUE < 0) || (VALUE >= MAX_INT_1)) VALUE = 0;

  BYTES_TO_BITS(A_0,3,A_BITS);
  BYTES_TO_BITS(VALUE,3,VAL_BITS);

  for (COUNT = 0; COUNT < BITS_PER_WORD_1; COUNT ++)
    A_BITS[COUNT] = A_BITS[COUNT] && VAL_BITS[COUNT];

  A_0 = BITS_TO_BYTE(A_BITS,BITS_PER_WORD_1);

  if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");
}

/* --------------------------------- COMP_P -------------------------------- */
void COMP_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;
  int A_VAL;

  VALUE = FORMAT3_4(VALUE_1,3);

  if (A_0 >= MAX_INT_1 / 2) A_VAL = A_0 - MAX_INT_1;
  else A_VAL = A_0;

  if (VALUE >= MAX_INT_1 / 2) VALUE = VALUE - MAX_INT_1;

  if (A_VAL == VALUE) {
    SET_CC(EQUAL_1);
    if (PRINT_EFFECT) (void) printf("Set CC to =.\n");
  }
  if (A_VAL < VALUE) {
    SET_CC(LESS_THAN_1);
    if (PRINT_EFFECT) (void) printf("Set CC to <.\n");
  }
  if (A_VAL > VALUE) {
    SET_CC(GREATER_THAN_1);
    if (PRINT_EFFECT) (void) printf("Set CC to >.\n");
  }
}

/* ---------------------------------- DIV_P -------------------------------- */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments.                                                            */
void DIV_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;                  /* Value specified by the parameter.           */
  int ASIGN = 1;              /* Sign of Accumulator                         */
  int VALSIGN = 1;            /* Sign of VALUE                               */

  VALUE = FORMAT3_4(VALUE_1,3);

/* Check if VALUE if an impossible (in SIC/XE) unitialized value             */
  if ((VALUE < 0) || (VALUE >= MAX_INT_1)) VALUE = 1;

  if (VALUE == 0) {
    SIGNAL_INTERUPT(2,04);
    if (PRINT_EFFECT) (void) printf("Attempted division by zero.\n");
  } else {
/* --------------- This is needed to correctly dectect overflow ------------ */
    if (VALUE >= MAX_INT_1 / 2) VALSIGN = -1;
    if (A_0 >= MAX_INT_1 / 2) ASIGN = -1;
    
    if ((ASIGN == 1) && (VALSIGN == 1)) A_0 = A_0 / VALUE;
    if ((ASIGN == -1) && (VALSIGN == 1)) 
      A_0 = MAX_INT_1 - ((MAX_INT_1 - A_0) / VALUE);
    if ((ASIGN == 1) && (VALSIGN == -1))
      A_0 = MAX_INT_1 - (A_0 / (MAX_INT_1 - VALUE));
    if ((ASIGN == -1) && (VALSIGN == -1)) 
      A_0 = (MAX_INT_1 - A_0) / (MAX_INT_1 - VALUE);
    
    if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");
  }
}

/* ----------------------------------- J_P --------------------------------- */
void J_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(LOCATION_1,3);

  if ((VALUE < 0) || ((VALUE >= MEM_SIZE_1) && VALUE != RETURN_TO_OS_1)) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("J m, m is an illegal address.\n");
  } else {
    PC_0 = VALUE;
    if (PRINT_EFFECT)
      if (PC_0 == RETURN_TO_OS_1)
	(void) printf("Returning to Operating System.\n");
      else 
	PRINT_ADDRESS("Jump to ",PC_0,"\n");
  }
}

/* ---------------------------------- JEQ_P -------------------------------- */
void JEQ_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(LOCATION_1,3);

  if (CC() != EQUAL_1) {
    if (PRINT_EFFECT) (void) printf("No action taken.\n");
  } else 
    if ((VALUE < 0) || ((VALUE >= MEM_SIZE_1) && VALUE != RETURN_TO_OS_1)) {
      SIGNAL_INTERUPT(2,02);    
      if (PRINT_EFFECT) (void) printf("JEQ m, m is an illegal address.\n");
    } else {
      PC_0 = VALUE;
      if (PRINT_EFFECT)
	if (PC_0 == RETURN_TO_OS_1)
	  (void) printf("Returning to Operating System.\n");
	else 
	  PRINT_ADDRESS("Jump to ",PC_0,"\n");
    }
}

/* -------------------------------- JGT_P ---------------------------------- */
void JGT_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(LOCATION_1,3);

  if (CC() != GREATER_THAN_1) {
    if (PRINT_EFFECT) (void) printf("No action taken.\n");
  } else 
    if ((VALUE < 0) || ((VALUE >= MEM_SIZE_1) && VALUE != RETURN_TO_OS_1)) {
      SIGNAL_INTERUPT(2,02);    
      if (PRINT_EFFECT) (void) printf("JGT m, m is an illegal address.\n");
    } else {
      PC_0 = VALUE;
      if (PRINT_EFFECT)
	if (PC_0 == RETURN_TO_OS_1)
	  (void) printf("Returning to Operating System.\n");
	else 
	  PRINT_ADDRESS("Jump to ",PC_0,"\n");
    }
}

/* ------------------------------- JLT_P ----------------------------------- */
void JLT_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(LOCATION_1,3);

  if (CC() != LESS_THAN_1) {
    if (PRINT_EFFECT) (void) printf("No action taken.\n");
  } else 
    if ((VALUE < 0) || ((VALUE >= MEM_SIZE_1) && VALUE != RETURN_TO_OS_1)) {
      SIGNAL_INTERUPT(2,02);    
      if (PRINT_EFFECT) (void) printf("JLT m, m is an illegal address.\n");
    } else {
      PC_0 = VALUE;
      if (PRINT_EFFECT)
	if (PC_0 == RETURN_TO_OS_1)
	  (void) printf("Returning to Operating System.\n");
	else 
	  PRINT_ADDRESS("Jump to ",PC_0,"\n");
    }
}

/* --------------------------------- JSUB_P -------------------------------- */
void JSUB_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(LOCATION_1,3);

  if ((VALUE < 0) || (VALUE >= MEM_SIZE_1)) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("JSUB m, m is an illegal address.\n");
  } else {
    L_0 = PC_0;
    PC_0 = VALUE;
    if (PRINT_EFFECT) {
      PRINT_ADDRESS("Put ",L_0," into L and jump to ");
      PRINT_ADDRESS("",PC_0,".\n");
    }
  }
}

/* ------------------------------- LDA_P ----------------------------------- */
void LDA_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(VALUE_1,3);
  A_0 = VALUE;
  if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");

}

/* ------------------------------- LDB_P ----------------------------------- */
void LDB_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(VALUE_1,3);
  B_0 = VALUE;
  if (PRINT_EFFECT) PRINT_CONSTANT("Set B to ",B_0,".\n");

}

/* ------------------------------- LDCH_P ---------------------------------- */
void LDCH_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(VALUE_1,1);
  A_0 = ((int) A_0 / MAX_BYTE_VAL_0) * MAX_BYTE_VAL_0 + VALUE;
  if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");

}

/* ------------------------------- LDL_P ----------------------------------- */
void LDL_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(VALUE_1,3);
  L_0 = VALUE;
  if (PRINT_EFFECT) PRINT_CONSTANT("Set L to ",L_0,".\n");

}

/* ------------------------------- LDS_P ----------------------------------- */
void LDS_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(VALUE_1,3);
  S_0 = VALUE;
  if (PRINT_EFFECT) PRINT_CONSTANT("Set S to ",S_0,".\n");

}

/* ------------------------------- LDT_P ----------------------------------- */
void LDT_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(VALUE_1,3);
  T_0 = VALUE;
  if (PRINT_EFFECT) PRINT_CONSTANT("Set T to ",T_0,".\n");

}

/* ------------------------------- LDX_P ----------------------------------- */
void LDX_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;

  VALUE = FORMAT3_4(VALUE_1,3);
  X_0 = VALUE;
  if (PRINT_EFFECT) PRINT_CONSTANT("Set X to ",X_0,".\n");

}

/* ---------------------------------- MUL_P -------------------------------- */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
void MUL_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;                  /* Value specified by the parameter.           */
  BOOLEAN OVERFLOW1 = FALSE_1; /* Has an overflow been detected.             */
  int ASIGN = 1;              /* Sign of Accumulator                         */
  int VALSIGN = 1;            /* Sign of VALUE                               */
  double STORE_A;             /* So the simulator does not overflow          */

  VALUE = FORMAT3_4(VALUE_1,3);

/* Check if VALUE if an impossible (in SIC/XE) unitialized value             */
  if ((VALUE < 0) || (VALUE >= MAX_INT_1)) VALUE = 0;

/* --------------- This is needed to correctly dectect overflow ------------ */
  if (VALUE >= MAX_INT_1 / 2) VALSIGN = -1;
  if (A_0 >= MAX_INT_1 / 2) ASIGN = -1;
    
  if ((ASIGN == 1) && (VALSIGN == 1)) {
    STORE_A = (A_0*1.0) * VALUE;
    if (STORE_A >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    else A_0 = (int) STORE_A;
  }
  if ((ASIGN == -1) && (VALSIGN == 1)) {
    STORE_A = 1.0 * (MAX_INT_1 - A_0) * VALUE;
    STORE_A = STORE_A - ((int) (STORE_A / MAX_INT_1)) * MAX_INT_1;
    if (STORE_A >  MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    else A_0 = MAX_INT_1 - (int) STORE_A;
  }
  if ((ASIGN == 1) && (VALSIGN == -1)) {
    STORE_A = 1.0 * A_0 * (MAX_INT_1 - VALUE);
    STORE_A = STORE_A - ((int) (STORE_A / MAX_INT_1)) * MAX_INT_1;
    if (STORE_A >  MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    else A_0 = MAX_INT_1 - (int) STORE_A;
  }
  if ((ASIGN == -1) && (VALSIGN == -1)) {
    STORE_A = 1.0 *(MAX_INT_1 - A_0) * (MAX_INT_1 - VALUE);
    if (STORE_A >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    else A_0 = (int) STORE_A;
  }

  if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");
  if (OVERFLOW1) SIGNAL_INTERUPT(2,04);
}

/* --------------------------------- OR_P ---------------------------------- */
/* BITwise OR of 2 words.                                                    */
void OR_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;                  /* Value specified by the parameter.           */
  int COUNT;                  /* Loop counter                                */

  int A_BITS[BITS_PER_WORD_1]; /* BIT form of A                              */
  int VAL_BITS[BITS_PER_WORD_1]; /* BIT form of VALUE                        */

  VALUE = FORMAT3_4(VALUE_1,3);

/* Check if VALUE if an impossible (in SIC/XE) unitialized value             */
  if ((VALUE < 0) || (VALUE >= MAX_INT_1)) VALUE = 0;

  BYTES_TO_BITS(A_0,3,A_BITS);
  BYTES_TO_BITS(VALUE,3,VAL_BITS);

  for (COUNT = 0; COUNT < BITS_PER_WORD_1; COUNT ++)
    A_BITS[COUNT] = A_BITS[COUNT] || VAL_BITS[COUNT];

  A_0 = BITS_TO_BYTE(A_BITS,BITS_PER_WORD_1);

  if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");;
}

/* ----------------------------------- RD_P -------------------------------- */
void RD_P(BOOLEAN PRINT_EFFECT)
{
  int DEV;

  DEV = FORMAT3_4(VALUE_1,1);
  if ((0 > DEV) || (DEV > 255)) {
    SIGNAL_INTERUPT(2,00);
    if (PRINT_EFFECT) (void) printf("RD illegal device.\n");
  } else {
    if ((DEVICE[DEV].STREAM == NULL) || (DEVICE[DEV].STATUS == NOT_IN_USE_1)
	|| (DEV >= 128)) {
      SIGNAL_INTERUPT(2,00);
      if (PRINT_EFFECT) (void) printf("RD illegal device.\n");
    } else {
      int INPUT;

      INPUT = fgetc(DEVICE[DEV].STREAM);
      if (INPUT == -1) INPUT = 0;
      A_0 = ((int) A_0 / MAX_BYTE_VAL_0) * MAX_BYTE_VAL_0 + INPUT;
      DEVICE[DEV].STATUS = NOT_IN_USE_1;
      if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");
    }
  }
}

/* ---------------------------------- SUB_P -------------------------------- */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
void SUB_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;                  /* Value specified by the parameter.           */
  BOOLEAN OVERFLOW1 = FALSE_1; /* Has an overflow been detected.              */
  int ASIGN = 1;              /* Sign of Accumulator                         */
  int VALSIGN = 1;            /* Sign of VALUE                               */

  VALUE = FORMAT3_4(VALUE_1,3);

/* Check if VALUE if an impossible (in SIC/XE) unitialized value             */
  if ((VALUE < 0) || (VALUE >= MAX_INT_1)) VALUE = 0;

/* --------------- This is needed to correctly dectect overflow ------------ */
  if (VALUE >= MAX_INT_1 / 2) VALSIGN = -1;
  if (A_0 >= MAX_INT_1 / 2) ASIGN = -1;
    
  if ((ASIGN == 1) && (VALSIGN == 1)) {
    A_0 = A_0 - VALUE;
    if (A_0 < 0) A_0 = MAX_INT_1 + A_0;
  }
  if ((ASIGN == -1) && (VALSIGN == 1)) {
    A_0 = A_0 + (MAX_INT_1 - VALUE);
    A_0 = A_0 - ((int) A_0 / MAX_INT_1) * MAX_INT_1;
    if (A_0 < MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
  }
  if ((ASIGN == 1) && (VALSIGN == -1)) {
    A_0 = A_0 + (MAX_INT_1 - VALUE);
    if (A_0 >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
  }
  if ((ASIGN == -1) && (VALSIGN == -1)) {
    A_0 = A_0 + (MAX_INT_1 - VALUE);
    A_0 = A_0 - ((int) A_0 / MAX_INT_1) * MAX_INT_1;
  }

  if (PRINT_EFFECT) PRINT_CONSTANT("Set A to ",A_0,".\n");
  if (OVERFLOW1) SIGNAL_INTERUPT(2,04);
}

/* ---------------------------------- RSUB_P ------------------------------- */
void RSUB_P(BOOLEAN PRINT_EFFECT) 
{
  if ((L_0 < 0) || ((L_0 >= MEM_SIZE_1) && L_0 != RETURN_TO_OS_1)) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("JLT m, m is an illegal address.\n");
  } else {
    PC_0 = L_0;
    if (PRINT_EFFECT)
      if (PC_0 == RETURN_TO_OS_1)
	(void) printf("RSUB to Operating System.\n");
      else 
	PRINT_ADDRESS("RSUB to ",PC_0,"\n");
  }
}

/* ---------------------------------- STA_P -------------------------------- */
void STA_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  ADDRESS = FORMAT3_4(LOCATION_1,3);
  if (ADDRESS+2 >= MEM_SIZE_1) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("STA m, m is an illegal address.\n");
  } else {
    MEMORY[ADDRESS] = A_0 / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
    MEMORY[ADDRESS+1] = (A_0 - 
      INT(MEMORY[ADDRESS]) * (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) 
      / MAX_BYTE_VAL_0;
    MEMORY[ADDRESS+2] = (A_0 - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);

    if (PRINT_EFFECT) {
      PRINT_CONSTANT("Put ",A_0," into ");
      PRINT_ADDRESS("",ADDRESS,".\n");
    }
  }
}

/* ---------------------------------- STB_P -------------------------------- */
void STB_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  ADDRESS = FORMAT3_4(LOCATION_1,3);
  if (ADDRESS+2 >= MEM_SIZE_1) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("STB m, m is an illegal address.\n");
  } else {
    MEMORY[ADDRESS] = B_0 / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
    MEMORY[ADDRESS+1] = (B_0 - 
      INT(MEMORY[ADDRESS]) * (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) 
      / MAX_BYTE_VAL_0;
    MEMORY[ADDRESS+2] = (B_0 - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);

    if (PRINT_EFFECT) {
      PRINT_CONSTANT("Put ",B_0," into ");
      PRINT_ADDRESS("",ADDRESS,".\n");
    }
  }
}

/* ---------------------------------- STCH_P ------------------------------- */
void STCH_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  ADDRESS = FORMAT3_4(LOCATION_1,1);
  MEMORY[ADDRESS] = A_0 - (A_0 / MAX_BYTE_VAL_0) * MAX_BYTE_VAL_0;
 
 if (PRINT_EFFECT) {
   PRINT_CONSTANT("Put ",A_0," into ");
   PRINT_ADDRESS("",ADDRESS,".\n");
  }
}

/* ---------------------------------- STI_P -------------------------------- */
void STI_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  if (!SUPERVISOR_MODE()) {
    SIGNAL_INTERUPT(2,01);
    if (PRINT_EFFECT) (void) printf("STI is a privileged instruction.\n");
  } else {
    ADDRESS = FORMAT3_4(LOCATION_1,3);
    if (ADDRESS+2 >= MEM_SIZE_1) {
      SIGNAL_INTERUPT(2,02);    
      if (PRINT_EFFECT) (void) printf("STI m, m is an illegal address.\n");
    } else {
      MEMORY[ADDRESS] = INTERVAL_TIMER / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
      MEMORY[ADDRESS+1] = (INTERVAL_TIMER - 
			   INT(MEMORY[ADDRESS]) * 
			   (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) / MAX_BYTE_VAL_0;
      MEMORY[ADDRESS+2] = 
	(INTERVAL_TIMER - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);
      
      if (PRINT_EFFECT) {
	PRINT_CONSTANT("Put ",INTERVAL_TIMER," into ");
	PRINT_ADDRESS("",ADDRESS,".\n");
      }
    }
  }
}

/* ---------------------------------- STL_P -------------------------------- */
void STL_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  ADDRESS = FORMAT3_4(LOCATION_1,3);
  if (ADDRESS+2 >= MEM_SIZE_1) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("STL m, m is an illegal address.\n");
  } else {
    MEMORY[ADDRESS] = L_0 / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
    MEMORY[ADDRESS+1] = (L_0 - 
      INT(MEMORY[ADDRESS]) * (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) 
      / MAX_BYTE_VAL_0;
    MEMORY[ADDRESS+2] = (L_0 - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);

    if (PRINT_EFFECT) {
      PRINT_CONSTANT("Put ",L_0," into ");
      PRINT_ADDRESS("",ADDRESS,".\n");
    }
  }
}

/* ---------------------------------- STS_P -------------------------------- */
void STS_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  ADDRESS = FORMAT3_4(LOCATION_1,3);
  if (ADDRESS+2 >= MEM_SIZE_1) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("STS m, m is an illegal address.\n");
  } else {
    MEMORY[ADDRESS] = S_0 / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
    MEMORY[ADDRESS+1] = (S_0 - 
      INT(MEMORY[ADDRESS]) * (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) 
      / MAX_BYTE_VAL_0;
    MEMORY[ADDRESS+2] = (S_0 - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);

    if (PRINT_EFFECT) {
      PRINT_CONSTANT("Put ",S_0," into ");
      PRINT_ADDRESS("",ADDRESS,".\n");
    }
  }
}

/* --------------------------------- STSW_P -------------------------------- */
void STSW_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  if (!SUPERVISOR_MODE()) {
    SIGNAL_INTERUPT(2,01);
    if (PRINT_EFFECT) (void) printf("STSW is a privileged instruction.\n");
  } else {
    ADDRESS = FORMAT3_4(LOCATION_1,3);
    if (ADDRESS+2 >= MEM_SIZE_1) {
      SIGNAL_INTERUPT(2,02);    
      if (PRINT_EFFECT) (void) printf("STSW m, m is an illegal address.\n");
    } else {
      MEMORY[ADDRESS] = SW_0 / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
      MEMORY[ADDRESS+1] = (SW_0 - 
			   INT(MEMORY[ADDRESS]) * 
			   (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) / MAX_BYTE_VAL_0;
      MEMORY[ADDRESS+2] = (SW_0 - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);
      
      if (PRINT_EFFECT) {
	PRINT_CONSTANT("Put ",SW_0," into ");
	PRINT_ADDRESS("",ADDRESS,".\n");
      }
    }
  }
}

/* ---------------------------------- STT_P -------------------------------- */
void STT_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  ADDRESS = FORMAT3_4(LOCATION_1,3);
  if (ADDRESS+2 >= MEM_SIZE_1) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("STT m, m is an illegal address.\n");
  } else {
    MEMORY[ADDRESS] = T_0 / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
    MEMORY[ADDRESS+1] = (T_0 - 
      INT(MEMORY[ADDRESS]) * (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) 
      / MAX_BYTE_VAL_0;
    MEMORY[ADDRESS+2] = (T_0 - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);

    if (PRINT_EFFECT) {
      PRINT_CONSTANT("Put ",T_0," into ");
      PRINT_ADDRESS("",ADDRESS,".\n");
    }
  }
}

/* ---------------------------------- STX_P -------------------------------- */
void STX_P(BOOLEAN PRINT_EFFECT) 
{
  int ADDRESS;

  ADDRESS = FORMAT3_4(LOCATION_1,3);
  if (ADDRESS+2 >= MEM_SIZE_1) {
    SIGNAL_INTERUPT(2,02);    
    if (PRINT_EFFECT) (void) printf("STX m, m is an illegal address.\n");
  } else {
    MEMORY[ADDRESS] = X_0 / (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0);
    MEMORY[ADDRESS+1] = (X_0 - 
      INT(MEMORY[ADDRESS]) * (MAX_BYTE_VAL_0 * MAX_BYTE_VAL_0)) 
      / MAX_BYTE_VAL_0;
    MEMORY[ADDRESS+2] = (X_0 - INT(MEMORY[ADDRESS+1]) * MAX_BYTE_VAL_0);

    if (PRINT_EFFECT) {
      PRINT_CONSTANT("Put ",X_0," into ");
      PRINT_ADDRESS("",ADDRESS,".\n");
    }
  }
}

/* ---------------------------------- TD_P --------------------------------- */
void TD_P(BOOLEAN PRINT_EFFECT) 
{
  int DEV_NUM;

  DEV_NUM = FORMAT3_4(VALUE_1,1);
  if ((0 > DEV_NUM) || (DEV_NUM > 255)) {
    SIGNAL_INTERUPT(2,00);
    if (PRINT_EFFECT) (void) printf("TD illegal device.\n");
  }
  else if (DEVICE[DEV_NUM].STATUS == NOT_IN_USE_1) {
    OPEN_DEVICE(DEV_NUM);
    SET_CC(LESS_THAN_1);
    if (PRINT_EFFECT)
      PRINT_CONSTANT("Set CC to <. Marked Device ",DEV_NUM," as in use.\n");
  } else {
    SET_CC(EQUAL_1);
    if (PRINT_EFFECT) (void) printf("Set CC to =.\n");
  }
}

/* ---------------------------------- TIX_P -------------------------------- */
void TIX_P(BOOLEAN PRINT_EFFECT) 
{
  int VALUE;
  int X_VAL;

  VALUE = FORMAT3_4(VALUE_1,3);

  X_0 ++;
  X_0 = X_0 - ((int) X_0 / MAX_INT_1) * MAX_INT_1;

  if (X_0 == MAX_INT_1 / 2) {
    SIGNAL_INTERUPT(2,04);
    if (PRINT_EFFECT) (void) printf("X has overflowed.\n");
    return;
  }

  if (X_0 >= MAX_INT_1 / 2) X_VAL = X_0 - MAX_INT_1;
  else X_VAL = X_0;

  if (VALUE >= MAX_INT_1 / 2) VALUE = VALUE - MAX_INT_1;

  if (X_VAL == VALUE) {
    SET_CC(EQUAL_1);
    if (PRINT_EFFECT) PRINT_CONSTANT("Set X to ",X_0," and set CC to =.\n");
  }
  if (X_VAL < VALUE) {
    SET_CC(LESS_THAN_1);
    if (PRINT_EFFECT) PRINT_CONSTANT("Set X to ",X_0," and set CC to <.\n");
  }
  if (X_VAL > VALUE) {
    SET_CC(GREATER_THAN_1);
    if (PRINT_EFFECT) PRINT_CONSTANT("Set X to ",X_0," and set CC to >.\n");
  }
}

/* ---------------------------------- WD_P --------------------------------- */
void WD_P(BOOLEAN PRINT_EFFECT)
{
  int DEV;
  int VALUE;

  DEV = FORMAT3_4(VALUE_1,1);
  if ((0 > DEV) || (DEV > 255)) {
    SIGNAL_INTERUPT(2,00);
    if (PRINT_EFFECT) (void) printf("WD illegal device.\n");
  } else {
    if ((DEVICE[DEV].STREAM == NULL) || (DEVICE[DEV].STATUS == NOT_IN_USE_1)
	|| (DEV < 128)) {
      SIGNAL_INTERUPT(2,00);
      if (PRINT_EFFECT) (void) printf("WD illegal device.\n");
    } else {
      VALUE = A_0 - ((int) A_0/MAX_BYTE_VAL_0)*MAX_BYTE_VAL_0;
      (void) fprintf(DEVICE[DEV].STREAM,"%c",VALUE);
      if (PRINT_EFFECT) {
	PRINT_CONSTANT("Wrote ASCII char ",VALUE," to device ");
	PRINT_CONSTANT("",DEV,"\n");
      }
      DEVICE[DEV].STATUS = NOT_IN_USE_1;
    }
  }
}
