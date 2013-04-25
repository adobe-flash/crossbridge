/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
class C {
	print("hi");
	goto L1;
	static var x = 20;
L1: print("ho: " + x);
};
(new C);
