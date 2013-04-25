/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

 /*
  * Ensure the type annotation works while imported
 */

// var SECTION = " ";
// var VERSION = "AS3";
// var TITLE   = "import annoted type defined in .abc file";



Assert.expectEq( "import the annoted type - number:Number", 50, number);
Assert.expectEq( "import the annoted type - str1:String", "str1:String", str1);

