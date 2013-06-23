/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 42659 $
 *********************************************************************EHEADER*/

/*
 * File:	timer.c
 * Copyright:	(c) 1997 The Regents of the University of California
 * Author:	Scott Kohn (skohn@llnl.gov)
 * Description:	somewhat portable timing routines for C++, C, and Fortran
 *
 * If TIMER_USE_MPI is defined, then the MPI timers are used to get
 * wallclock seconds, since we assume that the MPI timers have better
 * resolution than the system timers.
 */

#include <time.h>
#ifndef __MINGW32__
#include <sys/times.h>
#endif
#ifdef TIMER_USE_MPI
#include "mpi.h"
#endif

double time_getWallclockSeconds(void)
{
   return(0.0);
}

double time_getCPUSeconds(void)
{
   return(0.0);
}

double time_get_wallclock_seconds_(void)
{
   return(time_getWallclockSeconds());
}

double time_get_cpu_seconds_(void)
{
   return(time_getCPUSeconds());
}
