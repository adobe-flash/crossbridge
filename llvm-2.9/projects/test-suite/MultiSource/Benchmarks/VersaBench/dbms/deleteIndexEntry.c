/*
 *  Name:           deleteIndexEntry
 *  Input:          IndexEntry to delete
 *                  level where entry resides
 *  Output:         none
 *  Return:         void
 *  Description:    Routine deletes input index entry.
 *  Calls:          deleteDataObject()
 *                  deleteIndexNode()
 *                  errorMessage()
 *      System:     assert()
 *                  free()
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

#include <assert.h>         /* for assert()                    */
#include <stdlib.h>         /* for free() and NULL definitions */
#include "dataManagement.h" /* for primitive type definitions  */
#include "errorMessage.h"   /* for errorMessage() definition   */
#include "index.h"          /* for IndexEntry definition       */

void deleteIndexEntry( IndexEntry * entry,  /*  entry to delete */
                       Int level )          /*  level of entry  */
{   /*  beginning of deleteIndexEntry() */
    static Char name[] = "deleteIndexEntry";

    assert( entry );
    assert( level >= LEAF );

    /*
     *  If level is greater than a leaf, then child is a node. Assert
     *  that the child exists and then delete child.
     */
    if ( level > LEAF ) {
        assert( entry->child.node );          /* assert that child exits */
        deleteIndexNode( entry->child.node ); /* delete node child       */
    }   /*  end of if ( level > LEAF )  */
    /*
     *  If level is a leaf, then child is a data object. Assert that
     *  the child exists and then delete the child.
     */
    else if ( level == LEAF ) {
        assert( entry->child.dataObject );           /* assert child exits  */
        deleteDataObject( entry->child.dataObject ); /* delete object child */
    }   /*  end of if ( level == LEAF ) */
    /*
     *  Else, don't know what child is since level is negative which is 
     *  undefined.
     */
    else {
        errorMessage( "invalid level", REPLACE );
        errorMessage( name, PREPEND );
    }   /*  end of else */

    /*
     *  Delete entry
     */
    free( entry );
    entry = NULL;

    return;
}   /*  end of deleteIndexEntry()   */
