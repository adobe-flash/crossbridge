// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package crypto {

public class BigInteger {
    public function BigInteger():void {
    }
    
    public static function nbv(i:int):BigInteger { 
		return new BigInteger();
    }
    
    public function modPow(e:BigInteger,m:BigInteger):BigInteger {
		print("Starting mod pow");
		var r:BigInteger = nbv(1); 
		return this;
    }
} // class BigInteger

function decryptBroken():void {
    var xp:BigInteger = new BigInteger();

    print("Decrypt modding");
    xp = xp.modPow(xp, xp);
    print("Did mod pow");
}

decryptBroken();

}// package
