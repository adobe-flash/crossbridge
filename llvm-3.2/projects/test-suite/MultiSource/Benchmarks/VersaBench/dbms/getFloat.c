
/*
 *  Name:           getFloat
 *  Input:          FILE ptr
 *  Output:         float value
 *  Return:         GET_FLOAT_SUCCESS, or
 *                  GET_FLOAT_EOL,
 *                  GET_FLOAT_EOF,
 *                  GET_FLOAT_GET_STRING_FAILURE
 *  Description:    The routine reads a space delimited float from the 
 *                  current position of the FILE pointer.  The routine uses 
 *                  getString() to obtain the character string representation 
 *                  of the float and then uses the system level strtod() to 
 *                  find the float value.  Note that the getString() routine 
 *                  does not allow the read to cross an end-of-line indicator 
 *                  as opposed to the system routines which do.  In the event 
 *                  of an error, the output value is set to an implausible 
 *                  amount, MINIMUM_VALUE_OF_FLOAT to indicate an error occurred
 *                  along with an appropriate return code.
 *  Calls:            
 *      System:     strtod()
 *                  strlen()
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
#include "getFloat.h"       /* for getFloat() return codes    */

extern int errno;

/*
 *  Function prototype
 */
extern Char *getString( FILE *file );

Int getFloat( FILE *file,       /*  FILE stream to read */
              Float *value )    /*  value to output     */
{   /*  begin getFloat()    */
    Char *temp;      /* temporary string used returned by getString()         */
    Char *endptr;    /* residual string after conversion from string to Float */
    Int  returnCode; /* return code for this routine                          */
    
    assert( file );
    assert( value );

    /*
     *  Use utility routine getString to read the string representation of the 
     *  Float.  Check return code of getString for success or errors.  On 
     *  successful read of string, use system routine strtod to convert string 
     *  to float value.  Check for errors returned from system call and range 
     *  errors.
     */
    temp = getString( file );
    if ( temp != NULL ) {
        /*
         *  A valid string was read from the input and is stored in temp
         */
        *value = strtod( temp, &endptr );
        if ( *value == 0.0 && strlen( endptr ) > 0 && errno == ERANGE ) {
            *value     = MINIMUM_VALUE_OF_FLOAT;
            returnCode = GET_FLOAT_BAD_CONVERSION;
        }   /*  end of strtod error check   */
        else {
            if ( *value < MINIMUM_VALUE_OF_FLOAT ) {
                *value     = MINIMUM_VALUE_OF_FLOAT;
                returnCode = GET_FLOAT_RANGE_EXCEEDED;
            }   /*  end of value < MINIMUM_VALUE_OF_FLOAT   */
            else if ( *value > MAXIMUM_VALUE_OF_FLOAT ) {
                *value     = MAXIMUM_VALUE_OF_FLOAT;
                returnCode = GET_FLOAT_RANGE_EXCEEDED;
            }   /*  end of value > MAXIMUM_VALUE_OF_FLOAT   */
            else {
                returnCode = GET_FLOAT_SUCCESS;
            }   /*  end of else - SUCCESS   */
        }   /*  end of else strtod did not error    */
    }   /*  end if temp != NULL */
    else {
        *value     = MINIMUM_VALUE_OF_FLOAT;
        returnCode = GET_FLOAT_EOI;
    }   /*  end if GET_STRING low-level I/O error */

    return ( returnCode );
}   /*  end of getFloat()   */

