/* -*- Mode: actionscript; -*- */

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {

print(caller(42))

function loop(n) {
	for (var i=0; i < n; i++)
		print(i)
}

function caller(n) {
	loop(n)
	return n
}

}
   
