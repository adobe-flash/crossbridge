
/*
 *  DIS Data Management: splitNode
 *
 *      This header file contains the return codes for the splitNode routine and
 *      the function prototype.  The only failure possible for the slitNode 
 *      routine is an allocation failure.
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_SPLIT_NODE_H
#define     DIS_SPLIT_NODE_H

#include "dataManagement.h" /* for primitive type definitions           */
#include "index.h"          /* for IndexNode and IndexEntry definitions */

/*
 *  Return Codes
 */
#define SPLIT_NODE_SUCCESS              0
#define SPLIT_NODE_ALLOCATION_FAILURE   1

/*
 *  Function prototypes
 */
extern Int splitNode( IndexNode *nodeToSplit, IndexEntry *entry, 
                      Int fan, IndexEntry **splitEntry );

#endif  /*  DIS_SPLIT_NODE_H    */
