/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
package PublicClassPackage {

  public class PublicClass {

    var array:Array;
    var boolean:Boolean;
    var date:Date;
    var myFunction:Function;
    var math:Math;
    var number:Number;
    var object:Object;
    var string:String;


    // ****************
    // constructor
    // ****************

    function PublicClass() {
    }

    // *****************
    // protected methods
    // *****************
    protected function getArray() : Array { return array; }
    protected function getBoolean() : Boolean { return boolean; }
    protected function getDate() : Date { return date; }
    protected function getFunction() : Function { return myFunction; }
    protected function getMath() : Math { return math; }
    protected function getNumber() : Number { return number; }
    protected function getObject() : Object { return object; }
    protected function getString() : String { return string; }

    protected function setArray( a:Array ) { array = a; }
    protected function setBoolean( b:Boolean ) { boolean = b; }
    protected function setDate( d:Date ) { date = d; }
    protected function setFunction( f:Function ) { myFunction = f; }
    protected function setMath( m:Math ) { math = m; }
    protected function setNumber( n:Number ) { number = n; }
    protected function setObject( o:Object ) { object = o; }
    protected function setString( s:String ) { string = s; }

  }
}
