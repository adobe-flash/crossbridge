/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ========================= sym_tab.h ===================================== */
#include "constants.h"

/* kind                   What kind/type of symbol is it.                    */
enum kind {RELATIVE,ABSOLUTE,EXTERN_REF,MODULE,GLOBAL,UNDEFINED};

/* SYMBOL_TABLE_ENTRY     type of a symbol table entry. Part of a linked list*/
typedef struct SYMBOL_TABLE_ENTRY {
  char LABEL[LABEL_SIZE_1+1];       /* Name of the symbol.                   */
  char MODULE[LABEL_SIZE_1+1];      /* [Program] Module the symbol was found */
                                    /*   int.                                */
  int  LOCATION;                    /* Location in SICs memory this label    */
                                    /*    represents. For module names it    */
                                    /*    is the starting address. For       */
                                    /*    constants it is the value.         */
  int LENGTH;                       /* Only for module names, the length of  */
                                    /*    the module.                        */
  enum kind TYPE;                   /* kind/type of label (relative, ...)    */
  struct SYMBOL_TABLE_ENTRY *NEXT;  /* Pointer to next entry in the list     */
} *SYMBOL_TABLE;

/* -------------------------------- INIT_SYM_TAB --------------------------- */
/* 1 parameter:                                                              */
/*   1) SYMBOL_TABLE *TABLE;                                                 */
/* Initialize the symbol table TABLE.                                        */
extern void INIT_SYM_TAB();

/* ------------------------- LOOK_UP_SYMBOL -------------------------------- */
/* 3 parameters:                                                             */
/*   1) char MODULE[];                   /* Name of module label found it.   */
/*   2) char LABEL[];                    /* Label to look for.               */
/*   3) SYMBOL_TABLE *TABLE;             /* Table to look in.                */
/* find a symbol in the table. Return a pointer to its entry in the table, if*/
/* found, otherwise return NULL.                                             */
extern struct SYMBOL_TABLE_ENTRY *LOOK_UP_SYMBOL();

/* -------------------------- INSERT_IN_SYM_TAB ---------------------------- */
/* 5 parameters:                                                             */
/*  1) char *LABEL;                    Label to add.                         */
/*  2) char *MODULE;                   Module that this label was found in.  */
/*  3) int  LOCATION;                  What the entries LOCATION field       */
/*                                        should be set to.                  */
/*  4)  enum kind TYPE;                What the entries TYPE field should be */
/*                                        set to.                            */
/*  5) SYMBOL_TABLE *TABLE;            Table to add <MODULE,LABEL> to.       */
/* If <MODULE,LABEL> pair is not in the symbol table, put it there and       */
/* TRUE (integer 1). Otherwise return FALSE (integer 0).                     */
/* Puts onto front of the linked list.                                       */
extern int INSERT_IN_SYM_TAB();


