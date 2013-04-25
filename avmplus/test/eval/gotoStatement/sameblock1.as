/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Test backward goto in the same block.
// This should print "PASSED".

var v = 0;

function test(n) {
L1: if (n == 0) return;
	v++;
	n--;
	goto L1;
}

test(10);
print(v == 10 ? "PASSED" : "FAILED");
