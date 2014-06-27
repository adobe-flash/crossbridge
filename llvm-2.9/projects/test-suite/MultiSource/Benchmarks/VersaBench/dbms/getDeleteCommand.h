
/*
 *  DIS Data Management: getDeleteCommand
 *
 *      This header file defines the return codes for the getDeleteCommand 
 *      function.  Any file which uses the getDeleteCommand routine should 
 *      include this header.  The first code indicates success.  The second 
 *      code is the result of an I/O error at a lower-level read function.  The 
 *      third code indicates that an end-of-line or end-of-file indicator was 
 *      read before insert command was finished.  The fourth error is when an 
 *      attribute code is unknown or invalid.  The fifth error is an allocation 
 *      error for the non-key attribute character strings.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_GET_DELETE_COMMAND_H
#define     DIS_GET_DELETE_COMMAND_H

#include <stdio.h>          /* for FILE definition          */
#include "dataManagement.h" /* for primitive data types     */
#include "dataObject.h"     /* for DataAttribute definition */
#include "indexKey.h"       /* for IndexKey definition      */

/*
 *  Return codes
 */
#define GET_DELETE_SUCCESS              0
#define GET_DELETE_IO_ERROR             1
#define GET_DELETE_EARLY_EOI            2
#define GET_DELETE_INVALID_CODE_ERROR   3
#define GET_DELETE_ALLOCATION_ERROR     4

/*
 *  Function Prototype
 */
extern Int getDeleteCommand( FILE *file, IndexKey *searchKey, 
                             DataAttribute **searchNonKey );

#endif  /*  DIS_GET_DELETE_COMMAND_H */
