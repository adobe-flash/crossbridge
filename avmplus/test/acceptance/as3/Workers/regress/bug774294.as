/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
package {
    import flash.system.Worker
    import flash.system.WorkerDomain
    import avmplus.System
    import com.adobe.test.Assert

//         var SECTION = "Workers";
//         var VERSION = "as3";
//         var TITLE   = "regression test 774294";

    if (Worker.current.isPrimordial) {
        var w:Worker=WorkerDomain.current.createWorkerFromPrimordial();
        w.start();
        // after terminiate() call: 
        // Assertion failed: "((from == Isolate::RUNNING || from == Isolate::STARTING || from == Isolate::FINISHING || from == Isolate::NEW))" ("../core/Isolate.cpp":380)
        w.terminate();
        var start:uint=getTimer();
        while (w.state!="terminated") {
            System.sleep(10);
            if (getTimer()-start>2000) break;
        }
        Assert.expectEq("assert state is terminated","terminated",w.state);
    } else {
        trace("started background worker");
    }
}
