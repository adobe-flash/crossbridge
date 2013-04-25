/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import ExtractMethPackage.*;
import com.adobe.test.Assert;


// var SECTION = "Method Closures";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Extract methods from a public class";        // Provide ECMA section title or a description
var BUGNUMBER = "";





var obj:PubClassExtractMeth = new PubClassExtractMeth();

// *****************************
// Extract a public method
// *****************************
var mc : Function = obj.pubGetX;
Assert.expectEq( "*** extract a public function from a public class", 1, 1 );
Assert.expectEq( "obj.pubGetX() == mc()", obj.pubGetX(), mc() );


// *****************************
// Extract a static method
// *****************************
var mc2 : Function = PubClassExtractMeth.statGetX;
Assert.expectEq( "*** extract a static function from a public class", 1, 1 );
Assert.expectEq( "PubClassExtractMeth.statGetX() == mc2()", PubClassExtractMeth.statGetX(), mc2() );


            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
