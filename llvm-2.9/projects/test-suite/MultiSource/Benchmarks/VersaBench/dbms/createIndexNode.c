
/*
 *  Name:           createIndexNode
 *  Input:          integer level
 *  Output:         new index node
 *  Return:         IndexNode pointer, or
 *                  NULL (if allocation failed)
 *  Description:    Routine creates new node using system allocation routine, 
 *                  malloc.  Returns pointer to new node or NULL if allocation 
 *                  failed.  The level of the new node is input and stored in 
 *                  new node and the list of index entries is specified to be 
 *                  empty.
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

#include <stdlib.h>         /* for malloc() and NULL definitions */
#include "dataManagement.h" /* for primitive type definitions    */
#include "errorMessage.h"   /* for errorMessage() definition     */
#include "index.h"          /* for IndexNode definition          */

IndexNode * createIndexNode( Int level )    /*  level of new node   */
{   /*  beginning of createIndexNode()  */
    IndexNode * node; /* node to create */

    static Char name[] = "createIndexNode";

    node = NULL;
    
    /*
     *  Check for invalid node level
     */
    if ( level < LEAF ) {
        errorMessage( "invalid level specified", REPLACE );
        errorMessage( name, PREPEND );
        node = NULL;
    }   /*  end of if ( level < LEAF )  */
    /*
     *  Create new node with specified level
     */
    else {
        /*
         *  Allocate memory for new node
         */
        node = (IndexNode *)malloc( sizeof( IndexNode ) );
        if ( node == NULL ) {
            errorMessage( "allocation failure", REPLACE );
            errorMessage( name, PREPEND );
        }   /*  end of if ( node == NULL )  */
        else {
            node->level = level;
        }   /*  end of else */
    }   /*  end of else */
    
    /*
     *  Set entries of new node to empty list
     */
    node->entries = NULL;

    return ( node );
}   /*  end of createIndexNode  */
