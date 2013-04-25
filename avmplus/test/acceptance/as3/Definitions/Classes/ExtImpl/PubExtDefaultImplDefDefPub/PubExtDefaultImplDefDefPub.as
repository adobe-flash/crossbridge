/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DefaultClass {

import DefaultClass.*;

public class PubExtDefaultImplDefDefPub extends DefaultClass implements DefaultIntDef, DefaultInt {

    // ****************************************
    // define a default definition from
    // default interface as a default method
    // ****************************************

    public function iGetBoolean() : Boolean { return true; }
    
    // ****************************************
    // define a public defintion from
    // default interface as a default method
    // ****************************************

    public function iGetPubBoolean() : Boolean { return this.pubBoolean; }

    // ****************************************
    // define a default definition from
    // default interface 2 as a default method
    // ****************************************

   public function iGetNumber() : Number { return 420; }

    // ****************************************
    // define a public defintion from
    // default interface 2 as a default method
    // ****************************************

    public function iGetPubNumber() : Number { return this.pubNumber; }

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

    // ************************************
    // access public method of parent
    // from final method of sub class
    // ************************************

    final function finSubGetArray() : Array { return this.getPubArray(); }
    final function finSubSetArray(a:Array) { this.setPubArray(a); }

    // function to test above from test scripts
    public function testFinSubArray(a:Array) : Array {
        this.finSubSetArray(a);
        return this.finSubGetArray();
    }

   

 }

public class PubExtDefaultImplDefDefPubAccessor{
var s = new PubExtDefaultImplDefDefPub();
var i:DefaultInt=s;
var j:DefaultIntDef=s;

public function acciiGetBoolean():Boolean{return i.iGetBoolean();}
public function acciiGetNumber():Number{return i.iGetNumber();}
public function accjiGetBoolean():Boolean{return s.DefaultIntDef::iGetBoolean();}
public function accjiGetNumber():Number{return s.DefaultIntDef::iGetNumber();}
}
}
