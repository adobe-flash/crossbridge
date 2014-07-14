
/*
 *  Name:           getInt
 *  Input:          FILE ptr
 *  Output:         integer value read
 *  Return:         GET_INT_SUCCESS, or
 *                  GET_INT_GET_STRING_FAILURE
 *  Description:    The routine reads a space delimited integer from the 
 *                  current position of the FILE pointer.  The routine uses 
 *                  getString() to obtain the character string representation 
 *                  of the integer and then uses the system level strtol() to 
 *                  find the integer value.  Note that the getString() routine 
 *                  does not allow the read to cross an end-of-line indicator 
 *                  as opposed to the system routines which do.  In the event 
 *                  of an error, the output value is set to an implausible 
 *                  amount, MINIMUM_VALUE_OF_INT, to indicate an error occurred
 *                  along with an appropriate return code.
 *  Calls:          
 *      System:     
 *                  strlen()
 *                  strtol()
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

#include <assert.h>         /* for assert()                   */
#include <errno.h>          /* for extern errno definition    */
#include <stdlib.h>         /* for NULL definition            */
#include <stdio.h>          /* for FILE definition            */
#include <string.h>         /* for strlen() definition        */
#include "dataManagement.h" /* for primitive type definitions */
#include "getInt.h"         /* for getInt() return codes      */

extern int errno;

/*
 *  Function prototype
 */
extern Char *getString( FILE *file );

Int getInt( FILE *file,     /*  FILE stream to read */
            Int *value )    /*  value to output     */
{   /*  begin getInt()  */
    Char *temp;         /* temporary string returned by getString()    */
    Char *endptr;       /* residual from conversion from string to int */
    Int  returnCode;    /* return code for this routine                */

    assert( file );
    assert( value );

    /*
     *  Use utility routine getString to read the string representation of the 
     *  Int.  Check return code of getString for success or errors.  On 
     *  successful read of string, use system routine strtod to convert string 
     *  to integer value.  Check for errors returned from system call and range 
     *  errors.
     */
    temp = getString( file );
    if ( temp != NULL ) {
        /*
         *  A valid string was read from the input and is reference by temp
         */
        *value = strtol( temp, &endptr, 0 );
        if ( *value == 0 && strlen( endptr ) > 0 && errno == ERANGE ) {
            *value     = MINIMUM_VALUE_OF_INT;
            returnCode = GET_INT_BAD_CONVERSION;
        }   /*  end of strtol error check   */
        else {
            if ( *value < MINIMUM_VALUE_OF_INT ) {
                *value     = MINIMUM_VALUE_OF_INT;
                returnCode = GET_INT_RANGE_EXCEEDED;
            }   /*  end of value < MINIMUM_VALUE_OF_INT   */
            else if ( *value > MAXIMUM_VALUE_OF_INT ) {
                *value     = MAXIMUM_VALUE_OF_INT;
                returnCode = GET_INT_RANGE_EXCEEDED;
            }   /*  end of value > MAXIMUM_VALUE_OF_INT   */
            else {
                returnCode = GET_INT_SUCCESS;
            }   /*  end of else - SUCCESS   */
        }   /*  end of else strtol did not error    */
    }   /*  end if temp != NULL */
    else {
        *value     = MINIMUM_VALUE_OF_INT;
        returnCode = GET_INT_EOI;
    }   /*  end if GET_STRING end-of-input */

    return ( returnCode );
}   /*  end of getInt() */
