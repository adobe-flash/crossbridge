/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package A
{
    public function info()
    {
        return "A.info";
    }
}

package B
{
    public function info()
    {
        return "B.info";
    }
}

// var SECTION = "Definitions\Packages";                   // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";           // Version of JavaScript or ECMA
// var TITLE   = "access ambiguous function by fully qualified namen";       // Provide ECMA section title or a description
var BUGNUMBER = "";

import A.*;
import B.*;
import com.adobe.test.Assert;


Assert.expectEq("Access ambiguious function by fully qualified name", "A.info", A.info());

