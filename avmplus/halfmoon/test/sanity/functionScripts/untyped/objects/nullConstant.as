// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
var a = [
	function f1() { return print(null.p) },
	function f2() { return print(undefined.p) },
	function f3() { return print(null[0]) },
	function f4() { return print(undefined[0]) },
]
for each (var f in a) { try { f() } catch (e) { print('pass') }}
}
