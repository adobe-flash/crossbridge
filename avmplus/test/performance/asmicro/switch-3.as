/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Switch on string value, var-length string keys";
include "driver.as"

function switchloop():uint {
    return switchloop2("10");
}

function switchloop2(k:String):uint {
    var i:uint=0;
    var v:uint=0;
    while (i < 100000) {
        i++;
        switch (k) {
        case "0": v += 1; break;
        case "1": v += 1; break;
        case "2": v += 2; break;
        case "3": v += 3; break;
        case "4": v += 5; break;
        case "5": v += 8; break;
        case "6": v += 13; break;
        case "7": v += 21; break;
        case "8": v += 34; break;
        case "9": v += 55; break;
        case "10": v += 89; break;
        case "11": v += 144; break;
        }
    }
    return v;
}

TEST(switchloop, "switch-3");
