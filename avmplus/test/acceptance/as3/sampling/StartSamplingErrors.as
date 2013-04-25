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


stopSampling();
if (isdebugger) {
    Assert.expectEq("StopBeforeStarted: call stopSampling() before startSampling is called",
    0,
    getSampleCount()
    );
} else {
    Assert.expectEq("StopBeforeStarted: call stopSampling() before startSampling is called",
    -1,
    getSampleCount()
    );
}

pauseSampling();
if (isdebugger) {
    Assert.expectEq("PauseBeforeStarted: call pauseSampling() before startSampling is called",
    0,
    getSampleCount()
    );
}
sampleInternalAllocs(true);
startSampling();
startSampling();
startSampling();
startSampling();
simpleLoop();
pauseSampling();

if (isdebugger) {
    Assert.expectEq(
      "StartSamplingMult:  call startSampling() multiple times in a row",
      true,
      getSampleCount()>50
    );
}
clearSamples();
clearSamples();

if (isdebugger) {
    Assert.expectEq(
      "ClearSamplingTwice: call clearSampling() twice in a row",
      0,
      getSampleCount()
    );
}
startSampling();
simpleLoop();
pauseSampling();
pauseSampling();
pauseSampling();

if (isdebugger) {
    Assert.expectEq(
      "PauseSamplingTwice: call pauseSamples() twice in a row",
      true,
      getSampleCount()>50
    );
}

function simpleLoop() {
    for (var i:int=0;i<100;i++) {
       objs.push(new simpleobject(""+i));
    }
}
