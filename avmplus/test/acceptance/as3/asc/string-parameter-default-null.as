/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 706719

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "ASC misinterprets default <null> value as 'null' string value";


// -- begin test
class MyClass {
    public function MyClass(a:String=null)
    {
        var b:Object = a;
        Assert.expectEq("b==null", true, b==null);
        Assert.expectEq("b=='null'", false, b=="null");
    }
}
var foo:MyClass = new MyClass();
// -- end test

