/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Multiple literal regexes in a loop, do fit in regex cache";
include "driver.as"

function matchloop() {
    var s:String = "supercallifragilistic";
    var x:Array;
    for ( var i:int=0 ; i < 1000 ; i++ ) {
        var re1:RegExp = /call?i/;    /* not "g", we want searching to start at the beginning */
        var re2:RegExp = /dall?i/;    /* not "g", we want searching to start at the beginning */
        var re3:RegExp = /eall?i/;    /* not "g", we want searching to start at the beginning */
        var re4:RegExp = /fall?i/;    /* not "g", we want searching to start at the beginning */
        x = re2.exec(s);
        x = re3.exec(s);
        x = re4.exec(s);
        x = re1.exec(s);
    }
    return x;
}

TEST(matchloop, "regex-exec-4");
