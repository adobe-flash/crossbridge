
/*
 *  DIS Data Management: delete
 *
 *      This header file contains the return codes for the delete routine 
 *      and the function prototype.  There are two different failure modes for
 *      the routine.  The first indicates that the search index key passed as 
 *      input to the routine is invalid.  The second indicates that the 
 *      attribute code specifying which non-key attribute to search on is 
 *      invalid (too small or too large).
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_DELETE_H
#define     DIS_DELETE_H

#include "dataObject.h"     /* for DataAttribute definition   */
#include "dataManagement.h" /* for primitive type definitions */
#include "index.h"          /* for IndexNode definition       */
#include "indexKey.h"       /* for IndexKey definition        */

/*
 *  Return Codes
 */
#define DELETE_SUCCESS                      0
#define DELETE_INVALID_KEY_SEARCH_VALUE     1
#define DELETE_INVALID_NON_KEY_SEARCH_VALUE 2

/*
 *  Function prototype
 */
extern Int delete( IndexNode **root, IndexKey *searchKey,
                   DataAttribute *searchNonKey );

#endif  /*  DIS_DELETE_H    */
