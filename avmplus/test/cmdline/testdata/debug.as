/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
 public class debug {
    public function debug() {
        print("in constructor()");
    }
    public function pubfunc() {
        print("in pubfunc()");
        privfunc();
    }
    public function privfunc() {
        print("in privfunc()");
    }
    public function locals(arg1,arg2:int) {
        var local1;
        var local2:int=10;
        local2=15;
    }

    private var value;
    public function get accessor() {
        print("in get accessor");
        return value;
    }
    public function set accessor(value) {
        this.value=value;
    }
    public function exception() {
        callUnknownFunc(); // should throw an exception
    }
    public var var1="var1";
    public var var2=15;
}
import avmplus.System
var d=new debug();
d.locals(110,115);
d.pubfunc();
d.accessor="set value";
print(d.accessor);
d.exception();
}
