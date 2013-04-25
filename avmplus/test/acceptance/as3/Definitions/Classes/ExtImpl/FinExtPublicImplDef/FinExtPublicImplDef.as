/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package PublicClass {

import PublicClass.*;

final class FinExtPublicImplDef extends PublicClass implements DefaultIntDef {

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

      public function iGetPubBoolean(): Boolean {return false;}


    // ****************************************
    // define a default defintion from
    // default interface 2 as a default method
    // ****************************************

    function iGetNumber() : Number { return this.number; }

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

    // ************************************
    // access default method of parent
    // from final method of sub class
    // ************************************

    final function finSubGetArray() : Array { return this.getArray(); }
    final function finSubSetArray(a:Array) { this.setArray(a); }

    // function to test above from test scripts
    public function testFinSubArray(a:Array) : Array {
        this.finSubSetArray(a);
        return this.finSubGetArray();
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

    // Create an instance of the final class in the package.
    // This object will be used for accessing the methods created
    // within the sub class given above.
    var FINEXTDCLASS = new FinExtPublicImplDef();

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

public class accFinExtPublicImplDef{
    private var obj:FinExtPublicImplDef = new FinExtPublicImplDef();

      public function acciGetPubBoolean() : Boolean {return obj.iGetPubBoolean();}
}

}
