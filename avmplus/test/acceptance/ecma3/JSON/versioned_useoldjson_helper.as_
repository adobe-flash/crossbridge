/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// NOTE: This source file is provided for reference by humans, not for
// use by the test infrastructure.  See var explanation below and also
// see versioned_defjson_helper.as.  You should probably not be
// attempting to compile this or versioned_defjson_helper.as in the
// first place, but instead directly use the .abc's that were checked
// into the repository.

package versioned_useoldjson {

    var explanation =
"The point of this test is to ensure that old content that\
 defines a JSON class in one file and uses that class in\
 another file all continues to run properly when run\
 in a dynamic context before version SWF_13.\
 \
 This is the file that uses the JSON class that is defined\
 in versioned_defjson.as (and is incompatible with version\
 SWF_13 of the builtins).  The JSON there (defined in the\
 unnamed package) is a trivial class JSON that defines a single\
 unary method 'encode' that returns \"encodeOutput\".\
 \
 The secondary package in this file is named \"versioned_useoldjson\";\
 it provides two public functions: 'otherfile_call_stringify' and\
 'otherfile_call_encode' each of which are simple wrappers around\
 attempts to invoke the corresponding methods in JSON (but note that\
 one and only one of the calls should ever succeed)."

    public function otherfile_call_stringify(a) {
        return JSON.stringify(a);
    }

    public function otherfile_call_encode(a) {
        return JSON.encode(a);
    }
}
