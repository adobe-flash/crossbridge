/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;
import avmshell.*;
import com.adobe.test.Assert;
import com.adobe.test.Utils;


// standard Assert.expectError function doesn't handle no-error well.
function ErrorTesting(desc:String, expectedErr:String, testFunc:Function)
{
    actualErr = "no error";
    try {
        testFunc();
    } catch (e) {
        actualErr = e;
    }
    Utils.grabError(actualErr, expectedErr);
    Assert.expectEq(desc, expectedErr, String(actualErr).substr(0, expectedErr.length));
}


const SUCCESS:String = "no error";
const FAILURE:String = "ArgumentError: Error #2012";

function expectSuccess(msg:String, cls:Class)
{
    ErrorTesting(msg, SUCCESS, function() { new cls; });
}

function expectFailure(msg:String, cls:Class)
{
    ErrorTesting(msg, FAILURE, function() { new cls; });
}


expectFailure("instantiate for construct=none", System);

class MySubclassOfAbstractBase2 extends avmshell.SubclassOfAbstractBase
{
}


expectFailure("instantiate for construct=abstract", AbstractBase);
expectSuccess("instantiate for native child of construct=abstract", NativeSubclassOfAbstractBase);
expectSuccess("instantiate for child of construct=abstract", SubclassOfAbstractBase);
expectSuccess("instantiate for grandchild of construct=abstract", MySubclassOfAbstractBase2);


class MySubclassOfRestrictedBase extends avmshell.RestrictedBase
{
}

class MySubclassOfRestrictedBase2 extends avmshell.SubclassOfRestrictedBase
{
}

class MySubclassOfRestrictedBase3 extends avmshell.NativeSubclassOfRestrictedBase
{
}

expectSuccess("instantiate for construct=restricted", RestrictedBase);
expectSuccess("instantiate for native child of construct=restricted", NativeSubclassOfRestrictedBase);
expectSuccess("instantiate for child of construct=restricted", SubclassOfRestrictedBase);
expectFailure("instantiate for external child of construct=restricted", MySubclassOfRestrictedBase);
expectSuccess("instantiate for external grandchild of construct=restricted", MySubclassOfRestrictedBase2);
expectSuccess("instantiate for external grandchild of native construct=restricted", MySubclassOfRestrictedBase3);

class MySubclassOfAbstractRestrictedBase extends avmshell.AbstractRestrictedBase
{
}

class MySubclassOfAbstractRestrictedBase2 extends MySubclassOfAbstractRestrictedBase
{
}

expectFailure("instantiate for construct=abstract-restricted", AbstractRestrictedBase);
expectSuccess("instantiate for native child of construct=abstract-restricted", NativeSubclassOfAbstractRestrictedBase);
expectSuccess("instantiate for child of construct=abstract-restricted", SubclassOfAbstractRestrictedBase);
expectFailure("instantiate for external child of construct=abstract-restricted", MySubclassOfAbstractRestrictedBase);
expectFailure("instantiate for external grandchild of construct=abstract-restricted", MySubclassOfAbstractRestrictedBase2);

ErrorTesting("instantiate for construct=check", "ArgumentError: Error #1001", function() { new CheckBase; });

expectFailure("instantiate for construct=native", avmshell.NativeBase);
expectFailure("instantiate for construct=native (pure AS3)", avmshell.NativeBaseAS3);


