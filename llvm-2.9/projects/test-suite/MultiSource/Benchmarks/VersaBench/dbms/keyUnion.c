
/*
 *  File Name:      keyUnion.c
 *  Purpose:        Calculates the index key union of either a pair of index 
 *                  keys or a list of index entries.  The routine which 
 *                  calculates the union of a list of index entries uses the 
 *                  routine which calculates the union of a pair of index keys.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                   */
#include <stdlib.h>         /* for NULL definition            */
#include "dataManagement.h" /* for primitive type definitions */
#include "index.h"          /* for IndexEntry definition      */
#include "indexKey.h"       /* for IndexKey definition        */

/*
 *  Macro definitions for MAX and MIN for use of this file only.
 */
#define MAX( a, b ) ( (a) > (b) ? (a) : (b) )
#define MIN( a, b ) ( (a) < (b) ? (a) : (b) )

/*
 *
 *  Name:           keyUnion
 *  Input:          index key, A
 *                  index key, B
 *  Output:         index key, U
 *  Return:         void
 *  Description:    Determines the union of two index keys.  The union is 
 *                  defined as the minimal bounding index key which completely 
 *                  encloses the two input index keys, i.e., a "bounding-box".  
 *                  The "box" is found by simply determining the minimum and 
 *                  maximum values for each dimension of the index key. The
 *                  values of the output key U do not require "past" values
 *                  from A or B, so the pointers can be "reused", i.e., the
 *                  routine can be called as keyUnion( A, B, B ) which will
 *                  determine the key union of A and B and places the result
 *                  in B.
 *  Calls:          
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

void keyUnion( IndexKey *A,     /* first entry for key union    */
               IndexKey *B,     /* second entry for key union   */
               IndexKey *U )    /* union of entries A and B     */
{   /*  beginning of keyUnion() */
    
    assert( A );
    assert( B );
    assert( U );

    /*
     *  Find max and min for each dimension of index keys
     */
    U->lower.T = MIN( A->lower.T, B->lower.T );
    U->lower.X = MIN( A->lower.X, B->lower.X );
    U->lower.Y = MIN( A->lower.Y, B->lower.Y );
    U->lower.Z = MIN( A->lower.Z, B->lower.Z );
    U->upper.T = MAX( A->upper.T, B->upper.T );
    U->upper.X = MAX( A->upper.X, B->upper.X );
    U->upper.Y = MAX( A->upper.Y, B->upper.Y );
    U->upper.Z = MAX( A->upper.Z, B->upper.Z );
    
    return;
}   /*  end of keyUnion()   */

/*
 *
 *  Name:           keysUnion
 *  Input:          index entry list, I
 *  Output:         index key, U
 *  Return:         void
 *  Description:    Determines the union of the index keys of a list of index 
 *                  entries.  The union is defined as the minimal bounding 
 *                  index key which completely encloses all of the input index 
 *                  keys, i.e., a "bounding-box". The "box" is found by simply 
 *                  determing the minimum and maximum values for each dimension 
 *                  of the index key.
 *  Calls:          
 *                  keyUnion()
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

void keysUnion( IndexEntry *I,  /*  list of index entries   */
                IndexKey   *U ) /*  union of entries of I   */
{   /*  beginning of keysUnion() */

    assert( I );
    assert( U );

    /*
     *  Loop over entry list.  Must set value of index key U to some value to 
     *  prevent NaN or uninitialized memory issues.  Setting it to the key of 
     *  the first index entry is sufficient and saves one keyUnion calculation.
     */
    *U = I->key;
    for ( I = I->next; I != NULL; I = I->next ) {
        /*
         *  Expand hyper-cube bounding "box" by determining union of key of 
         *  current index entry with current index key.  This will always 
         *  "expand" the box or leave it unaltered.
         */
        keyUnion( &(I->key), U, U );
    }   /*  end of loop for I   */
    
    return;
}   /*  end of keysUnion()  */
