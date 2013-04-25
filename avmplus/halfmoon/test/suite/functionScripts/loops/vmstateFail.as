// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
// if vmstate = 1, state param is loaded at loop edges
// which fails. Test to ensure state is not loaded across edges
	function runAccessNbody():Number {
		var res:Number = 10;
		for ( var n:int = 3; n <= 24; n *= 2 ) {
		}
		return res;
	}
        var res:Number = runAccessNbody();

}
