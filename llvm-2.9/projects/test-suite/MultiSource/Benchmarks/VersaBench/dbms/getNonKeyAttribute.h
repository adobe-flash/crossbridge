
/*
 *  DIS Data Management: getNonKeyAttribute
 *
 *      This header file contains the return codes for the getNonKeyAttribute 
 *      routine.  The routine reads a non-key attribute from the input FILE 
 *      stream which is defined to be a space-delimited string, but does not 
 *      cross an end-of-line indicator as the default system routines do.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_GET_NON_KEY_ATTRIBUTE_H
#define     DIS_GET_NON_KEY_ATTRIBUTE_H

#include <stdio.h>          /* for FILE definition            */
#include "dataManagement.h" /* for primitive type definitions */

/*
 *  Return Codes
 */
#define GET_NON_KEY_ATTRIBUTE_SUCCESS               0
#define GET_NON_KEY_ATTRIBUTE_ALLOCATION_FAILURE    1
#define GET_NON_KEY_ATTRIBUTE_EOI                   2

/*
 *  Function Prototype
 */
extern Int getNonKeyAttribute( FILE *file, Char **value );

#endif  /*  DIS_GET_NON_KEY_ATTRIBUTE_H */
