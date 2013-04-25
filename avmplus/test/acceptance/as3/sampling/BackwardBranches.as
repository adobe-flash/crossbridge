/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* testcases:
 *   backward branches: execute loop to verify significant number of samples
 */

package {

    import flash.sampler.*
    import avmplus.*
    import com.adobe.test.Assert;

    /*Skipped test*/

//     var SECTION = "Sampling";
//     var VERSION = "AS3";
//     var TITLE   = "backward branches";

    var isdebugger=System.isDebugger();


    var callcount:uint=0;
    var samplescount:uint=0;
    var stoppagetime=0;

    function calculatepi(n:uint):Number {
        var sign=1;
        var result=0;
        var start=getTimer();
        var i=1;
        while (true) {
           callcount+=1;
           result+=sign*4/(i*2-1);
           sign*=-1;
           i++;
           if ((getTimer()-start>n && callcount>n) || getTimer()-start>5000)
               break;
        }
        return result;
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
    var result=calculatepi(1000);
    pauseSampling();
    totaltime=getTimer()-totaltime-stoppagetime;
    samplescount+=getSampleCount();
    Assert.expectEq("assert the number of samples is greater than 20, actual is "+samplescount,
        true,
        samplescount>20);
}
