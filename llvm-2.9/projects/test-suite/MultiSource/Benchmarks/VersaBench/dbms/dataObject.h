
/*
 *  DIS Data Management Data Object
 *
 *      This structure represents the base type of data object stored by the 
 *      R-Tree index for the DIS Benchmark Suite:Data Management.  The 
 *      structure is made up of two members.  The first is a enumerated type 
 *      defining the size of the data object, and the second is the array of 
 *      attributes for this data object.  Each attribute can be either a value 
 *      of Float for key attributes or Char * for non-key attributes.  A utility
 *      structure is define, DataAttribute, which includes the attribute code 
 *      for the DataObjectAttribute, which is used by the query and delete 
 *      commands.  It also provides link-list capability to the 
 *      DataObjectAttribute structure.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */
 
#ifndef     DIS_DATA_OBJECT_H
#define     DIS_DATA_OBJECT_H

#include "dataManagement.h" /* for data type primitives */

/*
 *  Data Object Type enumerator
 *      - values defined by DIS Benchmark Suite Specification
 *      - used to determine number of non-key attributes assigned to data object
 */

typedef enum
{
    SMALL   = 1,
    MEDIUM  = 2,
    LARGE   = 3
} DataObjectType;

/*
 *  Data Object Attribute structure
 *      - value of attribute is a union
 *      - value can be key/Float or non-key/Char *
 *      - used by DataObject structure
 */

typedef struct
{
    union
    {
        Float       key;
        Char *      nonKey;
    }               value;
} DataObjectAttribute;

/*
 *  Data Attribute structure
 *      - a subclass of DataObjectAttribute
 *      - adds command code and link-list capability to DataObjectAttribute
 *      - used by query and delete for key and non-key searches
 *      - code must lie between 0 and MAX_ATTRIBUTE_CODE
 *      - code value between 0 and NUM_OF_KEY_ATTRIBUTES is a float
 *      - all other code values are character strings
 */

struct DataAttribute
{
    Int                     code;       /*  code of attribute       */
    DataObjectAttribute     attribute;  /*  attribute for code      */

    struct DataAttribute    *next;      /*  next attribute in list  */
};

typedef struct DataAttribute    DataAttribute;

/*
 *  Data Object structure
 *      - basic storage unit of database which is referenced by R-Tree index
 *      - contains key and non-key attributes
 *      - number of attributes is determined by DataObjectType enumerator.
 */

struct DataObject
{
    DataObjectType          type;       /*  type of data object     */
    DataObjectAttribute *   attributes; /*  attributes for object   */
};

typedef struct DataObject   DataObject;

/*
 *  Attribute parameters
 *      - values defined by DIS Benchmark Suite Specification 
 */
#define MAX_SIZE_OF_NON_KEY_ATTRIBUTE   1024
#define NUM_OF_SMALL_ATTRIBUTES           18
#define NUM_OF_MEDIUM_ATTRIBUTES          25
#define NUM_OF_LARGE_ATTRIBUTES           51
#define NUM_OF_KEY_ATTRIBUTES              8
#define MIN_ATTRIBUTE_CODE                 0
#define MAX_ATTRIBUTE_CODE                50

/*
 *  Key attribute parameters
 *      - value represents sequence of values placed in index key
 *      - prevents hard-coded values placed in code
 */
enum {
    LOWER_POINT_T = 0,
    LOWER_POINT_X = 1,
    LOWER_POINT_Y = 2,
    LOWER_POINT_Z = 3,
    UPPER_POINT_T = 4,
    UPPER_POINT_X = 5,
    UPPER_POINT_Y = 6,
    UPPER_POINT_Z = 7
};

/*
 *  Prototypes for routines which create, delete, and display DataObject 
 *  structures.
 */

extern DataObject *    createDataObject( DataObjectType dataObjectType );
extern void            deleteDataObject( DataObject * dataObject );
extern void            outputDataObject( DataObject * dataObject, Int indent );

#endif  /*  DIS_DATA_OBJECT_H   */
