/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;
import flash.utils.Dictionary;
import flash.net.*;
import flash.utils.ObjectEncoding;
import avmplus.File;
import com.adobe.test.Assert;

// var SECTION = "AMFSerialization";
// var VERSION = "as3";
// var TITLE   = "test AMF Serialization of float and float4";


class UserDefClassAllTypes {
    //float
    public var flt_Float:float;
    //float4
    public var flt_Float4:float4;

    public function UserDefClassAllTypes() {
        // constructor code
    }

    public function Init():void {
        flt_Float = -12.375f;
        flt_Float4 = float4(-1.111f, 1.1f, 2.2f, 3.122f);
    }

}


//Register User Defined Classes
flash.net.registerClassAlias("UserDefClassAllTypes", UserDefClassAllTypes);

var userdef_Obj:UserDefClassAllTypes = new UserDefClassAllTypes();
userdef_Obj.Init();
//Array - Index : Mixed Value: Mixed (WithObject)
var arr_MixMixObj_Obj:Array = new Array();
arr_MixMixObj_Obj[3.14f] = 3.114f;
arr_MixMixObj_Obj["float4"] = float4(-1.111f, 1.1f, 2.2f, 3.122f);
testArrayTypes("Array Index:Mixed Value:Mixed" , arr_MixMixObj_Obj);

//Dictionary Key: String - Value: String
var dict_StrStr_Obj:Dictionary = new Dictionary();
dict_StrStr_Obj[124.22f] = float.MAX_VALUE;
dict_StrStr_Obj["float4"] = float4(-1.111f, 1.1f, 2.2f, 3.122f);
testDictionaryTypes("Dictionary Value: String", dict_StrStr_Obj);

var result:float = float();
buildBAnCallAddTC("Float constructor with no args", result);

var null_float:float = null;
buildBAnCallAddTC("Float constructor, with 'null' arg", null_float);

var boolean_float:float = true;
buildBAnCallAddTC("Float constructor, with 'boolean' arg", boolean_float);

var dble_float:float = 3.14;
buildBAnCallAddTC("Float constructor, with 'double' arg", dble_float);

var int_float:float = new float(3);
buildBAnCallAddTC("Float constructor, with 'int' arg", int_float);

var string_float:float = new float("3.14");
buildBAnCallAddTC("Float constructor, with 'String' arg", string_float);

var literal_float:float = new float(3.14f);
buildBAnCallAddTC("Float constructor, with 'FloatLiteral' arg", literal_float);

buildBAnCallAddTC("Float Max Value", float.MAX_VALUE);
buildBAnCallAddTC("Float Min Value", float.MIN_VALUE);
testNaN("Float NaN", float.NaN);
buildBAnCallAddTC("Float Positive Infinity", float.POSITIVE_INFINITY);
buildBAnCallAddTC("Float Negative Infinity", float.NEGATIVE_INFINITY);
buildBAnCallAddTC("Float base of natural logarithms", float.E);
buildBAnCallAddTC("Float base of natural logarithm of 10", float.LN10);
buildBAnCallAddTC("Float base of natural logarithms of 2", float.LN2);
buildBAnCallAddTC("Float base of natural logarithms of e to the base 2", float.LOG2E);
buildBAnCallAddTC("Float PI", float.PI);
buildBAnCallAddTC("Float square root of 1/2", float.SQRT1_2);
buildBAnCallAddTC("Float square root of 2", float.SQRT2);


var f1:float = 3.1415f;
var f2:float = float.MAX_VALUE;
var f3:float = -0.00032f;
var f4:float = -12.375f;

var vecFlt:Vector.<float> = new Vector.<float>();
vecFlt.push(f1);
vecFlt.push(f2);
vecFlt.push(f3);
vecFlt.push(f4);
testVectorTypesFloat("Float Vector ", vecFlt);



var result_float4:float4 = float4();
buildBAnCallAddTC("Float4 with no args",result_float4);

var null_float4:float4 = float4(null);
buildBAnCallAddTC("Float4 as constructor, with 'null' arg",null_float4);

var boolean_float4:float4 = float4(true);
buildBAnCallAddTC("Float4 as constructor, with 'boolean' arg",boolean_float4);

var dble_float4:float4 = float4(3.14);
buildBAnCallAddTC("Float4 as constructor, with 'double' arg",dble_float4);

var int_float4:float4 = float4(3);
buildBAnCallAddTC("Float4 as constructor, with 'int' arg",int_float4);

var string_float4:float4 = new float4("3.14");
buildBAnCallAddTC("Float4 as constructor, with 'String' arg",string_float4);

var literal_float4:float4 = float4(3.14f);
buildBAnCallAddTC("Float4 as constructor, with 'FloatLiteral' arg",literal_float4);

var flt4:float4 = float4(-0.00032f, -0.002012f, -0.1232f, 12445.1234f);
buildBAnCallAddTC("Float4 as a constructor with float4 arg",flt4);

buildBAnCallAddTC("Float4 Max Value", float4.MAX_VALUE);
buildBAnCallAddTC("Float4 Min Value", float4.MIN_VALUE);
testNaN("Float NaN", float.NaN);
buildBAnCallAddTC("Float4 Positive Infinity", float4.POSITIVE_INFINITY);
buildBAnCallAddTC("Float4 Negative Infinity", float4.NEGATIVE_INFINITY);
buildBAnCallAddTC("Float4 base of natural logarithms", float4.E);
buildBAnCallAddTC("Float4 base of natural logarithm of 10", float4.LN10);
buildBAnCallAddTC("Float4 base of natural logarithms of 2", float4.LN2);
buildBAnCallAddTC("Float4 base of natural logarithms of e to the base 2", float4.LOG2E);
buildBAnCallAddTC("Float4 base of natural logarithms of e to the base 10", float4.LOG10E);
buildBAnCallAddTC("Float4 PI", float4.PI);
buildBAnCallAddTC("Float4 square root of 1/2", float4.SQRT1_2);
buildBAnCallAddTC("Float4 square root of 2", float4.SQRT2);

var flt4_1:float4 = float4(-1f, 1f, 1f, 1f);
var flt4_2:float4 = float4(-1f, 1.1f, 2.2f, 3.13f);
var flt4_3:float4 = float4(-1.111f, 1.1f, 2.2f, 3.122f);
var flt4_4:float4 = float4(9999.596f,1298.296f,-128.596f,-112.596f)
var flt4_4:float4 = float4(201.334f,123.2236f,-101.111f,-102.121f)


var vecFlt_flt4:Vector.<float4> = new Vector.<float4>();
vecFlt_flt4.push(flt4_1);
vecFlt_flt4.push(flt4_2);
vecFlt_flt4.push(flt4_3);
vecFlt_flt4.push(flt4_4);
testVectorTypesFloat4("Float Vector ", vecFlt_flt4);


function buildBAnCallAddTC(strTitle:String, input:*)
{
    var ba_test_Obj:ByteArray = new ByteArray();
    ba_test_Obj.writeObject(input);
    ba_test_Obj.position = 0;
    Assert.expectEq(strTitle, input, ba_test_Obj.readObject());
    ba_test_Obj.position = 0;
    Assert.expectEq("Type check" + strTitle, typeof input, typeof ba_test_Obj.readObject());
}

function testNaN(strTitle:String, input:Number)
{
    var ba_test_Obj:ByteArray = new ByteArray();
    ba_test_Obj.writeObject(input);
    ba_test_Obj.position = 0;

    Assert.expectEq(strTitle, isNaN(input), isNaN(ba_test_Obj.readObject()));
    ba_test_Obj.position = 0;
    Assert.expectEq("Type check" + strTitle, typeof input, typeof ba_test_Obj.readObject());
}

function testArrayTypes(strTitle:String, input:Array)
{
    var ba_test_Obj:ByteArray = new ByteArray();
    ba_test_Obj.writeObject(input);
    ba_test_Obj.position = 0;
    var actual:Array = ba_test_Obj.readObject();
    compareArrays(strTitle, input, actual);
}

function compareArrays(strTitle:String, input:Array, actual:Array)
{
    var propInput:Array = new Array();
    for (var p:* in input) {
        propInput.push(p);
    }
    propInput = propInput.sort();


    var propActual:Array = new Array();
    for (var p1:* in input) {
        propActual.push(p1);
    }
    propActual = propActual.sort();


    for each (var prop:* in propInput)
    {
        if(input[prop] is UserDefClassAllTypes && actual[prop] is UserDefClassAllTypes)
        {
            compareUserDefTypes(strTitle, input[prop], actual[prop]);
        }
        else
        {
            Assert.expectEq(strTitle + " value check EXP vs ACT: " + prop, input[prop], actual[prop]);
        }
    }
    for each (var prop1:* in propActual)
    {
        if(input[prop1] is UserDefClassAllTypes && actual[prop1] is UserDefClassAllTypes)
        {
            compareUserDefTypes(strTitle, input[prop1], actual[prop1]);
        }
        else
        {
            Assert.expectEq(strTitle + " value check ACT vs EXP: " + prop1, actual[prop1], input[prop1]);
        }
    }
}

function testVectorTypesFloat(strTitle:String, input:Vector.<float>)
{
    var ba_test_Obj:ByteArray = new ByteArray();
    ba_test_Obj.writeObject(input);
    ba_test_Obj.position = 0;
    compareVectorsFloat(strTitle, input, ba_test_Obj.readObject());
}

function compareVectorsFloat(strTitle:String, input:Vector.<float>, actual:Vector.<float>)
{
    var inputlen:int = input.length;
    var actuallen:int = actual.length;
    Assert.expectEq(strTitle + " Vector Len ", inputlen, actuallen);
    if(inputlen == actuallen)
    {
        for (var i:int; i < inputlen; i++)
        {
            Assert.expectEq(strTitle, input[i], actual[i]);
        }
    }
}


function testVectorTypesFloat4(strTitle:String, input:Vector.<float4>)
{
    var ba_test_Obj:ByteArray = new ByteArray();
    ba_test_Obj.writeObject(input);
    ba_test_Obj.position = 0;
    compareVectorsFloat4(strTitle, input, ba_test_Obj.readObject());
}

function compareVectorsFloat4(strTitle:String, input:Vector.<float4>, actual:Vector.<float4>)
{
    var inputlen:int = input.length;
    var actuallen:int = actual.length;

    Assert.expectEq(strTitle + " Vector Len ", inputlen, actuallen);
    if(inputlen == actuallen)
    {
        for (var i:int; i < inputlen; i++)
        {
            Assert.expectEq(strTitle, input[i], actual[i]);
        }
    }
}


function testDictionaryTypes(strTitle:String, input:Dictionary)
{
    var ba_test_Obj:ByteArray = new ByteArray();
    ba_test_Obj.writeObject(input);
    ba_test_Obj.position = 0;
    compareDictionary(strTitle, input, ba_test_Obj.readObject());
}

function compareDictionary(strTitle:String, input:Dictionary, actual:Dictionary)
{

    var propInput:Array = new Array();
    for (var p:* in input)
    {
        propInput.push(p);
    }

    propInput = propInput.sort();

    for each (var prop:* in propInput)
    {
        if (input[prop] is float)
        {
            Assert.expectEq(strTitle + " float ", input[prop], actual[prop]);
        }
        else if (input[prop] is float4)
        {
            Assert.expectEq(strTitle + " float4 ", input[prop], actual[prop]);
        }
    }
}


function testUserDefTypes(strTitle:String, input:UserDefClassAllTypes)
{
    var ba_test_Obj:ByteArray = new ByteArray();
    ba_test_Obj.writeObject(input);
    ba_test_Obj.position = 0;
    compareUserDefTypes(strTitle, input, ba_test_Obj.readObject());
}

function compareUserDefTypes(strTitle:String, input:UserDefClassAllTypes, actual:UserDefClassAllTypes)
{
    Assert.expectEq(strTitle  + " User Defined Float " , input.flt_Float, actual.flt_Float);
    Assert.expectEq(strTitle  + " User Defined Float4 " , input.flt_Float4, actual.flt_Float4);
}

