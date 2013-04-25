// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testArrayJoin(o, sep):String
{
    var s:String = (sep === undefined) ? "," : String(sep)
    var out:String = ""
    for (var i:uint = 0, n:uint=uint(o.length); i < n; i++)
    {
        var x = o[i]
        if (x != null)
            out += x
        if (i+1 < n)
            out += s
    }
    return out
}

function doArrayJoin() {
     var x:Array = [0, 10, 15, 30];
    print(testArrayJoin(x, " = "));
}

doArrayJoin();
