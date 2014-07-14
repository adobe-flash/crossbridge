
/*
 *  Name:           getInitCommand
 *  Input:          FILE pointer
 *  Output:         integer fan
 *  Return:         GET_INIT_SUCCESS, or
 *                  GET_INIT_IO_ERROR,
 *                  GET_INIT_EARLY_EOI,
 *                  GET_INIT_INVALID_FAN
 *  Description:    Reads an Initialization command from the input stream via 
 *                  FILE pointer.  Assumes the current stream pointer is
 *                  correct, and returns file pointer open and current position 
 *                  immediately after command just read.  The file pointer is 
 *                  expected to be at the beginning of the Init command 
 *                  immediately after the command code.  The final position of 
 *                  the file pointer is immediately after the command, but 
 *                  before the final end-of-line indicator.  The Init command 
 *                  has a very different format than the other database commands
 *                  and the routine, so the routine is very different than the 
 *                  other command input routines.  Also, the command should only
 *                  appear once and at the beginning of the file.  The format is
 *                  an integer value which specifies the fan or order of the 
 *                  index.  The absence of the fan, an incorrect fan value (< 
 *                  MINIMUM_FAN_SIZE ), or any I/O fault causes an error to 
 *                  occur with the return of an approriate error code.
 *  Calls:          clearLine()
 *                  errorMessage()
 *                  getInt()
 *      System:     
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                             */
#include <stdio.h>          /* for FILE definition                      */
#include "dataManagement.h" /* for primitive data types                 */
#include "errorMessage.h"   /* for errorMessage() definition            */
#include "getInt.h"         /* for getInt() and return code definitions */
#include "getInitCommand.h" /* for getInitCommand() return codes        */

/*
 *  Function prototypes
 */
extern void clearLine( FILE *file );

Int getInitCommand( FILE  *file,    /*  file for reading    */
                    Int   *fan )  /*  output fan read     */
{   /*  begin getInitCommand()  */
    Int returnCode; /* return code value from getInt() */
    
    static Char name[] = "getInitCommand";

    assert( file );
    assert( fan );

    /*
     *  Read the integer value for the fan checking for error return codes.
     */
    returnCode = getInt( file, fan );
    if ( returnCode == GET_INT_SUCCESS ) {
        if ( *fan < MINIMUM_FAN_SIZE ) {
            errorMessage( "invalid fan specified", REPLACE );
            errorMessage( name, PREPEND );
            returnCode = GET_INIT_INVALID_FAN;
        }   /*  end of if ( *fan < MINIMUM_FAN_SIZE )   */
        else {
            returnCode = GET_INIT_SUCCESS;
        }   /*  end of if ( *fan >= MINIMUM_FAN_SIZE )  */
    }   /*  end of if returnCode == SUCCESS )   */
    else if ( returnCode == GET_INT_EOI ) {
        errorMessage( "improper format - early EOI", REPLACE );
        errorMessage( name, PREPEND );
        returnCode = GET_INIT_EARLY_EOI;
    }   /*  end of if ( returnCode == GET_INT_EOI ) */
    else if ( returnCode == GET_INT_RANGE_EXCEEDED ) {
        errorMessage( "invalid fan specified", REPLACE );
        errorMessage( name, PREPEND );
        returnCode = GET_INIT_INVALID_FAN;
    }   /*  end of if ( returnCode == GET_INT_GET_STRING_FAILURE ) */
    else if ( returnCode == GET_INT_BAD_CONVERSION ) {
        errorMessage( "improper format - fan must be an integer", REPLACE );
        errorMessage( name, PREPEND );
        returnCode = GET_INIT_IO_ERROR;
    }   /*  end of if ( returnCode == GET_INT_GET_STRING_FAILURE ) */

    return ( returnCode );
}   /*  end of getInitCommand() */
