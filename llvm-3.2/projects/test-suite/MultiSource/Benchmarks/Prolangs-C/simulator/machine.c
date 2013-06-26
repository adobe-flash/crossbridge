/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* ============================= machine.c ================================= */
/* Maintains all the information associated with the machine. Can be used to */
/* drive complete simulation of a program or just execute one instruction.   */
#include <math.h>
#include <stdio.h>
#include "boolean.h"
#include "constants.h"
#include "convert.h"
#include "memory.h"
#include "sim_debug.h"		
#include "debugger.h"
#include "instruct.h"
#include "instruct2.h"
#include "interupt.h"

/* --------------------------------- GLOBALS ------------------------------ */
/* MEMORY                            THE MAIN MEMORY.                       */
  MEM_SPACE MEMORY = NULL;        

/* INTERVAL_TIMER                    USED FOR TIMING OUT A PROCESS AND      */
int INTERVAL_TIMER = 0;           /* RETURNING CONTROL TO THE OS            */

/* ---------------------------- THE REGISTERS ----------------------------- */
int REGISTER[NUM_REGISTERS];
#define A_0       REGISTER[0]          /* 0     A       ACCUMULATOR         */
#define X_0       REGISTER[1]          /* 1     X       INDEX REGISTER      */
#define L_0       REGISTER[2]          /* 2     L       LINKAGE REGISTER    */
#define B_0       REGISTER[3]          /* 3     B       BASE REGISTER       */
#define S_0       REGISTER[4]          /* 4     S       GENERAL REGISTER    */
#define T_0       REGISTER[5]          /* 5     T       GENERAL REGISTER    */
#define F_0       REGISTER[6]          /* 6     F       FLOATING POINT - NOT*/
                                       /*                   IMPLEMENTED     */
                                       /* 7             NO REGISTER         */
#define PC_0      REGISTER[8]          /* 8     PC      PROGRAM COUNTER     */
#define SW_0      REGISTER[9]          /* 9     SW      STATUS WORD         */

/* ------------------------------ THE DEVICES ------------------------------ */
#define NOT_IN_USE_1      0
#define IN_USE_1          1
/* THE DEVICES:                                                              */
/*    0                         KEYBOARD (INPUT)                             */
/*    80                        TERMINAL (OUTPUT)                            */
/*    I (00 <= I <= FF)         FILE NAMED DEVI                              */
/*    I (00 <= I < 80) Input Devices.      I (80 <= I <= FF) Output Devices. */
  struct DEVICE_ENTRY {
    int STATUS;
    FILE *STREAM;
  } DEVICE[256];

/* --------------------- NON-GLOBAL MACHINE STRUCTURES -------------------- */
#include "define-instr-tab.h"
char REG_NAMES[NUM_REGISTERS][3] =     /* Name of the registers.             */
          {"A","X","L","B","S","T","F","7","PC","SW"};

BOOLEAN PRINT_INSTR = FALSE_1;         /* Not a nice fix, so that            */
                                       /* PRINT_ADDRESS can print            */
                                       /* in instruction format when needed  */
                                       /* Should be a parameter, but too     */
                                       /* much work for me to correct.       */

extern void FORMAT2(int *REG1,int *REG2);
extern int FORMAT3_4(int DEMAND,int BYTES);

void CALL_FUNCTION1(int FUNC,BOOLEAN PRINT_EFFECT)
{
    switch(FUNC) {
    case LDA_P0: LDA_P(PRINT_EFFECT);
	break;
    case LDX_P0: LDX_P(PRINT_EFFECT);
	break;
    case LDL_P0: LDL_P(PRINT_EFFECT);
	break;
    case STA_P0: STA_P(PRINT_EFFECT);
	break;
    case STX_P0: STX_P(PRINT_EFFECT);
	break;
    case STL_P0: STL_P(PRINT_EFFECT);
	break;
    case ADD_P0: ADD_P(PRINT_EFFECT);
	break;
    case SUB_P0: SUB_P(PRINT_EFFECT);
	break;
    case MUL_P0: MUL_P(PRINT_EFFECT);
	break;
    case DIV_P0: DIV_P(PRINT_EFFECT);
	break;
    case COMP_P0: COMP_P(PRINT_EFFECT);
	break;
    case TIX_P0: TIX_P(PRINT_EFFECT);
	break;
    case JEQ_P0: JEQ_P(PRINT_EFFECT);
	break;
    case JGT_P0: JGT_P(PRINT_EFFECT);
	break;
    case JLT_P0: JLT_P(PRINT_EFFECT);
	break;
    case J_P0: J_P(PRINT_EFFECT);
	break;
    case AND_P0: AND_P(PRINT_EFFECT);
	break;
    case OR_P0: OR_P(PRINT_EFFECT);
	break;
    case JSUB_P0: JSUB_P(PRINT_EFFECT);
	break;
    case RSUB_P0: RSUB_P(PRINT_EFFECT);
	break;
    case LDCH_P0: LDCH_P(PRINT_EFFECT);
	break;
    case STCH_P0: STCH_P(PRINT_EFFECT);
	break;
    case NULL0: break;
    case LDB_P0: LDB_P(PRINT_EFFECT);
	break;
    case LDS_P0: LDS_P(PRINT_EFFECT);
	break;
    case LDT_P0: LDT_P(PRINT_EFFECT);
	break;
    case STB_P0: STB_P(PRINT_EFFECT);
	break;
    case STS_P0: STS_P(PRINT_EFFECT);
	break;
    case STT_P0: STT_P(PRINT_EFFECT);
	break;
    case ADDR_P0: ADDR_P(PRINT_EFFECT);
	break;
    case SUBR_P0: SUBR_P(PRINT_EFFECT);
	break;
    case MULR_P0: MULR_P(PRINT_EFFECT);
	break;
    case DIVR_P0: DIVR_P(PRINT_EFFECT);
	break;
    case COMPR_P0: COMPR_P(PRINT_EFFECT);
	break;
    case SHIFTL_P0: SHIFTL_P(PRINT_EFFECT);
	break;
    case SHIFTR_P0: SHIFTR_P(PRINT_EFFECT);
	break;
    case RMO_P0: RMO_P(PRINT_EFFECT);
	break;
    case SVC_P0: SVC_P(PRINT_EFFECT);
	break;
    case CLEAR_P0: CLEAR_P(PRINT_EFFECT);
	break;
    case TIXR_P0: TIXR_P(PRINT_EFFECT);
	break;
    case STI_P0: STI_P(PRINT_EFFECT);
	break;
    case RD_P0: RD_P(PRINT_EFFECT);
	break;
    case WD_P0: WD_P(PRINT_EFFECT);
	break;
    case TD_P0: TD_P(PRINT_EFFECT);
	break;
    case STSW_P0: STSW_P(PRINT_EFFECT);
	break;
    }
}
/* --------------------------- RESET_DEVICES ------------------------------- */
/* Reset all the devices. This is needed so that if you run program twice    */
/* in one debugging section, you can be sure the devices will be okay.       */
void RESET_DEVICES(void)
{
  int COUNT;

  DEVICE[0].STATUS = NOT_IN_USE_1;
  DEVICE[0].STREAM = NULL;
  DEVICE[128].STATUS = NOT_IN_USE_1;
  DEVICE[128].STREAM = NULL;
  for (COUNT = 0; COUNT < 256; COUNT ++) {
    if (DEVICE[COUNT].STATUS == IN_USE_1) 
      DEVICE[COUNT].STATUS = NOT_IN_USE_1;
    if (DEVICE[COUNT].STREAM != NULL) {
      (void) fclose(DEVICE[COUNT].STREAM);
      DEVICE[COUNT].STREAM = NULL;
    }
  }
}

/* --------------------------- PRINT_REG_NAME ------------------------------ */
/* Prints out STR1, followed by the name of a register, followed by STR2.    */
void PRINT_REG_NAME(char *STR1,int REG_NUM,char *STR2)
{
  (void) printf("%S%S%S",STR1,REG_NAMES[REG_NUM],STR2);
}

/* ------------------------- INSTR_CODE (local) ---------------------------- */
/* Returns the first 6 bits of a BYTE. The relevant part to determining      */
/* which instruction it is.                                                  */
int INSTR_CODE(int LOC)
{
  return (INT(MEMORY[LOC]) / 4);
}

/* ----------------------------- BYTES_TO_BITS ----------------------------- */
/* Convert BYTES bytes store in VAL (the variable VAL) and set BINARY to be  */
/* the corresponding binary array of BITS.                                   */
void BYTES_TO_BITS(int VAL,int NUM_BYTES,int *BINARY)
{
  int I;

  for (I=1;I<=BITS_PER_BYTE_2*NUM_BYTES;I++) {
    BINARY[BITS_PER_BYTE_2*NUM_BYTES - I] = VAL - ((int) VAL / 2) * 2;
    VAL = VAL / 2;
  }
}

/* -------------------------- BITS_TO_BYTE --------------------------------- */
/* Convert an array of BITS bits to an integer (BYTE) and return result.     */
int BITS_TO_BYTE(int *BINARY,int BITS)
{
  int I;                      /* Counter variable.                           */
  int VAL;                    /* VALue to return.                            */

  VAL = 0;
  for ( I = 0; I < BITS; I ++) 
    VAL = VAL * 2 + BINARY[I];

  return VAL;
}

/* --------------------------- SET_CC -------------------------------------- */
/* SET the CC (Condition Code) to CODE                                       */
void SET_CC(int CODE)
{
  int SW[BITS_PER_WORD_1];

  if ((CODE >= 0) && (CODE <= 3)) {
    BYTES_TO_BITS(SW_0,3,SW);
    SW[6] = CODE / 2;
    SW[7] = CODE - SW[6]*2;
    SW_0 = BITS_TO_BYTE(SW,BITS_PER_WORD_1);
  } else (void) printf("SET_CC called improperly.\n");
}

/* ---------------------------------- CC ----------------------------------- */
/* Return the value stored in the condition code.                            */
int CC(void)
{
  int SW[BITS_PER_WORD_1];

  BYTES_TO_BITS(SW_0,3,SW);
  return SW[6] * 2 + SW[7];
}

/* ---------------------------- SUPERVISOR_MODE ---------------------------- */
/* Return TRUE iff in SUPERVISOR MODE.                                       */
BOOLEAN SUPERVISOR_MODE(void)
{
  int SW[BITS_PER_WORD_1];         /* BIT version of SW (status word)        */

  BYTES_TO_BITS(SW_0,3,SW);
  return (SW[0] == 1);
}

/* ------------------------ DIGIT_TO_CHAR (local) -------------------------- */
/* Convert a DIGIT to its character representation BASE 16.                  */
char DIGIT_TO_CHAR(int DIGIT)
{
  char CH;
  if (9 >= DIGIT) CH = DIGIT + '0';
  else CH = DIGIT - 10 + 'A';
  return CH;
}

/* --------------------------- OPEN_DEVICE --------------------------------- */
/* Mark device DEV as IN USE and, if needed, open it.                        */
void OPEN_DEVICE(int DEV)
{

  DEVICE[DEV].STATUS = IN_USE_1;
  if (DEVICE[DEV].STREAM == NULL) 
    {
      char NAME[6];

      NAME[0] = 'd'; NAME[1] = 'e'; NAME[2] = 'v'; NAME[5] = '\0';

      if (DEV == 0)
	DEVICE[0].STREAM = stdin;
      else if (DEV == 128) 
	DEVICE[128].STREAM = stdout;
      else {
	NAME[3] = DIGIT_TO_CHAR(DEV/16);
	NAME[4] = DIGIT_TO_CHAR(DEV - ((int) DEV/16)*16);
	if (DEV < 128) 
	  DEVICE[DEV].STREAM = fopen(NAME,"r");
	else DEVICE[DEV].STREAM = fopen(NAME,"w");
      }
    }
}

/* Declared later on in this file... see bellow.                             */
extern void PRINT_INSTRUCTION();

/* ---------------------------- PRINT_ADDRESS ------------------------------ */
/* Output PREFIX, followed by the ADDRESS, followed by SUFFIX. Print the     */
/* Address in hexidecimal, decimal or by a LABEL name.                       */
void PRINT_ADDRESS(char *PREFIX,int ADDRESS,char *SUFFIX)
{
  char ADDR[LABEL_SIZE_1+1];        /* String version of address.            */
  SYMBOL_TABLE NEXT;                /* Used to walk through the symbol table */

  NEXT = SYM_TAB;
  while (NEXT != NULL) 
    if ((*NEXT).LOCATION == ADDRESS) {
      int COUNT;
      char SAV;

      for (COUNT=LABEL_SIZE_1-1; 
	   (COUNT > 0) && ((*NEXT).LABEL[COUNT] == ' ');
	   COUNT --);
      SAV = (*NEXT).LABEL[COUNT+1];
      (*NEXT).LABEL[COUNT+1] = '\0';
      (void) printf("%s%s%s",PREFIX,(*NEXT).LABEL,SUFFIX);
      (*NEXT).LABEL[COUNT+1] = SAV;
      NEXT = NULL;
      return;
    } else NEXT = (*NEXT).NEXT;

  if (TYPE_OUT_MODE[0] == 'D') {
    NUM_TO_STR(ADDRESS,10,8,ADDR);
    (void) printf("%s%s [base 10]%s",PREFIX,ADDR,SUFFIX);
  }
  else {
    NUM_TO_STR(ADDRESS,16,6,ADDR);
    if (!PRINT_INSTR) 
      (void) printf("%s%s [base 16]%s",PREFIX,ADDR,SUFFIX);
    else (void) printf("%s%s%s",PREFIX,ADDR,SUFFIX);
  }
}

/* ---------------------------- PRINT_CONSTANT ----------------------------- */
/* Output PREFIX, followed by the CONSTANT in a base depending on            */
/* TYPE_OUT_MODE, followed by SUFFIX                                         */
void PRINT_CONSTANT(char *PREFIX,int CONST,char *SUFFIX)
{
  char STR_CONST[LABEL_SIZE_1+1];
  int DIGITS;

  if (TYPE_OUT_MODE[0] == 'D') {
    if (CONST == 0) DIGITS = 1;
    else DIGITS = ((int) (log(1.0*CONST)/log(10.0))) + 1;
    NUM_TO_STR(CONST,10,DIGITS,STR_CONST);
    (void) printf("%s%s [base 10]%s",PREFIX,STR_CONST,SUFFIX);
  }
  else {
    if (CONST == 0) DIGITS = 1;
    else DIGITS = ((int) (log(1.0*CONST)/log(16.0))) + 1;
    NUM_TO_STR(CONST,16,DIGITS,STR_CONST);
    if (!PRINT_INSTR) 
      (void) printf("%s%s [base 16]%s",PREFIX,STR_CONST,SUFFIX);
    else (void) printf("%s%s%s",PREFIX,STR_CONST,SUFFIX);
  }
}

/* ------------------------- PRINT_INSTRUCTION ----------------------------- */
/* Output a number as a SIC/XE instruction.                                  */
void PRINT_INSTRUCTION(int LOCATION,BOOLEAN PRINT,int *BYTES)
{
  int BIT_INSTR[4*BITS_PER_BYTE_2]; /* BIT array version of instruction.     */
  int CODE;                      /* Instruction code of instruction.         */
  int SAV_PC;                    /* SAVE PC because not executing anything   */

/* ~~~~~~~~~~~~~~~~~~~~~~~ Figure out the number of BYTES                    */
  PRINT_INSTR = TRUE_1;
  CODE = INSTR_CODE(LOCATION);
  if ((0 > LOCATION) || (MEM_SIZE_1 <= LOCATION)) {
    (*BYTES) = 0;
    if (PRINT) (void) printf("Address out of range.\n");
  } else
    if (INSTR[CODE].FORMAT < 3)
      (*BYTES) = INSTR[CODE].FORMAT;
  else {
      BYTES_TO_BITS(INT(MEMORY[LOCATION]),1,BIT_INSTR);
      BYTES_TO_BITS(INT(MEMORY[LOCATION+1]),1,&(BIT_INSTR[8]));
      BYTES_TO_BITS(INT(MEMORY[LOCATION+2]),1,&(BIT_INSTR[16]));
/* ------------------ if expended format bit on and not plane sic -----------*/
      if ((BIT_INSTR[11] == 1) && !((BIT_INSTR[6] == 0) 
				    && (BIT_INSTR[7] == 0))) {
	(*BYTES) = 4;
	BYTES_TO_BITS(INT(MEMORY[LOCATION+3]),1,&(BIT_INSTR[24]));
      }
      else (*BYTES) = 3;
    }
  if (PRINT) {
/* ~~~~~~~~~~~~~~~~~~~~~~~~~ Print out the instruction                       */
/* ---------- print location                                                 */
    PRINT_ADDRESS("======>",LOCATION,"	");

/* ---------- if extended format print out '+'                               */
    if ((*BYTES) == 4) (void) printf("+");
    else (void) printf(" ");
    (void) printf("%s	",INSTR[CODE].MNEMONIC);
    if ((*BYTES) == 1) {
      PRINT_INSTR = FALSE_1;
      return;
    }
    SAV_PC = PC_0;
    PC_0 = LOCATION;
    if ((*BYTES) == 2) {
/* ~~~~~~~~~~~~~~~~~~~~~~~~ Print out format2 arguments.                     */
      int REG1;                      /* register int bits 8-11               */
      int REG2;                      /* register int bits 12-15              */
      
      FORMAT2(&REG1,&REG2);
      PC_0 = SAV_PC;

/* ------ ONE REGISTER INSTRUCTIONS --------------------------------------- */
      if ((CODE == 45) || (CODE == 46)) {
	if (REG1 >= NUM_REGISTERS)  
	  (void) printf("illegal register\n");
	else (void) printf("%s\n", REG_NAMES[REG1]);
	PRINT_INSTR = FALSE_1;
	return;
      }
/* ----- ONE CONSTANT INSTRUCTIONS ---------------------------------------- */
      if (CODE == 44) {
	if (REG1 > 4)  
	  (void) printf("illegal argument\n");
	else (void) printf("%d\n", REG1);
	PRINT_INSTR = FALSE_1;
	return;
      }
/* ------ ONE REGISTER, ONE CONSTANT INSTRUCTIONS ------------------------- */

      if ((CODE == 41) || (CODE == 42)) {
	if (REG1 >= NUM_REGISTERS)  
	  (void) printf("illegal register,");
	else (void) printf("%s,", REG_NAMES[REG1]);
	PRINT_CONSTANT("",REG2,"\n");
	PRINT_INSTR = FALSE_1;
	return;
      }
/* ------ TWO REGISTER INSTRUCTIONS --------------------------------------- */
      if (REG1 >= NUM_REGISTERS) 
	(void) printf("illegal register,");
	else (void) printf("%s,", REG_NAMES[REG1]);
      if (REG2 >= NUM_REGISTERS)  
	  (void) printf("illegal register\n");
      else (void) printf("%s\n", REG_NAMES[REG2]);
      PRINT_INSTR = FALSE_1;
      return;
    }
/* ~~~~~~~~~~~~~~~~~~~~~~~~~ print FORMAT3/4 arguments                       */
    {
      int LOCATION;

/* -------------- Direct, immediate, or indirect.                            */
      if ((BIT_INSTR[6] == 1) && (BIT_INSTR[7] == 0)) (void) printf("@");
      else if ((BIT_INSTR[6] == 0) && (BIT_INSTR[7] == 1)) (void) printf("#");
      else (void) printf(" ");

      if ((*BYTES) == 3) {
/* --------------- Format 3 address                                          */
	LOCATION = BITS_TO_BYTE(&(BIT_INSTR[12]),12);
	if (BIT_INSTR[9] == 1) LOCATION += B_0;
	if (BIT_INSTR[10] == 1) {
	  if (LOCATION >= MAX_PC_RELATIVE_1 / 2)
	    LOCATION = (LOCATION - MAX_PC_RELATIVE_1) + PC_0 + 3;
	  else LOCATION += PC_0 + 3;
	}
      } else {
/* --------------- Format 4 address                                          */
	LOCATION = BITS_TO_BYTE(&(BIT_INSTR[12]),20);
      }     
      if (LOCATION < MEM_SIZE_1) PRINT_ADDRESS("",LOCATION,"");
      else (void) printf("invalid address");
      if (BIT_INSTR[8] == 1) (void) printf(",X");
      PC_0 = SAV_PC;
      (void) printf("\n");
      PRINT_INSTR = FALSE_1;
    }
    PRINT_INSTR = FALSE_1;
  }
}

/* ---------------------------------- EXEC --------------------------------- */
/* if DEBUG_MODE is FALSE, drives the whole simulation process, otherwise    */
/* Just does one instruction and returns.                                    */
void EXEC(BOOLEAN DEBUG_MODE,BOOLEAN PRINT_EFFECT)
{

  do {
/* ---------------------- Treats codes without assigned functions as no-ops */
    if (INSTR[INSTR_CODE(PC_0)].FUNCTION == NULL0) {
      (void) printf("%d %s ",PC_0,INSTR[INSTR_CODE(PC_0)].MNEMONIC);
      if (INSTR[INSTR_CODE(PC_0)].FORMAT == 3)
	(void) printf(" %d",FORMAT3_4(VALUE_1,3));
      else PC_0 += INSTR[INSTR_CODE(PC_0)].FORMAT;
      (void) printf("\n");
    } else 
	CALL_FUNCTION1(INSTR[INSTR_CODE(PC_0)].FUNCTION,PRINT_EFFECT);
    if (PC_0 == RETURN_TO_OS_1) RETURN_STATUS = HALT_1;
    else
      if (INTERVAL_TIMER > 0) {
	INTERVAL_TIMER --;
	if (INTERVAL_TIMER == 0) 
	  SIGNAL_INTERUPT(3,0);
      }

  } while ( !DEBUG_MODE && (RETURN_STATUS == EXECUTING_1 ));

}
