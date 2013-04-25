/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// if
// if-else

if (3 > 4)
    print("WRONG");

if (4 > 3)
    print("RIGHT");

if (1 > 2)
    print("WRONG");
else
    print("RIGHT");

if (2 > 1)
    print("RIGHT");
else
    print("WRONG");

// matching the correct else with the if

if (1 > 2)
    if (4 > 3)
        print("WRONG");
    else
        print("WRONG");

print("DONE");
