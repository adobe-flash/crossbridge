/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Switch on char value, keys";

// The compiler can compile this as a table lookup, if it's clever.
include "driver.as"

function switchloop():uint {
    return switchloop2("k");
}

function switchloop2(k:String):uint {
    var i:uint=0;
    var v:uint=0;
    while (i < 100000) {
        i++;
        switch (k) {
        case "a": v += 1; break;
        case "b": v += 1; break;
        case "c": v += 2; break;
        case "d": v += 3; break;
        case "e": v += 5; break;
        case "f": v += 8; break;
        case "g": v += 13; break;
        case "h": v += 21; break;
        case "i": v += 34; break;
        case "j": v += 55; break;
        case "k": v += 89; break;
        case "l": v += 144; break;
        }
    }
    return v;
}

TEST(switchloop, "switch-2");
