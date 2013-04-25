/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String allocation, accumulating medium/long-length strings (~2500 chars)";
include "driver.as"

function allocloop():uint {
    var s:String;
    for ( var i:uint=0 ; i < 1000 ; i++ ) {
        s = "abracadabra";
        for ( var j:uint=0 ; j < 100 ; j++ ) {
            s += "supercallifragilisticexpialidocious";
        }
    }
    return i;
}

TEST(allocloop, "alloc-13");
