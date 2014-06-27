
/*
 *  Name:           splitNode
 *  Input:          index node to split, nodeToSplit
 *                  integer fan value, fan
 *                  new index entry which caused split, entry
 *  Output:         index entry of new node after splitting, splitEntry
 *  Return:         SPLIT_NODE_SUCCESS, or
 *                  SPLIT_NODE_ALLOCATION_FAILURE
 *  Description:    This routine splits and index node.  An index node needs to 
 *                  split whenever a new index entry is added to the node and 
 *                  the node is full, i.e., the current number of entries 
 *                  residing on the node is equal to the fan or order of the 
 *                  index tree.  A node split consists of dividing the current 
 *                  entries of a node and the new entry into two groups via a 
 *                  partitionEntries routine.  One group is placed onto the 
 *                  node and the other group is placed onto a new node created 
 *                  for that purpose.  A new index entry is created for the new 
 *                  node and is returned as the splitEntry in the output.  
 *                  Since both a new index node and a new index entry are 
 *                  created during the splitting process, a memory allocation 
 *                  failure is possible during the execution of this 
 *                  subroutine.  The allocation failure is fatal for most uses 
 *                  of the method, but in certain cases, splitting a leaf node 
 *                  for instance, a recovery is possible at a higher level.  
 *                  For this reason, the splitNode routine will "clean-up" 
 *                  before returning, i.e., free memory, etc.
 *  Calls:          createIndexEntry()
 *                  createIndexNode()
 *                  deleteIndexNode()
 *                  errorMessage()
 *                  keysUnion()
 *                  partitionEntries()
 *      System:     
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                             */
#include <stdlib.h>         /* for NULL definition                      */
#include "dataManagement.h" /* for primitive type definitions           */
#include "errorMessage.h"   /* for errorMessage() definition            */
#include "index.h"          /* for IndexNode and IndexEntry definitions */
#include "splitNode.h"      /* for splitNode() return codes             */

/*
 *  Function prototypes
 */
extern void partitionEntries( IndexEntry *I, Int fan, IndexEntry **A, 
                              IndexEntry **B );
extern void keysUnion( IndexEntry *I, IndexKey *U );

Int splitNode( IndexNode *nodeToSplit,      /* node to split            */
               IndexEntry *entry,           /* entry which caused split */
               Int fan,                     /* fan of index             */
               IndexEntry **splitEntry )    /* output entry of new node */
{   /* beginning of splitNode() */
    IndexEntry  *listOfEntries; /* union of entries of nodeToSplit and entry */
    IndexNode   *splitNode;     /* new split node referenced by splitEntry   */

    static Char name[] = "splitNode";

    assert( nodeToSplit );
    assert( nodeToSplit->entries );
    assert( entry );
    assert( MINIMUM_FAN_SIZE > 1 );
    assert( fan >= MINIMUM_FAN_SIZE );

    /*
     *  Create new index entry which will reference the newly created node.  
     *  Check for memory allocation failure.  Also, create new node for 
     *  partitioning.  The new node is a "sibling" of the input node, so its 
     *  level is the same.  If memory allocation failure, free index entry 
     *  allocation in previous step, place message in error buffer, and return.
     */
    *splitEntry = createIndexEntry();
    if ( *splitEntry == NULL ) {
        errorMessage( "allocation failure - split entry", REPLACE );
        errorMessage( name, PREPEND );
        return ( SPLIT_NODE_ALLOCATION_FAILURE );
    }   /*  end of if ( *splitEntry == NULL )  */
    else {
        splitNode = createIndexNode( nodeToSplit->level );
        if ( splitNode == NULL ) {
            deleteIndexEntry( *splitEntry, nodeToSplit->level );
            errorMessage( "allocation failure - sibling node", REPLACE );
            errorMessage( name, PREPEND );
            return ( SPLIT_NODE_ALLOCATION_FAILURE );
        }   /*  end of if ( splitNode == NULL )  */
    }   /*  end of split entry and node allocation  */

    /*
     *  Create a list which is the union of the current entries of the node to
     *  split and the entry which caused the split to occur.
     */
    listOfEntries           = entry;
    listOfEntries->next     = nodeToSplit->entries;
    nodeToSplit->entries    = NULL;
    splitNode->entries      = NULL;

    /*
     *  Partition entries unto old nodeToSplit and new temporary node.
     */
    partitionEntries( listOfEntries,              /*  entries to partition    */
                      fan,                        /*  fan of index            */
                      &(nodeToSplit->entries),    /*  1st partitioned group   */
                      &(splitNode->entries) );    /*  2nd partitioned group   */
    
    /*
     *  Set reference of splitEntry to new split node and adjust 
     *  key of splitEntry for new node.  Note that the key of the entry 
     *  for nodeToSplit is adjusted in the same process that called splitNode.
     */
    keysUnion( splitNode->entries, &((*splitEntry)->key) );
    (*splitEntry)->child.node = splitNode;

    return ( SPLIT_NODE_SUCCESS );
}   /*  end splitNode() */
