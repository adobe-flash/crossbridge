/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Test goto in the same CaseBlock, label before case label
// This should print "PASSED".

var v = 0;

function test(n)
{
	switch (n) {
	case 0:
		v += 1;
		goto L2;
	case 1:
		v += 10;
	L2: ;
	case 2:
		v += 100;
	}
}

test(0);
print(v == 101 ? "PASSED" : "FAILED");
