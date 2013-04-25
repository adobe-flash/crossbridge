/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Tricky, but still an error: a block that introduces some label L1
// does not shadow a label L1 in the surrounding function scope,
// because the outer L1 statement is still a candidate target for the
// inner goto; candidate search stops only at the function boundary.

function f()
{
	var c = 0;
L1: if (c == 10) goto L2;
	pr(c);
	c++;
	goto L1;
L2: prln();

	{
		var d = 0;
	L1:	if (d == 10) goto L4;
		pr(d);
		d++;
		goto L1;
	L4:	prln();
	}
}
