/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* testcases:
 *   master string exposed to heap graph
 */

package {

    import flash.sampler.*
    import avmplus.*
import com.adobe.test.Assert;

//     var SECTION = "Sampling";
//     var VERSION = "AS3";
//     var TITLE   = "Master String";


    var isdebugger=System.isDebugger();

    var helloWorld = "hello, world";
    var hello = helloWorld.substr(0, 5);

    Assert.expectEq("assert that static string constant does not have a master string",
        null,
        getMasterString(helloWorld));

    Assert.expectEq("assert that substring has a master string",
        isdebugger ? helloWorld : null,
        getMasterString(hello));

}
