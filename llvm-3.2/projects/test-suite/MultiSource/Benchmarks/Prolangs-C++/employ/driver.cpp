/* driver.cc                                                        -*- C++ -*-
**    Driver for the Employee Example
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
** $Revision: 16680 $
** $Date: 2004-10-04 17:41:19 -0700 (Mon, 04 Oct 2004) $
**
** $Log$
** Revision 1.2  2004/10/05 00:41:19  lattner
** Don't print argv[0]
**
** Revision 1.1  2004/10/04 20:01:13  lattner
** Initial checkin of all of the source
**
** Revision 0.1  1994/12/24  01:48:26  bkuhn
**   # initial version
**
**
*/

#include "company.h"
#include "boss.h"
#include "commission.h"
#include "piece.h"
#include "hourly-no-over.h"
#include "hourly-over.h"

#include <stdio.h>

int
main(int argc, char *argv[])
{
    int        ii, totalWeeks;
    int        classCode;
    char       first[80], last[80];
    float      firstArg, secondArg;
    Employee * e;
    Company *  c;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <number_of_weeks>\n", "employ");
        return 1;
    }

    cout << setiosflags(ios::showpoint | ios::fixed) << setprecision(2);

    totalWeeks = atoi(argv[1]);

    c = new Company();

    while (scanf("%d%s%s%f", &classCode, first, last, &firstArg) == 4) {
        switch(classCode) {
          case BOSS_ID:
            e = new Boss(first, last, firstArg);
            break;

          case HOURLY_WORKER_NO_OVERTIME_ID:
            e = new HourlyWorkerNoOvertime(first, last, firstArg);
            break;

          case HOURLY_WORKER_OVERTIME_ID:
            e = new HourlyWorkerOvertime(first, last, firstArg);
            break;

          case PIECE_WORKER_ID:
            e = new PieceWorker(first, last, firstArg);
            break;

          case COMMISSIONED_WORKER_ID:
            scanf("%f", &secondArg);
            e = new CommissionedWorker(first, last, firstArg, secondArg);
            break;

          default:
            fprintf(stderr, "INVALID EMPLOYEE CODE(%d)\n", classCode);
            return -1;
        }

        c->AddEmployee(e);
    }

    for(ii = 0; ii < totalWeeks; ii++) {
        c->NewWeek();
        c->PrintWithEarnings();
        if ( (ii % 10) == 0) c->AcrossTheBoardRaise(1);
    }
    return 0;
}
