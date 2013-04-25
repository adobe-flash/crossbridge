/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// global var
var x = "GLOBAL";
print(x);

// local var, assuming functions are working
function f() {
    var x = "LOCAL";
    print(x);
}

f();
print(x);
