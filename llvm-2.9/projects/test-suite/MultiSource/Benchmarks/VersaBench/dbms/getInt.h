
/*
 *  DIS Data Management: getInt
 *
 *      This header file contains the return codes for the 
 *      getInt routine.  The routine reads a space-delimited integer from the 
 *      input FILE stream, but does not cross an end-of-line indicator as the 
 *      default system routines do.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_GET_INT_H
#define     DIS_GET_INT_H

#include <stdio.h>          /* for FILE definition      */
#include "dataManagement.h" /* for primitive data types */

/*
 *  Return Codes
 */
#define GET_INT_SUCCESS             0
#define GET_INT_EOI                 1
#define GET_INT_RANGE_EXCEEDED      2
#define GET_INT_BAD_CONVERSION      3

/*
 *  Function Prototype
 */
extern Int getInt( FILE *file, Int *value );

#endif  /*  DIS_GET_INT_H   */
