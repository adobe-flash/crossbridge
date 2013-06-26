/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/*============================ define-instr-tab.h ======================= */
/* A table for quick (constant time) look up of MNEMONIC, FORMAT, and     */
/* the function that knows what to do with an instruction give its code.  */

struct INSTR_INFO {
    char *MNEMONIC;
      int FORMAT;
      int FUNCTION;
  };


#define LDA_P0 22
#define LDX_P0 1
#define LDL_P0 2
#define STA_P0 3
#define STX_P0 4
#define STL_P0 5
#define ADD_P0 6
#define SUB_P0 7
#define MUL_P0 8
#define DIV_P0 9
#define COMP_P0 10
#define TIX_P0 11
#define JEQ_P0 12
#define JGT_P0 13
#define JLT_P0 14
#define J_P0 15
#define AND_P0 16
#define OR_P0 17
#define JSUB_P0 18
#define RSUB_P0 19
#define LDCH_P0 20
#define STCH_P0 21
#define NULL0 0
#define LDB_P0 23
#define LDS_P0 24
#define LDT_P0 25
#define STB_P0 26
#define STS_P0 27
#define STT_P0 28
#define ADDR_P0 29
#define SUBR_P0 30
#define MULR_P0 31
#define DIVR_P0 32
#define COMPR_P0 33
#define SHIFTL_P0 34
#define SHIFTR_P0 35
#define RMO_P0 36
#define SVC_P0 37
#define CLEAR_P0 38
#define TIXR_P0 39
#define STI_P0 40
#define RD_P0 41
#define WD_P0 42
#define TD_P0 43
#define STSW_P0 44


#define INSTR_TAB_SIZE_1         64
struct INSTR_INFO INSTR[INSTR_TAB_SIZE_1] =
  {{"LDA",3,LDA_P0},        /* Opcode = 00                                  */
   {"LDX",3,LDX_P0},        /* Opcode = 04                                  */
   {"LDL",3,LDL_P0},        /* Opcode = 08                                  */
   {"STA",3,STA_P0},        /* Opcode = 0C                                  */
   {"STX",3,STX_P0},        /* Opcode = 10                                  */
   {"STL",3,STL_P0},        /* Opcode = 14                                  */
   {"ADD",3,ADD_P0},        /* Opcode = 18                                  */
   {"SUB",3,SUB_P0},        /* Opcode = 1C                                  */
   {"MUL",3,MUL_P0},        /* Opcode = 20                                  */
   {"DIV",3,DIV_P0},        /* Opcode = 24                                  */
   {"COMP",3,COMP_P0},      /* Opcode = 28                                  */
   {"TIX",3,TIX_P0},        /* Opcode = 2C                                  */
   {"JEQ",3,JEQ_P0},        /* Opcode = 30                                  */
   {"JGT",3,JGT_P0},        /* Opcode = 34                                  */
   {"JLT",3,JLT_P0},        /* Opcode = 38                                  */
   {"J",3,J_P0},            /* Opcode = 3C                                  */
   {"AND",3,AND_P0},        /* Opcode = 40                                  */
   {"OR",3,OR_P0},          /* Opcode = 44                                  */
   {"JSUB",3,JSUB_P0},      /* Opcode = 48                                  */
   {"RSUB",3,RSUB_P0},      /* Opcode = 4C                                  */
   {"LDCH",3,LDCH_P0},      /* Opcode = 50                                  */
   {"STCH",3,STCH_P0},      /* Opcode = 54                                  */
   {"ADDF",3,NULL0},        /* Opcode = 58                                  */
   {"SUBF",3,NULL0},        /* Opcode = 5C                                  */
   {"MULF",3,NULL0},        /* Opcode = 60                                  */
   {"DIVF",3,NULL0},        /* Opcode = 64                                  */
   {"LDB",3,LDB_P0},        /* Opcode = 68                                  */
   {"LDS",3,LDS_P0},        /* Opcode = 6C                                  */
   {"LDF",3,NULL0},         /* Opcode = 70                                  */
   {"LDT",3,LDT_P0},        /* Opcode = 74                                  */
   {"STB",3,STB_P0},        /* Opcode = 78                                  */
   {"STS",3,STS_P0},        /* Opcode = 7C                                  */
   {"STF",3,NULL0},         /* Opcode = 80                                  */
   {"STT",3,STT_P0},        /* Opcode = 84                                  */
   {"COMPF",3,NULL0},       /* Opcode = 88                                  */
   {"UNDEF",1,NULL0},       /* Opcode = 8C                                  */
   {"ADDR",2,ADDR_P0},      /* Opcode = 90                                  */
   {"SUBR",2,SUBR_P0},      /* Opcode = 94                                  */
   {"MULR",2,MULR_P0},      /* Opcode = 98                                  */
   {"DIVR",2,DIVR_P0},      /* Opcode = 9C                                  */
   {"COMPR",2,COMPR_P0},    /* Opcode = A0                                  */
   {"SHIFTL",2,SHIFTL_P0},  /* Opcode = A4                                  */
   {"SHIFTR",2,SHIFTR_P0},  /* Opcode = A8                                  */
   {"RMO",2,RMO_P0},        /* Opcode = AC                                  */
   {"SVC",2,SVC_P0},        /* Opcode = B0                                  */
   {"CLEAR",2,CLEAR_P0},    /* Opcode = B4                                  */
   {"TIXR",2,TIXR_P0},      /* Opcode = B8                                  */
   {"UNDEF",1,NULL0},       /* Opcode = BC                                  */
   {"FLOAT",1,NULL0},       /* Opcode = C0                                  */
   {"FIX",1,NULL0},         /* Opcode = C4                                  */
   {"NORM",1,NULL0},        /* Opcode = C8                                  */
   {"UNDEF",1,NULL0},       /* Opcode = CC                                  */
   {"LPS",3,NULL0},         /* Opcode = D0                                  */
   {"STI",3,STI_P0},        /* Opcode = D4                                  */
   {"RD",3,RD_P0},          /* Opcode = D8                                  */
   {"WD",3,WD_P0},          /* Opcode = DC                                  */
   {"TD",3,TD_P0},          /* Opcode = E0                                  */
   {"UNDEF",1,NULL0},       /* Opcode = E4                                  */
   {"STSW",3,STSW_P0},      /* Opcode = E8                                  */
   {"SSK",3,NULL0},         /* Opcode = EC                                  */
   {"SIO",1,NULL0},         /* Opcode = F0                                  */
   {"HIO",1,NULL0},         /* Opcode = F4                                  */
   {"TIO",1,NULL0},         /* Opcode = F8                                  */
   {"UNDEF",1,NULL0}};      /* Opcode = FC                                  */
