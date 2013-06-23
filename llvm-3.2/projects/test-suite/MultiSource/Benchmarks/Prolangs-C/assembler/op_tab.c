/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== op_tab =================================== */
/* Defines and allows for loop up in the OPERATOR TABLE                      */
#include <stdio.h>
#include "pseudo.h"

/* ---------------- Possible formats for instructions.                       */
enum formats {ONE,TWO,THREE_FOUR,PSEUDO,NOT_FOUND};
/* ---------------- Possible operand types --------------------------------- */
enum operands 
   {NONE,REG,REG_REG,MEM,REG_NUM,NUM,PSEUDO_LOC,PSEUDO_NOLOC,PSEUDO_ADDR};

/* ---------------- Structure of the operator table                          */
struct OP_ENTRY {
  char *MNEMONIC;                /* Name of the operator                     */
  enum formats FORMAT;           /* FORMAT of this operator                  */
  char *OPCODE;                  /* OPCODE for this operator                 */
  enum operands OPERAND;         /* Number and type of operands              */
  int FUNCTION;                  /* Function to execute is PASS1. Pseudo     */
                                 /* operators only.                          */
};


#define NUM_OPCODES               59  /* number of opcodes in the table */
                                      /* NOT counting the NOT_FOUND entry */

/* table (sorted array) of keywords used in EMITID */
struct OP_ENTRY OP_TABLE[] = 
   {{"ADD",THREE_FOUR,"18",MEM,NULL},    
    {"ADDR",TWO,"90",REG_REG,NULL},
    {"AND",THREE_FOUR,"40",MEM,NULL},    
    {"BYTE",PSEUDO,"  ",PSEUDO_LOC,PSEUDO_BYTE0},
    {"CLEAR",TWO,"B4",REG,NULL},         
    {"COMP",THREE_FOUR,"28",MEM,NULL},
    {"COMPR",TWO,"A0",REG_REG,NULL},     
    {"CSECT",PSEUDO,"  ",PSEUDO_ADDR,PSEUDO_CSECT0},
    {"DIV",THREE_FOUR,"24",MEM,NULL},    {"DIVR",TWO,"9C",REG_REG,NULL},
    {"END",PSEUDO,"  ",PSEUDO_LOC,PSEUDO_END0},
    {"EQU",PSEUDO,"  ",PSEUDO_ADDR,PSEUDO_EQU0},
    {"EXTDEF",PSEUDO,"  ",PSEUDO_NOLOC,PSEUDO_EXTDEF0},
    {"EXTREF",PSEUDO,"  ",PSEUDO_NOLOC,PSEUDO_EXTREF0},
    {"HIO",ONE,"F4",NONE,NULL},          {"J",THREE_FOUR,"3C",MEM,NULL},
    {"JEQ",THREE_FOUR,"30",MEM,NULL},    {"JGT",THREE_FOUR,"34",MEM,NULL},
    {"JLT",THREE_FOUR,"38",MEM,NULL},    {"JSUB",THREE_FOUR,"48",MEM,NULL},
    {"LDA",THREE_FOUR,"00",MEM,NULL},    {"LDB",THREE_FOUR,"68",MEM,NULL},
    {"LDCH",THREE_FOUR,"50",MEM,NULL},   {"LDL",THREE_FOUR,"08",MEM,NULL},
    {"LDS",THREE_FOUR,"6C",MEM,NULL},    {"LDT",THREE_FOUR,"74",MEM,NULL},
    {"LDX",THREE_FOUR,"04",MEM,NULL},    {"LPS",THREE_FOUR,"D0",MEM,NULL},
    {"MUL",THREE_FOUR,"20",MEM,NULL},    {"MULR",TWO,"98",REG_REG,NULL},
    {"OR",THREE_FOUR,"44",MEM,NULL},     {"RD",THREE_FOUR,"D8",MEM,NULL},
    {"RESB",PSEUDO,"  ",PSEUDO_LOC,PSEUDO_RESB0},
    {"RESW",PSEUDO,"  ",PSEUDO_LOC,PSEUDO_RESW0},
    {"RMO",TWO,"AC",REG_REG,NULL},       {"RSUB",THREE_FOUR,"4C",NONE,NULL},
    {"SHIFTL",TWO,"A4",REG_NUM,NULL},    {"SHIFTR",TWO,"A8",REG_NUM,NULL},
    {"SIO",ONE,"F0",NONE,NULL},          {"SSK",THREE_FOUR,"EC",MEM,NULL},
    {"STA",THREE_FOUR,"0C",MEM,NULL},    
    {"START",PSEUDO,"  ",PSEUDO_ADDR,PSEUDO_START0},
    {"STB",THREE_FOUR,"78",MEM,NULL},    {"STCH",THREE_FOUR,"54",MEM,NULL},
    {"STI",THREE_FOUR,"D4",MEM,NULL},    {"STL",THREE_FOUR,"14",MEM,NULL},
    {"STS",THREE_FOUR,"7C",MEM,NULL},    {"STSW",THREE_FOUR,"E8",MEM,NULL},
    {"STT",THREE_FOUR,"84",MEM,NULL},    {"STX",THREE_FOUR,"10",MEM,NULL},
    {"SUB",THREE_FOUR,"1C",MEM,NULL},    {"SUBR",TWO,"94",REG_REG,NULL},
    {"SVC",TWO,"B0",NUM,NULL},           {"TD",THREE_FOUR,"E0",MEM,NULL},
    {"TIO",ONE,"F8",NONE,NULL},          {"TIX",THREE_FOUR,"2C",MEM,NULL},
    {"TIXR",TWO,"B8",REG,NULL},          {"WD",THREE_FOUR,"DC",MEM,NULL},
    {"WORD",PSEUDO,"  ",PSEUDO_LOC,PSEUDO_WORD0},
/* the next entry must always be last                                        */
    {"*END OF TABLE*",NOT_FOUND,"00",NONE,NULL}};


/* ---------------------------- LOOK_UP_OP --------------------------------- */
/* Find operator in the table. Return the entry if it is found, else return  */
/* the not found entry. Uses BINARY SEARCH.                                  */
struct OP_ENTRY *LOOK_UP_OP(char OP[])
{
  int LOW = 0,           /* LOWest element in OP_TABLE that may be the OP    */
    HIGH = NUM_OPCODES-1,/* HIGHest element that may be the OP               */
    MIDDLE;              /* (LOW+HIGH)/2... next element considering         */

  int CMP;               /* stores the result of a string comparison;        */
			 /*  0 str1 = str2, 1 str1 > str2, -1 str1 < str2    */

  MIDDLE = 0;
  while (LOW <= HIGH) 
    { 
      MIDDLE = (HIGH + LOW)/2;
      if (!(CMP = strcmp(OP_TABLE[MIDDLE].MNEMONIC,OP)))
	return &OP_TABLE[MIDDLE];
      if (CMP > 0)
	HIGH = MIDDLE - 1;
      else LOW = MIDDLE + 1;
    }
  return &OP_TABLE[NUM_OPCODES];
}
