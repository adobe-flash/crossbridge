
/*
 *  File Name:      valid.c
 *  Purpose:        Determines the validity for both index key and non-key 
 *                  attributes.
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                   */
#include <stdlib.h>         /* for NULL definition            */
#include "dataManagement.h" /* for primitive type definitions */
#include "dataObject.h"     /* for DataAttribute definition   */
#include "errorMessage.h"   /* for errorMessage() definition  */
#include "indexKey.h"       /* for IndexKey definition        */

/*
 *
 *  Name:           validIndexKey
 *  Input:          index key, A
 *  Output:         flag indicating validity
 *  Return:         TRUE or FALSE
 *  Description:    Returns boolean value indicating if the input index key is 
 *                  valid, i.e., the lower point is lower than the upper point, 
 *                  etc.
 *  Calls:          errorMessage()
 *      System:     
 *  Author:         M.L.Rivas
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

Boolean validIndexKey( IndexKey *key )  /*  key to validate */
{   /*  beginning of validIndexKey() */
    Boolean flag = TRUE;
    static Char name[] = "validIndexKey";

    assert( key );

    /*
     *  Check that lower point is less than upper point
     */
    if ( key->lower.T >= key->upper.T ) {
        errorMessage( "Lower T > Upper T", REPLACE );
        errorMessage( name, PREPEND );
        flag = FALSE;
    }
    else if ( key->lower.X >= key->upper.X ) {
        errorMessage( "Lower X > Upper X", REPLACE );
        errorMessage( name, PREPEND );
        flag = FALSE;
    }
    else if ( key->lower.Y >= key->upper.Y ) {
        errorMessage( "Lower Y > Upper Y", REPLACE );
        errorMessage( name, PREPEND );
        flag = FALSE;
    }
    else if ( key->lower.Z >= key->upper.Z ) {
        errorMessage( "Lower Z > Upper Z", REPLACE );
        errorMessage( name, PREPEND );
        flag = FALSE;
    }
    
    return ( flag );
}   /*  end of validIndexKey()   */

/*
 *
 *  Name:           validAttributes
 *  Input:          list of attributes
 *  Output:         flag indicating validity
 *  Return:         TRUE or FALSE
 *  Description:    Returns boolean value indicating if all attributes in list 
 *                  are valid.  An attribute is valid if the code lies within 
 *                  the specified range and if key values lie within range and 
 *                  non-key values are not NULL.
 *  Calls:          errorMessage()
 *      System:     
 *  Author:         M.L.Rivas
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

Boolean validAttributes( DataAttribute *attributes ) /* attribute to validate */
{   /*  beginning of validAttributes() */
    DataAttribute *temp; /* looping variable for checking list of attributes */

    static Char name[] = "validAttributes";

    assert( MIN_ATTRIBUTE_CODE < NUM_OF_KEY_ATTRIBUTES );
    assert( MIN_ATTRIBUTE_CODE < MAX_ATTRIBUTE_CODE );
    assert( MAX_ATTRIBUTE_CODE < NUM_OF_LARGE_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_SMALL_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_MEDIUM_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_LARGE_ATTRIBUTES );
    
    /*
     *  Loop through attribute list checking each attribute.  An attribute is 
     *  checked by first checking the code.  If the code is in range, check for 
     *  key or non-key value.  If key, check float range.  If non-key, check 
     *  non-NULL.
     */
    for ( temp = attributes; temp != NULL; temp = temp->next ) {
        /*
         *  check for bad attribute code
         */
        if ( temp->code < MIN_ATTRIBUTE_CODE ||
             temp->code > MAX_ATTRIBUTE_CODE ) {
            errorMessage( "invalid attribute code", REPLACE );
            errorMessage( name, PREPEND );
            return ( FALSE );
        }
        else {
            if ( temp->code >= MIN_ATTRIBUTE_CODE && 
                 temp->code <  NUM_OF_KEY_ATTRIBUTES ) {
                /*
                 *  Check key value for range
                 */
                if ( temp->attribute.value.key < MINIMUM_VALUE_OF_FLOAT ||
                     temp->attribute.value.key > MAXIMUM_VALUE_OF_FLOAT ) {
                    errorMessage( "key value out-of-range", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( FALSE );
                }
            }
            else {
                /*
                 *  Check non-key value for non-NULL
                 */
                if ( temp->attribute.value.nonKey == NULL ) {
                    errorMessage( "non-key value set to NULL", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( FALSE );
                }
            }
        }
    }
    
    return ( TRUE );
}   /*  end of validAttributes()  */
