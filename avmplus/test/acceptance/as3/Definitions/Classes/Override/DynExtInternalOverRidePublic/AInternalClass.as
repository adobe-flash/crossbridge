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
        // public methods
        // *****************
        public function getArray() : Array { return array; }
        public function getBoolean() : Boolean { return boolean; }
        public function getDate() : Date { return date; }
        public function getFunction() : Function { return myFunction; }
        public function getMath() : Math { return math; }
        public function getNumber() : Number { return number; }
        public function getObject() : Object { return object; }
        public function getString() : String { return string; }

        public function setArray( a:Array ) { array = a; }
        public function setBoolean( b:Boolean ) { boolean = b; }
        public function setDate( d:Date ) { date = d; }
        public function setFunction( f:Function ) { myFunction = f; }
        public function setMath( m:Math ) { math = m; }
        public function setNumber( n:Number ) { number = n; }
        public function setObject( o:Object ) { object = o; }
        public function setString( s:String ) { string = s; }

    }

}
