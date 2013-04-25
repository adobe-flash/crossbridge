/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// NOTE: This source file is provided for reference by humans, not for
// use by the test infrastructure.  See var explanation below.  If you
// attempt to compile this file with a version of the builtins were
// generated after the JSON builtin landed, then you are likely to
// encounter an error due to the duplicate class definition below.
// This is *deliberate*; in fact, it is the whole point of the test.
// You should probably not be attempting to compile this or
// versioned_useoldjson_helper.as in the first place, but instead
// directly use the .abc's that were checked into the repository.

// Deliberately defining this in the unnamed package since that
// is where the new SWF_13 JSON is going.
package {
    public class JSON {
        public static function encode(x:*):String {
            return "legacyEncodeOutput";
        }
    }
}

package versioned_defjson {
    var explanation =
"The point of this test is to ensure that old content that\
 defines a JSON class in one file and uses that class in\
 another file all continues to run properly when run\
 in a dynamic context before version SWF_13.\
 \
 This is the file that defines a JSON class in the unnamed\
 package that is incompatible with the one from version SWF_13\
 of the builtins.  It uses its definition of JSON in another \
 package, \"versioned_defjson\", mostly to illustrate that such \
 a usage in the same file can present a _false_positive_ for \
 testing: such a usage can succeed even if the version guard \
 is absent in the builtins and thus such a test within one file\
 on its own is inadequate.  The real test that the versioning \
 support has been implemented is the import of JSON from this \
 file into another file, \"versioned_useoldjson_helper.as\".\
 \
 The secondary package in this file is named \"versioned_defjson\";\
 it provides two public functions: 'samefile_call_stringify' and\
 'samefile_call_encode', each of which are simple wrappers around\
 attempts to invoke the corresponding methods in JSON (but note that\
 one and only one of the calls should ever succeed)."

    public function samefile_call_stringify(a) {
        return JSON.stringify(a);
    }
    public function samefile_call_encode(a) {
        return JSON.encode(a);
    }
}
