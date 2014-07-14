
/*
 *  Name:           delete
 *  Input:          root node of index, root
 *                  search index key, searchKey
 *                  search non-key list, searchNonKey
 *  Output:         updated node, node
 *  Return:         DELETE_SUCCESS, or
 *                  DELETE_INVALID_KEY_SEARCH_VALUE,
 *                  DELETE_INVALID_NON_KEY_SEARCH_VALUE
 *  Description:    The routine searches the current index and removes all data 
 *                  objects which are consistent with the search input values, 
 *                  key and non-key.  The routine makes use of the recursive 
 *                  routine deleteEntry() to perform the actual search and 
 *                  removal of data objects in the index.  The deleteEntry() 
 *                  routine will also remove empty nodes from the index all the 
 *                  way up to the root node.  After the return from 
 *                  deleteEntry(), two checks need to be made.  The first checks
 *                  to see if the current root node level is not a LEAF and the 
 *                  list of entries for the root node is empty, i.e., NULL.  
 *                  This case indicates that the input search values completely
 *                  removed every data object in the index and thus all nodes 
 *                  except the root node.  For this case, the level for the root
 *                  is reset to the LEAF level and the updated root node is 
 *                  returned.  The second case checks to see if the root node 
 *                  has only one entry in its list.  The R-Tree is required to 
 *                  replace any root node with only one child with the child 
 *                  node, unless the root is a LEAF node.
 *  Calls:          deleteEntry()
 *                  errorMessage()
 *                  validIndexKey()
 *                  validAttributes()
 *      System:     free()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                             */
#include <stdlib.h>         /* for free() and NULL definitions          */
#include "dataManagement.h" /* for primitive type definitions           */
#include "errorMessage.h"   /* for errorMessage() definition            */
#include "dataObject.h"     /* for DataAttribute definition             */
#include "index.h"          /* for IndexNode and IndexEntry definitions */
#include "indexKey.h"       /* for IndexKey definition                  */
#include "delete.h"         /* for delete() return codes                */

/*
 *  Function prototypes
 */
extern Boolean validIndexKey( IndexKey *key );
extern Boolean validAttributes( DataAttribute *attributes );
extern void deleteEntry( IndexNode *node, IndexKey *searchKey,
                         DataAttribute *searchNonKey, Boolean *adjustmentFlag );

Int delete( IndexNode **root,               /*  root node of index      */
            IndexKey *searchKey,            /*  index key search values */
            DataAttribute *searchNonKey )   /*  non-key search values   */
{   /* beginning of delete()    */
    Boolean     adjustmentFlag; /*  place-holder for deleteEntry routine     */

    static Char name[] = "delete";

    assert( root );
    assert( *root );
    assert( searchKey );
    assert( LEAF >= 0 );

    /*
     *  Check validity of search values
     */
    if ( validIndexKey( searchKey ) == FALSE ) {
        errorMessage( "invalid index key search values", REPLACE );
        errorMessage( name, PREPEND );
        return ( DELETE_INVALID_KEY_SEARCH_VALUE );
    }   /*  end validity check of key values    */
    else if ( validAttributes( searchNonKey ) == FALSE ) {
        errorMessage( "invalid non-key search values", REPLACE );
        errorMessage( name, PREPEND );
        return ( DELETE_INVALID_NON_KEY_SEARCH_VALUE );
    }   /*  end validity check of non-key values    */

    /*
     *  Call deleteEntry routine for root node which will recursively process 
     *  the entire index.  Note that the adjustment flag is passed to 
     *  deleteEntry but its not needed for the root node since adjustments would
     *  be made to the parent which the root node does not have.
     */
    deleteEntry( *root, searchKey, searchNonKey, &adjustmentFlag );

    /*
     *  Check case where level of the root node is not LEAF and there are no 
     *  entries residing on root node, i.e., entries == NULL.  This indicates 
     *  that the input search values completely removed all of the data objects 
     *  referenced by the index, which will also remove all index nodes and 
     *  entries in the index except for the root node.  Because there are no 
     *  more objects in the index, simply set the current level of the root node
     *  to be the LEAF level and continue.
     */
    if ( (*root)->level > LEAF && (*root)->entries == NULL ) {
        (*root)->level = LEAF;
    }

    /*
     *  If there is at least one entry on the current root node (root->entries 
     *  != NULL), but there is only one entry ((root->entries)->next == NULL), 
     *  then replace the root node with the only child unless the root node is a
     *  LEAF node.  This behavior is required by the R-Tree definition.  The old
     *  root node is deleted, but the entries is first set to EMPTY/NULL.  This 
     *  prevents the deleteIndexNode() routine from removing the child entry 
     *  which has become the new root.
     */
    while ( (*root)->level != LEAF &&            /* current root not LEAF     */
            (*root)->entries != NULL &&          /* at least one child entry  */
            ((*root)->entries)->next == NULL ) { /* only one child entry      */
        IndexNode *temp;    /* placeholder for old root to delete */

        temp  = (*root);                         /* save old root for removal */
        *root = ((*root)->entries)->child.node;  /* replace root with child   */
                                                 /* which is referenced by    */
                                                 /* entries                   */
        free( temp->entries );                   /* delete old entry which    */
                                                 /* referenced old root's     */
                                                 /* child which is now the    */
                                                 /* new root.                 */
        temp->entries = NULL;                    /* Need to set the value to  */
                                                 /* NULL which will prevent   */
                                                 /* the next step from        */
                                                 /* deleting the entire index */
        deleteIndexNode( temp );                 /* delete old root node      */
    }   /*  end of loop for checking number of entries <= 1 */

    return ( DELETE_SUCCESS );
}   /*  end delete()    */
