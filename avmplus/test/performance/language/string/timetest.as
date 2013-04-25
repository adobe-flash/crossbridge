/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

function timetest(func:Function, iterationsDesktop:Number=1, iterationsMobile:Number=0) {
    if (iterationsMobile == 0)
        iterationsMobile = iterationsDesktop;

    var start:Number;
    var totaltime:Number;
    
    if (CONFIG::desktop) {
        start = new Date();
        func(iterationsDesktop);
        totaltime = new Date() - start;
    } else {
        start = getTimer();
        func(iterationsMobile);
        totaltime = getTimer() - start;
    }
    print('metric time '+totaltime);
}