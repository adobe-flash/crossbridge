/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var x = 10;
var o = { x: 20 }

var n = 2;
while (n-- > 0) {
  with (o) {
    print("HERE " + n);
    continue;
  }
  print("WRONG");
}

print("CONTINUE-WITH " + x);
print("DONE");
