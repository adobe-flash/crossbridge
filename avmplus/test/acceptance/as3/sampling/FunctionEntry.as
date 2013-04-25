/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* testcases:
 *   function entry: execute deep recursion verify significant number of samples
 */

package {

    import flash.sampler.*
    import avmplus.*
    import com.adobe.test.Assert;

//     var SECTION = "Sampling";
//     var VERSION = "AS3";
//     var TITLE   = "Function Entry";

    var isdebugger=System.isDebugger();


    var callcount:uint=0;
    var samplescount:uint=0;
    var stoppagetime=0;

    function ackermann(m:uint,n:uint):Number {
        callcount+=1;
        if (m==0) return n+1;
        if (m>0 && n==0) return ackermann(m-1,1);
        return ackermann(m-1,ackermann(m,n-1));
    }
    function samplecallback() {
        var time=getTimer();
        samplescount+=getSampleCount();
        clearSamples();
        stoppagetime+=getTimer()-time;
        return true;

    }
    setSamplerCallback(samplecallback);
    sampleInternalAllocs(false);
    startSampling();
    var totaltime=getTimer();
    var result=0;
    // if exception is thrown, stack overflow ok
    try {
        var start=getTimer();
        result=ackermann(3,7);
        if (getTimer()-start<200) {
            result=ackermann(3,8);
        }
    } catch (e) {
        print("exception, result="+result);
        print(e.getStackTrace());
    }
    pauseSampling();
    totaltime=getTimer()-totaltime-stoppagetime;
    samplescount+=getSampleCount();
    Assert.expectEq("assert the number of samples is greater than 20, actual is "+samplescount,
        true,
        samplescount>20);
}
