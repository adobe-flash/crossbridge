/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "5.4";
// var VERSION = "AS3";
// var TITLE   = "The bitwise shift operators augmented by float4 values";




var f1:float4 = float4(3f, 6f, 9f, 3f);

AddStrictTestCase("float4<<2", 0, f1<<2);
AddStrictTestCase("12<<float4", 12, 12<<f1);
AddStrictTestCase("float4<<float", 0, f1<<float(2f));
AddStrictTestCase("float<<float4", 3, float(3f)<<f1);


