
/*
 *  Name:           updateMetricsData
 *  Input:          Metrics structure
 *  Output:         Metrics structure
 *  Return:         void
 *  Description:    This routine determines the time required to complete
 *                  a command.  The routine uses the lastCommand field of
 *                  of the metrics structure to determine which command 
 *                  to update.  If the value of lastCommand is not one
 *                  of the commands which has a command metric, then the
 *                  user is informed and the routine exits.  The routine
 *                  updates the best and worst time fields of the command
 *                  metric.  Also, the running sum and sum of the squares
 *                  for this command are updated.
 *  Calls:          errorMessage()
 *                  flushErrorMessage()
 *                  getTime()
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
#include "errorMessage.h"       /* for errorMessage() definition  */
#include "getNextCommandCode.h" /* for CommandType enumeration    */
#include "metrics.h"            /* for Metrics definition         */

/*
 *  Function prototypes
 */
extern Time getTime( void );

void updateMetricsData( Metrics *metrics ) /* metrics struct to update */
{   /*  begin updateMetricsData()   */
    Time          temp;           /*  variable used to find current time  */
    CommandMetric *commandMetric; /*  command metric to update            */

    static Char name[] = "updateMetricsData";

    assert( metrics );
    assert( !( metrics->lastCommand != INIT   && \
               metrics->lastCommand != INSERT && \
               metrics->lastCommand != QUERY  && \
               metrics->lastCommand != DELETE && \
               metrics->lastCommand != NONE   && \
               metrics->lastCommand != INVALID ) );

    /*
     *  Set the commandMetric pointer to the correct command metric to update 
     *  based on the lastCommand field.  The lastCommand field is assumed to be 
     *  correctly setup before the call to updateMetricsData().  If the value of
     *  lastCommand is not an INSERT, QUERY, or DELETE, an error message is 
     *  displayed, lastCommand is set to INVALID, and the routine returns 
     *  without updating any of the other command metric structures.
     */
    if ( metrics->lastCommand == INSERT ) {
        commandMetric = &(metrics->insertCommandMetric);
    }   /*  end of command == INSERT        */    
    else if ( metrics->lastCommand == QUERY ) {
        commandMetric = &(metrics->queryCommandMetric);
    }   /*  end of command == QUERY         */    
    else if ( metrics->lastCommand == DELETE ) {
        commandMetric = &(metrics->deleteCommandMetric);
    }   /*  end of command == DELETE        */
    else {
        metrics->lastCommand = INVALID;
        return;
    }   /*  end of inappropriate command    */

    /*
     *  Get the current time and update the command metric.  The first step is 
     *  to calculate the time difference between the current time and the 
     *  lastTimeMark stored in the command metric structure.  A check is made to
     *  insure that the time returned is a valid time, i.e., non-negative.  The 
     *  next step is to compare the time difference, stored as temp, with the 
     *  best and worst time differences stored in the command structure.  If 
     *  temp is less than the best, the best is set to temp.  If temp is greater
     *  than the worst, the worst is set to temp.  Next, the running sum is 
     *  updated and the running sum of the squares is also updated.  Finally, 
     *  the number of commands updated for this command metric is incremented 
     *  for later statistical calculations in calcMetricsData().
     */
    temp = getTime() - commandMetric->lastTimeMark;

    if ( temp < 0 ) {
        errorMessage( "lastTimeMark doesn't seem to be set", REPLACE );
        errorMessage( name, PREPEND );
        flushErrorMessage();
    }   /*  end of validity check on time */
    else {
        if ( temp < commandMetric->best ) {
            commandMetric->best = temp;
        }   /*  end of temp < best */

        if ( temp > commandMetric->worst ) {
            commandMetric->worst = temp;
        }   /*  end of temp > worst */

        commandMetric->sum        += (Double)temp;
        commandMetric->sumSquares += ( (Double)temp * (Double)temp );

        commandMetric->numOfCommands++;
    }   /*  end of update of command metric */

    /*
     *  Set lastCommand field to INVALID to prevent any erroneous update repeats
     */
    metrics->lastCommand = INVALID;

    return;
}   /*  end of updateMetricsData()  */
