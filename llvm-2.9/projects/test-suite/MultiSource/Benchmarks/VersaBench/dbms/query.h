
/*
 *  DIS Data Management: query
 *
 *      This header file contains the return codes for the query routine 
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
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_QUERY_H
#define     DIS_QUERY_H

#include "dataManagement.h" /* for primitive type definitions               */
#include "dataObject.h"     /* for DataAttribute and DataObject definitions */
#include "indexKey.h"       /* for IndexKey definition                      */
#include "index.h"          /* for IndexNode definition                     */

/*
 *  Return Codes
 */
#define QUERY_SUCCESS                           0
#define QUERY_INVALID_KEY_SEARCH_VALUE          1
#define QUERY_INVALID_NON_KEY_SEARCH_VALUE      2

/*
 *  Function prototype
 */
extern Int query( IndexNode *node, IndexKey *searchKey,
                  DataAttribute *searchNonKey, Boolean checkValidity,
                  void (*outputQuery)( DataObject *) );

#endif  /*  DIS_QUERY_H */
