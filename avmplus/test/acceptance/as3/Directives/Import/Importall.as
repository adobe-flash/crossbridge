/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Importall {

 public class PublicClass {

   var array:Array;
    var boolean:Boolean;
    var date:Date;
    var myFunction:Function;
    var math:Math;
    var number:Number;
    var object:Object;
    var string:String;
    //var simple:Simple;

    public var pubArray:Array;
    
    public var pubBoolean:Boolean;
    public var pubDate:Date;
    public var pubFunction:Function;
    public var pubMath:Math;
    public var pubNumber:Number;
    public var pubObject:Object;
    public var pubString:String;
    //public var pubSimple:Simple;

  
    // *******************
    // public methods
    // *******************

    public function setPubArray( a:Array ) { pubArray = a; }
    public function setPubBoolean( b:Boolean ) { pubBoolean = b; }
    public function setPubDate( d:Date ) { pubDate = d; }
    public function setPubFunction( f:Function ) { pubFunction = f; }
    public function setPubMath( m:Math ) { pubMath = m; }
    public function setPubNumber( n:Number ) { pubNumber = n; }
    public function setPubObject( o:Object ) { pubObject = o; }
    public function setPubString( s:String ) { pubString = s; }
    //public function setPubSimple( s:Simple ) { pubSimple = s; }

    public function getPubArray() : Array { return this.pubArray; }
    public function getPubBoolean() : Boolean { return this.pubBoolean; }
    public function getPubDate() : Date { return this.pubDate; }
    public function getPubFunction() : Function { return this.pubFunction; }
    public function getPubMath() : Math { return this.pubMath; }
    public function getPubNumber() : Number { return this.pubNumber; }
    public function getPubObject() : Object { return this.pubObject; }
    public function getPubString() : String { return this.pubString; }
    //public function getPubSimple() : Simple { return this.pubSimple; }

  }
}


// var SECTION = "Directives";                     // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";               // Version of JavaScript or ECMA
// var TITLE   = "Import all public names in a package";           // Provide ECMA section title or a description
var BUGNUMBER = "";



import Importall.*

import com.adobe.test.Assert;
var obj = new PublicClass();

var arr = new Array(1, 2, 3);
date = new Date(0);
func = new Function();
num = new Number();
str = new String("test");

// ********************************************
// access all public method
// ********************************************

Assert.expectEq( "obj.setPubArray(arr), obj.pubArray", arr, (obj.setPubArray(arr), obj.pubArray) );
Assert.expectEq( "obj.setPubBoolean(true), obj.pubBoolean", true, (obj.setPubBoolean(true), obj.pubBoolean) );
Assert.expectEq( "obj.setPubFunction(func), obj.pubFunction", func, (obj.setPubFunction(func), obj.pubFunction) );
Assert.expectEq( "obj.setPubNumber(num), obj.pubNumber", num, (obj.setPubNumber(num), obj.pubNumber) );
Assert.expectEq( "obj.setPubObject(obj), obj.pubObject", obj, (obj.setPubObject(obj), obj.pubObject) );
Assert.expectEq( "obj.setPubString(str), obj.pubString", str, (obj.setPubString(str), obj.pubString) );

// ********************************************
// access public property from outside
// the class
// ********************************************

Assert.expectEq( "Access public property from outside the class", arr, (obj.pubArray = arr, obj.pubArray) );


/*===========================================================================*/


              // displays results.
