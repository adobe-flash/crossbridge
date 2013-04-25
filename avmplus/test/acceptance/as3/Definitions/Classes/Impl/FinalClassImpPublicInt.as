/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import FinalClassImpPublicInt.*;
import com.adobe.test.Assert;
// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS3";        // Version of ECMAScript or ActionScript
// var TITLE   = "Final class implements public interface";       // Provide ECMA section title or a description
var BUGNUMBER = "";



///////////////////////////////////////////////////////////////
// add your tests here
  
var obj = new ClassGet();
//Final class implements a public interface with a public method
Assert.expectEq("Final class implements a public interface with a public method", "PASSED", obj.accdeffunc());

////////////////////////////////////////////////////////////////

              // displays results.
