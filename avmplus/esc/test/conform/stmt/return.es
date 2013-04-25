/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

function f() {
    print("RIGHT");
    return;
    print("WRONG");
}
f();

function g() {
    return "RIGHT";
}
print(g());

print("DONE");

