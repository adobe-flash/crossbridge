// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function ArrayConstructor(...args)
    {
        var n:uint = args.length
        if (n == 1 && (args[0] is Number)) {
            print("false");
        } else {
            print("Correct");
        }
    }


function stringArray() {
    var x:Object = new ArrayConstructor();
}

stringArray();
