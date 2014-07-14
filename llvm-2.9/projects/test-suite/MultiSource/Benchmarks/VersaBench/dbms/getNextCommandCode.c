
/*
 *  Name:           getNextCommandCode
 *  Input:          FILE pointer
 *  Output:         command code
 *  Return:         GET_NEXT_COMMAND_CODE_SUCCESS, or
 *                  GET_NEXT_COMMAND_CODE_IO_ERROR,
 *                  GET_NEXT_COMMAND_CODE_INVALID_COMMAND
 *  Description:    Reads next command code from file and returns the command 
 *                  value.  The file input is assumed to be a DIS Benchmark 
 *                  Suite: Data Management data set file where each line of the 
 *                  file is a separate command to read.  There are five valid 
 *                  values for the command code which correspond to the 
 *                  database commands (Insert, Query, Delete, and Init) and a 
 *                  NONE type which indicates that the data file is "empty", 
 *                  i.e., all of the commands in the file have been processed.  
 *                  A command code of NONE does not indicate an error condition 
 *                  which would be returned by an error flag.
 *  Calls:          errorMessage() 
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

#include <assert.h>             /* for assert()                             */
#include <stdio.h>              /* for FILE definition                      */
#include "dataManagement.h"     /* for primitive data types                 */
#include "errorMessage.h"       /* for errorMessage() definition            */
#include "getNextCommandCode.h" /* for getNextCommandCode() return codes    */
#include "getInt.h"             /* for getInt() and return code definitions */

Int getNextCommandCode( FILE        *file,          /*  file for reading     */
                        CommandType *commandCode )  /*  code of next command */
{   /*  begin getNextCommandCode()  */
    Int returnCode;
    Int command;
    
    static Char name[] = "getNextCommandCode";

    assert( file );
    assert( commandCode );

    /*
     *  Read integer value from FILE and check for errors.  If an early 
     *  end-of-line or low-level getString failure occurred, return error 
     *  code.  If EOF is returned, assume that the file is done and return code 
     *  of appropriate value.  If valid command is read, place value in code 
     *  and return success.  If an unknown command is read, place INVALID value 
     *  in command code, clear line of unknown data, and return error.
     */
    returnCode = getInt( file, &command );
    if ( returnCode == GET_INT_SUCCESS ) {
        if ( command == INIT ) {
            *commandCode = INIT;
            returnCode   = GET_NEXT_COMMAND_CODE_SUCCESS;
        }   /*  end of if ( command == INIT ) */
        else if ( command == INSERT ) {
            *commandCode = INSERT;
            returnCode   = GET_NEXT_COMMAND_CODE_SUCCESS;
        }   /*  end of if ( command == INSERT ) */
        else if ( command == QUERY ) {
            *commandCode = QUERY;
            returnCode   = GET_NEXT_COMMAND_CODE_SUCCESS;
        }   /*  end of if ( command == QUERY ) */
        else if ( command == DELETE ) {
            *commandCode = DELETE;
            returnCode   = GET_NEXT_COMMAND_CODE_SUCCESS;
        }   /*  end of if ( command == DELETE ) */
        else {
            errorMessage( "unknown command code", REPLACE );
            errorMessage( name, PREPEND );
            *commandCode = INVALID;
            returnCode   = GET_NEXT_COMMAND_CODE_INVALID_COMMAND;
        }   /*  end of command code branches    */
    }   /*  end of if ( returnCode == GET_INT_SUCCESS ) */
    else if ( returnCode == GET_INT_EOI ) {
        *commandCode = NONE;
        returnCode   = GET_NEXT_COMMAND_CODE_SUCCESS;
    }   /*  end of if ( returnCode == GET_INT_EOI ) */
    else if ( returnCode == GET_INT_RANGE_EXCEEDED ) {
        errorMessage( "unknown command code", REPLACE );
        errorMessage( name, PREPEND );
        *commandCode = INVALID;
        returnCode   = GET_NEXT_COMMAND_CODE_INVALID_COMMAND;
    }   /*  end of if ( returnCode == GET_INT_EOF ) */
    else if ( returnCode == GET_INT_BAD_CONVERSION ) {
        errorMessage( "improper format - code must be an integer", REPLACE );
        errorMessage( name, PREPEND );
        *commandCode = INVALID;
        returnCode   = GET_NEXT_COMMAND_CODE_INVALID_COMMAND;
    }   /*  end of if ( returnCode == GET_INT_EOF ) */

    return ( returnCode );
}   /*  end of getNextCommandCode() */
