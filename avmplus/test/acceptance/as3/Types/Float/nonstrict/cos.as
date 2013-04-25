/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

// var SECTION = "4.5.21";
// var VERSION = "AS3";
// var TITLE   = "public function cos(x:float):float";


Assert.expectError("float.cos() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.cos(); });

AddStrictTestCase("float.cos(undefined)", float.NaN, float.cos(undefined));
AddStrictTestCase("float.cos(true)", 0.5403022766113281f, float.cos(true));
AddStrictTestCase("float.cos(false)", float(1), float.cos(false));
AddStrictTestCase("float.cos(string)", float.NaN, float.cos("string"));
AddStrictTestCase("float.cos('0')", float(1), float.cos('0'));


