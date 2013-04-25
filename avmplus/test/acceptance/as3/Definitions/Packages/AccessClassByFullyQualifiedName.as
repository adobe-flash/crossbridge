/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package A
{
    public class C
    {
        public function info()
        {
            return "A.C";
        }
    }
}

package B
{
    public class C
    {
        public function info()
        {
            return "B.C";
        }
    }
}

// var SECTION = "Definitions\Packages";                   // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";           // Version of JavaScript or ECMA
// var TITLE   = "access ambiguous class by fully qualified name";       // Provide ECMA section title or a description
var BUGNUMBER = "";

import A.*;
import B.*;
import com.adobe.test.Assert;


var obj = new B.C();

Assert.expectEq("Access ambiguious class by fully qualified name", "B.C", obj.info());

