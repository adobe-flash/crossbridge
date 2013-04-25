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
//         var TITLE   = "Test Worker creating large number of workers.";

        var max=50;
        var exception="no exception";
        var workers=new Array();
        var i:uint;
        for (i=0;i<max;i++) {
            try {
                var worker:Worker=WorkerDomain.current.createWorkerFromPrimordial();
                worker.start();
                workers.push(worker);
            } catch (e) {            
                exception=e.toString();
            }
        }
        for (i=0;i<max;i++) {
            workers[i].terminate();
        }
        Assert.expectEq("Exception is not thrown when 50 workers are created","no exception",exception);
        Assert.expectEq("Maxium number of workers was not exceeded",50,workers.length);
    }
}
