
/*
 *  File Name:      timer.c
 *  Purpose:        Provides timing routines for the DIS Data Management 
 *                  Benchmark application.  Preprocessor directives are setup to
 *                  allow either the ANSI/ISO C standard time() routine to be 
 *                  used or the BSD Unix gettimeofday() routine to be used.  The
 *                  gettimeofday() routine has a fidelity of microseconds while 
 *                  the ANSI/ISO time() routine is only seconds.  The Data 
 *                  Management baseline data sets will generally require a 
 *                  better timing resolution than seconds, but nothing exists 
 *                  within the ANSI/ISO C standard which provides the required 
 *                  fidelity.  Because the DIS hardware teams are only required 
 *                  to conform to the ANSI/ISO C standard libraries and this 
 *                  will cause inaccuracies in the metric statistics, a 
 *                  compromise is implemented.  If USE_GETTIMEOFDAY is defined 
 *                  (not TRUE/FALSE, just defined), the metrics will use the 
 *                  Unix standard gettimeofday() promoting the microseconds to 
 *                  milliseconds to allow storage in a long integer.  If  
 *                  USE_GETTIMEOFDAY is not defined, the metrics will use the 
 *                  ANSI/ISO C standard time() routine.  This will generally 
 *                  invalidate the metric statistical values for  individual 
 *                  commands, but may provide a relatively  accurate overall 
 *                  time required to complete the data set.
 *
 *                  Modifications required to run the program on different 
 *                  hardware systems with different system timing routines 
 *                  should only need to change this function.  As long as the 
 *                  getTime() routine returns milliseconds from program start, 
 *                  the rest of the baseline source code should function 
 *                  correctly.
 *
 *  Revision History:
 *  Date    Name            Revision
 *  ------- --------------- ------------------------------
 *  27May99 Matthew Rivas   Created
 *  04Jun99 Matthew Rivas   Added SECONDS_TO_MILLI to ANSI/ISO C standard ver. 
 *
 *	Copyright 1999, Atlantic Aerospace Electronics Corp.
 */

#ifdef USE_GETTIMEOFDAY
#include <sys/time.h>       /* for timeval definition           */
#include <unistd.h>         /* for gettimeofday() definition    */
#include "errorMessage.h"   /* for errorMessage() definition    */
#else  /* USE_GETTIMEOFDAY undefined */
#include <time.h>           /* for time() definition            */
#endif /* USE_GETTIMEOFDAY */

#include <assert.h>         /* for assert()                     */
#include "dataManagement.h" /* for primitive type definitions   */
#include "metrics.h"        /* for Time type definition         */

/*
 *  Static value of program start time
 *      - value is stored as timeval returned by gettimeofday()
 *      - timeval struct represents the time in seconds and microseconds since 
 *        Jan 1, 1970.
 *      - don't use structure because difficulty in working with two longs for 
 *        statistical calcs and milliseconds will suffice for timing fidelity.
 */
#ifdef USE_GETTIMEOFDAY
static struct timeval startTime;

#define MILLION             1000000
#define MILLION_TO_MILLI    1e-3
#define SECONDS_TO_MILLI    1000
#else
static time_t startTime;

#define SECONDS_TO_MILLI    1000
#endif /* USE_GETTIMEOFDAY */

/*
 *  Name:           initTime
 *  Input:          none
 *  Output:         none
 *  Return:         void
 *  Description:    Initialize the static startTime variable which will be used 
 *                  by getTime() to return time in milliseconds since program  
 *                  start, i.e., when the initTime() routine was called.
 *  Calls:          errorMessage()
 *      System:     gettimeofday(), or 
 *                  time()
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

void initTime( void )
{   /*  beginning of initTime() */
#ifdef USE_GETTIMEOFDAY
    Int error;  /* flag for error checking on gettimeofday()    */
    static Char name[] = "initTime";
#endif /* USE_GETTIMEOFDAY */
    
#ifdef USE_GETTIMEOFDAY

    error = gettimeofday( &startTime, NULL );
    if ( error == -1 ) { /* check for error */
        errorMessage( "system time failure - can't set start time", REPLACE );
        errorMessage( name, PREPEND );
        flushErrorMessage();
    }   /*  end of if error == -1 */

    /*
     *  Check that seconds and microseconds from Jan 1, 1970 are positive, and
     *  that the microseconds value is less than a million, otherwise the
     *  seconds would have been incremented.
     */
    assert( startTime.tv_sec  >= 0 && \
            startTime.tv_usec >= 0 && \
            startTime.tv_usec < MILLION );

#else  /* USE_GETTIMEOFDAY undefined */

    startTime = time( NULL );

#endif /* USE_GETTIMEOFDAY */

    return;
}   /*  end of initTime()       */

/*
 *  Name:           getTime
 *  Input:          none
 *  Output:         milliseconds since program start
 *  Return:         Time 
 *  Description:    The routine provides a wrapper for the system timing 
 *                  routines and returns the current time in milliseconds from 
 *                  the beginning of program execution, i.e., when initTime() 
 *                  routine was called.  The routine is setup to be compiled 
 *                  using the gettimeofday() routine or the ANSI/ISO standard 
 *                  time() routine.
 *  Calls:          errorMessage()
 *      System:     gettimeofday(), or 
 *                  difftime()
 *                  time()
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

Time getTime( void )
{   /*  beginning of getTime()  */
    Time currentTime;           /*  current time in milliseconds            */

#ifdef USE_GETTIMEOFDAY
    Int error;                  /*  flag for checking on gettimeofday()     */
    struct timeval temp;        /*  current time returned by gettimeofday() */
    static Char name[] = "getTime";
#else  /* USE_GETTIMEOFDAY undefined */
    time_t temp;                /*  current time returned by time()         */ 
#endif /* USE_GETTIMEOFDAY */

#ifdef USE_GETTIMEOFDAY

    /*
     *  Check that seconds and microseconds from Jan 1, 1970 are positive, and
     *  that the microseconds value is less than a million, otherwise the
     *  seconds would have been incremented.
     */
    assert( startTime.tv_sec  >= 0 && \
            startTime.tv_usec >= 0 && \
            startTime.tv_usec < MILLION );

    /*
     *  Get the current time and store in temp.  Check for error, an if none,
     *  calculate difference in milliseconds between current time and startTime.
     */
    error = gettimeofday( &temp, NULL );
    if ( error == -1 ) { /* check for error */
        errorMessage( "system time failure - can't get time", REPLACE );
        errorMessage( name, PREPEND );
        flushErrorMessage();
        
        currentTime = MINIMUM_VALUE_OF_INT;
    }   /*  end of if error == -1 */
    else {
        /*
         *  Check that seconds and microseconds from Jan 1, 1970 are positive, 
         *  and that the microseconds value is less than a million, otherwise
         *  the seconds would have been incremented.
         */
        assert( temp.tv_sec  >= 0 && \
                temp.tv_usec >= 0 && \
                temp.tv_usec < MILLION );
        /*
         *  The current time must be later than the start time for the time
         *  difference arithmetic to work properly.
         */
        assert( temp.tv_sec > startTime.tv_sec || \
                ( temp.tv_sec  == startTime.tv_sec && \
                  temp.tv_usec >= startTime.tv_usec ) );

        /*
         *  Determine milliseconds conversion of difference between the current 
         *  time, temp, and the start time, startTime.  Because 0 <= usec < 
         *  MILLION and it is assumed that the current time is later than the 
         *  start time, only two cases exist for the conversion.  The first is 
         *  that the current time microseconds are greater than the start time 
         *  microseconds.  For this case, the current time in milliseconds is 
         *  the sum of the seconds difference and the microseconds difference 
         *  with the appropriate conversion factors, i.e., SECONDS_TO_MILLI = 
         *  1000 and MILLION_TO_MILLI = 1e-3.  The second case is that the 
         *  current time microseconds is less than the start time microseconds. 
         *  For this case, the current time in milliseconds is the sum of the 
         *  seconds difference minus one and the microseconds difference plus a 
         *  MILLION with the appropriate conversion factors.  Note that the 
         *  minus one to the seconds difference and plus a MILLION to the 
         *  microseconds, is the carry, i.e., a second was taken from the 
         *  seconds place and added to the microseconds place.
         */
        if ( temp.tv_usec >= startTime.tv_usec ) {
            currentTime  = SECONDS_TO_MILLI * ( temp.tv_sec  - startTime.tv_sec );
            currentTime += MILLION_TO_MILLI * ( temp.tv_usec - startTime.tv_usec );
        }   /*  end of temp.usec >= start.usec */
        else {
            currentTime  = SECONDS_TO_MILLI 
                         * ( temp.tv_sec - startTime.tv_sec - 1 );
            currentTime += MILLION_TO_MILLI 
                         * ( temp.tv_usec - startTime.tv_usec + MILLION );
        }   /*  end of temp.usec <  start.usec */
        
        /*
         *  Check for overflow of negative time difference, both of which should
         *  never happen since the system call should ensure non-negative 
         *  differences and the maximum value for a time difference using 
         *  milliseconds is 49.71 days.
         */
        if ( currentTime > MAXIMUM_VALUE_OF_INT ) {
            errorMessage( "overflow error calculating current time", REPLACE );
            errorMessage( name, PREPEND );
            flushErrorMessage();
            
            currentTime = MAXIMUM_VALUE_OF_INT;
        }   /*  end of upper bound check on currentTime */
        else if ( currentTime < 0 ) {
            errorMessage( "start time appears not to be set", REPLACE );
            errorMessage( name, PREPEND );
            flushErrorMessage();
            
            currentTime = 0;
        }   /*  end of lower bound check on currentTime */
    }   /*  end of if error != -1 */

#else  /* USE_GETTIMEOFDAY undefined */

    /*
     *  Get the current time, determine the difference in seconds since
     *  the start time, convert seconds to milliseconds, and set current
     *  time to correct value. 
     */
    temp        = time( NULL );
    currentTime = SECONDS_TO_MILLI * (Int)difftime( temp, startTime );

#endif /* USE_GETTIMEOFDAY */

    return ( currentTime );
}   /*  end of getTime()        */
