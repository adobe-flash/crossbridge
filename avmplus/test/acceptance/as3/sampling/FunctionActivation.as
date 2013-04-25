/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* testcases:
 *   function activation exposed to heap graph
 */

package {

    import flash.sampler.*
    import avmplus.*
import com.adobe.test.Assert;

//     var SECTION = "Sampling";
//     var VERSION = "AS3";
//     var TITLE   = "Function Activation";


    var isdebugger=System.isDebugger();

    var activationWasInScopes = false
    var activation
    var arg = "ARG!";

    function f(a)
    {
        function g() { return a }
    return g
    }

    var func = f(arg);
    print(func is Function);
    var scopes=getLexicalScopes(func);
    for each (var scope in scopes) {
        if(scope.a == arg) {
       activationWasInScopes = true;
       activation = scope
       break;
    }
   }

    Assert.expectEq("assert the activation object was in getLexicalScopes",
        true,
        !isdebugger || activationWasInScopes );

    Assert.expectEq("assert the activation object was right object",
        true,
        !isdebugger || activation && activation.a == arg );
}
