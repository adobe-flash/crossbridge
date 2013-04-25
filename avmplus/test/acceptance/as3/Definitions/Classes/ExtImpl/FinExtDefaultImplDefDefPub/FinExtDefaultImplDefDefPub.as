/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DefaultClass {

import DefaultClass.*;

final class FinExtDefaultImplDefDefPub extends DefaultClass implements DefaultIntDef, DefaultInt {

    // ****************************************
    // define a default definition from
    // default interface as a default method
    // ****************************************

    public function iGetBoolean() : Boolean { return this.boolean; }
    
    // ****************************************
    // define a public defintion from
    // default interface as a default method
    // ****************************************

    public function iGetPubBoolean(): Boolean {return false;}

    // ****************************************
    // define a default definition from
    // default interface 2 as a default method
    // ****************************************

    public function iGetNumber() : Number { return 5; }
    
    // ****************************************
    // define a public defintion from
    // default interface 2 as a default method
    // ****************************************

    public function iGetPubNumber() : Number { return 5; }

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

    // Create an instance of the final class in the package.
    // This object will be used for accessing the methods created
    // within the sub class given above.
    var FINEXTDCLASS = new FinExtDefaultImplDefDefPub();
        
    // Create a series of public functions that call the methods.
    // The same names can be used as used in the class; because we are now
    // outside of the class definition.

    // The default method of the sub class.
    public function setPubArray( a:Array )  { return FINEXTDCLASS.setPubArray( a ); }
    public function setPubBoolean( a:Boolean )  { return FINEXTDCLASS.setPubBoolean( a ); }
    public function setPubDate( a:Date )  { return FINEXTDCLASS.setPubDate( a ); }
    public function setPubFunction( a:Function )  { return FINEXTDCLASS.setPubFunction( a ); }
    public function setPubNumber( a:Number )  { return FINEXTDCLASS.setPubNumber( a ); }
    public function setPubString( a:String )  { return FINEXTDCLASS.setPubString( a ); }
    public function setPubObject( a:Object )  { return FINEXTDCLASS.setPubObject( a ); }

    // The dynamic method of the sub class.
    public function testGetSetBoolean( a:Array )  { FINEXTDCLASS.testGetSetBoolean( a ); }
    public function testPubGetSetBoolean( a:Array )  { FINEXTDCLASS.testPubGetSetBoolean( a ); }


    // The public method of the sub class.
    public function testGetSubArray( a:Array )  { return FINEXTDCLASS.testGetSubArray( a ); }
    public function pubSubSetArray( a:Array )  { return FINEXTDCLASS.pubSubSetArray( a ); }
    public function pubSubGetArray()  { return FINEXTDCLASS.pubSubGetArray(  ); }

    // The private method of the sub class. Only one is used as we need to call only the
    // test function, which in turn calls the actual private methods, as within the class
    // we can access the private methods; but not outside of the class.
    public function testPrivSubArray( a:Array ) : Array  { return FINEXTDCLASS.testPrivSubArray( a ); }

    // The default property being accessed by the different method attributes.
    // The default method attribute.
    public function pubSubSetDPArray( a:Array ) { return FINEXTDCLASS.pubSubSetDPArray(a); }
    public function testSubGetDPArray(a:Array) : Array { return FINEXTDCLASS.testSubGetDPArray(a); }


    // the private static method attribute
    public function testPrivSubGetDPArray(a:Array) : Array { return FINEXTDCLASS.testPrivSubGetDPArray(a); }

public class accFinExtDefaultImplDefDefPub{
    private var obj:FinExtDefaultImplDefDefPub = new FinExtDefaultImplDefDefPub();
                    var i:DefaultInt = obj;
                    var j:DefaultIntDef=obj;
      public function acciGetPubBoolean() : Boolean {return obj.iGetPubBoolean();}
      public function acciGetPubNumber() : Number { return obj.iGetPubNumber(); }
      public function acciiGetBoolean():Boolean{return i.iGetBoolean();}
      public function acciiGetNumber():Number{return i.iGetNumber();}
      public function accjiGetBoolean():Boolean {return obj.DefaultIntDef::iGetBoolean();}
      public function accjiGetNumber():Number{return obj.DefaultIntDef::iGetNumber();}
}


}
