/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Multiple literal regexes in a loop, do fit in regex cache";

function matchloop() {
    var s = "supercallifragilistic";
    var x;
    for ( var i=0 ; i < 1000 ; i++ ) {
        var re1 = /call?i/;    /* not "g", we want searching to start at the beginning */
        var re2 = /dall?i/;    /* not "g", we want searching to start at the beginning */
        var re3 = /eall?i/;    /* not "g", we want searching to start at the beginning */
        var re4 = /fall?i/;    /* not "g", we want searching to start at the beginning */
	x = re2.exec(s);
	x = re3.exec(s);
	x = re4.exec(s);
        x = re1.exec(s);
    }
    return x;
}

TEST(matchloop, "regex-exec-4");
