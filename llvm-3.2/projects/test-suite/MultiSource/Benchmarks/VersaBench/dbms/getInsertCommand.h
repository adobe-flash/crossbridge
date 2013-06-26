
/*
 *  DIS Data Management: getInsertCommand
 *
 *      This header file defines the return codes for the getInsertCommand 
 *      function.  Any file which uses the getInsertCommand routine should 
 *      include this header.  The first code indicates success.  The second 
 *      code is the result of an I/O error at a lower-level read function.  The 
 *      third code indicates that an end-of-line or end-of-file indicator was 
 *      read before insert command was finished.  The fourth error is when the 
 *      data object type specifier, which is part of the insert command, is of 
 *      an unknown type.  The fifth error is an allocation error for the 
 *      non-key attribute character strings.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_GET_INSERT_COMMAND_H
#define     DIS_GET_INSERT_COMMAND_H

#include <stdio.h>          /* for FILE definition       */
#include "dataManagement.h" /* for primitive data types  */
#include "dataObject.h"     /* for DataObject definition */

/*
 *  Return codes
 */
#define GET_INSERT_SUCCESS                  0
#define GET_INSERT_IO_ERROR                 1
#define GET_INSERT_EARLY_EOI                2
#define GET_INSERT_UNKNOWN_DATA_OBJECT_TYPE 3
#define GET_INSERT_ALLOCATION_ERROR         4

/*
 *  Function Prototype
 */
extern Int getInsertCommand( FILE *file, DataObject **dataObject );

#endif  /*  DIS_GET_INSERT_COMMAND_H    */
