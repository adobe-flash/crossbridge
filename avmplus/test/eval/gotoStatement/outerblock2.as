/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Test forward goto out of multiple blocks, past some stuff that should not be executed
// This should print "PASSED".

var v = 0;

function test(n)
{
	for (;;) {
		for (;;) {
			if (n == 0) goto L1;
			v++;
			n--;
		}
	}
	print("DEAD CODE, SHOULD NOT RUN");
L1: return;
}

test(10);
print(v == 10 ? "PASSED" : "FAILED");
