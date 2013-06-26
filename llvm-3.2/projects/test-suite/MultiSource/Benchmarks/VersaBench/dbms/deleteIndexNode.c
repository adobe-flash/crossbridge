/*
 *  Name:           deleteIndexNode
 *  Input:          IndexNode to delete
 *  Output:         none
 *  Return:         void
 *  Description:    Routine deletes input node. Recursively descends all 
 *                  children of node to allow deletion of branches.
 *  Calls:          deleteIndexEntry()
 *      System:     free()
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

#include <assert.h>  /* for assert() debug check        */
#include <stdlib.h>  /* for NULL and free() definitions */
#include "index.h"   /* for IndexNode definitions       */

void deleteIndexNode( IndexNode * node )    /*  node to delete  */
{   /*  beginning of deleteIndexNode()  */
    IndexEntry * entry; /* entry used for looping */

    assert( node );

    /*
     *  Delete the entries which reside on the node
     */
    entry = node->entries;
    while ( entry != NULL ) {
        IndexEntry * temp;
        
        /*
         *  For each entry, save next entry in list for next
         *  loop, assert that the level of the current node
         *  is a valid value, i.e., >= LEAF level, and delete
         *  entry.  After deletion, setup entry value for
         *  next loop. 
         */
        temp = entry->next;
        
        assert( node->level >= LEAF );

        deleteIndexEntry( entry, node->level );
        
        entry = temp;
    }   /*  end while ( entry != NULL )  */

    /*
     *  Delete node
     */
    free( node );
    node = NULL;

    return;
}   /*  end of deleteIndexNode()    */
