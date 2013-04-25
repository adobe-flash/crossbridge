/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.5.2";
// var VERSION = "AS3";
// var TITLE   = "Vector.<float4> smoketest";


// Vector.<float4> smoketest.  Tests that vectors are initialized to
// zero, that reading and writing works, that the data written can be
// read again, and that we're not scribbling on memory.
//
// Run this with -Ojit and -Dinterp in a Debug build; the debugging
// functionality in MMgc will catch memory overwrites, should they
// occur.
//
// We test for a lot of different vector lengths, and with and without
// type information, in order to test more JIT and runtime paths.

// Global code, no annotations

for ( var i=0 ; i < 100 ; i++ ) {
    var w = new Vector.<float4>(i);
    Assert.expectEq("Global: length", i, w.length);
    for ( var j=0 ; j < i ; j++ )
        Assert.expectEq("Global: zero", float4(0,0,0,0), w[j]);
    for ( var j=0 ; j < i ; j++ )
        w[j] = float4(1000+i, 2000+i, 3000+i, 4000+i);
    for ( var j=0 ; j < i ; j++ )
        Assert.expectEq("Global: read", float4(1000+i, 2000+i, 3000+i, 4000+i), w[j]);
}

// Function code, full annotations

function f(): void
{
    for ( var i:int=0 ; i < 100 ; i++ ) {
        var w:Vector.<float4> = new Vector.<float4>(i);
        var j:int;
        Assert.expectEq("Local: length", i, w.length);
        for ( j=0 ; j < i ; j++ )
            Assert.expectEq("Local: zero", float4(0,0,0,0), w[j]);
        for ( j=0 ; j < i ; j++ )
            w[j] = float4(1000+i, 2000+i, 3000+i, 4000+i);
        for ( j=0 ; j < i ; j++ )
            Assert.expectEq("Local: read", float4(1000+i, 2000+i, 3000+i, 4000+i), w[j]);
    }
}

f();

