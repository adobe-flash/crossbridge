/* boss.h                                                         -*- C++ -*-
**    Include file for Boss class
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
**         Prentice Hall, New Jersey, pp. 533-4
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
** Revision 0.1  1994/12/24  00:35:59  bkuhn
**   # initial version
**
** Revision 0.1  1994/12/24  00:35:59  bkuhn
**   # initial version
**
**
*/

#ifndef _BOSS_H
#define _BOSS_H

#include "employee.h"

#define BOSS_ID 1

#include <iostream>
using namespace std;


/* A boss gets a weekly salary, regardless of how much (s)he works */

class Boss : public Employee {
  private:
    float weeklySalary;

  public:
    Boss(const char *, const char * , float = 0.0);

    void          SetWeeklySalary(float);
    virtual float Earnings();
    virtual void  Print();
    virtual void  Raise(int);
    virtual void  NewWeek();
};
/*****************************************************************************/
Boss::Boss(const char *first, const char *last, float startSalary) 
 : Employee(first, last)        // this will call Employee's constructor
{
    SetWeeklySalary(startSalary);
    dollarsToRaise = 100.0;
}
/*****************************************************************************/
void
Boss::SetWeeklySalary(float newSalary) 
{
    weeklySalary = (newSalary > 0.0) ? newSalary : 0.0;
}
/*****************************************************************************/
float
Boss::Earnings() {
    return weeklySalary;
}
/*****************************************************************************/
void
Boss::Print() {
    cout << "               Boss: " << FirstName() << ' ' << LastName();
}
/*****************************************************************************/
void
Boss::Raise(int units)
{
    if (units > 0)
      weeklySalary += units * dollarsToRaise;
}
/*****************************************************************************/
void
Boss::NewWeek()
{
    return;
}
#endif
