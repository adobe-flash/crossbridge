/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{
    public var num1 = 3;
}

package A
{
    public var num2 = num1;
}

// var SECTION = "Definitions\Packages";                   // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";           // Version of JavaScript or ECMA
// var TITLE   = "access default package variable in other package";       // Provide ECMA section title or a description
var BUGNUMBER = "";

import A.*;
import com.adobe.test.Assert;


Assert.expectEq("Using default package variable in other package", 3, A.num2);

