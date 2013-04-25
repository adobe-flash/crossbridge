/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Single literal regex in a loop";

function matchloop() {
    var s = "supercallifragilistic";
    var x;
    for ( var i=0 ; i < 10000 ; i++ ) {
        var re = /call?i/;    /* not "g", we want searching to start at the beginning */
        x = re.exec(s);
    }
    return x;
}

TEST(matchloop, "regex-exec-1");
