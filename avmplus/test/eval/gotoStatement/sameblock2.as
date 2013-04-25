/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Test forward goto in the same block.
// This should print "PASSED".

var v = 0;

function test(n)
{
	if (n == 0) goto L1;
	v++;
	if (n == 1) goto L1;
	v++;
L1: return;
}

test(2);
print(v == 2 ? "PASSED" : "FAILED");
