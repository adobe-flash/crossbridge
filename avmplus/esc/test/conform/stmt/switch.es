/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

switch (10) {
case 5:
    print("WRONG");
case 10:
    print("RIGHT 10");
    // fall through
case 20:
    print("RIGHT 20");
    break;
}

switch (10) {
case 5:
    print("WRONG");
}

switch (10) {
case 5:
    print("WRONG");
default:
    print("RIGHT default");
    break;
case 8:
    print("WRONG");
}

print("DONE");
