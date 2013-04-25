/* -*- mode: fundamental; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Various kinds of semicolon insertion

var x = 10
print("VAR")

x = x*2
print("EXPR 1 " + x)

x = 2
* 3
print("EXPR 2 " + x)

while (true)
    break
print("BREAK")

while (false)
    continue
print("CONTINUE")

function f() {
   return
   "not this value"
}
print("RETURN " + f()) // should be undefined

print("DONE")
