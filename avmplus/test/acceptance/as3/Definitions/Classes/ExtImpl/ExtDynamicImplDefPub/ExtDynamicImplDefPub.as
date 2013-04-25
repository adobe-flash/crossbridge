/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DynamicClass {

import DynamicClass.*;

class ExtDynImplDefPubInner extends DynamicClass implements DefaultIntDef {

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
    // default interface as a public method
    // ****************************************

    public function iGetPubBoolean() : Boolean { return this.pubBoolean; }

    
    // ************************************
    // access public method of parent
    // from default method of sub class
    // ************************************

    function subGetArray() : Array { return this.getPubArray(); }
    function subSetArray(a:Array) { this.setPubArray(a); }

    public function testGetSubArray(a:Array) : Array {
        this.subSetArray(a);
        return this.subGetArray();
    }


    // ************************************
    // access public method of parent
    // from public method of sub class
    // ************************************

    public function pubSubGetArray() : Array { return this.getPubArray(); }
    public function pubSubSetArray(a:Array) { this.setPubArray(a); }

    // ************************************
    // access public method of parent
    // from private method of sub class
    // ************************************

    private function privSubGetArray() : Array { return this.getPubArray(); }
    private function privSubSetArray(a:Array) { this.setPubArray(a); }

    // function to test above from test scripts
    public function testPrivSubArray(a:Array) : Array {
        this.privSubSetArray(a);
        return this.privSubGetArray();
    }


 }

public class ExtDynamicImplDefPub extends ExtDynImplDefPubInner {}
}
