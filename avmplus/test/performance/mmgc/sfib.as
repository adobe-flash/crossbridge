/* -*- mode: java; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var param = 30;

function sfib(n) {
    if (n < 2)
        return "x";
    else
        return sfib(n-1) + sfib(n-2);
}
var then = new Date();
sfib(param);
var now = new Date();
print("metric time " + (now - then));
