/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

class MyClass {
    function MyClass() {}
    include "includes/op_add.as";
}



var obj:MyClass = new MyClass();
obj.runner();


} // package

// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Class level functions";
var DESC = "Include all of the includes/* float functions at the class level";

