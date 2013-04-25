/* -*- Mode: js; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "15.11.1 ErrorObject getStackTrace()";
// var VERSION = "";
// var TITLE = "Tests based on code coverage";


var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    // This is covered in other areas of the testsuite, but this is an explicit test
    // for code coverage.
    try {
        var result = "error";
        var n:Number = new Number(1);
        n.toPrecision(0);
    } catch (err) {
        if (err.getStackTrace() == null)
        {
            // Running without a debugger, to stackTrace() is returned as null
            result = "OK";
        }
        else if (err.getStackTrace().search("builtin::toPrecision")!=0)
        {
            /* Running with a debugger, so just make sure that the stack trace is as expected
               by looking for "builtin::toPrecision" in the string
                   atNumber$/_convert()
                   atNumber/http://adobe.com/AS3/2006/builtin::toPrecision()
                   atFunction/<anonymous>()
                   atglobal$init()
            */
            result = "OK";
        }

    } finally {
        array[item++] = Assert.expectEq( "getStackTrace result!='error'", true, result!='error' );
    }

    return ( array );
}
