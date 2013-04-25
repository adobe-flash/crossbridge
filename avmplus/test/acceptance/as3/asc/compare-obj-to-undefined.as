/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 707661

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "Strict: Allow undefined to compare with other types";


// -- begin test

function f(o:Object = undefined)  // This was, and remains, allowed
{
    if (o == undefined)           // This was sometimes allowed, depended on CEE.  It is now allowed.
        print("Ho");
}

// -- end test

Assert.expectEq("Trivially true", true, true);

