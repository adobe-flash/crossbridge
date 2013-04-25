/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Closure creation, deep environment and many free variables";
include "driver.as"

var g = function ():int { return 1; }

function allocloop():int {
    var v1:int=g();
    function f1(a:int):int {
        var v2:int=g();
        function f2(b:int):int {
            var v3:int=g();
            function f3(c:int):int {
                var v:Function;
                for ( var i:uint=0 ; i < 100000 ; i++ )
                    v = function (x:int):int { return x+v1+v2+v3+a+b+c }
                return i;
            }
            return f3(v1);
        }
        return f2(v1);
    }
    return f1(v1);
}

TEST(allocloop, "alloc-10");
