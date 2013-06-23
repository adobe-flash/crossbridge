
/*
 *  Name:           initMetricsData
 *  Input:          Metrics structure
 *  Output:         none
 *  Return:         void
 *  Description:    This routine initializes the Metrics module by setting the 
 *                  appropriate values and/or flags for later metric collection.
 *                  The timing information for total execution time, input time,
 *                  and output time are set to the current system time returned 
 *                  as timing marks for later processing.
 *
 *                  The individual command metric data are also initialized to 
 *                  either logical values (number of commands, sum of command 
 *                  times, sum of squares of command times) or to "highly 
 *                  unlikely" values (worst, best, average, deviation) which 
 *                  should be easily noticed if output.
 *
 *                  The routine updateMetricsData() uses the value of the 
 *                  lastCommand field to determine which command metric to 
 *                  update.  The first time through, there is no command so an 
 *                  initial value of NONE is placed in the field.  The 
 *                  updateMetricsData() routine should interpret this value so as 
 *                  not to update any command metrics structure for that call.
 *  Calls:          getTime()
 *                  initTime()
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

#include <assert.h>          /* for assert()                   */
#include "dataManagement.h"  /* for primitive type definitions */
#include "metrics.h"         /* for Metrics definition         */

/*
 *  Function prototypes
 */
extern void initTime( void );
extern Time getTime( void );

void initMetricsData( Metrics *metrics )  /*  metrics struct to initialize */
{   /*  begin initMetricsData() */
    Time temp;   /*  variable used to find current time          */

    assert( metrics );

    /*
     *  Initialize timer
     */
    initTime();

    /*
     *  Setup timing marks for metric collection
     */
    temp = getTime();
    metrics->totalTime  = temp;
    metrics->inputTime  = temp;
    metrics->outputTime = temp;
    
    /*
     *  Set initial values for each command metric.  The statistical values for 
     *  the worst, best, average, and standard deviation values are set to 
     *  "impossible" values which should be easily noticed if unchanged and
     *  output as a metric result.
     */
    metrics->insertCommandMetric.lastTimeMark   = MINIMUM_VALUE_OF_TIME;
    metrics->insertCommandMetric.numOfCommands  = 0;
    metrics->insertCommandMetric.sum            = 0.0;
    metrics->insertCommandMetric.sumSquares     = 0.0;
    metrics->insertCommandMetric.worst          = MINIMUM_VALUE_OF_TIME;
    metrics->insertCommandMetric.best           = MAXIMUM_VALUE_OF_TIME;
    metrics->insertCommandMetric.avg            = MINIMUM_VALUE_OF_FLOAT;
    metrics->insertCommandMetric.deviation      = MINIMUM_VALUE_OF_FLOAT;

    metrics->queryCommandMetric.lastTimeMark    = MINIMUM_VALUE_OF_TIME;
    metrics->queryCommandMetric.numOfCommands   = 0;
    metrics->queryCommandMetric.sum             = 0.0;
    metrics->queryCommandMetric.sumSquares      = 0.0;
    metrics->queryCommandMetric.worst           = MINIMUM_VALUE_OF_TIME;
    metrics->queryCommandMetric.best            = MAXIMUM_VALUE_OF_TIME;
    metrics->queryCommandMetric.avg             = MINIMUM_VALUE_OF_FLOAT;
    metrics->queryCommandMetric.deviation       = MINIMUM_VALUE_OF_FLOAT;

    metrics->deleteCommandMetric.lastTimeMark   = MINIMUM_VALUE_OF_TIME;
    metrics->deleteCommandMetric.numOfCommands  = 0;
    metrics->deleteCommandMetric.sum            = 0.0;
    metrics->deleteCommandMetric.sumSquares     = 0.0;
    metrics->deleteCommandMetric.worst          = MINIMUM_VALUE_OF_TIME;
    metrics->deleteCommandMetric.best           = MAXIMUM_VALUE_OF_TIME;
    metrics->deleteCommandMetric.avg            = MINIMUM_VALUE_OF_FLOAT;
    metrics->deleteCommandMetric.deviation      = MINIMUM_VALUE_OF_FLOAT;

    /*
     *  The routine updateMetricsData() expects the lastCommand field of the 
     *  Metric structure to indicate which command metric should be updated.  
     *  The value is set to INVALID for initialization, but should be set before
     *  the first call to updateMetricsData().
     */
    metrics->lastCommand = INVALID;

    return;
}   /*  end of initMetricsData()    */
