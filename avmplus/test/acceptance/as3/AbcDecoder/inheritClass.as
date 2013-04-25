/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

 /*
  * This file implements the class defined '.abc' file
 */

// var SECTION = " ";
// var VERSION = "AS3";
// var TITLE   = "import a class defined in .abc file";


class impClass implements inInterface {
    public function intFunc1() {
        return "Inside func1()";
    }
    public function intFunc2() {
        return "Inside func2()";
    }
    function intFunc3() {
        return "Inside func3()";
    }
}

obj = new impClass();

Assert.expectEq( "invoke the method 'func1()' from the imported file", "Inside func1()", obj.intFunc1());
Assert.expectEq( "invoke the method 'func2()' from the imported file", "Inside func2()", obj.intFunc2());
Assert.expectEq( "invoke the method 'func3()'", "Inside func3()", obj.intFunc3());

