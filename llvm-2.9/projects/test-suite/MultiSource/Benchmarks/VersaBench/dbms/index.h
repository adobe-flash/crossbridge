
/*
 *  DIS Data Management Index
 *
 *      This file contains the index structures for the DIS R-Tree basline 
 *      application.  The two structures defined are for index entries and for 
 *      index nodes.
 *
 *      An index entry will reference either an index node or a 
 *      data object.  All entries which are  in the index reside on an index 
 *      node.  An entry which resides on a leaf node (level = 0) references a 
 *      data object, while an entry which resides on a non-leaf node (level > 0)
 *      references an index node.  Since the entry may reference two different 
 *      types of objects, the member is a union between the two types.  The 
 *      entry contains an index key which is used for traversals of the index.  
 *      For the R-Tree index, the index key is the hyper-cube which minimally 
 *      encloses the object it references.  If the child reference is a data 
 *      object, the index key for the entry is the appropriate values of the 
 *      data object.  If the child reference is an index node, the index key of 
 *      the entry is the bounding hyper-cube of all of the entries residing on 
 *      that node.  The final member of the index entry structure is a pointer 
 *      to another index entry structure which gives linked-list behavior to 
 *      entries.
 *
 *      An index node is a container of a list of index entries.  Each node 
 *      contains an integer level value.  Note that the leaf level is zero and 
 *      the level increases as the tree is ascended, i.e., the root level is 
 *      always greater than or equal to the leaf level.  The number of entries 
 *      in the list is always between one and the fan or order of the index.  
 *      Note that the fan of the index is read at run-time, so a fixed array of 
 *      entries is not permitted.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_INDEX_H
#define     DIS_INDEX_H

#include "dataManagement.h" /* for basic data types       */
#include "dataObject.h"     /* for data object definition */
#include "indexKey.h"       /* for index key definition   */

/*
 *  Index Node
 *      - contains a list of entries which reside on node
 *      - has a level specifier where all levels are greater than or equal to 
 *        the LEAF level.
 */
typedef struct
{
    Int                 level;      /*  level where node resides in index   */
    struct IndexEntry   *entries;   /*  list of entries on node             */
} IndexNode;

/*
 *  Prototypes for routines which create, delete, and display IndexNode 
 *      structures.
 */

extern IndexNode   *createIndexNode( Int level );
extern void        deleteIndexNode( IndexNode * node );
extern void        outputIndexNode( IndexNode * node, Int indent );

/*
 *  Index Entry
 *      - references a child object, either a data object or index node
 *      - has a key which "encloses" the child object
 *      - has a pointer to support single link-list capability
 */
struct IndexEntry
{
    union {
        IndexNode       *node;
        DataObject      *dataObject;
    }                   child;

    IndexKey            key;

    struct IndexEntry   *next;
};

typedef struct IndexEntry   IndexEntry;

/*
 *  Prototypes for routines which create, delete, and display IndexEntry 
 *      structures.
 */

extern IndexEntry  *createIndexEntry( void );
extern void        deleteIndexEntry( IndexEntry * entry, Int level );
extern void        outputIndexEntry( IndexEntry * entry, Int level, Int indent );

#endif  /*  DIS_INDEX_H */
