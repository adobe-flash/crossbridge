/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* --------------------- Execution Return Status Codes --------------------- */
#define EXECUTING_1            0     /* Process still executing              */
#define HALT_1                 1     /* Normal Termination                   */
#define ABNORMAL_1             2     /* Adnormal Termination                 */
#define QUIT_1                 3     /* Terminate SIC/XE emulation           */

#define NUM_REGISTERS         10     /* Number of registers in machine + 1   */

/* Used for FORMAT3_4 (file format.c) to determine if returns a location or  */
/* value.                                                                    */
#define LOCATION_1             0   
#define VALUE_1                1

/* ----------------------- Values stored in CC (condition code) for =,<, & > */
#define EQUAL_1                0
#define LESS_THAN_1            1
#define GREATER_THAN_1         2

#define RETURN_TO_OS_1  16777215        /* 24 bit 2's complement value of -1 */

					  
