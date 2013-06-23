
/*
 *  Name:           insert
 *  Input:          root node of index, root
 *                  new index entry to place, entry
 *                  fan or order of index, fan
 *  Output:         updated node, node
 *  Return:         INSERT_SUCCESS, or
 *                  INSERT_INSERT_ENTRY_FAILURE_FATAL,
 *                  INSERT_INSERT_ENTRY_FAILURE_NON_FATAL,
 *                  INSERT_ALLOCATION_FAILURE
 *  Description:    The insert routine places a data object into the index with 
 *                  the specified fan.  The index is input as the root node and 
 *                  the root node is returned as the output.  The insert method 
 *                  descends the tree until the LEAF level is reached.  Note 
 *                  that the leaf level is zero and the level increases as the 
 *                  tree is ascended, i.e., the root level is always greater 
 *                  than or equal to the leaf level.  The branch or node chosen 
 *                  for descent is determined by comparing the penalty for all 
 *                  possible branches and the new data object.  The branch with 
 *                  the smallest or minimum penalty is chosen.  Once the correct
 *                  level is reached, a node is chosen on that level (in the 
 *                  same manner as a branch is chosen) and the new data object 
 *                  is placed on that node.  Placement of the object may exceed 
 *                  the specified fan which causes the node to split.  The node 
 *                  split separates or partitions the union of the old entries 
 *                  of the node and the new data object/entry into two groups.  
 *                  One group is placed back onto the old node, and the other 
 *                  group is placed onto the parent of the old node, i.e., a 
 *                  sibling node is created for the second group of partitioned 
 *                  entries.  The addition of the sibling node to the parent may
 *                  cause the parent to split, etc.  This splitting may ascend 
 *                  to the root node which by definition has no parent and is a 
 *                  special case.  When the root node is split, a new root is 
 *                  created which "grows" the index tree.  The old root and the 
 *                  node split off of the old root are then placed onto the new 
 *                  root, and the new root is returned as the updated index.
 *
 *                  The insert() routine creates a new index entry for the data 
 *                  object.  Then, the routine places the new entry into the 
 *                  index using the insertEntry() subroutine to recursively 
 *                  descend the index tree.  The root node is used to begin the 
 *                  descent.  If the root node splits, the insertEntry parameter
 *                  will be non-NULL and the index tree "grows" by creating a 
 *                  new root and placing the old root and splitEntry parameter 
 *                  onto the new root.  The primary difference between the 
 *                  insert() and insertEntry() routines is that insert() is used
 *                  for the special case of the root node, while insertEntry() 
 *                  is for every other node.  The reason the implementation uses
 *                  both is because every node has a parent for splitEntry 
 *                  placements except the root node.  This special case could be
 *                  handled by a conditional check within a modified 
 *                  insertEntry() routine which would eliminate an "extra" 
 *                  subroutine since there would be no need for insert().  
 *                  However, then each node would need to be checked to see if 
 *                  it's the root and the extra links or parameters 
 *                  would unnecessarily complicate the source code.
 *  Calls:          createIndexEntry()
 *                  createIndexNode()
 *                  errorMessage()
 *                  insertEntry()
 *                  keysUnion()
 *      System:     
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                                  */
#include <stdlib.h>         /* for NULL definition                           */
#include "dataManagement.h" /* for primitive type definitions                */
#include "dataObject.h"     /* for DataObject definition                     */
#include "errorMessage.h"   /* for errorMessage() definition                 */
#include "index.h"          /* for IndexNode and IndexEntry definitions      */
#include "insertEntry.h"    /* for insertEntry() and return code definitions */
#include "insert.h"         /* for insert() return codes                     */

/*
 *  Function prototypes
 */
extern void keysUnion( IndexEntry *I, IndexKey *U );

Int insert( IndexNode **root,       /*  root node of index          */
            DataObject *dataObject, /*  object to insert into index */
            Int fan )               /*  fan or order of index       */
{   /* beginning of insert()    */
    IndexEntry  *entry;      /* entry created for data object                */
    IndexEntry  *splitEntry; /* placeholder for splitEntry for insertEntry() */
    Int         returnCode;  /* return code for insertEntry()                */

    static Char name[] = "insert";

    assert( root );
    assert( *root );
    assert( dataObject );
    assert( dataObject->attributes );
    assert( MINIMUM_FAN_SIZE > 1 );
    assert( fan >= MINIMUM_FAN_SIZE );

    /*
     *  Create index entry for data object and set reference and index key to 
     *  appropriate values.
     */
    entry = createIndexEntry();
    if ( entry == NULL ) {
        errorMessage( "can't create entry for new data object", REPLACE );
        errorMessage( name, PREPEND );
        return ( INSERT_ALLOCATION_FAILURE );
    }   /*  end of entry == NULL    */
    entry->child.dataObject = dataObject;
    entry->key.lower.T      = dataObject->attributes[ LOWER_POINT_T ].value.key;
    entry->key.lower.X      = dataObject->attributes[ LOWER_POINT_X ].value.key;
    entry->key.lower.Y      = dataObject->attributes[ LOWER_POINT_Y ].value.key;
    entry->key.lower.Z      = dataObject->attributes[ LOWER_POINT_Z ].value.key;
    entry->key.upper.T      = dataObject->attributes[ UPPER_POINT_T ].value.key;
    entry->key.upper.X      = dataObject->attributes[ UPPER_POINT_X ].value.key;
    entry->key.upper.Y      = dataObject->attributes[ UPPER_POINT_Y ].value.key;
    entry->key.upper.Z      = dataObject->attributes[ UPPER_POINT_Z ].value.key;

    /*
     *  Place new entry into index using recursive routine insertEntry() which 
     *  will descend the index until the leaf level.  Check return code for 
     *  success and error conditions.  If successful insert, then check for 
     *  splitting.
     */
    returnCode = insertEntry( *root, entry, LEAF, fan, &splitEntry );
    if ( returnCode == INSERT_ENTRY_SUCCESS ) {
        /*
         *  The new entry has been successfully inserted somewhere beneath 
         *  the root node.  If a split occurred, then splitEntry references a 
         *  new sibling node.  Since the root node has no parent, a new parent 
         *  node is created which becomes the new root node.  The old node and 
         *  splitEntry are placed onto this new root.
         */
        if ( splitEntry != NULL ) {     /*  check for root splitting  */
            IndexNode   *newRoot = NULL;

            /*
             *  Create new root node and new entry which will reference old root
             */
            newRoot = createIndexNode( (*root)->level + 1 );
            if ( newRoot == NULL ) {
                errorMessage( "can't create new root node", REPLACE );
                errorMessage( name, PREPEND );
                return ( INSERT_ALLOCATION_FAILURE );
            }   /*  end of newRoot == NULL  */

            newRoot->entries = createIndexEntry();
            if ( newRoot->entries == NULL ) {
                errorMessage( "can't create entry for old root", REPLACE );
                errorMessage( name, PREPEND );
                return ( INSERT_ALLOCATION_FAILURE );
            }   /*  end of newRoot->entries == NULL */

            /*
             *  Setup new entry for old root, which will also place old root 
             *  onto new root node.  Place splitEntry onto new root.
             */
            newRoot->entries->child.node = *root;
            keysUnion( (*root)->entries, &(newRoot->entries->key) );
            newRoot->entries->next = splitEntry;

            /*
             *  Set root to updated value
             */
            *root = newRoot;
        }   /* end of if ( splitEntry != NULL ) */
    }   /*  end of returnCode == INSERT_ENTRY_SUCCESS   */
    else if ( returnCode == INSERT_ENTRY_CHOOSE_ENTRY_FAILURE ) {
        return ( INSERT_INSERT_ENTRY_FAILURE_NON_FATAL );
    }   /*  end of returnCode == INSERT_ENTRY_CHOOSE_ENTRY_FAILURE */
    else if ( returnCode == INSERT_ENTRY_SPLIT_NODE_FATAL ) {
        return ( INSERT_INSERT_ENTRY_FAILURE_FATAL );
    }   /*  end of returnCode == INSERT_ENTRY_SPLIT_NODE_FATAL */
    else if ( returnCode == INSERT_ENTRY_SPLIT_NODE_NON_FATAL ) {
        return ( INSERT_INSERT_ENTRY_FAILURE_NON_FATAL );
    }   /*  end of returnCode == INSERT_ENTRY_SPLIT_NODE_NON_FATAL */

    return ( INSERT_SUCCESS );
}   /*  end insert()    */
