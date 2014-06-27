
/*
 *  Name:           clearLine
 *  Input:          FILE ptr
 *  Output:         none
 *  Return:         void
 *  Description:    Reads from the current file position to the end of the line 
 *                  indicated by a new line character '\n' or carriage return 
 *                  '\r'.  Will also stop at EOF.  Contents of line are not 
 *                  saved and are lost.  The end-of-line indicator is also 
 *                  removed from the stream.
 *  Calls:          
 *      System:     fgetc()
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <stdio.h>          /* for FILE and fgetc() definitions */
#include "dataManagement.h" /* for primitive data types         */

void clearLine( FILE *file )
{  /* begin clearLine() */
    Int c; /* temporary character read from file */
    
    c = fgetc( file );
    while ( c != EOF && c != '\n' && c != '\r' ) {
        c = fgetc( file );
    }  /*  end loop while ( c != EOF, etc. */

    return;
}  /* end of clearLine() */
