
/*
 *  DIS Data Management: insertEntry
 *
 *      This header file contains the return codes for the insertEntry routine 
 *      and the function prototype.  There are three different failure modes for
 *      the routine.  The first indicates that no entry on the current node was 
 *      chosen for the insertion.  This always indicates that the current node 
 *      is empty of any entry and is an extremely rare debugging error.  The 
 *      second mode of failure indicates a fatal split node error.  It is fatal 
 *      since the index has been altered in some way before the error and, since
 *      no "undo" mechanism is implemented for the baseline application, the 
 *      state of the index can't be gauranteed which produces a fatal error.  
 *      The third mode of failure indicates a non-fatal split node error.  It's 
 *      not fatal since the index is unchanged and its current state can be 
 *      assumed to be still valid.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_INSERT_ENTRY_H
#define     DIS_INSERT_ENTRY_H

#include "dataManagement.h" /* for primitive type definitions           */
#include "index.h"          /* for IndexNode and IndexEntry definitions */

/*
 *  Return Codes
 */
#define INSERT_ENTRY_SUCCESS                0
#define INSERT_ENTRY_CHOOSE_ENTRY_FAILURE   1
#define INSERT_ENTRY_SPLIT_NODE_FATAL       2
#define INSERT_ENTRY_SPLIT_NODE_NON_FATAL   3

/*
 *  Function prototypes
 */
extern Int insertEntry( IndexNode *node, IndexEntry *entry, Int level, Int fan, 
                        IndexEntry **splitEntry );

#endif  /*  DIS_INSERT_ENTRY_H  */
