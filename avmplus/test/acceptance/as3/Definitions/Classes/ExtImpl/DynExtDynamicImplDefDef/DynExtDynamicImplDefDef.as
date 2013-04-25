/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DynamicClass {

import DynamicClass.*;

dynamic class DynExtDynImplDefDefInner extends DynamicClass implements DefaultIntDef, DefaultInt {

    // ****************************************
    // define a default definition from
    // default interface as a default method
    // ****************************************

    public function iGetBoolean() : Boolean { return this.boolean; }

    public function testGetSetBoolean( flag:Boolean ) : Boolean {

      setBoolean(flag);
      return iGetBoolean();
    }


    // ****************************************
    // define a public defintion from
    // default interface as a default method
    // ****************************************

    public function iGetPubBoolean() : Boolean { return this.pubBoolean; }

    public function testPubGetSetBoolean( flag:Boolean ) : Boolean {

      setPubBoolean(flag);
      return iGetPubBoolean();
    }


    // ****************************************
    // define a default defintion from
    // default interface 2 as a default method
    // ****************************************

    public function iGetNumber() : Number { return this.number; }

    public function testGetSetNumber( num:Number ) : Number {

      setNumber(num);
      return iGetNumber();
    }


    // ****************************************
    // define a public definition from
    // default interface 2 as a default method
    // ****************************************

    public function iGetPubNumber() : Number { return this.pubNumber; }

    public function testPubGetSetNumber( num:Number ) : Number {

      setPubNumber(num);
      return iGetPubNumber();
    }


    // ************************************
    // access default method of parent
    // from default method of sub class
    // ************************************

    function subGetArray() : Array { return this.getArray(); }
    function subSetArray(a:Array) { this.setArray(a); }

    public function testGetSubArray(a:Array) : Array {
        this.subSetArray(a);
        return this.subGetArray();
    }


    // ************************************
    // access default method of parent
    // from public method of sub class
    // ************************************

    public function pubSubGetArray() : Array { return this.getArray(); }
    public function pubSubSetArray(a:Array) { this.setArray(a); }

    // ************************************
    // access default method of parent
    // from private method of sub class
    // ************************************

    private function privSubGetArray() : Array { return this.getArray(); }
    private function privSubSetArray(a:Array) { this.setArray(a); }

    // function to test above from test scripts
    public function testPrivSubArray(a:Array) : Array {
        this.privSubSetArray(a);
        return this.privSubGetArray();
    }

    // ***************************************
    // access default property from
    // default method of sub class
    // ***************************************

    function subGetDPArray() : Array { return array; }
    function subSetDPArray(a:Array) { array = a; }

    public function testSubGetDPArray(a:Array) : Array {
        this.subSetDPArray(a);
        return this.subGetDPArray();
    }


    // ***************************************
    // access default property from
    // public method of sub class
    // ***************************************

    public function pubSubGetDPArray() : Array { return this.array; }
    public function pubSubSetDPArray(a:Array) { this.array = a; }

    // ***************************************
    // access default property from
    // private method of sub class
    // ***************************************

    private function privSubGetDPArray() : Array { return this.array; }
    private function privSubSetDPArray(a:Array) { this.array = a; }

    public function testPrivSubGetDPArray(a:Array) : Array {
        this.privSubSetDPArray(a);
        return this.privSubGetDPArray();
    }
 }

public class DynExtDynamicImplDefDef extends DynExtDynImplDefDefInner {}


}
