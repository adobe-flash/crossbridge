/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector.filter() with float";


function EvenChecker(value,index,obj) {
  if (value%2==0)
    return true;
  return false;
}

var vf:Vector.<float>=new Vector.<float>();
for (var i=0;i<10;i++) vf[i]=i;
var result=vf.filter(EvenChecker);
Assert.expectEq("filter small float vector",
        "0,2,4,6,8",
        result.toString());


