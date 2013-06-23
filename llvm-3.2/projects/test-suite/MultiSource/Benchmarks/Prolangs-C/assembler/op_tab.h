/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================ op_tab.h =================================== */

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

/* ---------------------------- LOOK_UP_OP --------------------------------- */
/* 1 parameter:                                                              */
/*    1) char OP[];                 Name of the OPerator to find.            */
/* Find operator in the table. Return the entry if it is found, else return  */
/* the not found entry. Uses BINARY SEARCH.                                  */
extern struct OP_ENTRY *LOOK_UP_OP();

