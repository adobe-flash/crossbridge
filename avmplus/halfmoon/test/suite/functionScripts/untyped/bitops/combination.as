// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
var seed = 49734321;
class Math2 {
    public static function random() {
        // Robert Jenkins' 32 bit integer hash function.
        print(0x7ed55d16);
        print(seed + 0x7ed55d16);
        print(seed << 12);
        seed = ((seed + 0x7ed55d16) + (seed << 12))  & 0xffffffff;
        print(seed);
        seed = ((seed ^ 0xc761c23c) ^ (seed >>> 19)) & 0xffffffff;
        print(seed);
        seed = ((seed + 0x165667b1) + (seed << 5))   & 0xffffffff;
        print(seed);
        seed = ((seed + 0xd3a2646c) ^ (seed << 9))   & 0xffffffff;
        print(seed);
        seed = ((seed + 0xfd7046c5) + (seed << 3))   & 0xffffffff;
        print(seed);
        seed = ((seed ^ 0xb55a4f09) ^ (seed >>> 16)) & 0xffffffff;
        print(seed);
        var returnValue = (seed & 0xfffffff) / 0x10000000;
        print(returnValue);
        return returnValue;
    }

}

    Math2.random();

}


