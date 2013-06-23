
/*
 *  Name:           closeFiles
 *  Input:          input FILE pointer
 *                  output FILE pointer
 *                  metric FILE pointer
 *  Output:         none
 *  Return:         void
 *  Description:    Routine closes the three files used during application.  
 *                  The return values for the system call to close the files 
 *                  are checked but not acted upon.  Any error occuring during 
 *                  the closing of the files is non-fatal, since the routine is 
 *                  part of the Input & Output module exit process.  
 *                  Appropriate messages are placed in the error message buffer 
 *                  whenever an error does occur.
 *  Calls:          errorMessage()
 *      System:     assert()
 *                  fclose()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /* for assert()                     */
#include <stdio.h>          /* for FILE definition and fclose() */
#include "dataManagement.h" /* for primitive data types         */
#include "errorMessage.h"   /* for errorMessage() definition    */

void closeFiles( FILE *inputFile,   /*  input file to close   */
                 FILE *outputFile,  /*  output file to close  */
                 FILE *metricFile ) /*  metrics file to close */
{   /* beginning of closeFiles() */
    Int error;  /* error return from fclose() system call */

    static Char name[] = "closeFiles";

    assert( inputFile );
    assert( outputFile );
    assert( metricFile );

    /*
     *  Close each file provided.  Return values are checked, but not acted 
     *  upon.  Any error return is non-fatal since the files will only be 
     *  closed during the Input & Output module exit.
     */
    error = fclose( inputFile );
    if ( error != 0 ) {
        errorMessage( "error closing input file", REPLACE );
        errorMessage( name, PREPEND );
    }   /*  end of if ( error != 0 )    */
    
    error = fclose( outputFile );
    if ( error != 0 ) {
        errorMessage( "error closing output file", REPLACE );
        errorMessage( name, PREPEND );
    }   /*  end of if ( error != 0 )    */
    
    error = fclose( metricFile );
    if ( error != 0 ) {
        errorMessage( "error closing metric file", REPLACE );
        errorMessage( name, PREPEND );
    }   /*  end of if ( error != 0 )    */
    
    return;
}   /* end of closeFiles() */
