/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.Domain
import flash.utils.ByteArray
import com.adobe.test.Assert;

var section = "RuntimeErrors"
// var VERSION = "as3"
// var TITLE = "test for exception Final methods cannot be overwritten"

/* asc will detect the error as a compiler error, so the testcase was generated
 *  with esc, here is the source:
 *
 class C {
    final function f() { print("in C") }
 }
 class D extends C {
    override function f() { print("in D") }
 }
 (new D).f()

*/
var abcdata:Array=[16,0,46,0,1,1,1,19,0,6,79,98,106,101,99,116,5,65,114,114,97,121,6,82,101,103,69,120,112,16,60,35,105,110,116,101,114,110,97,108,32,116,46,101,115,62,8,105,110,116,101,114,110,97,108,9,78,97,109,101,115,112,97,99,101,6,112,117,98,108,105,99,1,67,7,112,114,105,118,97,116,101,9,112,114,111,116,101,99,116,101,100,1,102,5,112,114,105,110,116,4,105,110,32,67,10,36,99,111,110,115,116,114,117,99,116,6,36,99,105,110,105,116,1,68,4,105,110,32,68,8,22,1,23,5,8,1,5,9,24,9,5,17,24,17,5,1,3,4,4,5,3,2,2,3,2,4,6,7,3,2,20,7,1,2,7,1,3,7,1,4,7,2,6,7,3,7,7,2,8,7,3,9,9,2,1,7,4,10,7,4,11,7,3,12,9,13,2,7,3,17,9,9,3,7,6,10,7,6,11,9,13,4,9,17,3,9,12,3,7,0,0,12,0,0,0,15,2,0,0,16,0,0,0,12,0,0,0,15,2,0,0,16,0,0,0,0,0,0,2,7,8,9,5,0,1,1,11,17,0,0,13,14,9,7,0,4,1,11,33,0,3,2,2,9,6,0,5,4,8,10,6,0,5,5,8,5,2,15,6,0,5,6,8,16,6,0,5,7,8,1,6,4,4,6,0,5,2,8,6,6,0,5,3,8,7,4,0,0,13,4,0,1,7,0,2,1,0,1,11,208,48,93,12,44,14,76,12,1,41,71,0,0,1,5,2,0,2,19,208,42,48,29,87,42,213,42,28,208,73,0,29,208,48,48,8,1,71,0,0,2,1,1,0,1,3,208,48,71,0,0,3,2,1,0,1,11,208,48,93,17,44,18,76,17,1,41,71,0,0,4,5,2,0,2,19,208,42,48,29,87,42,213,42,28,208,73,0,29,208,48,48,8,1,71,0,0,5,1,1,0,1,3,208,48,71,0,0,6,2,2,0,2,48,208,48,93,8,102,8,42,48,88,0,29,42,48,29,100,43,104,7,93,14,102,14,42,48,88,1,29,42,48,29,100,43,104,13,93,18,102,18,66,0,70,19,0,130,213,209,72,71,0,0];
var ba:ByteArray=new ByteArray;
for (var i=0;i<abcdata.length;i++) {
    ba.writeByte(abcdata[i]);
}
var domain:Domain=Domain.currentDomain;
var err:String="no error";
try {
   domain.loadBytes(ba);
} catch (e) {
   err=e.toString();
}
if (err.length>21 && err.substring(0,24)=="VerifyError: Error #1053") {
    err=err.substring(0,24);
}

Assert.expectEq(
    "Final methods should throw a Verifier error if they are overriden",
    "VerifyError: Error #1053",
    err
);

