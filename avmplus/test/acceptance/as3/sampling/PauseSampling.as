/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.sampler.*
import avmplus.*
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
function simpleLoop() {
    for (var i:int=0;i<10;i++) {
       objs.push(new simpleobject(""+i));
    }
}

sampleInternalAllocs(true);

simpleLoop();
if (isdebugger) {
    Assert.expectEq(
      "BeforeStartSampling: test before startSampling sample count 0",
      0,
      getSampleCount()
    );
} else {
    Assert.expectEq(
      "BeforeStartSampling: test before startSampling sample count 0",
      -1,
      getSampleCount()
    );
}
startSampling();
simpleLoop();
pauseSampling();
var time=getTimer();
while (getTimer()<time+50) {}
var count=getSampleCount();
var newcount=getNewObjectCount();

simpleLoop();

if (isdebugger) {
    Assert.expectEq(
      "PauseSampling: after pauseSampling verify no more NewObjectSamples",
      newcount,
      getNewObjectCount()
    );
} else {
    Assert.expectEq(
      "PauseSampling: after pauseSampling verify no more samples are counted",
      -1,
      getSampleCount()
    );
}
startSampling();
simpleLoop();
stopSampling();

if (isdebugger) {
    Assert.expectEq(
      "StopSampling: after stopSampling samples are reset",
      0,
      getSampleCount()
    );
} else {
    Assert.expectEq(
      "StopSampling: after stopSampling samples are reset",
      -1,
      getSampleCount()
    );
}

function getNewObjectCount():int {
    var ct:int=0;
    var nos:NewObjectSample;
    for each (var sample in getSamples()) {
        if ((nos=sample as NewObjectSample) != null) {
            ct++;
        }
    }
    return ct;
}
