/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

// var SECTION = "4.4";
// var VERSION = "AS3";
// var TITLE   = "The float class object called as a constructor";


/*
4.4 The 'float' class object called as a constructor
When the float class object is called as part of a new expression it is a constructor: it it returns a float
value.
*/

Assert.expectError("Float constructor, with more than 1 arg", Utils.ARGUMENTERROR+1063,  function(){ var float_wrongargs = new float(3.14f, 1.0f); });


