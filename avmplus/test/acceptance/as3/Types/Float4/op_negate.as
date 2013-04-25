/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.1.3";
// var VERSION = "AS3";
// var TITLE   = "The negate operatror -";


var flt4:float4 = new float4(1f);
flt4 = -flt4;
AddStrictTestCase("unary minus on float4(1f).x", -1f, flt4.x);
AddStrictTestCase("unary minus on float4(1f).y", -1f, flt4.y);
AddStrictTestCase("unary minus on float4(1f).z", -1f, flt4.z);
AddStrictTestCase("unary minus on float4(1f).w", -1f, flt4.w);

flt4 = new float4(-1f);
flt4 = -flt4;
AddStrictTestCase("unary minus on float4(-1f).x", 1f, flt4.x);
AddStrictTestCase("unary minus on float4(-1f).y", 1f, flt4.y);
AddStrictTestCase("unary minus on float4(-1f).z", 1f, flt4.z);
AddStrictTestCase("unary minus on float4(-1f).w", 1f, flt4.w);


flt4 = new float4(0f);
flt4 = -flt4;
AddStrictTestCase("unary minus on float4(0f).x sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/flt4.x);
AddStrictTestCase("unary minus on float4(0f).y sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/flt4.y);
AddStrictTestCase("unary minus on float4(0f).z sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/flt4.z);
AddStrictTestCase("unary minus on float4(0f).w sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/flt4.w);

flt4 = new float4(-0f);
flt4 = -flt4;
AddStrictTestCase("unary minus on float4(-0f).x sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/flt4.x);
AddStrictTestCase("unary minus on float4(-0f).y sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/flt4.y);
AddStrictTestCase("unary minus on float4(-0f).z sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/flt4.z);
AddStrictTestCase("unary minus on float4(-0f).w sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/flt4.w);

var u = -flt4;
Assert.expectEq("returns a float4", "float4", typeof(u));



