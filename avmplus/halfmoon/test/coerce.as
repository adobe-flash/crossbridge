// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

class A {}
function docast1(x):A {
	var a:A = x
	return a
}
docast1(new A())

function docast2(x:A):A {
	var a:A = x
	return a
}
docast2(new A())
