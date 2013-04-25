/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package
{
    public class MyFloatClass
    {
        public static var pubclassvar1:float = 1;
        public static var pubclassvar2:float = 1f;
        public static var pubclassvar4:float = 1.25e-1f;
        public static var pubclassvar5:float = 0x00000400;
        public static var pubclassvar6:float = float.MAX_VALUE;
        public static var pubclassvar7:float = new float(1);
        public static var pubclassvar8:float = float(1);


        public var pubinstancevar1:float = 1;
        public var pubinstancevar2:float = 1f;
        public var pubinstancevar4:float = 1.25e-1f;
        public var pubinstancevar5:float = 0x00000400;
        public var pubinstancevar6:float = float.MAX_VALUE;
        public var pubinstancevar7:float = new float(1);
        public var pubinstancevar8:float = float(1);
    }
}

// var SECTION = "6.12";
// var VERSION = "AS3";
// var TITLE   = "The assignment operators augmented by float values";


AddStrictTestCase("static class float 1", 1f, MyFloatClass.pubclassvar1);
AddStrictTestCase("static class float 1f", 1f, MyFloatClass.pubclassvar2);
AddStrictTestCase("static class float 1.25e-1f", 0.125f, MyFloatClass.pubclassvar4);
AddStrictTestCase("static class float 0x00000400", 1024f, MyFloatClass.pubclassvar5);
AddStrictTestCase("static class float float.MAX_VALUE", float.MAX_VALUE, MyFloatClass.pubclassvar6);
AddStrictTestCase("static class float new float(1)", 1f, MyFloatClass.pubclassvar7);
AddStrictTestCase("static class float float(1)", 1f, MyFloatClass.pubclassvar8);

var myFloatClass:MyFloatClass = new MyFloatClass();
AddStrictTestCase("public instance float 1", 1f, myFloatClass.pubinstancevar1);
AddStrictTestCase("public instance float 1f", 1f, myFloatClass.pubinstancevar2);
AddStrictTestCase("public instance float 1.25e-1f", 0.125f, myFloatClass.pubinstancevar4);
AddStrictTestCase("public instance float 0x00000400", 1024f, myFloatClass.pubinstancevar5);
AddStrictTestCase("public instance float float.MAX_VALUE", float.MAX_VALUE, myFloatClass.pubinstancevar6);
AddStrictTestCase("public instance float new float(1)", 1f, myFloatClass.pubinstancevar7);
AddStrictTestCase("public instance float float(1)", 1f, myFloatClass.pubinstancevar8);

var var1:float = 1;
var var2:float = 1f;
var var4:float = 1.25e-1f;
var var5:float = 0x00000400;
var var6:float = float.MAX_VALUE;
var var7:float = new float(1);
var var8:float = float(1);
AddStrictTestCase("global var float 1", 1f, var1);
AddStrictTestCase("global var float 1f", 1f, var2);
AddStrictTestCase("global var float 1.25e-1f", 0.125f, var4);
AddStrictTestCase("global var float 0x00000400", 1024f, var5);
AddStrictTestCase("global var float float.MAX_VALUE", float.MAX_VALUE, var6);
AddStrictTestCase("global var float new float(1)", 1f, var7);
AddStrictTestCase("global var float float(1)", 1f, var8);
var var9 = var1;
AddStrictTestCase("float var set as another float var", 1f, var9);




