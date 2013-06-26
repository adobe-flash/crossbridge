/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================= constants.h =============================== */
/* A collection of commonly used constants for the assembler.                */


/* DEBUG_FLAG_1            Controls whether certain lines of the program.    */
/*                         Execution or non-execution of these lines aids in */
/*                         Debugging the program.                            */
#define DEBUG_FLAG_1            0

/* LABEL_SIZE_1            Size of the largest valid LABEL in characters     */
#define LABEL_SIZE_1            8

/* MEM_SIZE_1              Size of SIC/XE main memory in bytes               */
/*                         NOTE: 2^20 = 1048576                              */
#define MEM_SIZE_1        1048576

/* MEM_ADDR_SIZE_1         Number of BITS in an address {log2(MEM_SIZE_1)}   */
#define MEM_ADDR_SIZE_1        20

/* BITS_PER_WORD_1         Number of BITS in a WORD                          */
#define BITS_PER_WORD_1        24

/* MAX_INT_1               Biggest integer+1: 2^BITS_PER_WORD_1              */
#define MAX_INT_1        16777216

/* BITS_PER_HALFBYTE_1     Number of BITS in a Half BYTE                     */
#define BITS_PER_HALFBYTE_1         4

/* BITS_PER_BYTE_2         Number of BITS in a BYTE                          */
#define BITS_PER_BYTE_2         8

/* HEX_CHAR_PER_BYTE_1     Number of hexidecimal characters (ASCII) must be  */
/*                         used to produce a 1 BYTE value.                   */
#define HEX_CHAR_PER_BYTE_1     2

/* LARGEST_INSTRUCT_SIZE_1 Size in BITS of the the largest instruction in    */
/*                         SIC/XE. Format 4 is 32 bits                       */
#define LARGEST_INSTRUCT_SIZE_1   32

/* MAX_PC_RELATIVE_1       Smallest number that will NOT fit in a PC relative*/
/*                         address. {4096 = 2^12}                            */
#define MAX_PC_RELATIVE_1    4096

/* GLOBAL_1                Module name for Global labels -- used by LOADER.  */
#define GLOBAL_1         "_GLOBAL "

/* -------------------------- Boolean Constants ---------------------------- */
#define FALSE_1                 0
#define TRUE_1                  1


/* ------------------- Expression types (LABELS,CONSTANTS,etc) ------------- */
#define ABSOLUTE_OR_RELATIVE_1  0    /* Don't know type                      */
#define ABSOLUTE_VALUE_1        1    /* Absolute expressions                 */
#define RELATIVE_VALUE_1        2    /* Relative expressions                 */
#define EXTERN_VALUE_1          3    /* An external reference                */


