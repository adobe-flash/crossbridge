/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var sec=0
while (true) {
    if (getTimer()>5000) {
        break
    }
    if (int(getTimer()/1000)==sec) {
        print(sec);
        sec++;
    }
}
