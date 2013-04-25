/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DynamicClass {

import  DynamicClass.*;

final class FinExtDynamicImplDefPubStat extends DynamicClass implements DefaultIntDef {

    
    public function iGetBoolean() : Boolean { return this.boolean; }
    public function iGetPubBoolean(): Boolean {return false;}

    // ************************************
    // access public static method of parent
    // from default method of sub class
    // ************************************

    function subGetArray() : Array { return getPubStatArray(); }
    function subSetArray(a:Array) { setPubStatArray(a); }

    public function testGetSubArray(a:Array) : Array {
        this.subSetArray(a);
        return this.subGetArray();
    }


    // ************************************
    // access public static method of parent
    // from public method of sub class
    // ************************************

    public function pubSubGetArray() : Array { return getPubStatArray(); }
    public function pubSubSetArray(a:Array) { setPubStatArray(a); }

    // ************************************
    // access public static method of parent
    // from private method of sub class
    // ************************************

    private function privSubGetArray() : Array { return getPubStatArray(); }
    private function privSubSetArray(a:Array) { setPubStatArray(a); }

    // function to test above from test scripts
    public function testPrivSubArray(a:Array) : Array {
        this.privSubSetArray(a);
        return this.privSubGetArray();
    }

    // ***************************************
    // access public static method of parent
    // from static method of sub class
    // ***************************************

    static function statSubGetArray() : Array { return getPubStatArray(); }
    static function statSubSetArray(a:Array) { setPubStatArray(a); }

    public static function testStatSubArray(a:Array) : Array {
        statSubSetArray(a);
        return statSubGetArray();
    }


    // ***************************************
    // access public static method of parent
    // from public static method of sub class
    // ***************************************

    public static function pubStatSubGetArray() : Array { return getPubStatArray(); }
    public static function pubStatSubSetArray(a:Array) { setPubStatArray(a); }

    // ***************************************
    // access public static method of parent
    // from private static method of sub class
    // ***************************************

    private static function privStatSubGetArray() : Array { return getPubStatArray(); }
    private static function privStatSubSetArray(a:Array) { setPubStatArray(a); }

    // function to test above test case
    public function testPrivStatSubArray(a:Array) : Array {
        privStatSubSetArray(a);
        return privStatSubGetArray();
    }

    // ***************************************
    // access public static property from
    // default method of sub class
    // ***************************************

    function subGetDPArray() : Array { return pubStatArray; }
    function subSetDPArray(a:Array) { pubStatArray = a; }

    public function testSubDPArray(a:Array):Array {
        subSetDPArray(a);
        return subGetDPArray();
    }

    public function testSubGetDPArray(a:Array) : Array {
        this.subSetDPArray(a);
        return this.subGetDPArray();
    }

    // ***************************************
    // access public static property from
    // public method of sub class
    // ***************************************

    public function pubSubGetDPArray() : Array { return pubStatArray; }
    public function pubSubSetDPArray(a:Array) { pubStatArray = a; }

    public function testPubSubDPArray(a:Array):Array {
        pubSubSetDPArray(a);
        return pubSubGetDPArray();
    }

    // ***************************************
    // access public static property from
    // private method of sub class
    // ***************************************

    private function privSubGetDPArray() : Array { return pubStatArray; }
    private function privSubSetDPArray(a:Array) { pubStatArray = a; }

    public function testPrivSubDPArray(a:Array):Array {
        privSubSetDPArray(a);
        return privSubGetDPArray();
    }


    // ***************************************
    // access public static property from
    // static method of sub class
    // ***************************************

    static function statSubGetDPArray() : Array { return pubStatArray; }
    static function statSubSetDPArray(a:Array) { pubStatArray = a; }

    public static function testStatSubDPArray(a:Array):Array {
        statSubSetDPArray(a);
        return statSubGetDPArray();
    }


    // ***************************************
    // access public static property from
    // public static method of sub class
    // ***************************************

    public static function pubStatSubGetDPArray() : Array { return pubStatArray; }
    public static function pubStatSubSetDPArray(a:Array) { pubStatArray = a; }

    // ***************************************
    // access public static property from
    // private static method of sub class
    // ***************************************

    private static function privStatSubGetDPArray() : Array { return pubStatArray; }
    private static function privStatSubSetDPArray(a:Array) { pubStatArray = a; }

    // function to test above

    public function testPrivStatSubDPArray(a:Array) {
        statArray = a;
        return statArray;
    }

    public function testPubSubGetDPArray(a:Array) : Array {
        this.pubSubSetDPArray(a);
        return this.pubSubGetDPArray();
    }
    public function testPrivSubGetDPArray(a:Array) : Array {
        this.privSubSetDPArray(a);
        return this.privSubGetDPArray();
    }
 }
    // Create an instance of the final class in the package.
    // This object will be used for accessing the methods created
    // within the sub class given above.
    var FINEXTDCLASS = new FinExtDynamicImplDefPubStat();

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
        public function testPubSubGetDPArray(a:Array) : Array { return FINEXTDCLASS.testPubSubGetDPArray(a); }

    // the private static method attribute
    public function testPrivSubGetDPArray(a:Array) : Array { return FINEXTDCLASS.testPrivSubGetDPArray(a); }

public class accFinExtDynamicImplDefPubStat{
    private var obj:FinExtDynamicImplDefPubStat = new FinExtDynamicImplDefPubStat();

      public function acciGetPubBoolean() : Boolean {return obj.iGetPubBoolean();}
}

}
