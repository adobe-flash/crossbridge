/*
 *
 *  Name:           penalty
 *  Input:          index entry, A
 *                  index entry. B
 *  Output:         float value of penalty
 *  Return:         float
 *  Description:    Calculates and returns the penalty for the two input index 
 *                  entries.  The penalty for the index is defined as the 
 *                  increase in hyper-cube volume.  Note that the penalty 
 *                  routine is not communative, i.e., the penalty( A, B ) is 
 *                  not necessarily the penalty( B, A ).
 *  Calls:          keyUnion()
 *                  volume()
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

#include "dataManagement.h" /* for primitive type definitions */
#include "index.h"          /* for IndexEntry definition      */
#include "indexKey.h"       /* for IndexKey definition        */

/*
 *  Function Prototypes
 */
extern void keyUnion( IndexKey *A, IndexKey *B, IndexKey *U );
extern Float volume( IndexKey key );

Float penalty( IndexEntry A,    /* first entry for penalty  */
               IndexEntry B )   /* second entry for penalty */
{   /*  beginning of penalty()  */
    Float       penalty;
    IndexKey    key;

    /*
     *  Find union of A and B
     */
    keyUnion( &A.key, &B.key, &key );
    
    /*
     *  Determine increase in "volume"
     */
    penalty = volume( key ) - volume( A.key );

    return ( penalty );
}   /*  end of penalty()    */
