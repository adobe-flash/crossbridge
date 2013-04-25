/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

import flash.sampler.*;
import avmplus.*;
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "Sampling";
// var VERSION = "AS3";
// var TITLE   = "test the callback functionality when sampler memory is too high";

var isdebugger=System.isDebugger();


var objs:Array=new Array();
class simpleobject {
    var str:String;
    function simpleobject(s:String) {
        this.str=s;
    }
}
function simpleLoop(n:int=10) {
    for (var i:int=0;i<n;i++) {
        var s="0123456789";
        objs.push(new simpleobject(s));
    }
}
var allocs:int=0;
var deallocs:int=0;
function testSamples():int {
    pauseSampling();
    var nos : NewObjectSample;
    var dos : DeleteObjectSample;
    for each (sample in getSamples()) {
        if ((nos=sample as NewObjectSample) != null) {
            allocs++;
        }
        if ((dos=sample as DeleteObjectSample) != null) {
            deallocs++;
        }
    }
    clearSamples();
    startSampling();
    return allocs;
}
var callback1_num:int=0;
var callback2_num:int=0;
var callback3_num:int=0;

function callback1() {
    print("callback1 called at "+objs.length)
    callback1_num++;
    testSamples();
}
function callback2():int {
    callback2_num++;
    print("callback2 called at "+objs.length)
    return testSamples();
}
function callback3(n:int):String {
    callback3_num++;
    print("callback3 called at "+objs.length)
    testSamples();
}

sampleInternalAllocs(true);
setSamplerCallback(callback1);
startSampling();
simpleLoop(80000);
pauseSampling();
print("memory summary: allocs="+allocs+" deallocs="+deallocs);

if (isdebugger)
Assert.expectEq(
  "Callback: callback called at least once callback#="+callback1_num,
  true,
  callback1_num>0
);

setSamplerCallback(callback2);
startSampling();
simpleLoop(80000);
pauseSampling();

if (isdebugger)
Assert.expectEq(
  "Callback: callback with return type called at least once callback#="+callback2_num,
  true,
  callback2_num>0
);

var errstr2="";

try {
    setSamplerCallback(abc);
    startSampling();
    simpleLoop(80000);
} catch (e) {
    errstr2=e.toString();
}
pauseSampling();
Assert.expectEq(
  "Callback: function is not defined",
  "ReferenceError: Error #1065",
  Utils.parseError(errstr2,"ReferenceError: Error #1065".length)
);

}
