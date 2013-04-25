// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
function test_switch(x) {
	switch (x) {
	case 1:	return print(1)
	case 2: return print(2)
	case 3: return print(3)
	default: return print('default')
	}
}
test_switch(2)
}
