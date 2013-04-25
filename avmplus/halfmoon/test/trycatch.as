// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


function trycatch(c) {
	try {
	    if (c)
		  throw c
		print('try')
	} catch (i:int) {
		print('catch ' + i)
	} catch (x) {
		print('catch ' + x)
	}
}
trycatch()
trycatch(1)
