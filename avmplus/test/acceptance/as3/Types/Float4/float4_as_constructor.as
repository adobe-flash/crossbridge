/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.3.1";
// var VERSION = "AS3";
// var TITLE   = "The float4 method called as a constructor new float4 (x)";



var result = new float4();
Assert.expectEq("Float4 with no args", "float4", getQualifiedClassName(result));

var undefined_float4 = new float4(undefined);
Assert.expectEq("Float4 as constructor, with 'undefined' arg", "float4", getQualifiedClassName(undefined_float4));

var null_float4 = new float4(null);
Assert.expectEq("Float4 as constructor, with 'null' arg", "float4", getQualifiedClassName(null_float4));

var boolean_float4 = new float4(true);
Assert.expectEq("Float4 as constructor, with 'boolean' arg", "float4", getQualifiedClassName(boolean_float4));

var dble_float4 = new float4(3.14);
Assert.expectEq("Float4 as constructor, with 'double' arg", "float4", getQualifiedClassName(dble_float4));

var int_float4 = new float4(3);
Assert.expectEq("Float4 as constructor, with 'int' arg", "float4", getQualifiedClassName(int_float4));

var string_float4 = new float4("3.14");
Assert.expectEq("Float4 as constructor, with 'String' arg", "float4", getQualifiedClassName(string_float4));

var literal_float4 = new float4(3.14f);
Assert.expectEq("Float4 as constructor, with 'FloatLiteral' arg", "float4", getQualifiedClassName(literal_float4));

var flt4 = new float4(1f, 1f, 1f, 1f);
var flt4_flt4 = new float4(flt4);
Assert.expectEq("Float4 as a constructor with float4 arg", "float4", getQualifiedClassName(flt4_flt4));
AddStrictTestCase("Return x if x is a float4, float4(x)", true, flt4 === flt4_flt4);


