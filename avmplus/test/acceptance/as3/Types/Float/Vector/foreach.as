/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector.foreach() with float";


function eacher(value,index,obj) {
  result+="("+value+":"+index+")";
}

var v1=new Vector.<float>();
var v1=Vector.<float>([4561,9121,13681]);
var result="";
v1.forEach(eacher);
Assert.expectEq(    "forEach simple float vector",
        "(4561:0)(9121:1)(13681:2)",
        result);

