/* -*- Mode: actionscript; -*- */

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {

public class vec {
	var x:int;
	var y:int;
	function vec(x:uint,y:uint) {
		this.x=x;
		this.y=y;
	}
	public function sum():int {
		return this.x+this.y;
	}
}

function caller(p1: int): int {
	print(new vec(p1, 1).sum())
}
caller(2)

}
