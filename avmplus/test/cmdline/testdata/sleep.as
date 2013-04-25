
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// sleeps for 60s
import avmplus.System

var time:uint=0;
while (getTimer()<60000) {
    var ctime=int(getTimer()/1000);
    if (ctime>time) {
        time=ctime;
        System.write(time+" ");
    }
}
print();