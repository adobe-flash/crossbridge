/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* ============================== intruction2.c ============================ */
/* Contains all the procedures that know how to handle FORMAT2 instructions  */
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

/* ----------------------------- MOD (local) ------------------------------- */
/* return (N mod M);                                                         */
int MOD(N,M)
     int N;
     int M;
{
  return N - ((int) N / M) * M;
}

/* ----------------------------------- ADDR_P ------------------------------ */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
void ADDR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  BOOLEAN OVERFLOW1 = FALSE_1; /* Has an overflow been detected.             */
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */
  int REG1_SIGN = 1;          /* Sign of value in the first register         */
  int REG2_SIGN = 1;          /* Sign of value in the second register        */
  
  FORMAT2(&REG1,&REG2);

  if (!( ( (REG1 >=0 ) && (REG1 < NUM_REGISTERS)) ||
	( (REG2 >=0 ) && (REG2 < NUM_REGISTERS)) )) {
    SIGNAL_INTERUPT(2,00);
  } else {
    
/* --------------- This is needed to correctly dectect overflow ------------ */
    if (REGISTER[REG1] >= MAX_INT_1 / 2) REG1_SIGN = -1;
    if (REGISTER[REG2] >= MAX_INT_1 / 2) REG2_SIGN = -1;

    if ((REG2_SIGN == 1) && (REG1_SIGN == 1)) {
      REGISTER[REG2] = REGISTER[REG2] + REGISTER[REG1];
      if (REGISTER[REG2] >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    }
    if ((REG2_SIGN == -1) && (REG1_SIGN == 1)) {
      REGISTER[REG2] = REGISTER[REG2] + REGISTER[REG1];
      REGISTER[REG2] = REGISTER[REG2] -
	((int) REGISTER[REG2] / MAX_INT_1) * MAX_INT_1;
    }
    if ((REG2_SIGN == 1) && (REG1_SIGN == -1)) {
      REGISTER[REG2] = REGISTER[REG2] + REGISTER[REG1];
      REGISTER[REG2] = REGISTER[REG2] -
	((int) REGISTER[REG2] / MAX_INT_1) * MAX_INT_1;
    }
    if ((REG2_SIGN == -1) && (REG1_SIGN == -1)) {
      REGISTER[REG2] = REGISTER[REG2] + REGISTER[REG1];
      REGISTER[REG2] = REGISTER[REG2] -
	((int) REGISTER[REG2] / MAX_INT_1) * MAX_INT_1;
      if (REGISTER[REG2] < MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    }
    
    if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG2,"");
      PRINT_CONSTANT(" to ",REGISTER[REG2],".\n");
    }
    if (OVERFLOW1) SIGNAL_INTERUPT(2,04);
  }
}
 
/* ----------------------------------- CLEAR_P ---------------------------- */
void CLEAR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */

  FORMAT2(&REG1,&REG2);

  if (!( (REG1 >=0 ) && (REG1 < NUM_REGISTERS)))
    SIGNAL_INTERUPT(2,00);
  else {
    REGISTER[REG1] = 0;
    if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG1,"");
      PRINT_CONSTANT(" to ",REGISTER[REG1],".\n");
    }
  }
}

/* ---------------------------------- COMPR_P ------------------------------ */
void COMPR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */

{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */
  int REG1_VAL;               /* What is stored in first register.           */
  int REG2_VAL;               /* What is stored in second register.          */
  
  FORMAT2(&REG1,&REG2);

  if (REGISTER[REG1] >= MAX_INT_1 / 2) REG1_VAL = REGISTER[REG1] - MAX_INT_1;
  else REG1_VAL = REGISTER[REG1];
  
  if (REGISTER[REG1] >= MAX_INT_1 / 2) REG2_VAL = REGISTER[REG2] - MAX_INT_1;
  else REG2_VAL = REGISTER[REG2];

  if (REG1_VAL == REG2_VAL) {
    SET_CC(EQUAL_1);
    if (PRINT_EFFECT) (void) printf("Set CC to =.\n");
  }
  if (REG1_VAL < REG2_VAL) {
    SET_CC(LESS_THAN_1);
    if (PRINT_EFFECT) (void) printf("Set CC to <.\n");
  }
  if (REG1_VAL > REG2_VAL) {
    SET_CC(GREATER_THAN_1);
    if (PRINT_EFFECT) (void) printf("Set CC to >.\n");
  }
}

/* ---------------------------------- DIVR_P ------------------------------ */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments.                                                            */
void DIVR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */
  int REG1_SIGN = 1;          /* Sign of value in the first register         */
  int REG2_SIGN = 1;          /* Sign of value in the second register        */

  FORMAT2(&REG1,&REG2);

  if (!( ( (REG1 >=0 ) && (REG1 < NUM_REGISTERS)) ||
	( (REG2 >=0 ) && (REG2 < NUM_REGISTERS)) )) {
    SIGNAL_INTERUPT(2,00);
  } else if (REGISTER[REG1] == 0) {
    SIGNAL_INTERUPT(2,04);
    if (PRINT_EFFECT) (void) printf("Attempted division by zero.\n");
  } else {
/* --------------- This is needed to correctly dectect overflow ------------ */
    if (REGISTER[REG1] >= MAX_INT_1 / 2) REG1_SIGN = -1;
    if (REGISTER[REG2] >= MAX_INT_1 / 2) REG2_SIGN = -1;

    if ((REG2_SIGN == 1) && (REG1_SIGN == 1)) 
      REGISTER[REG2] = REGISTER[REG2] / REGISTER[REG1];
    if ((REG2_SIGN == -1) && (REG1_SIGN == 1)) 
      REGISTER[REG2] = MAX_INT_1 - 
	((MAX_INT_1 - REGISTER[REG2]) / REGISTER[REG1]);
    if ((REG2_SIGN == 1) && (REG1_SIGN == -1))
      REGISTER[REG2] = MAX_INT_1 - 
	(REGISTER[REG2] / (MAX_INT_1 - REGISTER[REG1]));
    if ((REG2_SIGN == -1) && (REG1_SIGN == -1)) 
      REGISTER[REG2] = (MAX_INT_1 - REGISTER[REG2]) / 
	(MAX_INT_1 - REGISTER[REG1]);
  }
  
  if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG2,"");
      PRINT_CONSTANT(" to ",REGISTER[REG2],".\n");
    }
}
 
/* ---------------------------------- MULR_P ------------------------------ */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
void MULR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  BOOLEAN OVERFLOW1 = FALSE_1; /* Has an overflow been detected.             */
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */
  int REG1_SIGN = 1;          /* Sign of value in the first register         */
  int REG2_SIGN = 1;          /* Sign of value in the second register        */
  double STORE_VALUE;         /* So the simulator does not overflow          */

  FORMAT2(&REG1,&REG2);

  if (!( ( (REG1 >=0 ) && (REG1 < NUM_REGISTERS)) ||
	( (REG2 >=0 ) && (REG2 < NUM_REGISTERS)) )) {
    SIGNAL_INTERUPT(2,00);
  } else {
    
/* --------------- This is needed to correctly dectect overflow ------------ */
    if (REGISTER[REG1] >= MAX_INT_1 / 2) REG1_SIGN = -1;
    if (REGISTER[REG2] >= MAX_INT_1 / 2) REG2_SIGN = -1;

    if ((REG2_SIGN == 1) && (REG1_SIGN == 1)) {
      STORE_VALUE = 1.0 * REGISTER[REG2] * REGISTER[REG1];
      if (STORE_VALUE >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
      else REGISTER[REG2] = (int) STORE_VALUE;
    }
    if ((REG2_SIGN == -1) && (REG1_SIGN == 1)) {
      STORE_VALUE = 1.0 * (MAX_INT_1 - REGISTER[REG2]) * REGISTER[REG1];
      STORE_VALUE = STORE_VALUE - ((int)(STORE_VALUE / MAX_INT_1)) * MAX_INT_1;
      if (STORE_VALUE >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
      else REGISTER[REG2] = MAX_INT_1 - (int) STORE_VALUE;
    }
    if ((REG2_SIGN == 1) && (REG1_SIGN == -1)) {
      STORE_VALUE = 1.0 * REGISTER[REG2] * (MAX_INT_1 - REGISTER[REG1]);
      STORE_VALUE = STORE_VALUE - ((int)(STORE_VALUE / MAX_INT_1)) * MAX_INT_1;
      if (STORE_VALUE >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
      else REGISTER[REG2] = MAX_INT_1 - (int) STORE_VALUE;
    }
    if ((REG2_SIGN == -1) && (REG1_SIGN == -1)) {
      STORE_VALUE = 1.0 * (MAX_INT_1 - REGISTER[REG2]) * 
	(MAX_INT_1 - REGISTER[REG1]);
      if (STORE_VALUE >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
      else REGISTER[REG2] = (int) STORE_VALUE;
    }
   
    if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG2,"");
      PRINT_CONSTANT(" to ",REGISTER[REG2],".\n");
    }
    if (OVERFLOW1) SIGNAL_INTERUPT(2,04);
  }
}

/* ----------------------------------- RMO_P ------------------------------- */
void RMO_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */

  FORMAT2(&REG1,&REG2);

  if (!( ( (REG1 >=0 ) && (REG1 < NUM_REGISTERS)) ||
	( (REG2 >=0 ) && (REG2 < NUM_REGISTERS)) )) {
    SIGNAL_INTERUPT(2,00);
  } else {
    REGISTER[REG2] = REGISTER[REG1];
    if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG2,"");
      PRINT_CONSTANT(" to ",REGISTER[REG2],".\n");
    }
  }
}
  

/* --------------------------------- SHIFTL_P ------------------------------ */
void SHIFTL_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */

  FORMAT2(&REG1,&REG2);
  if (!( (REG1 >=0 ) && (REG1 < NUM_REGISTERS))) SIGNAL_INTERUPT(2,00);
  else {
    int LOOP;                 /* COUNTER VARIBLE                            */
    int REG_BITS[BITS_PER_WORD_1];
    int SHIFTED_BITS[BITS_PER_WORD_1];

    BYTES_TO_BITS(REGISTER[REG1],3,REG_BITS);
    for (LOOP = 0; LOOP < BITS_PER_WORD_1; LOOP ++)
      SHIFTED_BITS[LOOP] = REG_BITS[MOD(LOOP+REG2+1,BITS_PER_WORD_1)];
    
    REGISTER[REG1] = BITS_TO_BYTE(SHIFTED_BITS,BITS_PER_WORD_1);

    if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG1,"");
      PRINT_CONSTANT(" to ",REGISTER[REG1],".\n");
    }
  }
}

/* --------------------------------- SHIFTR_P ------------------------------ */
void SHIFTR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */

  FORMAT2(&REG1,&REG2);
  if (!( (REG1 >=0 ) && (REG1 < NUM_REGISTERS))) SIGNAL_INTERUPT(2,00);
  else {
    int LOOP;                 /* COUNTER VARIBLE                            */
    int REG_BITS[BITS_PER_WORD_1];
    int SHIFTED_BITS[BITS_PER_WORD_1];

    BYTES_TO_BITS(REGISTER[REG1],3,REG_BITS);
    for (LOOP = 0; LOOP < BITS_PER_WORD_1; LOOP ++)
      if (LOOP <= REG2) SHIFTED_BITS[LOOP] = REG_BITS[0];
      else SHIFTED_BITS[LOOP] = REG_BITS[LOOP-REG2-1];
    
    REGISTER[REG1] = BITS_TO_BYTE(SHIFTED_BITS,BITS_PER_WORD_1);

    if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG1,"");
      PRINT_CONSTANT(" to ",REGISTER[REG1],".\n");
    }
  }
}

/* ---------------------------------- SUBR_P ------------------------------- */
/* This proceedure is messy because work had to be done with the signs of    */
/* the arguments so that overflow could be detected.                         */
void SUBR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  BOOLEAN OVERFLOW1 = FALSE_1; /* Has an overflow been detected.             */
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */
  int REG1_SIGN = 1;          /* Sign of value in the first register         */
  int REG2_SIGN = 1;          /* Sign of value in the second register        */
  
  FORMAT2(&REG1,&REG2);

  if (!( ( (REG1 >=0 ) && (REG1 < NUM_REGISTERS)) ||
	( (REG2 >=0 ) && (REG2 < NUM_REGISTERS)) )) {
    SIGNAL_INTERUPT(2,00);
  } else {
    
/* --------------- This is needed to correctly dectect overflow ------------ */
    if (REGISTER[REG1] >= MAX_INT_1 / 2) REG1_SIGN = -1;
    if (REGISTER[REG2] >= MAX_INT_1 / 2) REG2_SIGN = -1;

    if ((REG2_SIGN == 1) && (REG1_SIGN == 1)) {
      REGISTER[REG2] = REGISTER[REG2] + (MAX_INT_1 - REGISTER[REG1]);
      REGISTER[REG2] = REGISTER[REG2] -
	((int) REGISTER[REG2] / MAX_INT_1) * MAX_INT_1;
    }
    if ((REG2_SIGN == -1) && (REG1_SIGN == 1)) {
      REGISTER[REG2] = REGISTER[REG2] + (MAX_INT_1 - REGISTER[REG1]);
      REGISTER[REG2] = REGISTER[REG2] -
	    ((int) REGISTER[REG2] / MAX_INT_1) * MAX_INT_1;
      if (REGISTER[REG2] < MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    }
    if ((REG2_SIGN == 1) && (REG1_SIGN == -1)) {
      REGISTER[REG2] = REGISTER[REG2] + (MAX_INT_1 - REGISTER[REG1]);
      if (REGISTER[REG2] >= MAX_INT_1 / 2) OVERFLOW1 = TRUE_1;
    }
    if ((REG2_SIGN == -1) && (REG1_SIGN == -1)) {
      REGISTER[REG2] = REGISTER[REG2] + (MAX_INT_1 - REGISTER[REG1]);
      REGISTER[REG2] = REGISTER[REG2] -
	((int) REGISTER[REG2] / MAX_INT_1) * MAX_INT_1;
    }
    
    if (PRINT_EFFECT) {
      PRINT_REG_NAME("Set ",REG2,"");
      PRINT_CONSTANT(" to ",REGISTER[REG2],".\n");
    }
    if (OVERFLOW1) SIGNAL_INTERUPT(2,04);
  }
}
 
/* ----------------------------------- SVC_P ------------------------------- */
void SVC_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */

  FORMAT2(&REG1,&REG2);

  if ( (REG1 < 0 ) || (REG1 > 4))
      SIGNAL_INTERUPT(2,00);
  else {
    SIGNAL_INTERUPT(1,REG1);
    if (PRINT_EFFECT) (void) printf("Generated SVC interupt %d.\n",REG1);
  }
}
  
/* ---------------------------------- TIXR_P ------------------------------- */
void TIXR_P(PRINT_EFFECT)
     BOOLEAN PRINT_EFFECT;    /* Output effect of this command for debugging */
{
  int REG1;                   /* First Register in instruction               */
  int REG2;                   /* Second Register in instruction              */
  int X_VAL;                  /* Value stored in X                           */
  int REG_VAL;                /* Value stored in REG1                        */

  FORMAT2(&REG1,&REG2);

  if (!( (REG1 >=0 ) && (REG1 < NUM_REGISTERS)))
    SIGNAL_INTERUPT(2,00);
  else {

    X_0 ++;
    X_0 = X_0 - ((int) X_0 / MAX_INT_1) * MAX_INT_1;
    
    if (X_0 == MAX_INT_1 / 2) {
      SIGNAL_INTERUPT(2,04);
      if (PRINT_EFFECT) (void) printf("X has overflowed.\n");
      return;
    }
    
    if (X_0 >= MAX_INT_1 / 2) X_VAL = X_0 - MAX_INT_1;
    else X_VAL = X_0;
    
    REG_VAL = REGISTER[REG1];
    
    if (REG_VAL >= MAX_INT_1 / 2) REG_VAL = REG_VAL - MAX_INT_1;
    
    if (X_VAL == REG_VAL) {
      SET_CC(EQUAL_1);
      if (PRINT_EFFECT) 
	PRINT_CONSTANT("Set X to ",X_0," and set CC to =.\n");
    }
    if (X_VAL < REG_VAL) {
      SET_CC(LESS_THAN_1);
      if (PRINT_EFFECT) 
	PRINT_CONSTANT("Set X to ",X_0," and set CC to <.\n");
    }
    if (X_VAL > REG_VAL) {
      SET_CC(GREATER_THAN_1);
      if (PRINT_EFFECT) 
	PRINT_CONSTANT("Set X to ",X_0," and set CC to >.\n");
    }
  }
}
