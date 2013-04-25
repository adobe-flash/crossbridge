/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.6.3";
// var VERSION = "AS3";
// var TITLE   = "Component accesses";


// Access component via v.x
var flt4:float4 = new float4(1f, 2f, 3f, 4f);
AddStrictTestCase("float4.x as a getter", 1f, flt4.x);
AddStrictTestCase("float4.y as a getter", 2f, flt4.y);
AddStrictTestCase("float4.z as a getter", 3f, flt4.z);
AddStrictTestCase("float4.w as a getter", 4f, flt4.w);
flt4 = new float4(1f, 2f, 3f, 4f);
AddStrictTestCase("float4.xxxx as a getter", new float4(1f), flt4.xxxx);
AddStrictTestCase("float4.yyyy as a getter", new float4(2f), flt4.yyyy);
AddStrictTestCase("float4.zzzz as a getter", new float4(3f), flt4.zzzz);
AddStrictTestCase("float4.wwww as a getter", new float4(4f), flt4.wwww);
AddStrictTestCase("float4.xyzw as a getter", new float4(1f, 2f, 3f, 4f), flt4.xyzw);
AddStrictTestCase("float4.wzyx as a getter", new float4(4f, 3f, 2f, 1f), flt4.wzyx);

// Access component via v['x'] section 4.6.3.3.b
flt4 = new float4(1f);
Assert.expectError("float4['x'] as a setter", Utils.REFERENCEERROR, function(){ flt4['x'] = 12f });
Assert.expectError("float4['y'] as a setter", Utils.REFERENCEERROR, function(){ flt4['y'] = 12f });
Assert.expectError("float4['z'] as a setter", Utils.REFERENCEERROR, function(){ flt4['z'] = 12f });
Assert.expectError("float4['w'] as a setter", Utils.REFERENCEERROR, function(){ flt4['w'] = 12f });
flt4 = new float4(1f, 2f, 3f, 4f);
AddStrictTestCase("float4['x'] as a getter", 1f, flt4['x']);
AddStrictTestCase("float4['y'] as a getter", 2f, flt4['y']);
AddStrictTestCase("float4['z'] as a getter", 3f, flt4['z']);
AddStrictTestCase("float4['w'] as a getter", 4f, flt4['w']);
flt4 = new float4(1f, 2f, 3f, 4f);
AddStrictTestCase("float4['xxxx'] as a getter", new float4(1f), flt4['xxxx']);
AddStrictTestCase("float4['yyyy'] as a getter", new float4(2f), flt4['yyyy']);
AddStrictTestCase("float4['zzzz'] as a getter", new float4(3f), flt4['zzzz']);
AddStrictTestCase("float4['wwww'] as a getter", new float4(4f), flt4['wwww']);
AddStrictTestCase("float4['xyzw'] as a getter", new float4(1f, 2f, 3f, 4f), flt4['xyzw']);
AddStrictTestCase("float4['wzyx'] as a getter", new float4(4f, 3f, 2f, 1f), flt4['wzyx']);

// Access component via v[0] section 4.6.3.2.a
flt4 = new float4(1f);
Assert.expectError("float4[0] as a setter", Utils.REFERENCEERROR, function(){ flt4[0] = 12f });
Assert.expectError("float4[1] as a setter", Utils.REFERENCEERROR, function(){ flt4[1] = 12f });
Assert.expectError("float4[2] as a setter", Utils.REFERENCEERROR, function(){ flt4[2] = 12f });
Assert.expectError("float4[3] as a setter", Utils.REFERENCEERROR, function(){ flt4[3] = 12f });

// Access outside of valid index ranges section 4.6.3.2.b
var i:Number = -1;
Assert.expectError("float4[i] -1", Utils.REFERENCEERROR, function(){ flt4[i]; });
i = 4;
Assert.expectError("float4[i] 4", Utils.RANGEERROR, function(){ flt4[i]; });


// Access component via v['0'] section 4.6.3.3.a
flt4 = new float4(1f);
Assert.expectError("float4['0'] as a setter", Utils.REFERENCEERROR, function(){ flt4['0'] = 12f });
Assert.expectError("float4['1'] as a setter", Utils.REFERENCEERROR, function(){ flt4['1'] = 12f });
Assert.expectError("float4['2'] as a setter", Utils.REFERENCEERROR, function(){ flt4['2'] = 12f });
Assert.expectError("float4['3'] as a setter", Utils.REFERENCEERROR, function(){ flt4['3'] = 12f });
flt4 = new float4(1f, 2f, 3f, 4f);
AddStrictTestCase("float4['0'] as a getter", 1f, flt4['0']);
AddStrictTestCase("float4['1'] as a getter", 2f, flt4['1']);
AddStrictTestCase("float4['2'] as a getter", 3f, flt4['2']);
AddStrictTestCase("float4['3'] as a getter", 4f, flt4['3']);


// Access component via v['onProto'] section 4.6.3.3.c
float4.prototype.onProto = function(){ return "foobar"; };
flt4 = new float4(1f);
Assert.expectEq("float4['onProto']", "function", typeof flt4['onProto']);
delete(float4.prototype.onProto);
Assert.expectError("float4['onProto'] after removed", Utils.REFERENCEERROR, function(){ flt4['onProto']; });


