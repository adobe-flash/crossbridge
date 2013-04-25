/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.1.2";
// var VERSION = "AS3";
// var TITLE   = "Other primitive expressions";


var o: Object = {1:"foo"};
Assert.expectEq("Member access using float literal", "foo", o[1.0f]);

var o2: Object = {1f:"bar"};
Assert.expectEq("Member access using float literal to a float property index", "bar", o2[1.0f]);
Assert.expectEq("Member access using Number literal to a float property index", "bar", o2[1]);

o[float.MIN_VALUE] = "floatmin";
Assert.expectEq("Member access using float to a float property index", "floatmin", o[float.MIN_VALUE]);

var index:float = 27182818.284e-2;
o[Number(index)] = "bar";
Assert.expectEq("internFloat and internDouble should produce the same result on the same float value", "bar", o[index]);

o.NaN = 'Non est numerus';
Assert.expectEq("o.NaN same thing as o[float.NaN]", 'Non est numerus', o[float.NaN]);


