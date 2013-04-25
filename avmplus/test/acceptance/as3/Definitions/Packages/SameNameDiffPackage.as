/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package A
{
    public class C
    {
        public function C()
        {
        }
    }
}

package B
{
    import A.*;
    public class C extends A.C
    {
        var _x = "FAILED";
        public function get x(){ return _x; }
        public function C()
        {
            _x = "PASSED";
        }
    }
}

import B.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";


var TESTER:C = new C();

Assert.expectEq( "Same class name, different package name", "PASSED", TESTER.x );

              // displays results.
