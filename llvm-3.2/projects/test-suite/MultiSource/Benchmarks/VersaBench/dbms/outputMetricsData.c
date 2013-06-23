
/*
 *  Name:           outputMetricsData
 *  Input:          FILE pointer for output, file
 *                  metrics structure, metrics
 *  Output:         none
 *  Return:         void
 *  Description:    The routine displays the metrics collected during a 
 *                  execution of the baseline application.  The routine 
 *                  calcMetricsData() is called to setup all of the displayed 
 *                  fields.
 *  Calls:          calcMetricsData()
 *      System:     fprintf()
 *  Author:         M.L.Rivas
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  28May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#include <assert.h>         /*  for assert()                    */
#include <stdio.h>          /*  for fprintf() definition        */
#include "dataManagement.h" /*  for primitive type definitions  */
#include "metrics.h"        /*  for Metrics definitions         */

/*
 *  Function prototype
 */
extern void calcMetricsData( Metrics *metrics );

void outputMetricsData( FILE *file,         /*  output file         */
                        Metrics *metrics )  /*  metrics to output   */
{   /*  beginning of outputMetricsData()    */

    assert( file );
    assert( metrics );

    /*
     *  Calculate the metric statistical values
     */
    calcMetricsData( metrics );
    
    /*
     *  Display the metric statistical values
     */
    fprintf( file, "DIS Data Management Metrics\n" );
    fprintf( file, "    total time  = %li msecs\n", metrics->totalTime );
    fprintf( file, "    input time  = %li msecs\n", metrics->inputTime );
    fprintf( file, "    output time = %li msecs\n", metrics->outputTime );
    
    fprintf( file, "    Insert Commmand Metrics:\n" );
    fprintf( file, "        best time          = %li msecs\n",
             metrics->insertCommandMetric.best );
    fprintf( file, "        worst time         = %li msecs\n",
             metrics->insertCommandMetric.worst );
    fprintf( file, "        average            = %f msecs\n",
             metrics->insertCommandMetric.avg );
    fprintf( file, "        standard deviation = %f msecs\n",
             metrics->insertCommandMetric.deviation );

    fprintf( file, "    Query Commmand Metrics:\n" );
    fprintf( file, "        best time          = %li msecs\n",
             metrics->queryCommandMetric.best );
    fprintf( file, "        worst time         = %li msecs\n",
             metrics->queryCommandMetric.worst );
    fprintf( file, "        average            = %f msecs\n",
             metrics->queryCommandMetric.avg );
    fprintf( file, "        standard deviation = %f msecs\n",
             metrics->queryCommandMetric.deviation );

    fprintf( file, "    Delete Commmand Metrics:\n" );
    fprintf( file, "        best time          = %li msecs\n",
             metrics->deleteCommandMetric.best );
    fprintf( file, "        worst time         = %li msecs\n",
             metrics->deleteCommandMetric.worst );
    fprintf( file, "        average            = %f msecs\n",
             metrics->deleteCommandMetric.avg );
    fprintf( file, "        standard deviation = %f msecs\n",
             metrics->deleteCommandMetric.deviation );

    return;
}   /*  end of outputMetricsData()  */
