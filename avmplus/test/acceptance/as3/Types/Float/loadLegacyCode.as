/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.Domain;
import com.adobe.test.Assert;


// var SECTION = "Types: float";
// var VERSION = "as3";
// var TITLE   = " - running legacy code on float values";

var ABC_FILENAME = "as3/Types/Float/legacyFunctions._abc_";

function initTestCases():void{
   var d:Domain = Domain.currentDomain;
   d.load(ABC_FILENAME);
   var myc:Class = d.getClass("p.MyCls");
   myc.Assert.expectEq = Assert.expectEq;

   var f = myc.foo(1.1f);
   Assert.expectEq("ret value", 1.1f, f);
   Assert.expectEq("ret type", "float", typeof(f));

   f = myc.foo2(1.1f);
   var n:Number = 1.1f;
   ++n;
   Assert.expectEq("ret value after increment", n, f);
   Assert.expectEq("ret type after increment", "number", typeof(f))
}



    initTestCases();
;
