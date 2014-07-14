/*
 *
 *  Name:           volume
 *  Input:          index key, key
 *  Output:         float value of volume of key
 *  Return:         float
 *  Description:    Calculates and returns the volume of the provided index key.
 *  Calls:          
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

#include <assert.h>         /* for assert()                   */
#include "dataManagement.h" /* for primitive type definitions */
#include "indexKey.h"       /* for IndexKey definition        */

Float volume( IndexKey key )   /* input key for volume calc */
{   /*  beginning of volume()  */
    Float vol;  /*  calculated volume of index key  */

    /*
     *  Determine "volume" of key
     */
    vol  = ( key.upper.T - key.lower.T );
    vol *= ( key.upper.X - key.lower.X );
    vol *= ( key.upper.Y - key.lower.Y );
    vol *= ( key.upper.Z - key.lower.Z );

    /* assert( vol > 0.0 ); */
	if (vol <= 0.0) {
	  Float* p = 0;
	  vol = *p;
	}

    return ( vol );
}   /*  end of volume()    */
