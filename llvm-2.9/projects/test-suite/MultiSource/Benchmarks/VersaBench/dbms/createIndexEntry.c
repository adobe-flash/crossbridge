
/*
 *  Name:           createIndexEntry
 *  Input:          none
 *  Output:         new index entry
 *  Return:         IndexEntry pointer, or
 *                  NULL (if allocation failed)
 *  Description:    Routine creates new index entry using system allocation 
 *                  routine.  Returns pointer to new index entry or NULL if 
 *                  allocation failed.  The value for the child reference is 
 *                  set to NULL for later assignment and the index entry is set 
 *                  to the largest possible volume.
 *  Calls:          errorMessage()
 *      System:     malloc()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <stdlib.h>       /* for NULL and malloc() definitions */
#include "errorMessage.h" /* for errorMessage() definition     */
#include "index.h"        /* for IndexEntry definition         */

IndexEntry * createIndexEntry( void )
{   /*  beginning of createIndexEntry() */
    IndexEntry * entry; /* entry to create */

    static Char name[] = "createIndexEntry";

    entry = NULL;
    
    /*
     *  Allocate memory for new entry
     */
    entry = (IndexEntry *)malloc( sizeof( IndexEntry ) );
    if ( entry == NULL ) {
        errorMessage( "allocation failure", REPLACE );
        errorMessage( name, PREPEND );
    }   /*  end of if ( entry == NULL ) */
    else {
        /*
         *  Set values of entry
         *      - the pointer to the child reference is set to NULL
         *      - the index key values are set to the largest
         *        possible hyper-cube possible.  This give a good
         *        probability that uninitialized key values will
         *        be noticed in the case of an error. 
         */
        entry->child.node       = NULL;
        entry->child.dataObject = NULL;
        
        entry->key.lower.T      = MINIMUM_VALUE_OF_FLOAT;
        entry->key.lower.X      = MINIMUM_VALUE_OF_FLOAT;
        entry->key.lower.Y      = MINIMUM_VALUE_OF_FLOAT;
        entry->key.lower.Z      = MINIMUM_VALUE_OF_FLOAT;
        entry->key.upper.T      = MAXIMUM_VALUE_OF_FLOAT;
        entry->key.upper.X      = MAXIMUM_VALUE_OF_FLOAT;
        entry->key.upper.Y      = MAXIMUM_VALUE_OF_FLOAT;
        entry->key.upper.Z      = MAXIMUM_VALUE_OF_FLOAT;
        
        entry->next             = NULL;
    }   /*  end of entry default assignment */

    return ( entry );
}   /*  end of createIndexEntry()   */
