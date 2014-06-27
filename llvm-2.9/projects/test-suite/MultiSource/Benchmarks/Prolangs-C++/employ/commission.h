/* commission.h                                                     -*- C++ -*-
**    Include file for Commission Worker class
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
**         Prentice Hall, New Jersey, pp. 534-5
**
** RCS       :
**
** $Source$
** $Revision: 16687 $
** $Date: 2004-10-04 17:57:47 -0700 (Mon, 04 Oct 2004) $
**
** $Log$
** Revision 1.3  2004/10/05 00:57:47  lattner
** Don't use unitiialized values
**
** Revision 1.2  2004/10/05 00:37:32  lattner
** Stop using deprecated headers
**
** Revision 1.1  2004/10/04 20:01:13  lattner
** Initial checkin of all of the source
**
** Revision 0.2  1994/12/31  01:21:59  bkuhn
**   -- version were getting data from
**
** Revision 0.1  1994/12/28  01:36:49  bkuhn
**   # initial version
**
**
*/

#ifndef _COMMISSION_H
#define _COMMISSION_H

#include "employee.h"

#include <iostream>
#include <stdlib.h>
using namespace std;

#define COMMISSIONED_WORKER_ID 2
/* A commissioned worker gets a weekly salary + commission per quantity sold */

class CommissionedWorker : public Employee {
  private:
    float        weeklySalary;         // base salary
    float        commissionRate;       // % commission made on weekly sales
    float        thisWeekSales;        // total sales this week

  protected:
    void          SalesThisWeek(float);

  public:
    CommissionedWorker(const char *, const char * , float = 0.0, float = 0.0);

    void          SetWeeklySalary(float);
    void          SetCommissionRate(float);
    virtual float Earnings();
    virtual void  Print();
    virtual void  Raise(int);
    virtual void  NewWeek();
};
/*****************************************************************************/
CommissionedWorker::CommissionedWorker(const char *first, const char *last,
                                     float startSalary, float startCommission) 
 : Employee(first, last)        // this will call Employee's constructor
{
    thisWeekSales = 0;
    SetWeeklySalary(startSalary);
    SetCommissionRate(startCommission);
    dollarsToRaise = 10.0;
}
/*****************************************************************************/
void
CommissionedWorker::SetWeeklySalary(float newSalary) 
{
    weeklySalary = (newSalary > 0.0) ? newSalary : 0.0;
}
/*****************************************************************************/
void
CommissionedWorker::SetCommissionRate(float newRate) 
{
    if (newRate < 0.0)
      commissionRate = 0.0;
    else if (newRate > 60.0)
      commissionRate = 60.0;
    else
      commissionRate = newRate;
}
/*****************************************************************************/
void
CommissionedWorker::SalesThisWeek(float sales) 
{
    thisWeekSales = sales;
}
/*****************************************************************************/
float
CommissionedWorker::Earnings() {
    return weeklySalary + (commissionRate / 100.0) * thisWeekSales;
}
/*****************************************************************************/
void
CommissionedWorker::Print() {
    cout << "Commissioned Worker: " << FirstName() << ' ' << LastName();
}
/*****************************************************************************/
void
CommissionedWorker::Raise(int units)
{
    if (units > 0) {
        SetCommissionRate(commissionRate + units);
        weeklySalary += units * dollarsToRaise;
    }
}
/*****************************************************************************/
void
CommissionedWorker::NewWeek()
{
    int quantity = 5; // rand() % 5;

    SalesThisWeek(quantity * 5000.0);
}

#endif
