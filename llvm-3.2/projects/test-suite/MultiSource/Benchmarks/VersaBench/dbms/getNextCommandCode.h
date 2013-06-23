
/*
 *  DIS Data Management: getNextCommandCode
 *
 *      This header file defines the command types for each of the 
 *      database commands, and the return codes for the routine are provided 
 *      as #define's.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_GET_NEXT_COMMAND_CODE_H
#define     DIS_GET_NEXT_COMMAND_CODE_H

#include <stdio.h>          /* for FILE definition      */
#include "dataManagement.h" /* for primitive data types */

/*
 *  Command Types 
 *      - values defined by DIS Benchmark Suite Specification
 *      - used during input of index commands
 *      - includes a value for NONE indicating data set finished
 */
typedef enum {
    INIT    = 0,
    INSERT  = 1,
    QUERY   = 2,
    DELETE  = 3,
    NONE    = 4,  /* termination specifier, i.e., no command left */
    INVALID = 5   /* placeholder for invalid command              */
} CommandType;

/*
 *  Return codes
 */
#define GET_NEXT_COMMAND_CODE_SUCCESS           0
#define GET_NEXT_COMMAND_CODE_IO_ERROR          1
#define GET_NEXT_COMMAND_CODE_INVALID_COMMAND   2

/*
 *  Function Prototype
 */
extern Int getNextCommandCode( FILE *file, CommandType *commandCode );

#endif  /*  DIS_GET_NEXT_COMMAND_CODE_H */
