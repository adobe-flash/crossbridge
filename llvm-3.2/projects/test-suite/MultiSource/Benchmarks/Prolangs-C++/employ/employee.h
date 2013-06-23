/* employee.h                                                       -*- C++ -*-
**    Include file for abstract employee base class
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
**         Prentice Hall, New Jersey, pp. 531-2
**
** RCS       :
**
** $Source$
** $Revision: 53839 $
** $Date: 2008-07-20 23:02:06 -0700 (Sun, 20 Jul 2008) $
**
** $Log$
** Revision 1.2  2004/10/05 00:37:32  lattner
** Stop using deprecated headers
**
** Revision 1.1  2004/10/04 20:01:13  lattner
** Initial checkin of all of the source
**
** Revision 0.1  1994/12/24  01:26:08  bkuhn
**   # initial version
**
**
*/

#ifndef _EMPLOYEE_H
#define _EMPLOYEE_H

#include <cstring>
#include <iostream>
#include <iomanip>
using namespace std;

#define EMPLOYEE_ID 0

class Employee {

  private:
    char * firstName;
    char * lastName;

  protected:
    float  dollarsToRaise;      // the dollar value added to salary when raised

  public:
    Employee(const char *, const char *);
    virtual ~Employee();

    const char * FirstName();
    const char * LastName();
          void   PrintWithEarnings(int);

    // pure virtual functions

    virtual float Earnings() = 0;
    virtual void  Print()    = 0;
    virtual void  Raise(int) = 0;
    virtual void  NewWeek()  = 0;
};
/*****************************************************************************/
Employee::Employee(const char * first, const char * last)
{
    firstName = new char [ strlen(first) + 1 ];
    strcpy(firstName, first);

    lastName = new char [ strlen(last) + 1 ];
    strcpy(lastName, last);
}
/*****************************************************************************/
Employee::~Employee()
{
    delete [] firstName;
    delete [] lastName;
}
/*****************************************************************************/
const char *
Employee::FirstName()
{

    return firstName; // caller must make a copy
}
/*****************************************************************************/
const char *
Employee::LastName()
{
    return lastName; // caller must make a copy
}
/*****************************************************************************/
void
Employee::PrintWithEarnings(int weekNumber)
{
    this->Print();

    cout << " earned $" << this->Earnings() << " in week "
         << weekNumber << '\n';
}

#endif
