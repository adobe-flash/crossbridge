// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package crypto {

public class BigInteger {
    public var s:int;
    
    public function BigInteger():void {
    }
    
    public function abs():BigInteger { 
        return (this.s) ?this:this; 
    }
    
    // r != q, this != m.  q or r may be null.
    // bnpDivRemTo
    public function divRemTo(m:BigInteger):void {
        var pm:BigInteger = m.abs();
		print("Did abs");
    }
} // class BigInteger

function encrypt():void {
	var a:BigInteger = new BigInteger();
	a.divRemTo(a);
}

encrypt();


}// package
