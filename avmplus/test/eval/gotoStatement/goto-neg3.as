/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Missing label - not in scope, in top-level group

function f()
{
    { L1: print("x"); }
	goto L1;
}
f();
