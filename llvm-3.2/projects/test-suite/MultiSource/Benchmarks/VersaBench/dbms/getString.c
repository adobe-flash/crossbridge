
/*
 *  Name:           getString
 *  Input:          FILE ptr
 *  Output:         character string pointer
 *  Return:         Char pointer to string read, or
 *                  NULL if error
 *  Description:    Reads a white-space delimited string from the current 
 *                  position of the file pointer.  The read will not cross an 
 *                  end-of-line indicated by a new line '\n' or carriage return 
 *                  '\r'.  If an end-of-line indicator is read before a string 
 *                  is encountered, the indicator is put back onto the stream 
 *                  and an error code is returned.  The maximum length of a 
 *                  string is set at 1024 as defined by the DIS Benchmark 
 *                  Suite: Data Management Bechmark, for a non-key attribute.  
 *                  The length will also work for integers and floats which use 
 *                  this routine for reading. The routine returns a pointer to 
 *                  its internal static buffer which holds the string just read.
 *                  If an error occurs during the read, a NULL is returned.
 *  Calls:          errorMessage()
 *      System:     fgetc()
 *                  isspace()
 *                  strlen()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                   */
#include <ctype.h>          /* for isspace() definition       */
#include <stdlib.h>         /* for NULL definition            */
#include <stdio.h>          /* for FILE definition            */
#include <string.h>         /* for strlen() definition        */
#include "dataManagement.h" /* for primitive type definitions */
#include "errorMessage.h"   /* for errorMessage() definition  */

/*
 *  Maximum length of buffer for temporary storage of string
 */
#define MAXIMUM_BUFFER_LENGTH   1024

Char *getString( FILE *file )     /*  input stream to get string from */
{   /*  begin getString()   */
    Int         nextChar;       /* next character read from stream   */
    Int         bufferLength;   /* current length of static buffer   */
    /* static buffer for reading */
    static Char buffer[ MAXIMUM_BUFFER_LENGTH + 1 ];
    
    static Char name[] = "getString";

    assert( file );

    /*
     *  Skip all "whitespace" before the beginning of the string.  Note that 
     *  the carriage return, '\r', and new line, '\n', characters are not  
     *  included as "whitespace" for this routine.  After the loop which skips  
     *  the "whitespace", a check is made to determine if the last character  
     *  read indicates EOF. If so, an error is returned since no string value 
     *  was read.  Also, a check is made to see if the last character read is a 
     *  new line or carriage return.  If so, the last character is "put back" 
     *  onto the stream and an error is returned since no string value was read.
     *  The value is "put back" since the next call to getString should also 
     *  indicate that there are no string values left on this line.
     */
    nextChar = fgetc( file );
    while ( nextChar != EOF     && 
            nextChar != '\n'    && 
            nextChar != '\r'    && 
            isspace( nextChar ) ) {
        nextChar = fgetc( file );
    }   /*  end of loop while ( !EOF || '\n', etc. )    */
    
    if ( nextChar == EOF ) {
        return ( NULL );            /*  return output value of NULL     */
    }   /*  end of if ( nextChar == EOF )   */
    else if ( nextChar == '\n' || nextChar == '\r' ) {
        ungetc( nextChar, file );   /*  "push" character back on stream */
        return ( NULL );            /*  return output value of NULL     */
    }   /*  end of if ( nextChar == '\n' || nextChar == '\r' )  */
    
    /*
     *  Read string until next delimiter, i.e., a white space, or EOF.  Add 
     *  character to buffer and increase buffer length.  Check length for 
     *  overflow during loop.  If overflow occurs, copy value in buffer to 
     *  value and return with error flag.  After loop, copy value of buffer to 
     *  value allocating memory.
     */
    buffer[ 0 ]  = nextChar;
    bufferLength = 1;

    nextChar = fgetc( file );
    while ( nextChar != EOF && !isspace( nextChar ) ) {
        if ( bufferLength >= MAXIMUM_BUFFER_LENGTH ) {
            errorMessage( "maximum buffer length exceeded", REPLACE );
            errorMessage( name, PREPEND );
            flushErrorMessage();
            
            buffer[ bufferLength + 1 ] = '\0';  /*  terminate string        */
            return ( buffer );                  /*  return current buffer   */
        }   /*  end of if ( bufferLength >= MAXIMUM_BUFFER_LENGTH ) */

        buffer[ bufferLength ] = (Char)nextChar;
        bufferLength++;
        
        nextChar = fgetc( file );
    }   /*  end of loop for nextChar != EOF and !isspace    */

    buffer[ bufferLength ] = '\0';  /*  terminate string        */
    
    /*
     *  The last character read from stream is an EOF, end-of-line indicator, 
     *  or a white space character.  Except for EOF, push all other characters 
     *  back onto stream for the next read.  The getString routine will NEVER 
     *  read beyond the current line and will NOT remove the end-of-line 
     *  character from the input stream.  In case pushing the last character 
     *  back onto the stream fails, delete the string and return error code.
     */
    if ( nextChar != EOF ) {
        Int check;
        
        check = ungetc( nextChar, file );
        if ( check != nextChar || check == EOF ) {
            errorMessage( "error pushing character back onto stream", REPLACE );
            errorMessage( name, PREPEND );
            flushErrorMessage();

            return ( NULL );    /*  return current buffer */
        }   /*  end of if check != nextChar || check == EOF */
    }   /*  end of if ( nextChar != EOF )   */
    
    return ( buffer );
}   /*  end of getString()  */
