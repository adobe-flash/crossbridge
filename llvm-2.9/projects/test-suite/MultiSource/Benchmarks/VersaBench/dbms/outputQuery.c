
/*
 *  File Name:      outputQuery.c
 *  Purpose:        This file contains three routines which handle the output
 *                  of the index queries.  The first routine initializes the
 *                  the output by saving the output FILE ptr which is needed
 *                  by the output buffer for display.  It's possible to pass
 *                  this FILE ptr to the display routine, but this would also
 *                  require the outputQuery() routine to have the FILE as
 *                  part of it's parameter list, since the outputQuery()
 *                  routine may need to flush the output buffer.  For clarity,
 *                  the FILE ptr is saved and is static to this file.  The
 *                  second routine is the routine that the query() method
 *                  requires and places one data object into the output 
 *                  buffer.  The object is converted to a string 
 *                  representation and placed in the output buffer. Before
 *                  placement, an overflow condition is checked and, if
 *                  necessary, the buffer is flushed before.  Note that
 *                  the size of the output buffer MUST be at least the
 *                  length of the largest possible data object string
 *                  representation.  The final routine flushes the output
 *                  buffer by displaying the contents to the output stream,
 *                  via the output FILE ptr.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *  23Jun99 Matthew Rivas   Added initializer to numberOfAttributes to prevent
 *                          warning message during compilation
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /*  for assert()                    */
#include <stdlib.h>         /*  for NULL definition             */
#include <stdio.h>          /*  for sprintf() definition        */
#include <string.h>         /*  for strcpy() definition         */
#include "dataManagement.h" /*  for primitive type definitions  */
#include "dataObject.h"     /*  for DataObject definitions      */
#include "errorMessage.h"   /*  for errorMessage() definition   */

/*
 *  Static definition of output buffer
 *      - approximate maximum length of a single data object represented
 *        as a character string is:
 *
 *              44483 characters = 8  * 50     key float values
 *                               + 43 * 1024   non-key char strings
 *                               + 50          place delimiters
 *                               + 1           line-feed/carriage return
 *      - output buffer length should be set
 */
 
#define LENGTH_OF_KEY_VALUE         50 
#define LENGTH_OF_NON_KEY_VALUE     MAX_SIZE_OF_NON_KEY_ATTRIBUTE
#define MAX_LENGTH_OF_ATTRIBUTE     MAX_SIZE_OF_NON_KEY_ATTRIBUTE
#define NUM_OF_NON_KEY_ATTRIBUTES ( NUM_OF_LARGE_ATTRIBUTES - NUM_OF_KEY_ATTRIBUTES )
#define REPRESENTATION_LENGTH       44483 
#define OUTPUT_BUFFER_LENGTH        2 * REPRESENTATION_LENGTH
#define ATTRIBUTE_DELIMITER         " "
#define END_OF_LINE_INDICATOR       "\n"

static Char outputBuffer[ OUTPUT_BUFFER_LENGTH + 1 ];
static FILE *outputFile = NULL;

/*
 *  Function prototype
 */
void flushOutputBuffer( void );

/*
 *  Name:           outputQuery
 *  Input:          data object to place in output buffer, dataObject
 *  Output:         none
 *  Return:         void
 *  Description:    This routine places a single data object into the output
 *                  buffer.  The object is first converted to a string 
 *                  representation which consists of each attribute in
 *                  character format separated by a single space.  A local
 *                  static character array is used to build the data object
 *                  string representation.  The static array, called
 *                  dataObjectString, must have a length which can handle a
 *                  data object of maximum size, i.e., largest possible key
 *                  values, large type with each non-key string of maximum
 *                  length.  Also, the output buffer size must be at least this
 *                  size.  A check is made to see if the output buffer needs to
 *                  be flushed prior to the dataObjectString placement and then
 *                  the dataObjectString is placed in the output buffer.
 *  Calls:          errorMessage()
 *                  flushErrorMessage() 
 *                  flushOutputBuffer()
 *      System:     strcat()
 *                  strlen()
 *                  sprintf()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

void outputQuery( DataObject *dataObject )  /*  object to place in buffer */
{   /*  beginning of outputQuery()  */
    Int i;                  /*  loop variable for attributes         */
    Int numberOfAttributes; /*  number of attributes for object      */
    
#ifndef NDEBUG
    static Char name[] = "outputQuery";
#endif /* NDEBUG */
    
    /*
     *  Static character strings for converting dataObject to string 
     *  representation.  temp is used to convert individual key attributes to a 
     *  string and then strcat() is used to place the attribute string version 
     *  into the dataObjectString which holds the entire data object string 
     *  representation.  When all of the attributes have been processed, 
     *  dataObjectString is placed into the output buffer.
     */
    static Char temp[ LENGTH_OF_KEY_VALUE + 1 ];
    static Char dataObjectString[ REPRESENTATION_LENGTH + 1 ];

    assert( outputFile );    
    assert( dataObject );
    assert( !(dataObject->type != SMALL  && \
              dataObject->type != MEDIUM && \
              dataObject->type != LARGE ) );
    assert( LENGTH_OF_KEY_VALUE     <= MAX_LENGTH_OF_ATTRIBUTE && \
            LENGTH_OF_NON_KEY_VALUE <= MAX_LENGTH_OF_ATTRIBUTE );
    assert( REPRESENTATION_LENGTH >= \
            LENGTH_OF_KEY_VALUE * NUM_OF_KEY_ATTRIBUTES \
          + LENGTH_OF_NON_KEY_VALUE * NUM_OF_NON_KEY_ATTRIBUTES \
          + ( NUM_OF_LARGE_ATTRIBUTES - 1 ) \
          + 1 );
    assert( REPRESENTATION_LENGTH <= OUTPUT_BUFFER_LENGTH );
    assert( MIN_ATTRIBUTE_CODE < NUM_OF_KEY_ATTRIBUTES );
    assert( MIN_ATTRIBUTE_CODE < MAX_ATTRIBUTE_CODE );
    assert( MAX_ATTRIBUTE_CODE < NUM_OF_LARGE_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_SMALL_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_MEDIUM_ATTRIBUTES );
    assert( NUM_OF_KEY_ATTRIBUTES < NUM_OF_LARGE_ATTRIBUTES );
    assert( REPRESENTATION_LENGTH <= OUTPUT_BUFFER_LENGTH );

    /*
     *  Determine the number of attributes for data object
     */
    numberOfAttributes = 0;
    if ( dataObject->type == SMALL ) {
        numberOfAttributes = NUM_OF_SMALL_ATTRIBUTES - 1;
    }   /*  end if ( dataObject->type == SMALL )      */
    else if ( dataObject->type == MEDIUM ) {
        numberOfAttributes = NUM_OF_MEDIUM_ATTRIBUTES - 1;
    }   /*  end if ( dataObject->type == MEDIUM )    */
    else if ( dataObject->type == LARGE ) {
        numberOfAttributes = NUM_OF_LARGE_ATTRIBUTES - 1;
    }   /*  end if ( dataObject->type == LARGE )      */
    
    /*
     *  Place each attribute into dataObjectString. Key and Non-Key values are 
     *  handled separately because of the format command, i.e. %f or %s.  The 
     *  temp string is used to hold the character string representation of the 
     *  key attribute values returned from sprintf().  Then strcat() is used to 
     *  add the attribute string representation to the full data object 
     *  representation being built up in dataObjectString.  All of the non-key 
     *  attributes are added to the dataObjectString using strcat() except for 
     *  the last attribute.  This attribute has no space delimiter after and so 
     *  is handled separately.  Finally, an end-of-line indicator is added to 
     *  the data object string and the string is place in the output buffer.  
     *  Note that the dataObjectString needs to be initialized to EMPTY/'\0' 
     *  each time the routine is called, since the array is static for memory 
     *  efficiency.
     */
    dataObjectString[ 0 ] = '\0';
    for ( i = MIN_ATTRIBUTE_CODE; i < NUM_OF_KEY_ATTRIBUTES; i++ ) {
        sprintf( temp, "%.8e", dataObject->attributes[ i ].value.key );
        strcat( dataObjectString, temp );
        strcat( dataObjectString, ATTRIBUTE_DELIMITER );
    }   /*  end of loop for key attributes  */

    for ( i = NUM_OF_KEY_ATTRIBUTES; i < numberOfAttributes; i++ ) {
        assert( dataObject->attributes[ i ].value.nonKey );
        assert( strlen( dataObject->attributes[ i ].value.nonKey ) > 0 );

        strcat( dataObjectString, dataObject->attributes[ i ].value.nonKey );
        strcat( dataObjectString, ATTRIBUTE_DELIMITER );
    }   /*  end of loop for non-key attributes  */

    assert( dataObject->attributes[ numberOfAttributes ].value.nonKey );
    assert(strlen(dataObject->attributes[numberOfAttributes].value.nonKey) > 0);

    strcat( dataObjectString, 
            dataObject->attributes[ numberOfAttributes ].value.nonKey );
    strcat( dataObjectString, END_OF_LINE_INDICATOR );

    assert( strlen( dataObjectString ) <= REPRESENTATION_LENGTH );
    
    /*
     *  Place the data object string representation into the output buffer, 
     *  flushing if necessary.
     */
    if ( strlen( dataObjectString ) + strlen( outputBuffer ) 
         > OUTPUT_BUFFER_LENGTH ) {
#ifndef NDEBUG
        errorMessage( "output buffer full - flushing", REPLACE );
        errorMessage( name, PREPEND );
        flushErrorMessage();
#endif /* NDEBUG */

        flushOutputBuffer();
    }
    
    strcat( outputBuffer, dataObjectString );

    return;
}   /*  end of outputQuery()    */

/*
 *  Name:           initOutputBuffer
 *  Input:          FILE ptr to output stream, file
 *  Output:         none
 *  Return:         void
 *  Description:    This routine initializes the output buffer by
 *                  saving the output FILE ptr for later buffer
 *                  display.
 *  Calls:          
 *      System:     
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

void initOutputBuffer( FILE *file )
{   /*  beginning of initOutputBuffer() */
    assert( file );
    
    outputFile = file;
}   /*  end of initOutputBuffer()   */

/*
 *  Name:           flushOutputBuffer
 *  Input:          none
 *  Output:         none
 *  Return:         void
 *  Description:    This routine flushes the output buffer by displaying
 *                  the current contents of the buffer into the output
 *                  stream via the static outputFile FILE ptr.  A check
 *                  is made to ensure that the buffer has contents to 
 *                  display before a flush occurs.
 *  Calls:          
 *      System:     fprintf()
 *                  fflush()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

void flushOutputBuffer( void )
{   /*  beginning of flushOutputBuffer()    */
    assert( outputFile );
    
    /*
     *  If the contents are not empty, flush the buffer
     */
    if ( strlen( outputBuffer ) > 0 ) {
        fprintf( outputFile, "%s", outputBuffer );
        fflush( outputFile );
    } 

    outputBuffer[ 0 ] = '\0';
    
    return;
}   /*  end of flushOutputBuffer()  */
