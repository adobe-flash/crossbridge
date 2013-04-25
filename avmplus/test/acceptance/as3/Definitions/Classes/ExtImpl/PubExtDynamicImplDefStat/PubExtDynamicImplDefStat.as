/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DynamicClass {

import DynamicClass.*;

public class PubExtDynamicImplDefStat extends DynamicClass implements DefaultIntDef {

    // *******************************************************************
    // defining as static causes compiler error( tested in another file )
    // still need to define the interface methods to make the rest of the
    // tests work
    // *******************************************************************

    public function iGetBoolean() : Boolean { return this.boolean; }
    public function iGetPubBoolean() : Boolean { return this.pubBoolean; }

    // ************************************
    // access static method of parent
    // from default method of sub class
    // ************************************

    function subGetArray() : Array { return getStatArray(); }
    function subSetArray(a:Array) { setStatArray(a); }

    public function testSubSetArray(a:Array) : Array {
        this.subSetArray(a);
        return this.subGetArray();
    }


    // ************************************
    // access static method of parent
    // from public method of sub class
    // ************************************

    public function pubSubGetArray() : Array { return getStatArray(); }
    public function pubSubSetArray(a:Array) { setStatArray(a); }

    // ************************************
    // access static method of parent
    // from private method of sub class
    // ************************************

    private function privSubGetArray() : Array { return getStatArray(); }
    private function privSubSetArray(a:Array) { setStatArray(a); }

    // function to test above from test scripts
    public function testPrivSubArray(a:Array) : Array {
        this.privSubSetArray(a);
        return this.privSubGetArray();
    }

    // ***************************************
    // access static method of parent
    // from static method of sub class
    // ***************************************

    static function statSubGetArray() : Array { return getStatArray(); }
    static function statSubSetArray(a:Array) { setStatArray(a); }

    public static function testStatSubArray(a:Array) : Array {
        statSubSetArray(a);
        return statSubGetArray();
    }


    // ***************************************
    // access static method of parent
    // from public static method of sub class
    // ***************************************

    public static function pubStatSubGetArray() : Array { return getStatArray(); }
    public static function pubStatSubSetArray(a:Array) { setStatArray(a); }

    // ***************************************
    // access static method of parent
    // from private static method of sub class
    // ***************************************

    private static function privStatSubGetArray() : Array { return getStatArray(); }
    private static function privStatSubSetArray(a:Array) { setStatArray(a); }

    // function to test above test case
    public function testPrivStatSubArray(a:Array) : Array {
        privStatSubSetArray(a);
        return privStatSubGetArray();
    }

    // ***************************************
    // access static property from
    // default method of sub class
    // ***************************************

    function subGetDPArray() : Array { return statArray; }
    function subSetDPArray(a:Array) { statArray = a; }

    public function testSubSetDPArray(a:Array) : Array {
        subSetDPArray(a);
        return subGetDPArray();
    }


    // ***************************************
    // access static property from
    // public method of sub class
    // ***************************************

    public function pubSubGetDPArray() : Array { return statArray; }
    public function pubSubSetDPArray(a:Array) { statArray = a; }

    // ***************************************
    // access static property from
    // private method of sub class
    // ***************************************

    private function privSubGetDPArray() : Array { return statArray; }
    private function privSubSetDPArray(a:Array) { statArray = a; }

    public function testPrivSubSetDPArray(a:Array) : Array {
        privSubSetDPArray(a);
        return privSubGetDPArray();
    }


    // ***************************************
    // access static property from
    // static method of sub class
    // ***************************************

    static function statSubGetDPArray() : Array { return statArray; }
    static function statSubSetDPArray(a:Array) { statArray = a; }

    public static function testStatSetDPArray(a:Array) : Array {
        statSubSetDPArray(a);
        return statSubGetDPArray();
    }


    // ***************************************
    // access static property from
    // public static method of sub class
    // ***************************************

    public static function pubStatSubGetDPArray() : Array { return statArray; }
    public static function pubStatSubSetDPArray(a:Array) { statArray = a; }

    // ***************************************
    // access static property from
    // private static method of sub class
    // ***************************************

    private static function privStatSubGetDPArray() : Array { return statArray; }
    private static function privStatSubSetDPArray(a:Array) { statArray = a; }

    // function to test above

    public function testPrivStatSubDPArray(a:Array) {
        statArray = a;
        return statArray;
    }
 }
}
