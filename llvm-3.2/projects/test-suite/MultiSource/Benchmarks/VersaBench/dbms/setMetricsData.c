

/*
 *  Name:           setMetricsData
 *  Input:          Metrics structure
 *  Output:         Metrics structure
 *  Return:         void
 *  Description:    This routine sets two values for laster metric collection.  
 *                  The first is specific to the individual command being 
 *                  processed and sets the lastTimeMark field for that command 
 *                  metric structure.  The second value set is the command type 
 *                  for later update which is stored in the lastCommand filed of
 *                  the input Metric structure.  If a command type is passed 
 *                  which is not recognized, it sets the lastCommand field to
 *                  INVALID.
 *  Calls:          getTime()
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

#include <assert.h>             /* for assert()                   */
#include "dataManagement.h"     /* for primitive type definitions */
#include "getNextCommandCode.h" /* for Commandtype enumeration    */
#include "metrics.h"            /* for Metrics definition         */

/*
 *  Function prototype
 */
extern Time getTime( void );

void setMetricsData( Metrics     *metrics, /* metrics struct to update */
                     CommandType command ) /* command to update        */
{   /*  begin setMetricsData()  */
    Time temp;   /*  variable used to find current time          */

    assert( metrics );
    assert( command == INIT   || \
            command == INSERT || \
            command == QUERY  || \
            command == DELETE || \
            command == NONE   || \
            command == INVALID );

    /*
     *  Get the current time and set the appropriate command metric lastTimeMark
     *  field to this time.  Also, set lastCommand field.  If command is not an 
     *  INSERT, QUERY, or DELETE, set lastCommand to INVALID which will prevent
     *  the updateMetricsData() routine from incorrectly altering any of the 
     *  individual command metric statistics.
     */
    temp = getTime();
    if ( command == INSERT ) {
        metrics->insertCommandMetric.lastTimeMark = temp;
        metrics->lastCommand = INSERT;
    }   /*  end of command == INSERT        */    
    else if ( command == QUERY ) {
        metrics->queryCommandMetric.lastTimeMark = temp;
        metrics->lastCommand = QUERY;
    }   /*  end of command == QUERY         */    
    else if ( command == DELETE ) {
        metrics->deleteCommandMetric.lastTimeMark = temp;
        metrics->lastCommand = DELETE;
    }   /*  end of command == DELETE        */
    else {
        metrics->lastCommand = INVALID;
    }   /*  end of inappropriate command    */

    return;
}   /*  end of setMetricsData() */
