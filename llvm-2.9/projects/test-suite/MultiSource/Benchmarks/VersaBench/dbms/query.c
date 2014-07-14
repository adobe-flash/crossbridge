
/*
 *  Name:           query
 *  Input:          root node of index, root
 *                  search index key, searchKey
 *                  search non-key list, searchNonKey
 *                  validity check flag, checkValidity
 *                  output function pointer, outputQuery
 *  Output:         integer return code
 *  Return:         QUERY_SUCCESS, or
 *                  QUERY_INVALID_KEY_SEARCH_VALUE,
 *                  QUERY_INVALID_NON_KEY_SEARCH_VALUE
 *  Description:    The routine searchs the index and calls the outputQuery 
 *                  routine for each consistent data object found.  The routine 
 *                  first uses the R-Tree index to find individual leaf nodes 
 *                  which point to data objects which have index keys which are
 *                  consistent with the input search key.  The found data 
 *                  object's non-key attributes are then compared with the input
 *                  list of non-key search values for consistency.  The input 
 *                  list of non-key search values consist of the attribute code 
 *                  and a character sequence.  Two utility subroutines are used 
 *                  to check the validity of the input search values.  The input
 *                  is checked only when the validity input flag is set to TRUE 
 *                  which prevents multiple checks of the same data since this 
 *                  routine is applied recursively.  Note that the index is 
 *                  never altered by a query which means no error is fatal.  Two
 *                  utility subroutines are used to determine consistency which 
 *                  is intersection for the DIS application.  The utility 
 *                  subroutines are used to separate the specific hyper-cube 
 *                  dimension and character string from the general R-Tree 
 *                  algorithm.
 *
 *                  Once a consistent data object has been found, the input 
 *                  routine outputQuery() is called for this object.  It's 
 *                  expected that the outputQuery() routine will place some 
 *                  representation of the data object into an output buffer for 
 *                  the query response.  The query() routine does not define 
 *                  what the representation will be, which allows the calling 
 *                  program to determine the format and timing of the response 
 *                  display.
 *  Calls:          consistentKey()
 *                  consistentNonKey()
 *                  errorMessage()
 *                  validKey()
 *                  validNonKey()
 *      System:     
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                                 */
#include <stdlib.h>         /* for NULL definition                          */
#include "dataManagement.h" /* for primitive type definitions               */
#include "dataObject.h"     /* for DataAttribute and DataObject definitions */
#include "errorMessage.h"   /* for errorMessage() definition                */
#include "index.h"          /* for IndexNode and IndexEntry definitions     */
#include "indexKey.h"       /* for IndexKey definition                      */
#include "query.h"          /* for query return codes                       */

/*
 *  Function prototypes
 */
extern Boolean consistentKey( IndexKey *A, IndexKey *B );
extern Boolean consistentNonKey( Char *A, Char *B );
extern Boolean validIndexKey( IndexKey *key );
extern Boolean validAttributes( DataAttribute *attributes );

Int query( IndexNode *node,                     /*  current node of index   */
           IndexKey *searchKey,                 /*  index key search values */
           DataAttribute *searchNonKey,         /*  non-key search values   */
           Boolean checkValidity,               /*  flag to check validity  */
           void (*outputQuery)( DataObject *) ) /*  output valid object     */
{   /* beginning of query() */
    static Char name[] = "query";

    assert( node );
    assert( searchKey );
    assert( !( checkValidity != TRUE && checkValidity != FALSE ) );

    /*
     *  If flag is set, check validity of key and non-key values.
     */
    if ( checkValidity == TRUE ) {
        if ( validIndexKey( searchKey ) == FALSE ) {
            errorMessage( "invalid index key search values", REPLACE );
            errorMessage( name, PREPEND );
            return ( QUERY_INVALID_KEY_SEARCH_VALUE );
        }   /*  end validity check of key values    */
        else if ( validAttributes( searchNonKey ) == FALSE ) {
            errorMessage( "invalid non-key search values", REPLACE );
            errorMessage( name, PREPEND );
            return ( QUERY_INVALID_NON_KEY_SEARCH_VALUE );
        }   /*  end validity check of non-key values    */
    }   /*  end of checkValidity == TRUE    */

    /*
     *  The routine is applied recursively so the current node may or may not be
     *  a leaf node.  If it is a leaf node, the child referenced by the entries 
     *  residing on the node are data objects.  If it is not a leaf node, the 
     *  child referenced by the entries are other nodes.  So if the current 
     *  level is not a leaf, recursively call the query routine on each 
     *  consistent entry.
     */
    if ( node->level > LEAF ) {
        IndexEntry *entry;  /*  temp entry for looping through list */
        
        /*
         *  Loop through each entry on current node and query each consistent 
         *  child node.  Note that only the key values are available for 
         *  consistency checks at any level greater than the LEAF level.
         */
        for ( entry = node->entries; entry != NULL; entry = entry->next ) {
            if ( consistentKey( &entry->key, searchKey ) == TRUE ) {
                query( entry->child.node, searchKey, searchNonKey, FALSE,
                       outputQuery );
            }   /*  end of branch which is consistent   */
        }   /*  end of loop for entry   */
    }   /*  end of if ( node->level > LEAF ) */
    else {
        IndexEntry *entry;  /*  temp entry for looping through list */
        
        /*
         *  Loop through each entry on current node and query each consistent 
         *  child node.  The first consistency check is made on the key value.  
         *  If the key values are consistent, then the data object is checked 
         *  for its non-key values.  A temporary upperBound value is set to 
         *  prevent out-of-range checks on the three types of data objects.
         */
        for ( entry = node->entries; entry != NULL; entry = entry->next ) {
            if ( consistentKey( &entry->key, searchKey ) == TRUE ) {
                DataAttribute *temp;            /*  attribute for list loop */
                DataObject    *object;          /*  allows easier reading   */
                Int           upperBound;       /*  prevents out-of-range   */
                Boolean       acceptanceFlag;   /* flag to output object    */

                object = entry->child.dataObject;

                upperBound = 0;
                if ( object->type == SMALL ) {
                    upperBound = NUM_OF_SMALL_ATTRIBUTES;
                }   /*  end of type == SMALL    */
                else if ( object->type == MEDIUM ) {
                    upperBound = NUM_OF_MEDIUM_ATTRIBUTES;
                }   /*  end of type == MEDIUM   */
                else if ( object->type == LARGE ) {
                    upperBound = NUM_OF_LARGE_ATTRIBUTES;
                }   /*  end of type == LARGE    */

                /*
                 *  The loop checks each value of the non-key search list and 
                 *  compares that value for that specific attribute code to the 
                 *  value stored in the data object.  If all of the attributes 
                 *  are consistent, the flag is set to TRUE at the end of the 
                 *  loop, and the outputQuery routine is called for the data 
                 *  object.  If any of the attributes are not consistent, the 
                 *  flag is set to FALSE and the loop exits and the next entry 
                 *  is checked.
                 */
                acceptanceFlag = TRUE;
                temp = searchNonKey;
                while ( temp != NULL && acceptanceFlag == TRUE ) {
                    if ( temp->code < upperBound ) {
                        acceptanceFlag = consistentNonKey(
                            object->attributes[ temp->code ].value.nonKey,
                            temp->attribute.value.nonKey );
                    }   /*  end of code < upperBound    */
                    temp = temp->next;
                }   /*  end of loop through non-key search value list   */

                if ( acceptanceFlag == TRUE ) {
                    outputQuery( entry->child.dataObject );
                }   /*  end of acceptanceFlag == TRUE   */
            }   /*  end of if consistentKey == TRUE */
        }   /*  end of loop for entry   */
    }   /*  end of if ( node->level == LEAF )   */

    return ( QUERY_SUCCESS );
}   /*  end query() */
