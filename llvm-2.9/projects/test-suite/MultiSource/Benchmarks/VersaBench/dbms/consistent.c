
/*
 *  File Name:      consistent.c
 *  Purpose:        Determines if either the two key or non-key values are 
 *                  consistent.  Key values are consistent if the two 
 *                  hyper-cubes intersect.  Non-key values are consistent if the
 *                  first non-key character string completely constains the 
 *                  second non-key character string.
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                   */
#include <string.h>         /* for strstr() definition        */
#include "dataManagement.h" /* for primitive type definitions */
#include "indexKey.h"       /* for IndexKey definition        */

/*
 *
 *  Name:           consistentKey
 *  Input:          index key, A
 *                  index key, B
 *  Output:         flag indicating intersection
 *  Return:         integer, TRUE or FALSE
 *  Description:    Returns boolean value indicating if two input index keys 
 *                  intersect.  The routine is communative, i.e., 
 *                  consistent(A,B) == consistent(B,A).
 *  Calls:          
 *      System:     
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

Boolean consistentKey( IndexKey *A, IndexKey *B )
{   /*  beginning of consistentKey() */
    Boolean flag; /* flag indicating consistency */

    assert( A );
    assert( B );

    /*
     *  Check each dimension for intersection.  If any dimension fails, then no 
     *  intersection can occur, set flag and return.
     */
    flag = TRUE;
    if ( A->lower.T > B->upper.T || B->lower.T > A->upper.T ) {
        flag = FALSE;
    } /* end of bounds check on T */
    else {    
        if ( A->lower.X > B->upper.X || B->lower.X > A->upper.X ) {
            flag = FALSE;
        } /* end of bounds check on X */
        else {
            if ( A->lower.Y > B->upper.Y || B->lower.Y > A->upper.Y ) {
                flag = FALSE;
            } /* end of bounds check on Y */
            else {
                if ( A->lower.Z > B->upper.Z || B->lower.Z > A->upper.Z ) {
                    flag = FALSE;
                }   /*  end of bounds check on Z   */
            }   /*  end of check on Y   */
        }   /*  end of check on X   */
    }   /*  end of check on T   */
        
    return ( flag );
}   /*  end of consistentKey()   */

/*
 *
 *  Name:           consistentNonKey
 *  Input:          character string, A
 *                  character string, B
 *  Output:         flag indicating intersection
 *  Return:         integer, TRUE or FALSE
 *  Description:    Returns boolean value indicating if two input character 
 *                  strings are consistent.  The check is true if string A 
 *                  entirely contains string B.  The routine is NOT communative,
 *                  i.e., consistent(A,B) != consistent(B,A) in general.
 *  Calls:          
 *      System:     strstr()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

Boolean consistentNonKey( Char *A, Char *B )
{   /*  beginning of consistentNonKey() */
    Boolean flag;  /* flag indicating consistency */

    assert( A );
    assert( B );
    
    /*
     *  Check for string B inside string A using library
     *  routine.
     */
    if ( strstr( A, B ) != NULL ) {
        flag = TRUE;
    }  /*  end of if strstr != NULL */
    else {
        flag = FALSE;
    }  /*  end of if strstr == NULL */

    return ( flag );
}   /*  end of consistentNonKey()  */
