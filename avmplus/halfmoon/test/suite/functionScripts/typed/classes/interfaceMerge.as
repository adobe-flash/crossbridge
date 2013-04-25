// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/*
 * Copyright (c) 2003-2005  Tom Wu
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL TOM WU BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
 * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * In addition, the following condition applies:
 *
 * All redistributions must retain an intact copy of this copyright notice
 * and disclaimer.
 */

 
package crypto {

public class BigInteger {
  
    public function BigInteger():void {
    }
    
    public function modPow(i):BigInteger {
		// This blows up
		var z:ModularReduction;
        if(i < 8)
            z = new Classic();
        else 
            z = new Barrett();
    
        // precomputation
        var j:int = 10;
        while(j >= 0) {
			j++;
        }
        return z.revert(new BigInteger());
	} 
} // class BigInteger

  
public interface ModularReduction {
    function revert(x:BigInteger) : BigInteger;
} // interface ModularReduction

public class Classic implements ModularReduction {
    public function Classic():void {}
    public function revert(x:BigInteger):BigInteger { return x; }
} // class Classic
  
public class Barrett implements ModularReduction {
    public function Barrett() {}
    public function revert(x:BigInteger):BigInteger { return x; }
} // class Barrett
  
var x:BigInteger = new BigInteger();
x.modPow(10);

}// package
