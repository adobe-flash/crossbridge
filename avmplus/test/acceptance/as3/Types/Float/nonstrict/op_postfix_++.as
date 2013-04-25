/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "6.3.10";
// var VERSION = "AS3";
// var TITLE   = "The postfix ++ operator";



var flt:float = new float(3.1413119f);
var flt_plus_1 = flt + 1.0f;

AddStrictTestCase("postfix ++ on float", flt, flt++);
AddStrictTestCase("postfix ++ on float", flt_plus_1, flt);

flt = --flt;
var o = new MyObject(flt);
var u = o++;

Assert.expectEq("postfix ++ on Object (that converts to float through ToPrimitive)", u, flt)
Assert.expectEq("postfix ++ on Object (that converts to float through ToPrimitive)", o, flt_plus_1)
Assert.expectEq("postfix ++ on Object returns a float", "float", getQualifiedClassName(u));
Assert.expectEq("postfix ++ on Object returns a float", "float", getQualifiedClassName(o));

/*
12.375f++
-> int rep of memory 1096155136
-> 13.375 double is
01000000 00101010 11000000 00000000
00000000 00000000 00000000 00000000


12.375f
(12.375)10 =
(12)10 + (0.375)10 =
(1100)2 + (0.011)2 =
(1100.011)2 =
(1.100011)2 x2^3
sign = 0
exponent = 130 (127 biased, 127 +3, binary 1000 0010)
fraction = 100011
0-10000010-10001100000000000000000
01000001 01000110 00000000 00000000 -> int 1095106560


13.375f
(13.375)10 =
(13)10 + (0.375)10 =
(1101)2 + (0.011)2 =
(1101.011)2 =
(1.101011)2 x2^3
sign = 0
exponent = 130 (127 biased, 127 +3, binary 1000 0010)
fraction = 101011
0-10000010-10101100000000000000000
01000001 01010110 00000000 00000000 -> int 1096155136

*/


flt = new float(12.375f);
var flt_as_int = 1095106560;
var flt_plus_1_as_int = 1096155136;
Assert.expectEq("verify float bits", flt_as_int, FloatRawBits(flt));
Assert.expectEq("verify float bits calling flt++", flt_as_int, FloatRawBits(flt++));
Assert.expectEq("verify float after ++ is correct int representation", flt_plus_1_as_int, FloatRawBits(flt));


flt = float.MAX_VALUE;
AddStrictTestCase("float.MAX_VALUE++", float.MAX_VALUE, ++flt);
AddStrictTestCase("float.MAX_VALUE after call", float.MAX_VALUE, flt);
flt = float.MIN_VALUE;
AddStrictTestCase("float.MIN_VALUE++", 1f, ++flt);
AddStrictTestCase("float.MIN_VALUE after call", 1f, flt);
flt = float.POSITIVE_INFINITY;
AddStrictTestCase("float.POSITIVE_INFINITY++", float.POSITIVE_INFINITY, ++flt);
AddStrictTestCase("float.POSITIVE_INFINITY after call", float.POSITIVE_INFINITY, flt);
flt = float.NEGATIVE_INFINITY;
AddStrictTestCase("float.NEGATIVE_INFINITY++", float.NEGATIVE_INFINITY, ++flt);
AddStrictTestCase("float.NEGATIVE_INFINITY after call", float.NEGATIVE_INFINITY, flt);


