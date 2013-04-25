// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testSomething(queue, link) {
	print(queue.link);
}

var x = new Object();
x.link = 5;
print(x.link);
testSomething(x, null);
