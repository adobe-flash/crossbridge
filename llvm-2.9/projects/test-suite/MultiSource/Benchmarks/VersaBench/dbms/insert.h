
/*
 *  DIS Data Management: insert
 *
 *      This header file contains the return codes for the insert routine 
 *      and the function prototype.  There are three different failure modes for
 *      the routine.  The first two indicate that an error occurred during the 
 *      insertEntry() recursive call chain.  The error can be either FATAL 
 *      indicating that the index was changed prior to the error which means the
 *      index can't be guaranteed to be in a valid state and should not be 
 *      manipulated any further.  The error can also be NON-FATAL indicating 
 *      that the index was not changed prior to the error and can be guaranteed 
 *      to be in a valid state and can be manipulated, i.e., with other 
 *      commands.  The final error condition indicates a memory allocation 
 *      failure when attempting to "grow" the index tree because of root node 
 *      splitting.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_INSERT_H
#define     DIS_INSERT_H

#include "dataManagement.h" /* for primitive type definitions */
#include "dataObject.h"     /* for DataObject definition      */
#include "index.h"          /* for IndexNode definition       */

/*
 *  Return Codes
 */
#define INSERT_SUCCESS                          0
#define INSERT_INSERT_ENTRY_FAILURE_FATAL       1
#define INSERT_INSERT_ENTRY_FAILURE_NON_FATAL   2
#define INSERT_ALLOCATION_FAILURE               3

/*
 *  Function prototype
 */
extern Int insert( IndexNode **root, DataObject *dataObject, Int fan );

#endif  /*  DIS_INSERT_H    */
