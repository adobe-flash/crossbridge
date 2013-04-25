/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This code will produce a RangeError: Error #1002
var result = "no error";
var n:Number = new Number(1);
n.toPrecision(0);

