// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function stringArray(...args) {
    print("Args is: " +args[0]);
    var dlen:Number = args[0];
    var ulen:uint = dlen;
    print(dlen);
    print(ulen);
    print(ulen != dlen);
/*
//    var stringArray:Array = new Array();
//    stringArray['hello'] = 10;

//    print(stringArray['hello']);
*/
}

stringArray();
