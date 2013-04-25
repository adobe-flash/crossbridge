// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package TestAdd {

 function testWhile(x:int) {
     var somethingElse:int = 10;
     while (somethingElse < 100)
         somethingElse++;
 }

    function doSum(x:int, y:int):int {
        var sum:int;
        for (x = 0; x < y; x++) {
            sum += x + y;
        }

        testWhile(x);
        return sum;
    }

    function addNumbers() {
        doSum(10, 18);
    }

    addNumbers();
}
