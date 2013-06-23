
/*
 *  Name:           insertEntry
 *  Input:          node to place new entry, node
 *                  new index entry to place, entry
 *                  level to place entry, level
 *                  fan or order of index, fan
 *  Output:         updated node, node
 *  Return:         INSERT_ENTRY_SUCCESS, or
 *                  INSERT_ENTRY_CHOOSE_ENTRY_FAILURE,
 *                  INSERT_ENTRY_SPLIT_NODE_FATAL,
 *                  INSERT_ENTRY_SPLIT_NODE_NON_FATAL
 *  Description:    Inserts entry into index starting at provided input node and
 *                  at the specified level.  If the current node is not at the  
 *                  specified level, i.e., above, the method chooseEntry() is  
 *                  used to determine which branch or entry of the current node 
 *                  to use and the insertEntry() routine is recursively called  
 *                  on that branch.  This continues until the specified level  
 *                  has been reached.  At the specified level, the entry is  
 *                  placed onto the node which may cause splitting.  If so, the 
 *                  splitNode() method is used for the split. After the return  
 *                  for a recursive call to insertEntry(), the index key for the
 *                  child is adjusted to correctly enclose the lower branches.
 *  Calls:          chooseEntry()
 *                  errorMessage()
 *                  insertEntry()
 *                  keysUnion()
 *                  splitNode()
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

#include <assert.h>         /* for assert()                                */
#include <stdlib.h>         /* for NULL definition                         */
#include "dataManagement.h" /* for primitive type definitions              */
#include "errorMessage.h"   /* for errorMessage() definition               */
#include "index.h"          /* for IndexNode and IndexEntry definitions    */
#include "splitNode.h"      /* for splitNode() and return code definitions */
#include "insertEntry.h"    /* for insertEntry() return codes              */

/*
 *  Function prototypes
 */
extern void keysUnion( IndexEntry *I, IndexKey *U );
extern IndexEntry * chooseEntry( IndexNode *node, IndexEntry *entry );

Int insertEntry( IndexNode *node,           /*  node to begin insertion     */
                 IndexEntry *entry,         /*  entry to insert into index  */
                 Int level,                 /*  level to place entry        */
                 Int fan,                   /*  fan or order of index       */
                 IndexEntry **splitEntry )  /*  possible entry after split  */
{   /* beginning of insertEntry()   */
    Int     returnCode; /* return code for various function calls */

    static Char name[] = "insertEntry";

    assert( node );
    assert( entry );
    assert( level >= LEAF );
    assert( MINIMUM_FAN_SIZE > 1 );
    assert( fan >= MINIMUM_FAN_SIZE );

    /*
     *  If the current node is not at the specified level, then choose a 
     *  branch/entry and descend until correct level is reached.
     */
    if ( node->level > level ) {
        IndexEntry *chosen;
        
        assert( node->entries != NULL );
        
        /*
         *  Since the current node is "higher" than the specified level of 
         *  insertion, a branch or entry is chosen to descend the index tree.  
         *  Note that the only way for chooseEntry() to fail is if there are no 
         *  entries residing on the current node, which indicates an invalid 
         *  index since any node whose level is greater than the LEAF level must
         *  have at least one entry for a balanced index.
         *
         *  After an entry is chosen, the insertEntry() routine is recursively 
         *  called using the chosen entry as the new node for insertion.  The 
         *  return code for the insertEntry() routine is checked for success and
         *  the three types of errors.  All errors are propogated up the 
         *  recursive call chain until the original calling routine is notified.
         */
        chosen = chooseEntry( node, entry );
        if ( chosen == NULL ) {
            errorMessage( "can't choose entry on node", REPLACE );
            errorMessage( name, PREPEND );
            return ( INSERT_ENTRY_CHOOSE_ENTRY_FAILURE );
        }
        
        returnCode = insertEntry( chosen->child.node, entry, level, fan, 
                                  splitEntry );
        if ( returnCode == INSERT_ENTRY_SUCCESS ) {
            /*
             *  The new entry has been successfully inserted somewhere beneath 
             *  the current node.  The index key is adjusted to correctly 
             *  "enclose" the lower hyper-cubes and a check is made for node 
             *  splitting.  If a split occurred, then splitEntry references a 
             *  new sibling node which should be placed onto the current node.  
             *  This may also cause splitting and that entry should be passed to
             *  the calling process.  Eventually, the splitting may reach the 
             *  root node and the index may grow (index growth is handled in the
             *  insert() routine).
             */
            keysUnion( (chosen->child.node)->entries, &chosen->key );
            
            if ( *splitEntry != NULL ) {        /*  check for node splitting  */
                IndexEntry  *lastEntry      = NULL;
                IndexEntry  *tempEntry      = NULL;
                Int         numberOfEntries = 0;
                
                /*
                 *  Determine the number of entries which currently reside on 
                 *  the node.  Also, save the last entry in the list for later 
                 *  insertion.  They are done at the same time to prevent a 
                 *  second "loop" through the index entry list.
                 */
                tempEntry = node->entries;
                lastEntry = tempEntry;
                while ( tempEntry != NULL ) {
                    numberOfEntries++;
                    lastEntry = tempEntry;
                    tempEntry = tempEntry->next;
                }   /*  end of loop for tempEntry   */

                /*
                 *  If there is room on the current node, add splitEntry to the 
                 *  entry list and set splitEntry to NULL to inform the calling 
                 *  process that no new splitting occurred.  Otherwise, split 
                 *  the current node and set splitEntry to the appropriate 
                 *  value.  Note that splitting is done via the splitNode() 
                 *  routine which may return an allocation error.  Any error 
                 *  which occurs at a level greater than the LEAF level is a 
                 *  fatal error since the index has been altered before the 
                 *  error occurs.  Since the index has been altered, no 
                 *  gaurantee can be made on the state of the index and a fatal 
                 *  error follows.
                 */
                if ( numberOfEntries < fan ) { /*  there is room on node */
                    lastEntry->next = *splitEntry;
                    *splitEntry     = NULL;
                }   /*  end of if ( numberOfEntries < fan ) */
                else { /*  no room on node - split */
                    returnCode = splitNode( node, *splitEntry, fan, &tempEntry);
                    if ( returnCode == SPLIT_NODE_SUCCESS ) {
                        *splitEntry = tempEntry;
                    }   /*  end if SPLIT_NODE_SUCCESS   */
                    else if ( returnCode == SPLIT_NODE_ALLOCATION_FAILURE ) {
                        return ( INSERT_ENTRY_SPLIT_NODE_FATAL );
                    }   /*  end if SPLIT_NODE_ALLOCATION_FAILURE    */
                }   /*  end of numberOfEntries >= fan   */
            }   /*  end of if ( *splitEntry != NULL )   */
        }   /*  end of returnCode == INSERT_ENTRY_SUCCESS   */
        else if ( returnCode == INSERT_ENTRY_CHOOSE_ENTRY_FAILURE ||
                  returnCode == INSERT_ENTRY_SPLIT_NODE_FATAL     ||
                  returnCode == INSERT_ENTRY_SPLIT_NODE_NON_FATAL ) {
            /*
             *  Any error which occurs during a recursive call to insertEntry() 
             *  will be propogated back up the recursive call "chain" until the 
             *  original calling process is informed.
             */
            return ( returnCode );
        }   /*  end of returnCode == recursive call failure */
    }   /*  end of if ( node->level > level )   */
    else {
        IndexEntry  *lastEntry      = NULL;
        IndexEntry  *tempEntry      = NULL;
        Int         numberOfEntries = 0;

        /*
         *  Determine the number of entries which currently reside on the node. 
         *  Also, save the last entry in the list for later insertion.  They are
         *  done at the same time to prevent a second "loop" through the index 
         *  entry list.
         */
        tempEntry = node->entries;
        lastEntry = tempEntry;
        while ( tempEntry != NULL ) {
            numberOfEntries++;
            lastEntry = tempEntry;
            tempEntry = tempEntry->next;
        }   /*  end of loop for tempEntry   */

        /*
         *  If there is room on the current node, add splitEntry to the entry 
         *  list and set splitEntry to NULL to inform the calling process that 
         *  no new splitting occurred.  Otherwise, split the current node and 
         *  set splitEntry to the appropriate value.  Note that splitting is 
         *  done via the splitNode() routine which may return an allocation 
         *  error.  Any error which occurs at a level greater than the LEAF 
         *  level is a fatal error since the index has been altered before the 
         *  error occurs.  Since the index has been altered, no gaurantee can be
         *  made on the state of the index and a fatal error follows.  However, 
         *  a splitNode() error which occurs for a LEAF node is non-fatal since 
         *  the index is unchanged and its state can be gauranteed.
         */
        if ( numberOfEntries < fan ) { /*  there is room on node */
            /*
             *  Note that it's possible to have an empty node at the leaf level,
             *  i.e., the node is the root, so a check must be made to ensure 
             *  that lastEntry is a valid reference.
             */
            if ( lastEntry == NULL ) {
                node->entries   = entry;
            } /* end of lastEntry == NULL */
            else {
                lastEntry->next = entry;
            } /* end of lastEntry != NULL */
            *splitEntry = NULL;
        }   /*  end of if ( numberOfEntries < fan ) */
        else { /*  no room on node - split */
            returnCode = splitNode( node, entry, fan, splitEntry );
            if ( returnCode == SPLIT_NODE_ALLOCATION_FAILURE ) {
                if ( node->level == LEAF ) {
                    errorMessage( "can't split LEAF node", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( INSERT_ENTRY_SPLIT_NODE_NON_FATAL );
                }   /*  end of if ( node->level == LEAF )   */
                else {
                    errorMessage( "can't split non-LEAF node", REPLACE );
                    errorMessage( name, PREPEND );
                    return ( INSERT_ENTRY_SPLIT_NODE_FATAL );
                }   /*  end of if ( node->level != LEAF )   */
            }   /*  end of SPLIT_NODE_ALLOCATION_FAILURE    */
        }   /*  end of if ( numberOfEntries >= fan )    */
    }   /*  end of ( node->level == level ) */

    return ( INSERT_ENTRY_SUCCESS );
}   /*  end insertEntry()   */
