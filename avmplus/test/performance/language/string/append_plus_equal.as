/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

include 'timetest.as'

function append_plus_equal(loops) {
    var x = '';
    for (var i=0; i<loops; i++) {
        x += 'abcdefghijklmnopqrstuvwxyz0123456789';
    }
}

timetest(append_plus_equal, 300000);