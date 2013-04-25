/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;
import com.adobe.test.Assert;

// var TITLE =  "FLOAT TEST";
// var VERSION = "AS3";

var STEP = "initial";
var floatTest:Array = [];
var ft_count:int = 0;

function   EQ(expected:float,actual:float):void{
   if(expected!=actual)
       Assert.expectEq(   STEP+ " !FAILED! ", expected, actual);
   else
       Assert.expectEq(   STEP, "", "");// just print it

   STEP=">> "+STEP;
}

function APPROX_EQ(expected:float,actual:float):void{
    var epsilon = 0.0001f;
   var result:float = expected>actual? expected - actual:actual-expected;
   if(result==0)
       Assert.expectEq(   STEP, "", "");
   else
   if(result<epsilon)
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

  interface IFloatOps {
    function add(value:float):float;
    function sub(value:float):float;
    function mul(value:float):float;
    function div(value:float):float;
    function mod(value:float):float;
    function uminus():float;
    function uplus():float;
   function gt(value:float):Boolean;
   function gte(value:float):Boolean;
   function lt(value:float):Boolean;
   function lte(value:float):Boolean;
   function eq(value:float):Boolean;
   function seq(value:float):Boolean;
   function neq(value:float):Boolean;
   function sneq(value:float):Boolean;
  }
  class FltAnyOps implements IFloatOps
 {
    var _val;
   public function FltAnyOps(v){ _val=v;};
    public function add(value:float):float { return _val+value;};
    public function sub(value:float):float { return value-_val;};
    public function mul(value:float):float { return _val*value;};
    public function div(value:float):float { return value/_val;};
    public function mod(value:float):float { return _val%value;};
    public function uminus():float         { return -_val;};
    public function uplus():float          { return +_val;};
   public function gt(value:float):Boolean  { return _val>value;};
   public function gte(value:float):Boolean { return value>=_val;};
   public function lt(value:float):Boolean  { return value<_val;};
   public function lte(value:float):Boolean { return _val<=value;};
   public function eq(value:float):Boolean  { return value==_val;};
   public function seq(value:float):Boolean { return _val===value;};
   public function neq(value:float):Boolean { return value!=_val;};
   public function sneq(value:float):Boolean{ return _val!==value;};
 };
  class FltFltOps implements IFloatOps
 {
    var _val:float;
   public function FltFltOps(v:float){ _val=v;}
    public function add(value:float):float { return value+_val;}
    public function sub(value:float):float { return _val-value;}
    public function mul(value:float):float { return value*_val;}
    public function div(value:float):float { return _val/value;}
    public function mod(value:float):float { return value%_val;}
    public function uminus():float         { return -_val;}
    public function uplus():float          { return +_val;}
   public function gt(value:float):Boolean  { return value>_val;}
   public function gte(value:float):Boolean { return _val>=value;}
   public function lt(value:float):Boolean  { return _val<value;}
   public function lte(value:float):Boolean { return value<=_val;}
   public function eq(value:float):Boolean  { return _val==value;}
   public function seq(value:float):Boolean { return value===_val;}
   public function neq(value:float):Boolean { return _val!=value;}
   public function sneq(value:float):Boolean{ return _val!==value;}
 };

  class FltNumOps implements IFloatOps
 {
    var _val:Number;
   public function FltNumOps(v:Number){ _val=v;}
    public function add(value:float):float { return _val+value;}
    public function sub(value:float):float { return value-_val;}
    public function mul(value:float):float { return _val*value;}
    public function div(value:float):float { return value/_val;}
    public function mod(value:float):float { return _val%value;}
    public function uminus():float         { return -_val;}
    public function uplus():float          { return +_val;}
   public function gt(value:float):Boolean  { return _val>value;}
   public function gte(value:float):Boolean { return value>=_val;}
   public function lt(value:float):Boolean  { return value<_val;}
   public function lte(value:float):Boolean { return _val<=value;}
   public function eq(value:float):Boolean  { return value==_val;}
   public function seq(value:float):Boolean { return _val===value;}
   public function neq(value:float):Boolean { return value!=_val;}
   public function sneq(value:float):Boolean{ return _val!==value;}
 };
  class FltIntOps implements IFloatOps
 {
    var _val:int;
   public function FltIntOps(v:int){ _val=v;}
    public function add(value:float):float { return value+_val;}
    public function sub(value:float):float { return _val-value;}
    public function mul(value:float):float { return value*_val;}
    public function div(value:float):float { return _val/value;}
    public function mod(value:float):float { return value%_val;}
    public function uminus():float         { return -_val;}
    public function uplus():float          { return +_val;}
   public function gt(value:float):Boolean  { return value>_val;}
   public function gte(value:float):Boolean { return _val>=value;}
   public function lt(value:float):Boolean  { return _val<value;}
   public function lte(value:float):Boolean { return value<=_val;}
   public function eq(value:float):Boolean  { return _val==value;}
   public function seq(value:float):Boolean { return value===_val;}
   public function neq(value:float):Boolean { return _val!=value;}
   public function sneq(value:float):Boolean{ return _val!==value;}
 };


function initFltAnyTests(){
   var obj:IFloatOps = new FltAnyOps(3.14f);
   STEP="FltAnyOps ADD";   EQ(34.54f, obj.add(31.4f));
   STEP="FltAnyOps SUB";   EQ(28.26f, obj.sub(31.4f));
   STEP="FltAnyOps MUL";   EQ(98.596f, obj.mul(31.4f));
   STEP="FltAnyOps DIV";   EQ(10f, obj.div(31.4f));
   STEP="FltAnyOps MOD";   EQ(3.14f, obj.mod(31.4f));
   STEP="FltAnyOps UMINUS";EQ(-3.14f, obj.uminus());
   STEP="FltAnyOps UPLUS"; EQ(3.14f, obj.uplus());
   STEP="FltAnyOps GT";    EQ(false, obj.gt(31.4f));EQ(false, obj.gt(3.14f));EQ(true, obj.gt(.314f));
   STEP="FltAnyOps GTE";   EQ(true, obj.gte(31.4f));EQ(true, obj.gte(3.14f));EQ(false, obj.gte(.314f));
   STEP="FltAnyOps LT";    EQ(false, obj.lt(31.4f));EQ(false, obj.lt(3.14f));EQ(true, obj.lt(.314f));
   STEP="FltAnyOps LTE";   EQ(true, obj.lte(31.4f));EQ(true, obj.lte(3.14f));EQ(false, obj.lte(.314f));
   STEP="FltAnyOps EQ";    EQ(false, obj.eq(31.4f));EQ(true, obj.eq(3.14f));
   STEP="FltAnyOps SEQ";    EQ(false, obj.seq(31.4f));EQ(true, obj.seq(3.14f));
   STEP="FltAnyOps NEQ";    EQ(true, obj.neq(31.4f));EQ(false, obj.neq(3.14f));
   STEP="FltAnyOps SNEQ";   EQ(true, obj.sneq(31.4f));EQ(false, obj.sneq(3.14f));
}
function initFltFltTests(){
   var obj:IFloatOps = new FltFltOps(3.14f);
   STEP="FltFltOps ADD";   EQ(34.54f, obj.add(31.4f));
   STEP="FltFltOps ADD2";  EQ(34.54f, obj.add(31.4f));
   STEP="FltFltOps SUB";   EQ(-28.26f, obj.sub(31.4f));
   STEP="FltFltOps SUB2";  EQ(-28.26f, obj.sub(31.4f));
   STEP="FltFltOps MUL";   EQ(98.596f, obj.mul(31.4f));
   STEP="FltFltOps MUL2";  EQ(98.596f, obj.mul(31.4f));
   STEP="FltFltOps DIV";   EQ(0.1f, obj.div(31.4f));
   STEP="FltFltOps DIV2";  EQ(0.1f, obj.div(31.4f));
   STEP="FltFltOps MOD";   EQ(3.13999867f, obj.mod(31.4f));
   STEP="FltFltOps MOD2";  EQ(3.13999867f, obj.mod(31.4f));
   STEP="FltFltOps UMINUS";EQ(-3.14f, obj.uminus());
   STEP="FltFltOps UMINU2";EQ(-3.14f, obj.uminus());
   STEP="FltFltOps UPLUS"; EQ(3.14f, obj.uplus());
   STEP="FltFltOps UPLUS2";EQ(3.14f, obj.uplus());
   STEP="FltFltOps GT";    EQ(true, obj.gt(31.4f));EQ(false, obj.gt(3.14f));EQ(false, obj.gt(.314f));
   STEP="FltFltOps GTE";   EQ(false, obj.gte(31.4f));EQ(true, obj.gte(3.14f));EQ(true, obj.gte(.314f));
   STEP="FltFltOps LT";    EQ(true, obj.lt(31.4f));EQ(false, obj.lt(3.14f));EQ(false, obj.lt(.314f));
   STEP="FltFltOps LTE";   EQ(false, obj.lte(31.4f));EQ(true, obj.lte(3.14f));EQ(true, obj.lte(.314f));
   STEP="FltFltOps EQ";    EQ(false, obj.eq(31.4f));EQ(true, obj.eq(3.14f));
   STEP="FltFltOps SEQ";    EQ(false, obj.seq(31.4f));EQ(true, obj.seq(3.14f));
   STEP="FltFltOps NEQ";    EQ(true, obj.neq(31.4f));EQ(false, obj.neq(3.14f));
   STEP="FltFltOps SNEQ";   EQ(true, obj.sneq(31.4f));EQ(false, obj.sneq(3.14f));
}

function initFltNumTests(){
   var obj:IFloatOps = new FltNumOps(3.14);
   STEP="FltNumOps ADD";   EQ(34.54f, obj.add(31.4f));
   STEP="FltNumOps SUB";   EQ(28.26f, obj.sub(31.4f));
   STEP="FltNumOps MUL";   EQ(98.596f, obj.mul(31.4f));
   STEP="FltNumOps DIV";   EQ(10f, obj.div(31.4f));
   STEP="FltNumOps MOD";   EQ(3.14f, obj.mod(31.4f));
   STEP="FltNumOps UMINUS";EQ(-3.14f, obj.uminus());
   STEP="FltNumOps UPLUS"; EQ(3.14f, obj.uplus());
   STEP="FltNumOps GT";    EQ(false, obj.gt(31.4f));EQ(false, obj.gt(3.14f));EQ(true, obj.gt(.314f));
   STEP="FltNumOps GTE";   EQ(true, obj.gte(31.4f));EQ(true, obj.gte(3.14f));EQ(false, obj.gte(.314f));
   STEP="FltNumOps LT";    EQ(false, obj.lt(31.4f));EQ(false, obj.lt(3.14f));EQ(true, obj.lt(.314f));
   STEP="FltNumOps LTE";   EQ(true, obj.lte(31.4f));EQ(true, obj.lte(3.14f));EQ(false, obj.lte(.314f));
   STEP="FltNumOps EQ";    EQ(false, obj.eq(31.4f));EQ(false, obj.eq(3.14f));
   STEP="FltNumOps SEQ";    EQ(false, obj.seq(31.4f));EQ(false, obj.seq(3.14f));
   STEP="FltNumOps NEQ";    EQ(true, obj.neq(31.4f));EQ(true, obj.neq(3.14f));
   STEP="FltNumOps SNEQ";   EQ(true, obj.sneq(31.4f));EQ(true, obj.sneq(3.14f));
}
function initFltAnyTests2(){
   var obj:IFloatOps = new FltAnyOps(3.14); /*NOTE: not float! */
   STEP="FltAnyOps ADD";   EQ(34.54f, obj.add(31.4f)); 
   STEP="FltAnyOps ADD2";  EQ(34.54f, obj.add(31.4f)); 
   STEP="FltAnyOps SUB";   EQ(28.26f, obj.sub(31.4f));
   STEP="FltAnyOps SUB2";  EQ(28.26f, obj.sub(31.4f));
   STEP="FltAnyOps MUL";   EQ(98.596f, obj.mul(31.4f));
   STEP="FltAnyOps MUL2";  EQ(98.596f, obj.mul(31.4f));
   STEP="FltAnyOps DIV";   EQ(10f, obj.div(31.4f));
   STEP="FltAnyOps DIV2";  EQ(10f, obj.div(31.4f));
   STEP="FltAnyOps MOD";   EQ(3.14f, obj.mod(31.4f));
   STEP="FltAnyOps MOD2";  EQ(3.14f, obj.mod(31.4f));
   STEP="FltAnyOps UMINUS";EQ(-3.14f, obj.uminus());
   STEP="FltAnyOps UMINU2";EQ(-3.14f, obj.uminus());
   STEP="FltAnyOps UPLUS"; EQ(3.14f, obj.uplus());
   STEP="FltAnyOps UPLUS2";EQ(3.14f, obj.uplus());
   STEP="FltAnyOps GT";    EQ(false, obj.gt(31.4f));EQ(false, obj.gt(3.14f));EQ(true, obj.gt(.314f));
   STEP="FltAnyOps GTE";   EQ(true, obj.gte(31.4f));EQ(true, obj.gte(3.14f));EQ(false, obj.gte(.314f));
   STEP="FltAnyOps LT";    EQ(false, obj.lt(31.4f));EQ(false, obj.lt(3.14f));EQ(true, obj.lt(.314f));
   STEP="FltAnyOps LTE";   EQ(true, obj.lte(31.4f));EQ(true, obj.lte(3.14f));EQ(false, obj.lte(.314f));
   STEP="FltAnyOps EQ";    EQ(false, obj.eq(31.4f));EQ(false, obj.eq(3.14f));
   STEP="FltAnyOps SEQ";    EQ(false, obj.seq(31.4f));EQ(false, obj.seq(3.14f));
   STEP="FltAnyOps NEQ";    EQ(true, obj.neq(31.4f));EQ(true, obj.neq(3.14f));
   STEP="FltAnyOps SNEQ";   EQ(true, obj.sneq(31.4f));EQ(true, obj.sneq(3.14f));
}


function initFltIntTests(){
   var result:float;
   var epsilon:float=0.0001f;
   var obj:IFloatOps = new FltIntOps(3);
   STEP="FltIntOps ADD";   EQ(34.4f, obj.add(31.4f));
   STEP="FltIntOps SUB";   EQ(-28.4f, obj.sub(31.4f));
   STEP="FltIntOps MUL";   EQ(94.2f,obj.mul(31.4f));
   STEP="FltIntOps DIV";   EQ(0.0955414013f, obj.div(31.4f));
   STEP="FltIntOps MOD";   result = 1.4f - obj.mod(31.4f);if(result<0) result=-result;
                EQ(true,result<epsilon);
   STEP="FltIntOps UMINUS";EQ(-3f, obj.uminus());
   STEP="FltIntOps UPLUS"; EQ(3f, obj.uplus());
   STEP="FltIntOps GT";    EQ(true, obj.gt(3.0001f));EQ(false, obj.gt(3f));EQ(false, obj.gt(2.999f));
   STEP="FltIntOps GTE";   EQ(false, obj.gte(3.0001f));EQ(true, obj.gte(3f));EQ(true, obj.gte(2.999f));
   STEP="FltIntOps LT";    EQ(true, obj.lt(3.0001f));EQ(false, obj.lt(3f));EQ(false, obj.lt(2.999f));
   STEP="FltIntOps LTE";   EQ(false, obj.lte(3.0001f));EQ(true, obj.lte(3f));EQ(true, obj.lte(2.999f));
   STEP="FltIntOps EQ";    EQ(false, obj.eq(3.0001f));EQ(true, obj.eq(3f));
   STEP="FltIntOps SEQ";    EQ(false, obj.seq(3.0001f));EQ(false, obj.seq(3f));
   STEP="FltIntOps NEQ";    EQ(true, obj.neq(3.0001f));EQ(false, obj.neq(3f));
   STEP="FltIntOps SNEQ";   EQ(true, obj.sneq(3.0001f));EQ(true, obj.sneq(3f));
}

function initFltAr(){
  STEP = "float-typed arithmetic with floats ";
  var x1:float, y1:float,z1:float;
  x1 = 3.1415f; y1=2.73f; z1=1.44f;
  x1 += y1*z1++; y1 -= (z1--)/(++x1);
  z1 = -x1---y1;
  x1 = x1 + z1 % +y1;

  APPROX_EQ(6.283240318f,x1);
  APPROX_EQ(2.427746773f,y1);
  APPROX_EQ(-10.50044727f,z1);

  var cmpres:String="FAIL";
  if(x1<y1)
      cmpres="x1<y1";
  else
  if(y1<=z1)
      cmpres="y1<=z1";
  else
  if(z1>x1)
      cmpres="z1>x1";
  else
  if(y1>=x1)
      cmpres="y1>=x1";
  else
  if(z1==y1)
      cmpres="z1==y1";
  else
  if(z1===x1)
      cmpres="z1===x1";
  else {
  x1 = y1++;
  z1 = --y1;
  if(z1!=x1)
      cmpres="x1!=z1";
  else
  if(x1!==z1)
      cmpres="x1!==z1";
  else
     cmpres="OK";
  }

  Assert.expectEq("float-typed comparisons with floats ", "OK",cmpres);

/*===========================================================================================================================*/

  STEP = "Any-typed arithmetic with floats ";
  var x2, y2,z2;
  x2 = 3.1415f; y2=2.73f; z2=1.44f;
  x2 += y2*z2++;   y2 -= (z2--)/(++x2);
  z2 = -x2---y2;
  x2 = x2 + z2 % +y2;

  APPROX_EQ(6.283240318f,x2);
  APPROX_EQ(2.427746773f,y2);
  APPROX_EQ(-10.50044727f,z2);

  Assert.expectEq("x2 type", "float",getQualifiedClassName(x2));
  Assert.expectEq("y2 type", "float",getQualifiedClassName(y2));
  Assert.expectEq("z2 type", "float",getQualifiedClassName(z2));

  cmpres"FAIL";
  if(x2<y2)
      cmpres="x2<y2";
  else
  if(y2<=z2)
      cmpres="y2<=z2";
  else
  if(z2>x2)
      cmpres="z2>x2";
  else
  if(y2>=x2)
      cmpres="y2>=x2";
  else
  if(z2==y2)
      cmpres="z2==y2";
  else
  if(z2===x2)
      cmpres="z2===x2";
  else {
  x2 = y2++;
  z2 = --y2;
  if(z2!=x2)
      cmpres="x2!=z2";
  else
  if(x2!==z2)
      cmpres="x2!==z2";
  else
     cmpres="OK";
  }

  Assert.expectEq("Any-typed comparisons with floats", "OK",cmpres);
/*===========================================================================================================================*/

  STEP = "Any-typed arithmetic with mixed types ";
  x2 = 3.1415; y2=2.73f; z2=1.44f;
  x2 += y2*z2++; y2 -= (z2--)/(++x2);
  z2 = -x2---y2;
  Assert.expectEq("x2 type", "Number",getQualifiedClassName(x2));
  Assert.expectEq("y2 type", "Number",getQualifiedClassName(y2));
  Assert.expectEq("z2 type", "Number",getQualifiedClassName(z2));
  /* convert to float */
  x1=x2;y1=y2;z1=z2;x2=x1;y2=y1;z2=z1;
  x2 = x2 + z2 % +y2;

  APPROX_EQ(6.283240318f,x2);
  APPROX_EQ(2.427746773f,y2);
  APPROX_EQ(-10.50044727f,z2);

  Assert.expectEq("x2 type", "float",getQualifiedClassName(x2));
  Assert.expectEq("y2 type", "float",getQualifiedClassName(y2));
  Assert.expectEq("z2 type", "float",getQualifiedClassName(z2));

  y2 = 2.427746773;
  cmpres"FAIL";
  if(x2<y2)
      cmpres="x2<y2";
  else
  if(y2<=z2)
      cmpres="y2<=z2";
  else
  if(z2>Number(x2))
      cmpres="z2>x2";
  else
  if(y2>=x2)
      cmpres="y2>=x2";
  else
  if(z2==y2)
      cmpres="z2==y2";
  else
  if(Number(z2)===x2)
      cmpres="z2===x2";
  else {
  x2 = float(y2++);
  z2 = float(--y2);
  if(z2!=Number(x2))
      cmpres="x2!=z2";
  else
  if(x2!==Number(z2) )
      cmpres="OK";
  else
     cmpres="x2!==z2";
  }

  Assert.expectEq("Any-typed comparisons with mixed types ", "OK",cmpres);


}

floatTest[ft_count++] = new TestSection("\n === float op Any (float) ===\n", initFltAnyTests);
floatTest[ft_count++] = new TestSection("\n === float op float ===\n", initFltFltTests);
floatTest[ft_count++] = new TestSection("\n === float op Number ===\n", initFltNumTests);
floatTest[ft_count++] = new TestSection("\n === float op Any (Number) ===\n", initFltAnyTests2);
floatTest[ft_count++] = new TestSection("\n === float op int ===\n", initFltIntTests);

floatTest[ft_count++] = new TestSection("\n === float arithmetic  ===\n", initFltAr);

trace("Starting tests...");
for(var i:int=0;i<ft_count;i++){
   floatTest[i].RUN();
}
