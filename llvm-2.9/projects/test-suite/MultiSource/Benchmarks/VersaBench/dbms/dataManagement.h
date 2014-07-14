
/*
 *  DIS Data Management
 *
 *      This header file contains general type definitions and #define values
 *      All routines in the Data Management baseline source code should
 *      include this file.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_DATA_MANAGEMENT_H
#define     DIS_DATA_MANAGEMENT_H

/*
 *  Typedef primitive numeric types for greater portability
 */
typedef long int        Int;
typedef float           Float;
typedef char            Char;
typedef char            Boolean;

/*
 *  Wild-Card parameters
 *      - values defined by IEEE 754 Specification as required by DIS Benchmark 
 *        Suite Specification
 */
#define MINIMUM_VALUE_OF_FLOAT     -3.40282347e38
#define MAXIMUM_VALUE_OF_FLOAT      3.40282347e38
#define MINIMUM_VALUE_OF_INT       -2147483647 
#define MAXIMUM_VALUE_OF_INT        2147483647

/*
 *  Set boolean values
 */
#define TRUE                        1
#define FALSE                       0

/*
 *  Index (R-Tree) Parameters
 *      - minimum fan size allowable for the R-Tree index.  The value must
 *        be greater than one, but no upper limit is made for the fan.
 */
#define MINIMUM_FAN_SIZE            2

/*
 *  LEAF level definition
 *      - level of a leaf node.  All levels are greater than or equal to the
 *        leaf level.
 */
#define LEAF                        0

#endif  /*  DIS_DATA_MANAGEMENT_H   */
