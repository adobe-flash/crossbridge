/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;
import avmshell.*;
import com.adobe.test.Assert;

// var SECTION = "regress_660985";
// var VERSION = "AS3";
// var TITLE   = "describeType API";
// var bug = "660985";



XML.ignoreComments = true;
XML.ignoreProcessingInstructions = true;
XML.ignoreWhitespace = true;
XML.prettyPrinting = false;
XML.prettyIndent = false;

var expected = '<type name="avmshell::public_class_FP_10_0" base="Object" isDynamic="false" isFinal="false" isStatic="false"><extendsClass type="Object"/></type>'
var actual = describeType(new public_class_FP_10_0(), FLASH10_FLAGS).toXMLString();
Assert.expectEq("Omit API metadata", expected, actual);


