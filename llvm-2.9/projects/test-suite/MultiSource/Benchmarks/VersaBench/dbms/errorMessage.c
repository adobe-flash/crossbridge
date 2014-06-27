
/*
 *  File Name:      errorMessage.c
 *  Purpose:        Routines to provide error messaging for the baseline 
 *                  application.
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <stdlib.h>         /* for NULL definition                           */
#include <stdio.h>          /* for fprintf() and fflush() definitions        */
#include <string.h>         /* for strlen(), strcpy(), strcat() definitions  */
#include "dataManagement.h" /* for primitive type definitions                */
#include "errorMessage.h"   /* for PREPEND and REPLACE definitions           */

/*
 *  Static definition of error buffer string
 */
#define ERROR_BUFFER_LENGTH 1023

static Char errorBuffer[ ERROR_BUFFER_LENGTH + 1 ] = "\0";

/*
 *  Name:           errorMessage
 *  Input:          message string
 *                  flag to prepend to current buffer 
 *  Output:         none
 *  Return:         void
 *  Description:    The routine places an error message into a buffer for 
 *                  (possible) later display/flushing.  The first argument is 
 *                  the message to add to buffer and the second argument is an 
 *                  integer flag on whether to prepend the message to the 
 *                  current buffer contents (PREPEND), or ignore the current 
 *                  contents and replace the buffer with the input message 
 *                  (REPLACE).
 *  Calls:          
 *      System:     fprintf()
 *                  strlen()
 *                  strcat()
 *                  strcpy()
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

void errorMessage( Char *message,   /*  message to prepend      */
                   Boolean prepend )/*  flag to clear buffer    */
{   /*  beginning of errorMessage() */
    /*
     *  If prepend flag is not set, replace current buffer with input message.  
     *  If prepend flag is set, then check if adding message to current buffer 
     *  will exceed maximum.  If it does exceed limit, flush output.  
     *  Otherwise, pre-pend message to current buffer.
     */
    if ( prepend == REPLACE ) {
        /*
         *  Check if message is too large for buffer
         */
        if ( strlen( message ) > ERROR_BUFFER_LENGTH ) {
            /*
             *  Message is too large for buffer. Flush message immediately.
             */
            fprintf( stderr, "Error Message Too Large for Buffer: flushing\n" );
            fprintf( stderr, "unconnected: %s\n", message );
        } /*  end of if strlen( message ) > ERROR_BUFFER_LENGTH */
        else {
            strcpy( errorBuffer, message );
        } /*  end of if strlen( message ) <= ERROR_BUFFER_LENGTH */
    } /* end of prepend = REPLACE */
    else {
        /*
         *  Check if buffer is large enough
         */
        if ( strlen( errorBuffer ) + strlen( message ) > ERROR_BUFFER_LENGTH ) {
            /*
             *  Current buffer plus message is too large.  Flush everything.
             */
            fprintf( stderr, "Error Message Buffer full: flushing\n" );
            fprintf( stderr, "unconnected: %s: %s\n", message, errorBuffer );
            
            errorMessage( "unconnected", REPLACE );
        } /* end of if strlen( errorBuffer + message ) > ERROR_BUFFER_LENGTH */
        else {
            /*
             *  Pre-pend message to current buffer
             */
            Char tempBuffer[ ERROR_BUFFER_LENGTH + 1 ];

            strcpy( tempBuffer, message );
            strcat( tempBuffer, ": " );
            strcat( tempBuffer, errorBuffer );
            strcpy( errorBuffer, tempBuffer );
        } /* end of if strlen( errorBuffer + message ) <= ERROR_BUFFER_LENGTH */
    } /* end of prepend == PREPEND */

    return;
}   /*  end of errorMessage()   */

/*
 *  Name:           flushErrorMessage
 *  Input:          none
 *  Output:         none
 *  Return:         void
 *  Description:    
 *  Calls:          
 *      System:     fprintf()
 *  Author:         M.L.Rivas
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

void flushErrorMessage( void )
{   /*  beginning of flushErrorMessage() */
    /*
     *  Place current error Buffer in stderr stream
     */
    if ( strlen( errorBuffer ) > 0 ) {
        fprintf( stderr, "%s\n", errorBuffer );
        fflush( stderr );
    } /* end of if strlen( errorBuffer ) > 0 */
    
    return;
}   /*  end of flushErrorMessage()   */
