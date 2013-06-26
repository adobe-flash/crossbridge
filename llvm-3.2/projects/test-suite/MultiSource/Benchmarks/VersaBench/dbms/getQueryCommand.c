
/*
 *  Name:           getQueryCommand
 *  Input:          FILE pointer
 *  Output:         index key
 *                  list of non-key data attributes
 *  Return:         GET_QUERY_SUCCESS, or
 *                  GET_QUERY_IO_ERROR,
 *                  GET_QUERY_EARLY_EOI,
 *                  GET_QUERY_INVALID_CODE_ERROR,
 *                  GET_QUERY_ALLOCATION_ERROR
 *  Description:    The routine reads a Query command from input stream via 
 *                  FILE pointer.  Assumes that the current stream pointer 
 *                  position is correct, and returns the file pointer open and 
 *                  the current position immediately after the command just 
 *                  read.  The file pointer is expected to be at the beginning 
 *                  of the Query command immediately after the command code. 
 *                  The Query command consists of a list of attribute code and 
 *                  value pairs.  An error occurs if any attribute code does 
 *                  not have an accompanying value, an attribute code is 
 *                  out-of-range as specified in the DIS Benchmark Suite: Data 
 *                  Management, or an I/O error occurs reading the values from 
 *                  the stream.  For any error during the read, the routine 
 *                  will leave the current values of the command attribute list 
 *                  intact and clear the FILE pointer to the end of the current 
 *                  line. 
 *
 *                  A Query command is made up of a list of attribute code and  
 *                  value pairs.  The total number of pairs can range from zero 
 *                  to the MAX_ATTRIBUTE_CODE.  Each command is carriage return 
 *                  delimited, i.e., one line per command.  So, to read a Query 
 *                  command, read until the end-of-line indicator is 
 *                  encountered.
 *
 *                  It is possible to have a Query command return an empty 
 *                  attribute list without producing an error.  Since missing 
 *                  attributes are defaulted to "wild-card" values, this type 
 *                  of query would logically return the entire database.  This 
 *                  type of query is a valid Query command as defined by the 
 *                  DIS Benchmark Suite: Data Management document.
 *  Calls:          errorMessage()
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
#include <stdlib.h>             /* for NULL                                   */
#include <stdio.h>              /* for FILE definition                        */
#include "dataManagement.h"     /* for primitive data types                   */
#include "dataObject.h"         /* for DataAttribute definition               */
#include "indexKey.h"           /* for IndexKey definition                    */
#include "errorMessage.h"       /* for errorMessage() definition              */
#include "getInt.h"             /* for getInt() and return code definitions   */
#include "getFloat.h"           /* for getFloat() and return code definitions */
#include "getKeyAttribute.h"    /* for prototype and return code definitions  */
#include "getNonKeyAttribute.h" /* for prototype and return code definitions  */
#include "getQueryCommand.h"    /* for getQueryCommand() return codes         */

Int getQueryCommand( FILE           *file,            /* file for reading     */
                     IndexKey       *searchKey,       /* index key            */
                     DataAttribute  **searchNonKey )  /* list of attributes   */
{   /*  begin getQueryCommand() */
    Int attributeCode; /* attribute code read from file     */
    Int returnCode;    /* return code from various routines */
    
    static Char name[] = "getQueryCommand";

    assert( file );
    assert( searchKey );
    assert( MIN_ATTRIBUTE_CODE < NUM_OF_KEY_ATTRIBUTES );
    assert( MIN_ATTRIBUTE_CODE < MAX_ATTRIBUTE_CODE );
    assert( MAX_ATTRIBUTE_CODE < NUM_OF_LARGE_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_SMALL_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_MEDIUM_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_LARGE_ATTRIBUTES );

    /*
     *  Set the values for the index key and non-key list to wild-card values.  
     *  The key values are float, and the wild-card is a hyper-cube of maximum 
     *  volume.  The non-key values are char, and the wild-card values are NULL 
     *  with an empty list.
     */
    searchKey->lower.T = MINIMUM_VALUE_OF_FLOAT;
    searchKey->lower.X = MINIMUM_VALUE_OF_FLOAT;
    searchKey->lower.Y = MINIMUM_VALUE_OF_FLOAT;
    searchKey->lower.Z = MINIMUM_VALUE_OF_FLOAT;
    searchKey->upper.T = MAXIMUM_VALUE_OF_FLOAT;
    searchKey->upper.X = MAXIMUM_VALUE_OF_FLOAT;
    searchKey->upper.Y = MAXIMUM_VALUE_OF_FLOAT;
    searchKey->upper.Z = MAXIMUM_VALUE_OF_FLOAT;
    
    *searchNonKey = NULL;

    /*
     *  A Query command is made up of a list of attribute code and value pairs. 
     *  The total number of pairs can range from zero to MAX_ATTRIBUTE_CODE.  
     *  Each command is on a separate line, i.e., an end-of-line indicator is 
     *  used to delimit commands.  So, to read the Query command, read 
     *  code/value pairs until an end-of-line indicator.  
     */
    returnCode = GET_INT_SUCCESS;
    while ( returnCode != GET_INT_EOI ) {
        /*
         *  Read the attribute code first and check return code.  If a 
         *  successful read, check range of code for error.  If code is in 
         *  range, check for key or non-key attribute.  For key attribute, read 
         *  in float value.  For non-key attribute, read in character string.  
         *  For each type of read, check for errors.
         */
        returnCode = getInt( file, &attributeCode );
        if ( returnCode == GET_INT_SUCCESS ) {
            if ( attributeCode < MIN_ATTRIBUTE_CODE || /* check code range */
                 attributeCode > MAX_ATTRIBUTE_CODE ) {
                errorMessage( "code out-of-range", REPLACE );
                errorMessage( name, PREPEND );
                return ( GET_QUERY_INVALID_CODE_ERROR );
            }   /*  end of check for invalid code   */
            else if ( attributeCode < NUM_OF_KEY_ATTRIBUTES ) {
                Float   value;

                returnCode = getKeyAttribute( file, &value );
                if ( returnCode == GET_KEY_ATTRIBUTE_SUCCESS ) {
                    if ( attributeCode == LOWER_POINT_T ) {
                        searchKey->lower.T = value;
                    }   /*  end of LOWER_POINT_T    */
                    else if ( attributeCode == LOWER_POINT_X ) {
                        searchKey->lower.X = value;
                    }   /*  end of LOWER_POINT_X    */
                    else if ( attributeCode == LOWER_POINT_Y ) {
                        searchKey->lower.Y = value;
                    }   /*  end of LOWER_POINT_Y    */
                    else if ( attributeCode == LOWER_POINT_Z ) {
                        searchKey->lower.Z = value;
                    }   /*  end of LOWER_POINT_Z    */
                    else if ( attributeCode == UPPER_POINT_T ) {
                        searchKey->upper.T = value;
                    }   /*  end of UPPER_POINT_T    */
                    else if ( attributeCode == UPPER_POINT_X ) {
                        searchKey->upper.X = value;
                    }   /*  end of UPPER_POINT_X    */
                    else if ( attributeCode == UPPER_POINT_Y ) {
                        searchKey->upper.Y = value;
                    }   /*  end of UPPER_POINT_Y    */
                    else if ( attributeCode == UPPER_POINT_Z ) {
                        searchKey->upper.Z = value;
                    }   /*  end of UPPER_POINT_Z    */
                }   /*  end of GET_KEY_ATTRIBUTE_SUCCESS */
                else if ( returnCode == GET_KEY_ATTRIBUTE_EOI ) {
                    errorMessage( "improper format - early EOI", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( GET_QUERY_EARLY_EOI );
                }   /*  end returnCode == GET_KEY_ATTRIBUTE_GET_FLOAT_FAILURE */
                else if ( returnCode == GET_KEY_ATTRIBUTE_GET_FLOAT_FAILURE ) {
                    errorMessage( "low-level I/O error", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( GET_QUERY_IO_ERROR );
                }   /*  end returnCode == GET_KEY_ATTRIBUTE_GET_FLOAT_FAILURE */
            }   /*  end of key attribute        */
            else {
                Char    *value;
                                
                returnCode = getNonKeyAttribute( file, &value );
                if ( returnCode == GET_NON_KEY_ATTRIBUTE_SUCCESS ) {
                    DataAttribute *newAttribute;
                    
                    /*
                     *  Allocate new DataAttribute, store values of attribute 
                     *  code and attribute string, and add new DataAttribute to 
                     *  output list.
                     */
                    newAttribute =
                        (DataAttribute *)malloc( sizeof( DataAttribute ) );
                    if ( newAttribute == NULL ) {
                        errorMessage( "allocation failure", REPLACE );
                        errorMessage( name, PREPEND );
                        return ( GET_QUERY_ALLOCATION_ERROR );
                    }   /*  end of if ( attribute == NULL ) */

                    newAttribute->code                   = attributeCode;
                    newAttribute->attribute.value.nonKey = value;
                    
                    newAttribute->next = *searchNonKey;
                    *searchNonKey      = newAttribute;
                }
                else if ( returnCode == GET_NON_KEY_ATTRIBUTE_EOI ){
                    errorMessage( "improper format - early EOI", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( GET_QUERY_EARLY_EOI );
                }   /*  end of if returnCode == GET_NON_KEY_ATTRIBUTE_EOI */
                else if ( returnCode==GET_NON_KEY_ATTRIBUTE_ALLOCATION_FAILURE){
                    errorMessage( "allocation of non-key attribute", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( GET_QUERY_ALLOCATION_ERROR );
                }   /*  end of if ( returnCode == ALLOCATION_FAILURE )  */
            }   /*  end of non-key attribute    */
        }   /*  end of if ( returnCode == GET_INT_SUCCESS ) */
        else if ( returnCode == GET_INT_EOI ) {
            /*
             *  empty
             */
        }   /*  end of if ( returnCode == GET_INT_EOI ) */
        else if ( returnCode == GET_INT_RANGE_EXCEEDED ) {
            return ( GET_QUERY_INVALID_CODE_ERROR );
        }   /*  end of if ( error == GET_INT_RANGE_EXCEEDED )  */
        else if ( returnCode == GET_INT_BAD_CONVERSION ) {
            return ( GET_QUERY_INVALID_CODE_ERROR );
        }   /*  end of if ( error == GET_INT_BAD_CONVERSION )  */
    }

    return ( GET_QUERY_SUCCESS );
}   /*  end of getQueryCommand()    */
