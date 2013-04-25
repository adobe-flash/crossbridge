// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
function escCtrlChars(str:String):String {  // escape control chars which might cause problems handling ciphertext
  var s:String = "";
  for ( var i:int=0 ; i < str.length ; i++ ) {
    var c:String = str.charAt(i);
    if (c == "\0")  
        s += "!"; 
  }
  return s;
}



function test():String {
	var ct:String = '';
	return escCtrlChars(ct);
}

function makeHot() {
	for (var i = 0; i < 1000; i++) {
		test();
	}
}

	makeHot();
}
