/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* ============================== interupt.c =============================== */
/* Handles interupts, rather simply.                                         */

#include "boolean.h"
#include "memory.h"
#include "machine.h"
#include "debugger.h"
#include "sim_debug.h"
#include "constants.h"

/* ------------------------------ SIGNAL_INTERUPT -------------------------- */
/* Handles a signal interupt.                                                */
void SIGNAL_INTERUPT(TYPE,CODE)
          int TYPE;                  /* TYPE/CLASS of interupt (1,2,3, or 4) */
          int CODE;                  /* Interupt code.                       */
{
  switch(TYPE) {
  case 2:
    SW_0 = ((int) SW_0 / 256) * 256 + CODE;
    RETURN_STATUS = ABNORMAL_1;
    break;
  default:
    ;
  }
}
 
