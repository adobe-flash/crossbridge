/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 706402
// -- begin test
package
{
    function f(x:Namespace = null)
    {
        return x;
    }
}
// -- end test

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "ASC rejects null as a default value for a namespace parameter.";


Assert.expectEq("Trivially true", true, true);


