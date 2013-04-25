/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector.every() with float";


function checker1(value,index,obj):Boolean {
  msg+="checker1("+value+","+index+",["+obj+"])";
  if (value==0)
    return false;
  return true;
}

var msg="";
var v1=new Vector.<float>();

var v1 = Vector.<float>([3.1415, float.MAX_VALUE, -0.00032]);
Assert.expectEq("every: float vector does not contain a zero",
            true,
            v1.every(checker1));

var v1 = Vector.<float>([3.1415, float.MAX_VALUE, 0.00, -0.00032]);
Assert.expectEq("every: float vector does contain a zero",
            false,
            v1.every(checker1));

