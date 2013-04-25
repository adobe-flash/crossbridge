/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

try {
    print("THROW");
    throw "Foo";
    print("WRONG");
}
catch (e) {
    print("CATCH");
}
print("DONE 1");

function f() {
    print("THROW");
    throw "Foo";
    print("WRONG");
}
try {
    f();
}
catch (e) {
    print("CATCH");
}
finally {
    print("FINALLY");
}
print("DONE 2");

try {
    try {
        f();
    }
    finally {
        print("FINALLY");
    }
}
catch (e) {
    print("CATCH");
}
print("DONE 3");
