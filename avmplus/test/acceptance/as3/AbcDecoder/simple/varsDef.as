/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

 /*
 This file is imported with other .as files
 */

package packOne {
    public var var1="packOne:var1";
    public var var2="packOne:var2";
    public var var3="packOne:var3";
    public var var4="packOne:var4";
}

package packTwo {
    public var p2var1="packTwo:var1";
    public var p2var2="packTwo:var2";
    public var p2var3="packTwo:var3";
    public var p2var4="packTwo:var4";
}

package {
    public var str="imported string";
    public var num=10;
}

class testClass {
    var testClassVar1 = "Inside testClass";
    function func1() {
        return "Inside func1()";
    }
    function func2() {
        return "Inside func2()";

    }
}

final class finClass {
    var finClassVar1 = "Inside finClass";
    function finFunc1() {
        return "Inside func1()";
    }
    function finFunc2() {
        return "Inside func2()";

    }
}

interface inInterface {
    function intFunc1();
    function intFunc2();
}


class accSpecClass {
    var accSpec1 = "string";
    public function func1() {
        return "Inside func1()-public function";
    }
    function func2() {
        return "Inside func2()-Dynamic function";
    }

}

dynamic class dynClass {
    var dynSpec1 = "string";
    public function func1() {
        return "Inside func1()-public function";
    }
    function func2() {
        return "Inside func2()-Dynamic function";
    }
}

class baseClass
{
  public function overrideFunc() {
  }
}

class childClass extends baseClass
{
}





var number:Number = 50;
var str1:String = "str1:String";


namespace Baseball;
namespace Basketball;

namespace foo = "http://www.macromedia.com/"



