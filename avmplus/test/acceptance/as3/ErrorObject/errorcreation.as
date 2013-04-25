/* -*- Mode: js; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.Dictionary;
import com.adobe.test.Assert;

// var SECTION = "";
// var VERSION = "";
// var TITLE = "Tests based on code coverage";


var testcases = getTestCases();

function getTestCases() {
    
    var array = new Array();
    var item = 0;

    caughtError:boolean = false;
    try {
        throw new DefinitionError("Ooops");
    } catch (err:DefinitionError) {
        // Caught the error
        caughtError=true;
    } finally {
        array[item++] = Assert.expectEq( "DefinitionError", true, caughtError );
    }


    caughtError:boolean = false;
    try {
        throw new SecurityError("Ooops");
    } catch (err:SecurityError) {
        // Caught the error
        caughtError=true;
    } finally {
        array[item++] = Assert.expectEq( "SecurityError", true, caughtError );
    }

    caughtError:boolean = false;
    try {
        throw new UninitializedError("Ooops");
    } catch (err:UninitializedError) {
        // Caught the error
        caughtError=true;
    } finally {
        array[item++] = Assert.expectEq( "UninitializedError", true, caughtError );
    }

    return ( array );

}
