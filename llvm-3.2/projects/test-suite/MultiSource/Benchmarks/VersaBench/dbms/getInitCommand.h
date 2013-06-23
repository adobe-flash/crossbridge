
/*
 *  DIS Data Management: getInitCommand
 *
 *      This header file defines the return codes for the getInitCommand 
 *      function.  Any file which uses the getInitCommand routine should 
 *      include this header.  The first code indicates success.  The second 
 *      code is the result of an I/O error at a lower-level read function.  The 
 *      third code indicates that an end-of-line or end-of-file indicator was 
 *      read before the init command was finished.  The fourth error is when 
 *      the fan size read was invalid (too large or too small).
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_GET_INIT_COMMAND_H
#define     DIS_GET_INIT_COMMAND_H

#include <stdio.h>          /* for FILE definition      */
#include "dataManagement.h" /* for primitive data types */

/*
 *  Return codes
 */
#define GET_INIT_SUCCESS        0
#define GET_INIT_IO_ERROR       1
#define GET_INIT_EARLY_EOI      2
#define GET_INIT_INVALID_FAN    3

/*
 *  Function Prototype
 */
extern Int getInitCommand( FILE *file, Int *fan );

#endif  /*  DIS_GET_INIT_COMMAND_H    */
