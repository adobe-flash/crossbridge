
/*
 *  DIS Data Management Error Message
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  24May99 Matthew Rivas   Created
 *
 *              Copyright 1999, Atlantic Aerospace Electronics Corp.
 */
 
#ifndef     DIS_ERROR_MESSAGE_H
#define     DIS_ERROR_MESSAGE_H

#include "dataManagement.h"

/*
 *  Message prepend or replace values
 */
#define PREPEND		TRUE
#define	REPLACE		FALSE

/*
 *  Function Prototypes
 */
extern void errorMessage( Char * message, Boolean prepend ); 
extern void flushErrorMessage( void );

#endif  /*  DIS_ERROR_MESSAGE_H */
