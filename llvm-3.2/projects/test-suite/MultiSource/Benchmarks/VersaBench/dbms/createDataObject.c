
/*
 *  Name:           createDataObject
 *  Input:          data object type
 *  Output:         new data object
 *  Return:         DataObject pointer, or
 *                  NULL (if allocation failed)
 *  Description:    Routine creates a data object of specified type by 
 *                  allocating appropriate amount of memory for number of 
 *                  non-key attributes.  Note that memory is not allocated for 
 *                  the individual non-key character strings since their 
 *                  specific size is not known before hand.  The values for the 
 *                  data object are also filled with default values which 
 *                  produces an index key of a hyper-point at the lowest 
 *                  possible position in the space and sets each non-key 
 *                  character string to NULL.
 *  Calls:          errorMessage()
 *      System:     assert()
 *                  malloc()
 *                  free()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                  */
#include <stdlib.h>         /* for NULL and malloc()         */
#include "dataManagement.h" /* for primitive data types      */
#include "dataObject.h"     /* for DataObject definition     */
#include "errorMessage.h"   /* for errorMessage() definition */

DataObject * createDataObject( DataObjectType dataObjectType )
{   /*  beginning of createDataObject() */
    Int             i;                  /* looping index for attributes */
    Int             numberOfAttributes; /* number of attribute by type  */
    DataObject *    dataObject;         /* data object to create        */

    static Char name[] = "createDataObject";
    
    assert( !(dataObjectType != SMALL  && \
              dataObjectType != MEDIUM && \
              dataObjectType != LARGE ) );
    assert( MIN_ATTRIBUTE_CODE < NUM_OF_KEY_ATTRIBUTES );
    assert( MIN_ATTRIBUTE_CODE < MAX_ATTRIBUTE_CODE );
    assert( MAX_ATTRIBUTE_CODE < NUM_OF_LARGE_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_SMALL_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_MEDIUM_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_LARGE_ATTRIBUTES );

    /*
     *  Create the data object
     */
    dataObject = (DataObject *)malloc( sizeof( DataObject ) );
    if ( dataObject == NULL ) {
        /*
         *  Done: memory allocation error
         */            
        errorMessage( "memory allocation failure", REPLACE );
        errorMessage( name, PREPEND );
        return ( NULL );
    }   /*  end if ( dataObject == NULL )    */

    /*
     *  Determine number of attributes based on type input
     */
    if ( dataObjectType == SMALL ) {
        dataObject->type   = SMALL;
        numberOfAttributes = NUM_OF_SMALL_ATTRIBUTES;
    }   /*  end if ( dataObjectType == SMALL )      */
    else if ( dataObjectType == MEDIUM ) {
        dataObject->type   = MEDIUM;
        numberOfAttributes = NUM_OF_MEDIUM_ATTRIBUTES;
    }   /*  end if ( dataObjectType == MEDIUM )    */
    else if ( dataObjectType == LARGE ) {
        dataObject->type   = LARGE;
        numberOfAttributes = NUM_OF_LARGE_ATTRIBUTES;
    }   /*  end if ( dataObjectType == LARGE )      */
    else {
        errorMessage( "invalid object type to create", REPLACE );
        errorMessage( name, PREPEND );
        free( dataObject );

        /*
         *  Done: data object type error
         */            
        return ( NULL );
    }   /*  end else    */
    
    /*
     *  Allocate memory for attributes
     */
    dataObject->attributes =
        (DataObjectAttribute *)malloc( numberOfAttributes * 
            sizeof( DataObjectAttribute ) );
    if ( dataObject->attributes == NULL ) {
        /*
         *  Done: memory allocation error
         */            
        errorMessage( "memory allocation failure", REPLACE );
        errorMessage( name, PREPEND );
        free( dataObject );
        return ( NULL );
    }   /* end if ( dataObject->attributes == NULL )    */

    /*
     *  Assign implausible initial key values
     */
    for ( i = MIN_ATTRIBUTE_CODE; i < NUM_OF_KEY_ATTRIBUTES; i++ ) {
        dataObject->attributes[ i ].value.key = MINIMUM_VALUE_OF_FLOAT;
    }   /*  end loop for i  */
                
    /*
     *  Assign default non-key values
     */
    for ( i = NUM_OF_KEY_ATTRIBUTES; i < numberOfAttributes; i++ ) {
        dataObject->attributes[ i ].value.nonKey = NULL;
    }   /*  end loop for i  */

    return ( dataObject );
}   /*  end of createDataObject()   */
