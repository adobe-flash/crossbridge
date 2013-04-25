/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

// var SECTION = "4.4.16.17";
// var VERSION = "AS3";
// var TITLE   = "Math and geometric methods public function rsqrt(arg:float4):float4";


Assert.expectError("float4.rsqrt() with no args", Utils.ARGUMENTERROR+1063,  function(){ float4.rsqrt(); });


