
/*
 *  DIS Data Management: openFiles
 *
 *      This header file contains the return codes and function prototype for 
 *      the openFiles routine.  The fist non-zero return code indicates that the
 *      usage flag was present in the command-line options.  The second non-zero
 *      return code is for a usage error and the final three error return codes 
 *      indicate which of the files could not be opened.
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *  06Jul99 Matthew Rivas   Added return flag USAGE_REQUEST and USAGE_ERROR
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_OPEN_FILES_H
#define     DIS_OPEN_FILES_H

#include <stdio.h>          /* for FILE definition            */
#include "dataManagement.h" /* for primitive type definitions */

/*
 *  Return codes
 */
#define OPEN_FILES_SUCCESS              0
#define OPEN_FILES_USAGE_REQUEST        1
#define OPEN_FILES_USAGE_ERROR          2
#define OPEN_FILES_INPUT_FILE_ERROR     3
#define OPEN_FILES_OUTPUT_FILE_ERROR    4
#define OPEN_FILES_METRICS_FILE_ERROR   5

/*
 *  Function Prototype
 */
extern Int openFiles( Int argc, Char **argv, FILE **inputFile, 
                      FILE **outputFile, FILE **metricsFile );

#endif  /*  DIS_OPEN_FILES_H    */
