/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package extClass{
    public class subClass extends parentClass {
    }
}

import extClass.*;
import com.adobe.test.Assert;

// var SECTION = " ";
// var VERSION = "AS3";
// var TITLE   = "import a class defined in .abc file";


pc = new parentClass();
sc = new subClass();


Assert.expectEq( "parent class from imported abc file", true, pc.test());
Assert.expectEq( "sub class from imported abc file", true, sc.test());




