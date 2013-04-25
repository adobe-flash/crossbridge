/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

import flash.sampler.*;
import avmplus.*;
import com.adobe.test.Assert;

// var SECTION = "Sampling";
// var VERSION = "AS3";
// var TITLE   = "Iterate and investigate the sample data";

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
startSampling();
simpleLoop();
pauseSampling();

var nallocs:uint=0;
var ndeallocs:uint=0;
var allocs:uint=0;
var deallocs:uint=0;
var alloctable:Array=new Array();
var badalloc:String="";
for each (var sample in getSamples()) {
    var nos : NewObjectSample;
    var dos : DeleteObjectSample;
    if ( (nos=sample as NewObjectSample) != null) {
        nallocs++;
        allocs+=nos.size;
        alloctable[nos.id]=nos.size;
    } else if ( (dos=sample as DeleteObjectSample) !=null) {
        ndeallocs++;
    deallocs+=dos.size;
    if (alloctable[dos.id]!=dos.size) {
        badalloc+="dealloc id:"+dos.id+" "+dos.size+" does not match alloc size: "+alloctable[dos.id]+"\n";
    }
    }
}
print("nallocs  ="+nallocs+" allocs  ="+allocs);
print("ndeallocs="+ndeallocs+" deallocs="+deallocs);

Assert.expectEq(
    "ProcessSamples: deallocs match allocs",
    "",
    badalloc
);

var sizeAllocs:uint=0;
var sizeDeallocs:uint=0;
for each (var sample1 in getSamples()) {
    var nos1 : NewObjectSample;
    var dos1 : DeleteObjectSample;
    if ( (nos1=sample as NewObjectSample) != null) {
        sizeAllocs+=getSize(nos1.object);
    }
}
print("sizeAllocs="+sizeAllocs);
print("sizeDeAllocs="+sizeDeallocs);

if (isdebugger) {
    Assert.expectEq(
        "GetSizeSamples: test getSize on NewObjectSamples returns a positive value",
        true,
        sizeAllocs>0
    );
} else {
    Assert.expectEq(
        "GetSizeSamples: test getSize on NewObjectSamples returns a positive value",
        false,
        sizeAllocs>0
    );
}
var objs1:Object=new Object();
for each (var sample2 in getSamples()) {
    var nos2 : NewObjectSample;
    var dos2 : DeleteObjectSample;
    if ( (nos2=sample2 as NewObjectSample) != null) {
        if (objs1.hasOwnProperty(nos2.stack[0].name)==false)
            objs1[nos2.stack[0].name]=0;
        objs1[nos2.stack[0].name]+=nos2.size;
    }
}
for (var item in objs1) {
   print(item+" "+objs1[item]);
}
if (isdebugger) {
    Assert.expectEq(
        "InternalsInStack: assert the [verify] internal is shown in the samples and has > 0 size",
        true,
        objs1['[verify]']>0
    );
}

for (var sample3 in getSamples()) {
    print("getsample:"+sample3);
}


}
