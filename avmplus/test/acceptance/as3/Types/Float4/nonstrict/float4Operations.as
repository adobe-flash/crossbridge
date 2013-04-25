/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;
import com.adobe.test.Assert;

// var TITLE =  "FLOAT4 TEST";
// var VERSION = "AS3";

var STEP = "initial";
var float4Test:Array = [];
var ft_count:int = 0;

function   EQ(expected:float4,actual:float4):void{
   if(expected!=actual)
       Assert.expectEq(   STEP+ " !FAILED! ", expected, actual);
   else
       Assert.expectEq(   STEP, "", "");// just print it

   STEP=">> "+STEP;
}

function APPROX_EQ(expected:float4,actual:float4):void{
    var epsilon = float4(0.0001f);
   var result:float = float4.max(expected,actual) - expected;
   if(result==0)
       Assert.expectEq(   STEP, "", "");
   else
   if( !float4.isGreater(result,epsilon) )
       Assert.expectEq(   STEP, "~", "~");
   else
       Assert.expectEq(   STEP+ " !FAILED! ", expected, actual);
   STEP=">> "+STEP;
}

function TestSection(section , initTests ){


   this.RUN = function(){
       SECTION = section;
       initTests();
   }
}

  interface IFloat4Ops {
    function add(value:float4):float4;
    function sub(value:float4):float4;
    function mul(value:float4):float4;
    function div(value:float4):float4;
    function mod(value:float4):float4;
    function uminus():float4;
    function uplus():float4;
    function Xyzzy(value:float4):float4; // X = multiplication. Get it? Multiplies with value.yzzy, does not teleport to debris room!
    function eq(value:float4):Boolean;
    function seq(value:float4):Boolean;
    function neq(value:float4):Boolean;
    function sneq(value:float4):Boolean;
  }
  class Flt4AnyOps implements IFloat4Ops
 {
    var _val;
    public function Flt4AnyOps(v){ _val=v;};
    public function add(value:float4):float4 { return _val+value;};
    public function sub(value:float4):float4 { return value-_val;};
    public function mul(value:float4):float4 { return _val*value;};
    public function div(value:float4):float4 { return value/_val;};
    public function mod(value:float4):float4 { return _val%value;};
    public function uminus():float4          { return -_val;};
    public function uplus():float4           { return +_val;};
    public function Xyzzy(value:float4):float4{ return _val*value.yzzy; } 
    public function eq(value:float4):Boolean  { return value==_val;};
    public function seq(value:float4):Boolean { return _val===value;};
    public function neq(value:float4):Boolean { return value!=_val;};
    public function sneq(value:float4):Boolean{ return _val!==value;};
 };
  class Flt4Flt4Ops implements IFloat4Ops
 {
    var _val:float4;
    public function Flt4Flt4Ops(v:float4){ _val=v;}
    public function add(value:float4):float4 { return value+_val;}
    public function sub(value:float4):float4 { return value-_val;}
    public function mul(value:float4):float4 { return value*_val;}
    public function div(value:float4):float4 { return value/_val;}
    public function mod(value:float4):float4 { return _val%value;}
    public function uminus():float4          { return -_val;}
    public function uplus():float4           { return +_val;}
    public function Xyzzy(value:float4):float4{ return _val*value.yzzy; }
    public function eq(value:float4):Boolean  { return _val==value;}
    public function seq(value:float4):Boolean { return value===_val;}
    public function neq(value:float4):Boolean { return _val!=value;}
    public function sneq(value:float4):Boolean{ return _val!==value;}
 };


function initFlt4AnyTests(){
   var obj:IFloat4Ops = new Flt4AnyOps(float4(3.14f,-3.14f,3.14f,-3.14f));
   STEP="Flt4AnyOps ADD";   EQ(float4(34.54f,-34.54f,3.14f,3.14f),obj.add( float4(31.4f,-31.4f, -0f,6.28f)));
   STEP="Flt4AnyOps ADD2";  EQ(float4(34.54f,-34.54f,3.14f,3.14f),obj.add( float4(31.4f,-31.4f, -0f,6.28f)));
   STEP="Flt4AnyOps SUB";   EQ(float4(28.26f,-28.26f,3.14f,0f), obj.sub(float4(31.4f,-31.4f,6.28f,-3.14f)));
   STEP="Flt4AnyOps SUB2";  EQ(float4(28.26f,-28.26f,3.14f,0f), obj.sub(float4(31.4f,-31.4f,6.28f,-3.14f)));
   STEP="Flt4AnyOps MUL";   EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.mul(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4AnyOps MUL2";  EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.mul(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4AnyOps DIV";   EQ(float4(10f,10f,-10f,-10f), obj.div(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4AnyOps DIV2";  EQ(float4(10f,10f,-10f,-10f), obj.div(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4AnyOps MOD";   EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.mod(float4(31.4f)));
   STEP="Flt4AnyOps MOD2";  EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.mod(float4(31.4f)));
   STEP="Flt4AnyOps UMINUS";EQ(float4(-3.14f,3.14f,-3.14f,3.14f), obj.uminus());
   STEP="Flt4AnyOps UMINU2";EQ(float4(-3.14f,3.14f,-3.14f,3.14f), obj.uminus());
   STEP="Flt4AnyOps UPLUS"; EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.uplus());
   STEP="Flt4AnyOps UPLUS2";EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.uplus());
   STEP="Flt4AnyOps EQ";    EQ(false, obj.eq(31.4f));EQ(true, obj.eq(float4(3.14f,-3.14f,3.14f,-3.14f)));
   STEP="Flt4AnyOps SEQ";   EQ(true, obj.seq(float4(3.14f,-3.14f,3.14f,-3.14f)));EQ(false, obj.seq(float4(3.14f,-3.14f,3.14f,3.14f)));
   STEP="Flt4AnyOps NEQ";   EQ(false, obj.neq(float4(3.14f,-3.14f,3.14f,-3.14f)));EQ(true, obj.neq(3.14f));
   STEP="Flt4AnyOps SNEQ";  EQ(true, obj.sneq(31.4f));EQ(false, obj.sneq(float4(3.14f,-3.14f,3.14f,-3.14f)));
   STEP="Flt4AnyOps Xyzzy"; EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.Xyzzy(float4(0f,31.4f,-31.4f,0f)));
   STEP="Flt4AnyOps Xyzzy2";EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.Xyzzy(float4(0f,31.4f,-31.4f,0f)));
}
function initFlt4Flt4Tests(){
   var obj:IFloat4Ops = new Flt4Flt4Ops(float4(3.14f,-3.14f,3.14f,-3.14f));
   STEP="Flt4Flt4Ops ADD";   EQ(float4(34.54f,-34.54f,3.14f,3.14f),obj.add( float4(31.4f,-31.4f, -0f,6.28f)));
   STEP="Flt4Flt4Ops ADD2";  EQ(float4(34.54f,-34.54f,3.14f,3.14f),obj.add( float4(31.4f,-31.4f, -0f,6.28f)));
   STEP="Flt4Flt4Ops SUB";   EQ(float4(28.26f,-28.26f,3.14f,0f), obj.sub(float4(31.4f,-31.4f,6.28f,-3.14f)));
   STEP="Flt4Flt4Ops SUB2";  EQ(float4(28.26f,-28.26f,3.14f,0f), obj.sub(float4(31.4f,-31.4f,6.28f,-3.14f)));
   STEP="Flt4Flt4Ops MUL";   EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.mul(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4Flt4Ops MUL2";  EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.mul(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4Flt4Ops DIV";   EQ(float4(10f,10f,-10f,-10f), obj.div(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4Flt4Ops DIV2";  EQ(float4(10f,10f,-10f,-10f), obj.div(float4(31.4f,-31.4f,-31.4f,31.4f)));
   STEP="Flt4Flt4Ops MOD";   EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.mod(float4(31.4f)));
   STEP="Flt4Flt4Ops MOD2";  EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.mod(float4(31.4f)));
   STEP="Flt4Flt4Ops UMINUS";EQ(float4(-3.14f,3.14f,-3.14f,3.14f), obj.uminus());
   STEP="Flt4Flt4Ops UMINU2";EQ(float4(-3.14f,3.14f,-3.14f,3.14f), obj.uminus());
   STEP="Flt4Flt4Ops UPLUS"; EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.uplus());
   STEP="Flt4Flt4Ops UPLUS2";EQ(float4(3.14f,-3.14f,3.14f,-3.14f), obj.uplus());
   STEP="Flt4Flt4Ops EQ";    EQ(false, obj.eq(31.4f));EQ(true, obj.eq(float4(3.14f,-3.14f,3.14f,-3.14f)));
   STEP="Flt4Flt4Ops SEQ";   EQ(true, obj.seq(float4(3.14f,-3.14f,3.14f,-3.14f)));EQ(false, obj.seq(float4(3.14f,-3.14f,3.14f,3.14f)));
   STEP="Flt4Flt4Ops NEQ";   EQ(false, obj.neq(float4(3.14f,-3.14f,3.14f,-3.14f)));EQ(true, obj.neq(3.14f));
   STEP="Flt4Flt4Ops SNEQ";  EQ(true, obj.sneq(31.4f));EQ(false, obj.sneq(float4(3.14f,-3.14f,3.14f,-3.14f)));
   STEP="Flt4Flt4Ops Xyzzy"; EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.Xyzzy(float4(0f,31.4f,-31.4f,0f)));
   STEP="Flt4Flt4Ops Xyzzy2";EQ(float4(98.596f,98.596f,-98.596f,-98.596f), obj.Xyzzy(float4(0f,31.4f,-31.4f,0f)));
}


float4Test[ft_count++] = new TestSection("\n === float4 op Any (float4) ===\n", initFlt4AnyTests);
float4Test[ft_count++] = new TestSection("\n === float4 op float4 ===\n", initFlt4Flt4Tests);

trace("Starting tests...");
for(var i:int=0;i<ft_count;i++){
   float4Test[i].RUN();
}
