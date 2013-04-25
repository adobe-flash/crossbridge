/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package test{
    public var a = 'PASSED';
    public function f() {
        var g = "g";
        return g;
    }
}

package test2{
    public function g() {
        var h = "h";
        return h;
    }
    public var b = "hello";
}

class C {
    import test2.*;
    public function returnB() {
        return b;
    }
}

import test.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var d = a;

Assert.expectEq( "import outside of package, variable", "PASSED",d );

Assert.expectEq( "import outside of package, function", "g", f());

var c = new C();

Assert.expectEq( "import outside of package, inside class", "hello", c.returnB());

              // displays results.
