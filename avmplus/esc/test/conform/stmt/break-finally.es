/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// break-past-finally tests

/* These don't work; problem with the verifier 
while (true) {
    try {
        break;
    }
    catch (exn1) {
    }
    finally {
        print("BROKE OUT OF 'TRY'");
    }
    print("WRONG 1");
}

while (true) {
    try {
        throw 0;
    }
    catch (exn2) {
        break;
    }
    finally {
        print("BROKE OUT OF 'CATCH'");
    }
    print("WRONG 2");
}
*/

print("DONE");
