/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.4.3";
// var VERSION = "AS3";
// var TITLE   = "The % operation agumented by float values";



var f1:float = 1.2345678e9;
var f2:float = 6.543210987;
 /*
    virgilp: 09/23/2011
    modulo on float & number should THEORETICALLY yield different results
    However, on gcc & VisualC (windows/x86), fmod seems to return identical results with fmodf
    if the input values are identical
    (and no, I didn't get fooled by the "fmod" version with float arguments - I explicitly
    casted the parameters to double)
    I suspect the runtime library implementations perform both operations on 64 bits.
    The thing is, there's no hardware instruction for modulo - except the x87 FPU
    (it has a "FPREM" instruction)
    Bottom line is - the C runtimelibrary makes no diffeernce (not on x86 and x64, Mac/Win, at least)
   */

// ABC Extension spec states that Float modulo can happen in double precision
Assert.expectEq("modulo on float & number should yield different results", true, (Number(f1)%Number(f2)) == (f1%f2) );


