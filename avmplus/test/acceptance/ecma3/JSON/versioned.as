/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "JSON";
// var VERSION = "AS3";
// var TITLE   = "JSON versioning tests";

// The point of this test is to ensure that old content that
// defines a JSON class in one file and uses that class in
// another file all continues to run properly when run
// in a dynamic context before version SWF_13.

// This is the file that drives the test.  It is designed
// so that it can be compiled by the current tool chain
// regardless of the current tool chain's version.
// The legacy content is kept in two .abc files that have
// been compiled with asc and a set of builtins before JSON
// was added to the builtins.


// versioned_defjson_helper.as_ and versioned_useoldjson_helper.as_ were
// both compiled with builtins _before_ JSON was added.
// to recompile the helper files sync to tr 6199
// rename the abc files to
// versioned_defjson_helper.abc_bin and versioned_useoldjson_helper.abc_bin

// the versioned.as.avm_args runs this test with:
//   -api SWF_12 ecma3/JSON/versioned_defjson_helper.abc_bin ecma3/JSON/versioned_useoldjson_helper.abc_bin

import versioned_defjson.samefile_call_stringify;
import versioned_defjson.samefile_call_encode;

import versioned_useoldjson.otherfile_call_stringify;
import versioned_useoldjson.otherfile_call_encode;

import com.adobe.test.Assert;

var callFailException;
var result;




function removeExceptionDetail(s:String) {
    var fnd=s.indexOf(" ");
    if (fnd>-1) {
        if (s.indexOf(':',fnd)>-1) {
                s=s.substring(0,s.indexOf(':',fnd));
        }
    }
    return s;
}

function sortObject(o:Object) {
    var keys=[];
    var key;
    for ( key in o ) {
        if (o[key]===undefined) {
           continue;
        }
        keys[keys.length]=key;
    }
    keys.sort();
    var ret="{";
    var value;
    for (var i in keys) {
        key=keys[i];
        value=o[key];
        if (value is String) {
            value='"'+value+'"';
        } else if (value is Array) {
            value='['+value+']';
        } else if (value is Object) {
        }
        ret += '"'+key+'":'+value+",";
    }
    ret=ret.substring(0,ret.length-1);
    ret+="}";
    return ret;
}



callFailException="no exception thrown";;
try {
    var a = {};
    samefile_call_stringify(a);
} catch (e) {
    callFailException=e.toString();
    callFailException=removeExceptionDetail(callFailException);
}
Assert.expectEq("user-defined JSON in same file not overridden by builtin stringify when < SWF_13.",
            "TypeError: Error #1006",
            callFailException);

try {
    var a = {};
    result = samefile_call_encode(a);
} catch (e) {
    result=e.toString();
}
Assert.expectEq("user JSON in same file not overridden by builtin lack of encode when < SWF_13.",
            "legacyEncodeOutput",
            result);

callFailException="no exception thrown";;
try {
    var a = {};
    otherfile_call_stringify(a);
} catch (e) {
    callFailException=e.toString();
     callFailException=removeExceptionDetail(callFailException);
}
Assert.expectEq("user-defined JSON in other file not overridden by builtin stringify when < SWF_13.",
            "TypeError: Error #1006",
            callFailException);

try {
    var a = {};
    result = otherfile_call_encode(a);
} catch (e) {
    result=e.toString();
}
Assert.expectEq("user JSON in other file not overridden by builtin lack of encode when < SWF_13.",
            "legacyEncodeOutput",
            result);

