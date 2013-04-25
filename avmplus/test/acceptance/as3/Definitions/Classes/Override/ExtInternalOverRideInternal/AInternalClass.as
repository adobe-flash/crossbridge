/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package InternalClassPackage {
    class InternalClass {
        internal var array:Array;
        internal var boolean:Boolean;
        internal var date:Date;
        internal var myFunction:Function;
        internal var math:Math;
        internal var number:Number;
        internal var object:Object;
        internal var string:String;

        // ****************
        // constructor
        // ****************
        function InternalClass() {}

        // *****************
        // internal methods
        // *****************
        internal function getArray() : Array { return array; }
        internal function getBoolean() : Boolean { return boolean; }
        internal function getDate() : Date { return date; }
        internal function getFunction() : Function { return myFunction; }
        internal function getMath() : Math { return math; }
        internal function getNumber() : Number { return number; }
        internal function getObject() : Object { return object; }
        internal function getString() : String { return string; }

        internal function setArray( a:Array ) { array = a; }
        internal function setBoolean( b:Boolean ) { boolean = b; }
        internal function setDate( d:Date ) { date = d; }
        internal function setFunction( f:Function ) { myFunction = f; }
        internal function setMath( m:Math ) { math = m; }
        internal function setNumber( n:Number ) { number = n; }
        internal function setObject( o:Object ) { object = o; }
        internal function setString( s:String ) { string = s; }

    }

}
