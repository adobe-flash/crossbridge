/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=689243

import com.adobe.test.Assert;
import flash.utils.ByteArray;
import avmplus.System;

// On 64-bit systems this will normally crash in all build modes,
// though it's not a sure thing.

// On 32-bit systems this will corrupt the heap in ways not easily
// detected.  The GC invocation usually causes a crash; beyond that,
// we should get an assertion in Debug builds because we corrupt the
// poison before the object.  In release builds it's not so easy to
// say.

// In a system with the bug fix we should get a reliable exception and
// the state of the object should be unchanged.

// Really only works with the JIT, since otherwise 0xFFFFFFFF is
// converted to some boxed Number and a different path is taken.  So
// wrap the test in a function here to trigger jitting in simplistic
// (jit on first invocation) situations, but generally for
// future-proofing (OSR) the test should be set up for a -Ojit-only
// run.

var a:ByteArray = new ByteArray;

var exn = "No exception";
function f() {
   var res = null;
   var v:uint = 0xFFFFFFFF;
   try {
       a[v] = 0;
   }
   catch (e) {
       if (e != null)
           res = e.toString().match(/\w*Error: Error #\d+/);
       print(e);
   }
   if (res != null)
       exn = res[0];

   // Often this provokes a crash in the old, buggy code.
   System.forceFullCollection();
}
f();

// "MemoryError" prints just as "Error"
Assert.expectEq("Correct exception thrown",
           "Error: Error #1000",
           exn);

Assert.expectEq("ByteArray length did not change",
           0,
           a.length);

Assert.expectEq("ByteArray position did not change",
           0,
           a.position);

// MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=689243
