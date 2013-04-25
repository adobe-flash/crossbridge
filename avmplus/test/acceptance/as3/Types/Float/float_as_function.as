/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.3";
// var VERSION = "AS3";
// var TITLE   = "The float class object called as a function";


/*
When the float class object is called as a function, it performs a type conversion and returns a float
value.
*/


var undefined_float = float(undefined);
Assert.expectEq("Float as function, with 'undefined' arg", "float", getQualifiedClassName(undefined_float));

var null_float = float(null);
Assert.expectEq("Float as function, with 'null' arg", "float", getQualifiedClassName(null_float));

var boolean_float = float(true);
Assert.expectEq("Float as function, with 'boolean' arg", "float", getQualifiedClassName(boolean_float));

var dble_float = float(3.14);
Assert.expectEq("Float as function, with 'double' arg", "float", getQualifiedClassName(dble_float));

var int_float = float(3);
Assert.expectEq("Float as function, with 'int' arg", "float", getQualifiedClassName(int_float));

var string_float = float("3.14");
Assert.expectEq("Float as function, with 'String' arg", "float", getQualifiedClassName(string_float));

var myObject:Object = {1:1};
var object_float = float(myObject);
Assert.expectEq("Float as function, with 'Object' arg", "float", getQualifiedClassName(object_float));

var literal_float = float(3.14f);
Assert.expectEq("Float as function, with 'FloatLiteral' arg", "float", getQualifiedClassName(literal_float));


// Note: not testing new float() - it's tested in section 4.4
/* Also - more complete testing of ToFloat() in section 9.2 */


