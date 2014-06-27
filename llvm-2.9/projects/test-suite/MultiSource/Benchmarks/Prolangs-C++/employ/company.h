/* company.h                                                        -*- C++ -*-
**    Include file for a company
** 
** COPYRIGHT (C) 1994 Bradley M. Kuhn
**
** Written   :   Bradley M. Kuhn                                 Loyola College
**   By
**
** Written   :   David W. Binkley                                Loyola College
**   For         
**
** RCS       :
**
** $Source$
** $Revision: 16679 $
** $Date: 2004-10-04 17:38:46 -0700 (Mon, 04 Oct 2004) $
**
** $Log$
** Revision 1.3  2004/10/05 00:38:46  lattner
** Methods cannot be named the same thing classes are
**
** Revision 1.2  2004/10/05 00:37:32  lattner
** Stop using deprecated headers
**
** Revision 1.1  2004/10/04 20:01:13  lattner
** Initial checkin of all of the source
**
** Revision 0.1  1994/12/24  00:39:43  bkuhn
**   # initial version
**
**
*/

#ifndef _COMPANY_H
#define _COMPANY_H

#include "employee.h"

#include <iostream>
using namespace std;


class EmployeeNode {

  private:
    Employee *     employee;
    EmployeeNode * next;
    
  public:
    EmployeeNode(Employee *, EmployeeNode *);

    Employee * getEmployee() { return employee; }
    EmployeeNode *  Next();
};
/*****************************************************************************/
EmployeeNode::EmployeeNode(Employee * e, EmployeeNode * n)
{
    employee = e;
    next = n;
}
/*****************************************************************************/
EmployeeNode *
EmployeeNode::Next()
{
    return next;
}
/*****************************************************************************/

class Company {
  private:
    EmployeeNode * employeeList;
    int            employeeCount;
    int            currentWeek;

  public:
    Company();

    void          AddEmployee(Employee *);
     int          EmployeeCount();

    void          PrintWithEarnings();
    void          NewWeek();
    void          AcrossTheBoardRaise(int);
};
/*****************************************************************************/
Company::Company()
{
    employeeList  = NULL;
    employeeCount = 0;
    currentWeek   = 0;
}
/*****************************************************************************/
int
Company::EmployeeCount()
{
    return employeeCount;
}
/*****************************************************************************/
void
Company::AddEmployee(Employee * e)
{
    EmployeeNode * newNode;

    newNode = new EmployeeNode(e, employeeList);
    employeeList = newNode;
}
/*****************************************************************************/
void
Company::PrintWithEarnings()
{
    EmployeeNode * curE;

    for(curE = employeeList; curE != NULL; curE = curE->Next())
      curE->getEmployee()->PrintWithEarnings(currentWeek);
}
/*****************************************************************************/
void
Company::NewWeek()
{
    EmployeeNode * curE;

    currentWeek++;

    for(curE = employeeList; curE != NULL; curE = curE->Next())
      curE->getEmployee()->NewWeek();
}
/*****************************************************************************/
void
Company::AcrossTheBoardRaise(int units)
{
    EmployeeNode * curE;

    for(curE = employeeList; curE != NULL; curE = curE->Next())
      curE->getEmployee()->Raise(units);
}

#endif
