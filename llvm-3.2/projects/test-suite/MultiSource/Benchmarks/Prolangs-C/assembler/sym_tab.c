/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ================================ sym_tab.c ============================== */
/* intialize, retrieve from, and store to the symbol table. Currently        */
/* implemented with a linked list and linear search.                         */
#include "sym_tab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* -------------------------------- INIT_SYM_TAB --------------------------- */
/* Initialize the symbol table TABLE.                                        */
void INIT_SYM_TAB(SYMBOL_TABLE *TABLE)
{
  *TABLE = NULL;
}

/* ------------------------- LOOK_UP_SYMBOL -------------------------------- */
/* find a symbol in the table. Return a pointer to its entry in the table, if*/
/* found, otherwise return NULL.                                             */
struct SYMBOL_TABLE_ENTRY *LOOK_UP_SYMBOL(char MODULE[],char LABEL[],
                                          SYMBOL_TABLE *TABLE)
{
  struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;
  TABLE_ENTRY = *TABLE;
   while (TABLE_ENTRY != NULL)
     { 
       if ( !strcmp((*TABLE_ENTRY).MODULE,MODULE) &&
	    !strcmp((*TABLE_ENTRY).LABEL,LABEL)) return TABLE_ENTRY;
       TABLE_ENTRY = (*TABLE_ENTRY).NEXT;
     } 
   return NULL;
}

/* -------------------------- INSERT_IN_SYM_TAB ---------------------------- */
/* If <MODULE,LABEL> pair is not in the symbol table, put it there and       */
/* TRUE (integer 1). Otherwise return FALSE (integer 0).                     */
/* Puts onto front of the linked list.                                       */
int INSERT_IN_SYM_TAB(char *MODULE,char *LABEL,int LOCATION,enum kind TYPE,
                      SYMBOL_TABLE *TABLE)
{
  struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;

  TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE,LABEL,TABLE);
  if (TABLE_ENTRY == NULL)
    {
      TABLE_ENTRY = (struct SYMBOL_TABLE_ENTRY *) malloc(sizeof(struct SYMBOL_TABLE_ENTRY)); 
      (*TABLE_ENTRY).NEXT = *TABLE;
      (void) strcpy((*TABLE_ENTRY).MODULE, MODULE);
      (void) strcpy((*TABLE_ENTRY).LABEL, LABEL);
      (*TABLE_ENTRY).LOCATION = LOCATION;
      (*TABLE_ENTRY).LENGTH = 0;
      (*TABLE_ENTRY).TYPE = TYPE;
      *TABLE = TABLE_ENTRY;
      return 1;
    }
  else return 0;
}
