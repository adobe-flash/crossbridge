/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================== assem.h ================================= */

#include "constants.h"

/* MODULE_NAME                       Name of (program) module currently     */
/*                                      assembling.                         */
extern char MODULE_NAME[LABEL_SIZE_1+1];

/* MAIN_ROUTINE                      Name of (program) module declared with */
/*                                      a START (main routine).             */
extern char MAIN_ROUTINE[LABEL_SIZE_1+1];

/* START_ADDRESS                     Start Address for program execution.   */
extern int START_ADDRESS;
