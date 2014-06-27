
/*
 *  DIS Data Management Index Key
 *
 *      Definitions of the index key used for the application.  An index key is 
 *      defined as two hyper-points representing the "upper" and "lower" points 
 *      and defines a hyper-cube.  Each hyper-point is four-dimensional with 
 *      parameters T, X, Y, and Z.  The index key is used to represent the key 
 *      values of data objects and bounding boxes for the R-Tree index.  A 
 *      valid index key will lower hyper-point values less than or equal to the 
 *      upper hyper-point values for all four values in a hyper-point.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_INDEX_KEY_H
#define     DIS_INDEX_KEY_H

#include "dataManagement.h"

/*
 *  Index Point structure
 *      - defines single point in hyper-space (4D)
 *      - used as bounding points for hyper-cubes which are the index key for
 *        the R-Tree algorithm.
 */

typedef struct {
    Float   T;
    Float   X;
    Float   Y;
    Float   Z;
} IndexPoint;

/*
 *  Index Key structure
 *      - represents the key for the R-Tree index
 *      - defines a hyper-cube by specifying the lower and upper hyper-points
 *        for the hyper-cube.
 *      - for a valid index key, all values of lower point MUST be less than or 
 *        equal to the values for the upper point.
 */

typedef struct {
    IndexPoint  lower;  /*  lower point of hyper-cube   */
    IndexPoint  upper;  /*  upper point of hyper-cube   */
} IndexKey;

#endif  /*  DIS_INDEX_KEY_H */
