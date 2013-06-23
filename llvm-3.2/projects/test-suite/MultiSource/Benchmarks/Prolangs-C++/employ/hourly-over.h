/* hourly-over.h                                                   -*- C++ -*-
**    Include file for Hourly Overtime class
** 
** COPYRIGHT (C) 1994 Bradley M. Kuhn
**
** Written   :   Bradley M. Kuhn                                 Loyola College
**   By
**
** Written   :   David W. Binkley                                Loyola College
**   For         
**
** Acknowledgements:
**    This code is based on code that appears in:
**       C++ How to Program by H. M. Deitel and P. J. Deitel
**         Prentice Hall, New Jersey, p. 537
**
** RCS       :
**
** $Source$
** $Revision: 16678 $
** $Date: 2004-10-04 17:37:32 -0700 (Mon, 04 Oct 2004) $
**
** $Log$
** Revision 1.2  2004/10/05 00:37:32  lattner
** Stop using deprecated headers
**
** Revision 1.1  2004/10/04 20:01:13  lattner
** Initial checkin of all of the source
**
** Revision 0.1  1994/12/24  00:50:58  bkuhn
**   # initial version
**
**
*/

#ifndef _HOURLY_OVER_H
#define _HOURLY_OVER_H

#include "hourly.h"

#include <iostream>
#include <stdlib.h>
using namespace std;

#define HOURLY_WORKER_OVERTIME_ID 6
/* An hourly worker overtime gets paid for every hour worked, and time and 
** a half for overtime hours */

class HourlyWorkerOvertime : public HourlyWorker {

  public:
    HourlyWorkerOvertime(const char *, const char * , float = 0.0);

    virtual float Earnings();
};
/*****************************************************************************/
HourlyWorkerOvertime::HourlyWorkerOvertime(const char *first, const char *last,
                                     float startWage) 
 : HourlyWorker(first, last, startWage)    // this will call Wage's constructor
{
    return;
}
/*****************************************************************************/
float
HourlyWorkerOvertime::Earnings() {
    float totHours, overHours;
    
    totHours = ThisWeekHours();

    if (totHours > 40.0) {
        overHours = totHours - 40.0;
    } else {
        overHours = 0.0;
    }

    return (Wage() * totHours) + ( (Wage() / 2.0) * overHours);
}

#endif
