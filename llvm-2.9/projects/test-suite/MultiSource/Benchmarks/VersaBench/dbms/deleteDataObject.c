
/*
 *  Name:           deleteDataObject
 *  Input:          DataObject to delete
 *  Output:         none
 *  Return:         void
 *  Description:    Routine deletes given data object, including all non-key 
 *                  character sequences.
 *  Calls:          
 *      System:     assert()
 *                  free()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *  23Jun99 Matthew Rivas   Added initializer to numberOfAttributes to prevent
 *                          warning message during compilation
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                                  */
#include <stdlib.h>         /* for free() and NULL definitions               */
#include "dataManagement.h" /* for primitive type definitions                */
#include "dataObject.h"     /* for DataObject and DataObjectType definitions */

void deleteDataObject( DataObject * dataObject )    /* object to delete */
{   /*  beginning of deleteDataObject() */
    Int i;                  /* looping index for attributes                  */
    Int numberOfAttributes; /* number of attributes specified by object type */

    assert( dataObject );
    assert( !( dataObject->type != SMALL  && \
               dataObject->type != MEDIUM && \
               dataObject->type != LARGE ) ); 
    assert( MIN_ATTRIBUTE_CODE < NUM_OF_KEY_ATTRIBUTES );
    assert( MIN_ATTRIBUTE_CODE < MAX_ATTRIBUTE_CODE );
    assert( MAX_ATTRIBUTE_CODE < NUM_OF_LARGE_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_SMALL_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_MEDIUM_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_LARGE_ATTRIBUTES );

    /*
     *  Determine number of attributes to delete based on type
     */
    numberOfAttributes = 0;
    if ( dataObject->type == SMALL ) {
        numberOfAttributes = NUM_OF_SMALL_ATTRIBUTES;
    }   /*  end if ( dataObjectType == SMALL )      */
    else if ( dataObject->type == MEDIUM ) {
        numberOfAttributes = NUM_OF_MEDIUM_ATTRIBUTES;
    }   /*  end if ( dataObjectType == MEDIUM )    */
    else if ( dataObject->type == LARGE ) {
        numberOfAttributes = NUM_OF_LARGE_ATTRIBUTES;
    }   /*  end if ( dataObjectType == LARGE )      */
    
    /*
     *  Delete the object's non-key attribute values.  The number of values is 
     *  based on the data object type.
     */
    for ( i = NUM_OF_KEY_ATTRIBUTES; i < numberOfAttributes; i++ ) {
        if ( dataObject->attributes[ i ].value.nonKey != NULL ) {
            free( dataObject->attributes[ i ].value.nonKey );
        }   /*  end of if ( dataObject->attributes[ i ].nonKey != NULL )    */
    }   /*  end of loop for i   */

    /*
     *  Delete data object
     */
    free( dataObject->attributes );
    free( dataObject );
    dataObject = NULL;

    return;
}   /*  end of deleteDataObject() */
