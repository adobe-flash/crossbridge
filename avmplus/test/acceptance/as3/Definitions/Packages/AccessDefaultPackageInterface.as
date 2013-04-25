/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{
    public interface B{};
}

package A
{
    public class D implements B
    {
    }
}

// var SECTION = "Definitions\Packages";                   // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";           // Version of JavaScript or ECMA
// var TITLE   = "access default package interface in other package";       // Provide ECMA section title or a description
var BUGNUMBER = "";

import A.*;
import com.adobe.test.Assert;


var obj = new D();
Assert.expectEq("Using default package interface in other package", true, obj is B);

