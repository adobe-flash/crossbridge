
/*
 *  Name:           getNonKeyAttribute
 *  Input:          FILE ptr
 *  Output:         character string handle
 *  Return:         GET_NON_KEY_ATTRIBUTE_SUCCESS, or
 *                  GET_NON_KEY_ATTRIBUTE_ALLOCATION_FAILURE,
 *                  GET_NON_KEY_ATTRIBUTE_GET_STRING_FAILURE
 *  Description:    Reads a non-key attribute from the current position of the 
 *                  file pointer.  A non-key attribute is defined to be a 
 *                  white-space delimited string. The read will not cross an 
 *                  end-of-line indicated by a new line '\n' or carriage return 
 *                  '\r'. The routine is a match for the getNonKeyAttribute() 
 *                  routine and simply calls the getFloat() routine to read the 
 *                  key value from the file.
 *  Calls:          errorMessage()
 *                  getFloat()
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

#include <assert.h>          /* for assert()                               */
#include <stdio.h>           /* for FILE definition                        */
#include "dataManagement.h"  /* for primitive type definitions             */
#include "errorMessage.h"    /* for errorMessage() definition              */
#include "getFloat.h"        /* for getFloat() and return code definitions */
#include "getKeyAttribute.h" /* for getKeyAttribute() return codes         */

Int getKeyAttribute( FILE *file,      /*  input stream to get string from */
                     Float *value )   /*  pointer to float storage        */
{   /*  begin getKeyAttribute() */
    Int  returnCode;    /* return code for this routine */
    
    static Char name[] = "getKeyAttribute";

    assert( file );
    assert( value );

    /*
     *  Read string using special utility getString(). Check for error during 
     *  read.  If no error, allocate memory and copy over contents of value 
     *  returned.
     */
    returnCode = getFloat( file, value );
    if ( returnCode == GET_FLOAT_SUCCESS ) {
        returnCode = GET_KEY_ATTRIBUTE_SUCCESS;
    }
    else if ( returnCode == GET_FLOAT_EOI ) {
        errorMessage( name, PREPEND );
        returnCode = GET_KEY_ATTRIBUTE_EOI;
    }
    else if ( returnCode == GET_FLOAT_RANGE_EXCEEDED     ||
              returnCode == GET_FLOAT_BAD_CONVERSION ) {
        errorMessage( name, PREPEND );
        returnCode = GET_KEY_ATTRIBUTE_GET_FLOAT_FAILURE;
    }

    return ( returnCode );
}   /*  end of getKeyAttribute()    */
