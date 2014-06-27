
/*
 *  Name:           deleteEntry
 *  Input:          node of index, node 
 *                  search index key, searchKey
 *                  search non-key list, searchNonKey
 *  Output:         update index node, node 
 *                  boolean key adjustment flag, adjustmentFlag
 *  Return:         void
 *  Description:    The routine recursively descends index on each branch which 
 *                  is consistent with the input search key values.  At the leaf
 *                  level, the routine removes all data objects which are 
 *                  consistent with both the input search key and non-key 
 *                  values.  The routine also removes empty nodes from the 
 *                  index.  The search over the index is performed in the same 
 *                  manner as the query() routine which checks only the key 
 *                  values for entries/branches at non-leaf nodes and both key 
 *                  and non-key values for leaf nodes.  An adjustment flag is 
 *                  used to tell upper level nodes that a change has occurred in
 *                  lower nodes.  If the flag is not set on return, no change 
 *                  occurred, i.e., no data objects or nodes removed, and no 
 *                  adjustments for the key or entry list need to be made.  If 
 *                  the flag is set on return, a change did occur and either the
 *                  index key needs to be changed or the node may need to be 
 *                  removed.
 *  Calls:          consistentKey()
 *                  consistentNonKey()
 *                  deleteEntry()
 *                  deleteIndexEntry()
 *                  keysUnion()
 *      System:     
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
#include "dataObject.h"     /* for DataAttribute definition             */
#include "errorMessage.h"   /* for errorMessage() definition            */
#include "index.h"          /* for IndexNode and IndexEntry definitions */
#include "indexKey.h"       /* for IndexKey definition                  */

/*
 *  Function prototypes
 */
extern Boolean consistentKey( IndexKey *A, IndexKey *B );
extern Boolean consistentNonKey( Char *A, Char *B );
extern void keysUnion( IndexEntry *I, IndexKey *U );

void deleteEntry( IndexNode *node,              /*  current node of index   */
                  IndexKey *searchKey,          /*  index key search values */
                  DataAttribute *searchNonKey,  /*  non-key search values   */
                  Boolean *adjustmentFlag )     /*  flag to adjust keys     */
{   /* beginning of deleteEntry() */

    assert( node );
    assert( searchKey );
    assert( adjustmentFlag );

    /*
     *  Set key adjustment flag to FALSE until adjustment is necessary through a
     *  delete or return flag.
     */
    *adjustmentFlag = FALSE;

    /*
     *  The routine is applied recursively so the current node may or may not be
     *  a leaf node.  If it is a leaf node, the child referenced by the entries 
     *  residing on the node are data objects.  If it is not a leaf node, the 
     *  child referenced by the entries are other nodes.  So if the current 
     *  level is not a leaf, recursively call the deleteEntry routine on each 
     *  consistent entry.
     */
    if ( node->level > LEAF ) {
        IndexEntry *entry;      /* temp entry for looping through list        */
        IndexEntry *prevEntry;  /* previous entry for re-linking after delete */

        /*
         *  Loop through each entry on current node and call deleteEntry() for 
         *  each consistent child node.  Note that only the key values are 
         *  available for consistency checks at any level greater than the LEAF 
         *  level.
         */
        prevEntry = NULL;           /* no previous entry for head of list */
        entry     = node->entries;  /* set current entry to head of list  */
        while ( entry != NULL ) {   /* loop through entries               */
            if ( consistentKey( &entry->key, searchKey ) == TRUE ) {
                Boolean tempAdjustFlag; /* flag to indicate adjustment in */
                                        /* child node                     */
                
                deleteEntry( entry->child.node, searchKey, searchNonKey, 
                             &tempAdjustFlag );
                
                /*
                 *  After a return from the recursive deleteEntry call, the 
                 *  index beneath this node is in one of three states: (1) No 
                 *  entries were removed, thus no key adjustment is required, 
                 *  (2) Some entries of the node were removed, but some are left
                 *  so need a key adjustment, and (3) All entries of the node 
                 *  were removed, so no key adjustment (adjust what?) and 
                 *  remove the entry (which also removes the empty node).  The 
                 *  first condition (1) does not cause any actions so there is 
                 *  no check. The second(2) and third(3) conditions do require 
                 *  actions, so they are checked and appropriate actions taken.
                 */
                if ( (entry->child.node)->entries == NULL ) { /* (3) */
                    IndexEntry *nextEntry;      /* temp storage for delete   */

                    nextEntry = entry->next;    /* save entry for re-linking */
                    deleteIndexEntry( entry,    /* delete current entry      */
                                      node->level );
                    entry     = nextEntry;      /* reset current entry       */

                    *adjustmentFlag = TRUE;     /* set adjustment flag       */

                    /*
                     *  If the deleted entry was not the head of the list, need 
                     *  to re-link the list, so set the prevEntry's next pointer
                     *  to current entry.  If the deleted entry was the head of 
                     *  the list, set the node->entries field to the current 
                     *  entry.  This allows the list to show as EMPTY if 
                     *  necessary on return.
                     */
                    if ( prevEntry != NULL ) {
                        prevEntry->next = entry;
                    }   /* end of if prevEntry != NULL */
                    else {
                        node->entries = entry;
                    }   /*  end of if prevEntry == NULL */
                }   /*  end of if entry->child.node.entries == NULL */
                else if ( tempAdjustFlag == TRUE ) { /* (2) */
                    keysUnion( (entry->child.node)->entries, &(entry->key) );
                    *adjustmentFlag = TRUE;      /* set adjustment flag */

                    /*
                     *  Loop to next entry and set previous entry.
                     */
                    prevEntry = entry;
                    entry     = entry->next;
                }   /*  end of tempAdjustFlag == TRUE   */
                else {
                    /*
                     *  Loop to next entry and set previous entry.
                     */
                    prevEntry = entry;
                    entry     = entry->next;
                }   /*  end of tempAdjustFlag == TRUE   */
            }   /*  end of branch which is consistent   */
            else {
                /*
                 *  Loop to next entry and set previous entry.
                 */
                prevEntry = entry;
                entry     = entry->next;
            }   /*  end of branch which is not consistent   */
        }   /*  end of loop for entry   */
    }   /*  end of if ( node->level > LEAF ) */
    else {
        IndexEntry *entry;      /* temp entry for looping through list        */
        IndexEntry *prevEntry;  /* previous entry for re-linking after delete */

        /*
         *  Loop through each entry on current LEAF node and delete each data 
         *  object/entry which is consistent with the input search values.  The 
         *  first consistency check is made on the key value.  If the key values
         *  are consistent, then the data object is checked for its non-key 
         *  values.  A temporary upperBound value is set to prevent out-of-range
         *  checks on the three types of data objects.
         */
        prevEntry = NULL;           /* no previous entry for head of list */
        entry     = node->entries;  /* set current entry to head of list  */
        while ( entry != NULL ) {   /* loop through entries               */
            if ( consistentKey( &entry->key, searchKey ) == TRUE ) {
                DataAttribute *temp;            /*  attribute for list loop */
                DataObject    *object;          /*  allows easier reading   */
                Int           upperBound;       /*  prevents out-of-range   */
                Boolean       acceptanceFlag;   /*  flag to output object   */

                object = entry->child.dataObject;   /* convenience */

                upperBound = 0;                             /* set upperBound */
                if ( object->type == SMALL ) {              /* to prevent     */
                    upperBound = NUM_OF_SMALL_ATTRIBUTES;   /* out-of-range   */
                }   /*  end of type == SMALL    */          /* errors when    */
                else if ( object->type == MEDIUM ) {        /* checking non-  */
                    upperBound = NUM_OF_MEDIUM_ATTRIBUTES;  /* key attributes */
                }   /*  end of type == MEDIUM   */
                else if ( object->type == LARGE ) {
                    upperBound = NUM_OF_LARGE_ATTRIBUTES;
                }   /*  end of type == LARGE    */

                /*
                 *  The loop checks each value of the non-key search list and 
                 *  compares that value for that specific attribute code to the 
                 *  value stored in the data object.  If all of the attributes 
                 *  are consistent, the flag is set to TRUE at the end of the 
                 *  loop.  If any of the attributes are not consistent, the flag
                 *  is set to FALSE and the loop exits and the next entry is 
                 *  checked.
                 */
                acceptanceFlag = TRUE;
                temp = searchNonKey;
                while ( temp != NULL && acceptanceFlag == TRUE ) {
                    if ( temp->code < upperBound ) {
                        acceptanceFlag = consistentNonKey(
                            object->attributes[ temp->code ].value.nonKey,
                            temp->attribute.value.nonKey );
                    }   /*  end of code < upperBound    */
                    temp = temp->next;
                }   /*  end of loop through non-key search value list   */

                /*
                 *  If the acceptance flag is set, the data object should be 
                 *  removed. If a data object is removed, the adjustment flag is
                 *  set which notifies the routine calling deleteEntry() for 
                 *  this node that something happened at this level which causes 
                 *  a node removal or key adjustment.  Care must be taken to 
                 *  properly re-link the node.entries list.
                 */
                if ( acceptanceFlag == FALSE ) {
                    /*
                     *  Loop to next entry and set previous entry.
                     */
                    prevEntry = entry;
                    entry     = entry->next;
                }   /*  end of acceptanceFlag == TRUE   */
                else {
                    IndexEntry *nextEntry;      /* next entry in list        */

                    nextEntry = entry->next;    /* save entry for re-linking */
                    deleteIndexEntry( entry,    /* delete current entry      */
                                      LEAF );
                    entry     = nextEntry;      /* reset current entry       */
                    *adjustmentFlag = TRUE;     /* set adjustment flag       */

                    /*
                     *  If the deleted entry was not the head of the list, need 
                     *  to re-link the list, so set the prevEntry's next pointer
                     *  to current entry.  If the deleted entry was the head of 
                     *  the list, set the node->entries field to the current 
                     *  entry.  This allows the list to show as EMPTY if 
                     *  necessary on return.
                     */
                    if ( prevEntry != NULL ) {
                        prevEntry->next = entry;
                    }   /* end of if prevEntry != NULL */
                    else {
                        node->entries = entry;
                    }   /*  end of if prevEntry == NULL */
                }   /*  end of acceptanceFlag == FALSE  */
            }   /*  end of if consistentKey == TRUE */
            else {
                /*
                 *  Loop to next entry and set previous entry.
                 */
                prevEntry = entry;
                entry     = entry->next;
            }   /*  end of if consistentKey == FALSE */
        }   /*  end of loop for entry   */
    }   /*  end of if ( node->level == LEAF )   */

    return;
}   /*  end deleteEntry() */
