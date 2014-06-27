
/*
 *  DIS Data Management Metrics
 *
 *      This header file contains the type definition for the timing
 *      variable and metric structures used by the DIS Data Management
 *      benchmark.  The metric structures consist of a single overall
 *      structure and individual command metric structures.
 *
 *  Revision History:
 *
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifndef     DIS_METRICS_H
#define     DIS_METRICS_H

#include "dataManagement.h"     /* for primitive type definitions */
#include "getNextCommandCode.h" /* for CommandType enumeration    */

/*
 *  Time value used by DIS Data Management benchmark
 *      - value represents milliseconds since program start
 */
typedef long int Time;

/*
 *  Double value used for statistic calculations 
 */
typedef double Double;

/*
 *  Command Metric structure
 *      - metric structure for individual command metric statistics
 *      - each command should have a separate structure
 *      - structure members naturally separate into two groups:
 *        (1) Internal use members which should only be altered by
 *            metric routines.
 *        (2) External use members which provide statistic values
 *      - Note that the typedef Time could be changed to a structure
 *        for different systems and/or timing methods.  This change
 *        should not require a change of this structure definition.
 */
typedef struct
{
    /*
     *  Internal use members
     */
    Time        lastTimeMark;   /*  timing mark for command duration calc   */
    Int         numOfCommands;  /*  number of commands (for average)        */
    Double      sum;            /*  sum of separate command times           */
    Double      sumSquares;     /*  sum of the squares of command times     */

    /*
     *  External use members
     */
    Time        worst;          /*  worst time recorded for this command       */
    Time        best;           /*  best time recorded for this command        */
    Double      avg;            /*  average time (as Double) for command       */
    Double      deviation;      /*  standard deviation (as Double) for command */
} CommandMetric;

/*
 *  Metric structure
 *      - performance data for current data set execution
 *      - includes total time to complete, input time, output time and separate
 *        performance measures of the insert, query, and delete commands.
 *      - Note that the typedef Time could be changed to a structure
 *        for different systems and/or timing methods.  This change
 *        should not require a change of this structure definition.
 */
typedef struct
{
    Time            totalTime;              /*  total time to complete      */
                                            /*  data set                    */
    Time            inputTime;              /*  total time required for     */
                                            /*  input of data set           */
    Time            outputTime;             /*  total time required for     */
                                            /*  output of queries           */
    CommandMetric   insertCommandMetric;    /*  insert metric data          */
    CommandMetric   queryCommandMetric;     /*  query metric data           */
    CommandMetric   deleteCommandMetric;    /*  delete metric data          */

    CommandType     lastCommand;
} Metrics;

/*
 *  Time value parameters
 */
#define MINIMUM_VALUE_OF_TIME   MINIMUM_VALUE_OF_INT
#define MAXIMUM_VALUE_OF_TIME   MAXIMUM_VALUE_OF_INT

#endif  /*  DIS_METRICS_H   */
