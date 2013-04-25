/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// DO loops

do
    print("LOOP 1");
while (false);

var x = 2;
do {
    print("LOOP 2");
    x = x - 1;
} while (x >= 0);

print("DONE");
