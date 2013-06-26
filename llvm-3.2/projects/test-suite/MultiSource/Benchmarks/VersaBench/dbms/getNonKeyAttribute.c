
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
 *                  '\r'. The primary reason for the routine is to copy over the
 *                  results from getString() out of the static buffer to new 
 *                  memory allocated for the attribute.
 *  Calls:          errorMessage()
 *                  getString()
 *      System:     malloc()
 *                  strcpy()
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

#include <assert.h>             /* for assert()                          */
#include <stdlib.h>             /* for malloc() and NULL definitions     */
#include <stdio.h>              /* for FILE definition                   */
#include <string.h>             /* for strcpy() and strlen() definitions */
#include "dataManagement.h"     /* for primitive type definitions        */
#include "dataObject.h"         /* for MAX_SIZE_OF_NON_KEY_ATTRIBUTE     */
#include "errorMessage.h"       /* for errorMessage() definition         */
#include "getNonKeyAttribute.h" /* for getNonKeyAttribute() return codes */

/*
 *  Function prototype
 */
extern Char *getString( FILE *file );

Int getNonKeyAttribute( FILE *file,      /*  input stream to get string from */
                        Char **value )   /*  handle to unallocated string    */
{   /*  begin getNonKeyAttribute()   */
    Char *temp;         /*  value returned from getString() */
    Int  returnCode;    /*  return code for this routine    */
    
    static Char name[] = "getNonKeyAttribute";

    assert( file );
    assert( value );

    /*
     *  Read string using special utility getString(). Check for error during 
     *  read.  If no error, allocate memory and copy over contents of value 
     *  returned.
     */
    temp = getString( file );
    if ( temp == NULL ) {
        errorMessage( name, PREPEND );
        *value = NULL;
        returnCode = GET_NON_KEY_ATTRIBUTE_EOI;
    }   /*  end of if *value == NULL */
    else {
        assert( strlen( temp ) < MAX_SIZE_OF_NON_KEY_ATTRIBUTE );
        
        *value = malloc( strlen( temp ) + 1 );      /* allocate memory */
        if ( *value == NULL ) {                     /* check for error */
            errorMessage( "allocation failure", REPLACE );
            errorMessage( name, PREPEND );
            returnCode = GET_NON_KEY_ATTRIBUTE_ALLOCATION_FAILURE;  
        }   /*  end of value == NULL */
        else {                                      /* copy over string */
            strcpy( *value, temp );
            returnCode = GET_NON_KEY_ATTRIBUTE_SUCCESS;
        }   /*  end of else - value != NULL */
    }   /*  end of if temp != NULL */
    
    return ( returnCode );
}   /*  end of getNonKeyAttribute()  */
