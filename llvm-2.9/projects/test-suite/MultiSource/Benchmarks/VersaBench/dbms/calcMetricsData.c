
/*
 *  Name:           calcMetricsData
 *  Input:          Metrics structure
 *  Output:         Metrics structure
 *  Return:         void
 *  Description:    This routine calculates the average and standard 
 *                  deviation of the provided command metric structures.
 *                  The routine checks that at least one sample time
 *                  difference is present in the command metric structure
 *                  which is placed via the updateMetricsData() routine.
 *                  Two errors are possible for this routine: (1) the
 *                  command metric structure has no samples, and (2) 
 *                  round-off has caused the calculated variance to be
 *                  a negative number, although this is analytically
 *                  impossible.  In the case of either error, the average
 *                  and standard deviation members are set to zero which 
 *                  may help prevent their later use if "junk" were left
 *                  there either as initialization or from previous
 *                  calculations.
 *  Calls:          getTime()
 *      System:     sqrt()
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

#include <assert.h>         /* for assert()                   */
#include <math.h>           /* for sqrt() definition          */
#include "dataManagement.h" /* for primitive type definitions */
#include "metrics.h"        /* for Metrics definition         */

/*
 *  Function prototype
 */
extern Time getTime( void );

void calcMetricsData( Metrics *metrics )  /*  metrics struct to calculate */
{   /*  begin calcMetricsData()  */
    Double temp;

    assert( metrics );

    /*
     *  Determine total time for application execution
     */
    metrics->totalTime = getTime() - metrics->totalTime;

    /*
     *  Calculate metrics for Insert Command: calculate the average and the
     *  the variance.  If the variance is non-negative, calculate the standard
     *  deviation.
     */
    if ( metrics->insertCommandMetric.numOfCommands > 0 ) {
        /*
         *  Calculate metrics for Insert Command: calculate the average and the
         *  the variance.  If the variance is non-negative, calculate the standard
         *  deviation.
         */
        metrics->insertCommandMetric.avg = metrics->insertCommandMetric.sum 
                               / metrics->insertCommandMetric.numOfCommands;
        temp = metrics->insertCommandMetric.sumSquares 
             - metrics->insertCommandMetric.sum 
             * metrics->insertCommandMetric.sum 
             / metrics->insertCommandMetric.numOfCommands;
        if ( temp < 0.0 ) {
            metrics->insertCommandMetric.avg       = MINIMUM_VALUE_OF_FLOAT;
            metrics->insertCommandMetric.deviation = MINIMUM_VALUE_OF_FLOAT;
        } /* end of temp < 0.0 - round-off error for variance */
        else {
            metrics->insertCommandMetric.deviation = sqrt( temp / 
                   metrics->insertCommandMetric.numOfCommands );
        } /* end of temp >= 0.0 */
    } /* end of insert->numOfCommands > 0 */
    else {
        metrics->insertCommandMetric.avg       = MINIMUM_VALUE_OF_FLOAT;
        metrics->insertCommandMetric.deviation = MINIMUM_VALUE_OF_FLOAT;
    } /* end of insert->numOfCommands == 0 */

    /*
     *  Calculate metrics for Query Command: calculate the average and the
     *  the variance.  If the variance is non-negative, calculate the standard
     *  deviation.
     */
    if ( metrics->queryCommandMetric.numOfCommands > 0 ) {
        metrics->queryCommandMetric.avg = metrics->queryCommandMetric.sum 
                              / metrics->queryCommandMetric.numOfCommands;
        temp = metrics->queryCommandMetric.sumSquares 
             - metrics->queryCommandMetric.sum 
             * metrics->queryCommandMetric.sum 
             / metrics->queryCommandMetric.numOfCommands;
        if ( temp < 0.0 ) {
            metrics->queryCommandMetric.avg       = MINIMUM_VALUE_OF_FLOAT;
            metrics->queryCommandMetric.deviation = MINIMUM_VALUE_OF_FLOAT;
        } /* end of temp < 0.0 - round-off error for variance */
        else {
            metrics->queryCommandMetric.deviation = sqrt( temp / 
                     metrics->queryCommandMetric.numOfCommands );
        } /* end of temp >= 0.0 */
    } /* end of query->numOfCommands > 0 */
    else {
        metrics->queryCommandMetric.avg       = MINIMUM_VALUE_OF_FLOAT;
        metrics->queryCommandMetric.deviation = MINIMUM_VALUE_OF_FLOAT;
    } /* end of query->numOfCommands == 0 */

    /*
     *  Calculate metrics for Delete Command: calculate the average and the
     *  the variance.  If the variance is non-negative, calculate the standard
     *  deviation.
     */
    if ( metrics->deleteCommandMetric.numOfCommands > 0 ) {
        metrics->deleteCommandMetric.avg = metrics->deleteCommandMetric.sum 
                               / metrics->deleteCommandMetric.numOfCommands;
        temp = metrics->deleteCommandMetric.sumSquares 
             - metrics->deleteCommandMetric.sum 
             * metrics->deleteCommandMetric.sum 
             / metrics->deleteCommandMetric.numOfCommands;
        if ( temp < 0.0 ) {
            metrics->deleteCommandMetric.avg       = MINIMUM_VALUE_OF_FLOAT;
            metrics->deleteCommandMetric.deviation = MINIMUM_VALUE_OF_FLOAT;
        } /* end of temp < 0.0 - round-off error for variance */
        else {
            metrics->deleteCommandMetric.deviation = sqrt( temp / 
                 metrics->deleteCommandMetric.numOfCommands );
        } /* end of temp >= 0.0 */
    } /* end of delete->numOfCommands > 0 */
    else {
        metrics->deleteCommandMetric.avg       = MINIMUM_VALUE_OF_FLOAT;
        metrics->deleteCommandMetric.deviation = MINIMUM_VALUE_OF_FLOAT;
    } /* end of delete->numOfCommands == 0 */

    return;
}   /*  end of calcMetricsData() */
