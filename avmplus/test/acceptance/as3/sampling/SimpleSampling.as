/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.sampler.*;
import avmplus.*;
import com.adobe.test.Assert;

// var SECTION = "Sampling";
// var VERSION = "AS3";
// var TITLE   = "Sampling Basic Usage";


var isdebugger=System.isDebugger();

var objs:Array=new Array();
class simpleobject {
    var str:String;
    function simpleobject(s:String) {
        this.str=s;
    }
}

sampleInternalAllocs(false);
startSampling();
simpleLoop();
pauseSampling();

if (isdebugger) {
    Assert.expectEq(
      "test getSampleCount on simple loop internalAllocs=false",
      true,
      getSampleCount()>20
    );
} else {
    Assert.expectEq(
      "test getSampleCount on simple loop internalAllocs=false",
      false,
      getSampleCount()>20
    );
}
var len:int=0;
for each (var sample in getSamples()) {
  len++;
}
if (isdebugger) {
    Assert.expectEq(
      "test getSamples length on simple loop internalAllocs=false",
      true,
      len>20
    );

    clearSamples();

    Assert.expectEq(
      "after clearSamples() sample count is 0, internalAllocs=false",
      0,
      getSampleCount()
    );
}
len=0;
for each (var sample in getSamples()) {
    len++;
}
if (isdebugger) {
    Assert.expectEq(
      "after clearSamples() getSamples() returns no samples, internalAllocs=false",
      0,
      len
    );
}
sampleInternalAllocs(true);
startSampling();
simpleLoop();
pauseSampling();

if (isdebugger) {
    Assert.expectEq(
      "test getSampleCount on simple loop internalAllocs=true",
      true,
      getSampleCount()>50
    );
}
var len:int=0;
for each (var sample in getSamples()) {
  len++;
}

if (isdebugger) {
    Assert.expectEq(
      "test getSamples length on simple loop internalAllocs=true",
      true,
      len>20
    );
}

clearSamples();
    var len:int=0;
    for (sample in getSamples()) {
        if ((sample as DeleteObjectSample)==null) {
            len++;
        }
    }

if (isdebugger) {
    Assert.expectEq(
      "after clearSamples() sample count is 0, internalAllocs=true",
      0,
      len
    );
}

len=0;
for each (var sample in getSamples()) {
    if ((sample as DeleteObjectSample)==null) {
        len++;
    }
}
if (isdebugger) {
    Assert.expectEq(
      "after clearSamples() getSamples() returns no samples, internalAllocs=true",
      0,
      len
    );
}


function simpleLoop() {
    for (var i:int=0;i<100;i++) {
       objs.push(new simpleobject(""+i));
    }
}
