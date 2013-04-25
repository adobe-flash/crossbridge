/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector.<type> constructor (length, fixed) with float";


var v12_f=new Vector.<float>();
v12_f.push(3.15);
Assert.expectEq( "constructor type float", float(3.15), v12_f[0]); // todo: use float literals

function bug449468_f() {
    var v : Vector.<Vector.<float>> = new Vector.<Vector.<float>>(4);
    return v;
}

Assert.expectEq("Replicating bug 449468 testcase on float: Crash with vector constructor in interp mode",
            "null,null,null,null",
            bug449468_f().toString()
            );



