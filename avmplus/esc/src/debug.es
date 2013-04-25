/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil;  -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

Debug function trace (s) {
    print (s);
}

internal var nesting = 0;

function arrows (c) : string {
    let str = "";
    for ( var n = nesting; n > 0; n = n - 1 )
        str = str + c;
    return nesting + " " + str+" ";
}

Debug function enter (s,a="") {
    nesting = nesting + 1;
    print (arrows(">"), s, a);
}

Debug function exit (s,a="") {
    print (arrows("<"), s, a);
    nesting = nesting - 1;
}

Release function enter (s,a="") { nesting = nesting + 1 }
Release function exit (s,a="") { nesting = nesting - 1 }
Release function trace (s) { }
