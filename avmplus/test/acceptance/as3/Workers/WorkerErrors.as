/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

    import flash.system.Worker
    import flash.system.WorkerDomain
import com.adobe.test.Assert;

    if (Worker.current.isPrimordial) {
//         var SECTION = "Workers";
//         var VERSION = "as3";
//         var TITLE   = "Test Worker errors.";

        // start a worker twice
        var worker1:Worker=WorkerDomain.current.createWorkerFromPrimordial();
        worker1.start();
        var exception1:String="no exception";
        try {
            worker1.start();
        } catch (e) {
            exception1=e.toString();
        }
        Assert.expectEq("exception is thrown when starting a worker already started","Error: Error #1511",exception1.substring(0,"Error: Error #1511".length));


        // stop a worker not started
        var worker2:Worker=WorkerDomain.current.createWorkerFromPrimordial();
        var code2:Boolean= worker2.terminate();
        Assert.expectEq("worker terminate returns false if worker not started",false,code2);
        
        worker1.terminate();
    } else {
        trace("in background worker");
    }
}
