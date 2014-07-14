/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== pseudo.h ================================= */
/* Does most of the 'real' work of pass1. Contains the routines for          */
/* processing each of the pseudo operators.                                  */
/* Each of the procedures have the same paramters because they are all called*/
/* by the same invocation (by use of a function pointer). Thus may of the    */
/* procedures have parameters they never use.                                */

/* ------------------------------- PSEUDO_BYTE ----------------------------- */
/* 5 parameters:                                                             */
/* ************** LABEL_NAME not used */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/* ************** SYMB_TAB not used */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the BYTE pseudo operator.                                         */
extern void PSEUDO_BYTE();
#define PSEUDO_BYTE0 1

/* --------------------------- PSEUDO_CSECT -------------------------------- */
/* 5 parameters:                                                             */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/* ************** ARGUMENTS not used */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the CSECT pseudo operator.                                        */
extern void PSEUDO_CSECT();
#define PSEUDO_CSECT0 2

/* --------------------------- PSEUDO_END ---------------------------------- */
/* ************** LABEL_NAME not used */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the END pseudo operator.                                          */
extern void PSEUDO_END();
#define PSEUDO_END0 3

/* --------------------------- PSEUDO_EQU ---------------------------------- */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the EQU pseudo operator.                                          */
extern void PSEUDO_EQU();
#define PSEUDO_EQU0 4

/* --------------------------- PSEUDO_EXTDEF ------------------------------- */
/* ************** LABEL_NAME not used */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/* ************** LOCATION not used */
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/* ************** SYM_TAB not used */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the EXTDEF pseudo operator.                                       */
extern void PSEUDO_EXTDEF();
#define PSEUDO_EXTDEF0 5

/* --------------------------- PSEUDO_EXTREF ------------------------------- */
/* ************** LABEL_NAME not used */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/* ************** LOCATION not used */
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the EXTREF pseudo operator.                                       */
extern void PSEUDO_EXTREF();
#define PSEUDO_EXTREF0 6

/* --------------------------- PSEUDO_RESB --------------------------------- */
/* ************** LABEL_NAME not used */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the RESB pseudo operator.                                         */
extern void PSEUDO_RESB();
#define PSEUDO_RESB0 7

/* --------------------------- PSEUDO_RESW --------------------------------- */
/* ************** LABEL_NAME not used */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the RESW pseudo operator.                                         */
extern void PSEUDO_RESW();
#define PSEUDO_RESW0 8

/* --------------------------- PSEUDO_START =------------------------------- */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the START pseudo operator.                                        */
extern void PSEUDO_START();
#define PSEUDO_START0 9

/* --------------------------- PSEUDO_WORD --------------------------------- */
/* ************** LABEL_NAME not used */
/*    1) char *LABEL_NAME;             LABEL found on source line.           */
/*    2) char *ARGUMENTS;              Everything after the operand in source*/
/*    3) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    4) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    5) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* Process the WORD pseudo operator.                                         */
extern void PSEUDO_WORD();
#define PSEUDO_WORD0 10

/* ---------------------------- DO_PSEUDO ---------------------------------- */
/*    1) int WHICH_PSEUDO;             Which pseudo func to execute.         */
/*    2) char *LABEL_NAME;             LABEL found on source line.           */
/*    3) char *ARGUMENTS;              Everything after the operand in source*/
/*    4) int *LOCATION;                What location counter was when this   */
/*                                     line of source was found.             */
/*    5) SYMBOL_TABLE *SYM_TAB;        Symbol table                          */
/*    6) FILE *OUTPUT_STREAM;          Stream for the output.                */
/* execute a pseudo function                                                 */
extern void DO_PSEUDO();
