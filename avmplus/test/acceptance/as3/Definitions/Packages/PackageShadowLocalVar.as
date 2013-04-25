/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package p
{
    public var x:String = "package";
}

import p.x
import com.adobe.test.Assert;
function f()
{
    var p = { x : "local" }
    return p.x;
}

// var SECTION = "Definitions\Packages";                   // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";           // Version of JavaScript or ECMA
// var TITLE   = "Package name shadow locally defined name independent of scope chain";       // Provide ECMA section title or a description
var BUGNUMBER = "";


Assert.expectEq("Package name shadow locally defined name", "package", f());

