
/*
 *  Name:           getInsertCommand
 *  Input:          FILE pointer
 *  Output:         data object
 *  Return:         GET_INSERT_SUCCESS, or
 *                  GET_INSERT_IO_ERROR
 *                  GET_INSERT_EARLY_EOI
 *                  GET_INSERT_UNKNOWN_DATA_OBJECT_TYPE
 *                  GET_INSERT_ALLOCATION_ERROR
 *  Description:    Reads an Insert command from the input stream via FILE 
 *                  pointer.  Assumes the current stream pointer is correct, 
 *                  and returns file pointer open and current position 
 *                  immediately after command just read.  The file pointer is 
 *                  expected to be at the beginning of the Insert command 
 *                  immediately after the command code.  The Insert command 
 *                  consists of the data object type identifier and a complete 
 *                  listing of the attributes (key and non-key) for the 
 *                  object.  Each command is carrriage return delimited, i.e., 
 *                  one line per command.  The command is read by first reading 
 *                  the type, allocating the correct amount of memory for that 
 *                  type, setting a local variable to determine how many 
 *                  attributes need to be read, and then processing the line.  
 *                  An error occurs if the object is of an unknown type, any 
 *                  of the attributes are missing, or a memory allocation 
 *                  failure for the non-key attribute character sequencyes.  
 *                  For any error during the read (missing attributes, bad 
 *                  conversions, etc.), the routine will leave the current 
 *                  values of the command attribute list intact, clear the 
 *                  current line, and return the error code.  The output of the 
 *                  routine is the data object which is ready to be inserted 
 *                  into the index.
 *  Calls:          clearLine()
 *                  errorMessage()
 *                  getInt()
 *                  getKeyAttribute()
 *                  getNonKeyAttribute()
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

#include <assert.h>             /* for assert()                               */
#include <stdio.h>              /* for FILE definition                        */
#include "dataManagement.h"     /* for primitive data types                   */
#include "dataObject.h"         /* for DataObject definition                  */
#include "errorMessage.h"       /* for errorMessage() definition              */
#include "getInt.h"             /* for getInt() and return code definitions   */
#include "getKeyAttribute.h"    /* for prototype and return code definitions  */
#include "getInsertCommand.h"   /* for getInsertCommand() return codes        */
#include "getNonKeyAttribute.h" /* for prototype and return code definitions  */

Int getInsertCommand( FILE        *file,        /*  file for reading    */
                      DataObject **dataObject ) /*  data object         */
{   /*  begin getInsertCommand()    */
    Int i;                  /* looping index variable for attributes      */
    Int returnCode;         /* return code from various routines          */
    Int dataObjectType;     /* data object type read from file            */
    Int numberOfAttributes; /* number of attributes to read from file and */
                            /* based on data object type                  */

    static Char name[] = "getInsertCommand";

    assert( file );
    assert( dataObject );
    assert( MIN_ATTRIBUTE_CODE < NUM_OF_KEY_ATTRIBUTES );
    assert( MIN_ATTRIBUTE_CODE < MAX_ATTRIBUTE_CODE );
    assert( MAX_ATTRIBUTE_CODE < NUM_OF_LARGE_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_SMALL_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_MEDIUM_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_LARGE_ATTRIBUTES );

    /*
     *  The first parameter to read is the type of data object identifier.  The 
     *  value is one of three types: SMALL, MEDIUM, or LARGE.  The value is 
     *  read and the return code is checked.  An early EOF marker or low-level 
     *  error causes an error message to be inserted in buffer, clears the 
     *  line, and returns with the appropriate error code.  If all error checks 
     *  pass, the value read determines the creation of the data object and 
     *  sets the local variable, numberOfAttributes, to be used later when 
     *  processing the line.  If the type is unknown, that also causes an error 
     *  message, clears the line, and returns.
     */
    returnCode = getInt( file, &dataObjectType );
    if ( returnCode == GET_INT_SUCCESS ) {
        /*
         *  empty
         */
    }
    else if ( returnCode == GET_INT_EOI ) {
        errorMessage( "improper format - early EOI", REPLACE );
        errorMessage( name, PREPEND );
        return ( GET_INSERT_EARLY_EOI );
    }   /*  end of if ( returnCode == GET_INT_EOI ) */
    else if ( returnCode == GET_INT_RANGE_EXCEEDED ) {
        errorMessage( "unknown data object type", REPLACE );
        errorMessage( name, PREPEND );
        return ( GET_INSERT_IO_ERROR );
    }   /*  end of if ( returnCode == GET_INT_GET_STRING_FAILURE ) */
    else if ( returnCode == GET_INT_BAD_CONVERSION) {
        errorMessage( "improper format - type must be an integer", REPLACE );
        errorMessage( name, PREPEND );
        return ( GET_INSERT_IO_ERROR );
    }   /*  end of if ( returnCode == GET_INT_GET_STRING_FAILURE ) */

    /*
     *  Create proper data object from type and store number of attributes
     */
    if ( dataObjectType == SMALL ) {
        *dataObject        = createDataObject( SMALL );
        numberOfAttributes = NUM_OF_SMALL_ATTRIBUTES;
    }   /*  end of if ( dataObjectType == SMALL )   */
    else if ( dataObjectType == MEDIUM ) {
        *dataObject        = createDataObject( MEDIUM );
        numberOfAttributes = NUM_OF_MEDIUM_ATTRIBUTES;
    }   /*  end of if ( dataObjectType == MEDIUM )  */
    else if ( dataObjectType == LARGE ) {
        *dataObject        = createDataObject( LARGE );
        numberOfAttributes = NUM_OF_LARGE_ATTRIBUTES;
    }   /*  end of if ( dataObjectType == LARGE )   */
    else {
        /*
         *  Done: unknown data object type
         */
        errorMessage( "unknown data object type", REPLACE );
        errorMessage( name, PREPEND );
        return ( GET_INSERT_UNKNOWN_DATA_OBJECT_TYPE );
    }   /*  end of data object type branches    */
    
    /*
     *  Check memory allocation error for data object
     */
    if ( *dataObject == NULL ) {
        errorMessage( "allocation failure", REPLACE );
        errorMessage( name, PREPEND );
        return ( GET_INSERT_ALLOCATION_ERROR );
    }   /*  end of if ( *dataObject == NULL )   */

    /*
     *  The attributes for the data object are specified to be in the "proper" 
     *  order, i.e., by the attribute code in the DIS Benchmark Suite:Data 
     *  Management document.  This means that the first NUM_OF_KEY_ATTRIBUTE 
     *  values are key values (floats).  A fixed loop reads these values.  Each 
     *  read is followed by a check for error conditions where each error 
     *  condition causes a return with appropriate error code.
     */
    for ( i = MIN_ATTRIBUTE_CODE; i < NUM_OF_KEY_ATTRIBUTES; i++ ) {
        Float temp;
        
        returnCode = getKeyAttribute( file, &temp );
        if ( returnCode == GET_KEY_ATTRIBUTE_SUCCESS ) {
            (*dataObject)->attributes[ i ].value.key = temp;
        }   /* end of if returnCode == GET_KEY_ATTRIBUTE_SUCCESS */
        else if ( returnCode == GET_KEY_ATTRIBUTE_EOI ) {
            errorMessage( "improper format - early EOI", REPLACE );
            errorMessage( name, PREPEND );
            (*dataObject)->attributes[ i ].value.key = MINIMUM_VALUE_OF_FLOAT;
            return ( GET_INSERT_EARLY_EOI );
        }   /*  end of if returnCode == GET_KEY_ATTRIBUTE_EOI */
        else if ( returnCode == GET_KEY_ATTRIBUTE_GET_FLOAT_FAILURE ) {
            errorMessage( "low-level I/O error", REPLACE );
            errorMessage( name, PREPEND );
            (*dataObject)->attributes[ i ].value.key = MINIMUM_VALUE_OF_FLOAT;
            return ( GET_INSERT_IO_ERROR );
        }   /*  end of if returnCode == GET_KEY_ATTRIBUTE_GET_FLOAT_FAILURE */
    }   /*  end of loop for key attributes  */
    
    /*
     *  The next series of attributes are non-key values (character sequences).
     *  The number of strings to read is determined by the local variable 
     *  numberOfAttributes which is determined by the data object type 
     *  specifier read at the beginning of the line.  A fixed loop reads the 
     *  correct number of strings.  Each read is followed by a check for error 
     *  conditions where each error condition causes a return with appropriate 
     *  error code.
     */    
    for ( i = NUM_OF_KEY_ATTRIBUTES; i < numberOfAttributes; i++ ) {
        Char *temp;

        returnCode = getNonKeyAttribute( file, &temp );
        if ( returnCode == GET_NON_KEY_ATTRIBUTE_SUCCESS ) {
            (*dataObject)->attributes[ i ].value.nonKey = temp;
        }   /*  end of if ( returnCode == GET_STRING_SUCCESS ) */
        else if ( returnCode == GET_NON_KEY_ATTRIBUTE_EOI ) {
            errorMessage( "improper format - early EOI", REPLACE );
            errorMessage( name, PREPEND );
            (*dataObject)->attributes[ i ].value.nonKey = NULL;
            return ( GET_INSERT_EARLY_EOI );
        }   /*  end of returnCode == GET_NON_KEY_ATTRIBUTE_EOI */
        else if ( returnCode == GET_NON_KEY_ATTRIBUTE_ALLOCATION_FAILURE ) {
            errorMessage( "allocation failure for non-key attribute", REPLACE );
            errorMessage( name, PREPEND );
            (*dataObject)->attributes[ i ].value.nonKey = NULL;
            return ( GET_INSERT_ALLOCATION_ERROR );
        }   /*  end of returnCode == GET_NON_KEY_ATTRIBUTE_ALLOCATION_FAILURE */
    }   /*  end of loop for non-key attributes  */

    return ( GET_INSERT_SUCCESS );
}   /*  end of getInsertCommand()   */
