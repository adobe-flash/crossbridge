/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// FOR-IN stmt

var x = new Object;
for ( i in x )
    print("WRONG " + i);
print("DONE 0");

var y = { foo: 10, bar: 20 }
for ( i in y )
    print("LOOP 1");

for ( var w in y )
    print("LOOP 2");

var z = 10;
var obj = {}
for ( z in obj )
    print("WRONG");
print("SHOULD BE 10: " + z );

print("DONE");
