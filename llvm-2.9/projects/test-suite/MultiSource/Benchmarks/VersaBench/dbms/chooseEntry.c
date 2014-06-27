
/*
 *  Name:           chooseEntry
 *  Input:          list of entries to choose from
 *                  entry to choose with
 *  Output:         entry within input list which minimizes penalty
 *  Return:         IndexEntry pointer, or
 *                  NULL if node is empty
 *  Description:    Determines which entry of input list to add new entry.  
 *                  Chosen entry provides the minimum penalty which is the 
 *                  increase of the total hyper-cube volume.  No error is 
 *                  possible, but the value of the entry returned can be set to 
 *                  NULL which should be checked for in the calling routine.
 *  Calls:          penalty();
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

#include <assert.h>          /* for assert()                             */
#include <stdlib.h>          /* for NULL definition                      */
#include "dataManagement.h"  /* for primitive type definitions           */
#include "index.h"           /* for IndexNode and IndexEntry definitions */

/*
 *  Function prototypes
 */
extern Float penalty( IndexEntry A, IndexEntry B );

IndexEntry * chooseEntry( IndexNode  *node,     /*  node to choose from  */
                          IndexEntry *entry )   /*  entry to choose with */
{   /*  beginning of chooseEntry()  */
    IndexEntry  *minPenaltyEntry;   /*  result entry to return  */
    IndexEntry  *temp;              /*  used for looping        */
    Float       minPenalty;         /*  value of min penalty    */
    
    assert( node );
    assert( node->entries );
    assert( entry );
    
    /*
     *  Loop through entries of node to find minimum penalty.
     */
    minPenaltyEntry = node->entries;
    minPenalty      = penalty( *minPenaltyEntry, *entry );
    for ( temp = minPenaltyEntry->next; temp != NULL; temp = temp->next ) {
        Float   tempPenalty;

        tempPenalty = penalty( *temp, *entry );
        if ( tempPenalty < minPenalty ) {
            minPenaltyEntry = temp;
            minPenalty      = tempPenalty;
        }   /*  end of if ( tempPenalty < minPenalty )  */
    }   /*  end of loop for temp    */

    return ( minPenaltyEntry );
}   /*  end of chooseEntry()    */
