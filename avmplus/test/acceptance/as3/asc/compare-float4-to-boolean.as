/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 698365

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "Allow float4 to be compared to Boolean and Number";


// -- begin test

print(float4(1,1,1,1) == true);   // This caused ASC to crash

// -- end test

Assert.expectEq("Trivially true", true, true);

// -- same bug, but a run-time failure before, then a compile-time failure (strict mode) later

Assert.expectEq("Widening comparison", true, float4(1,1,1,1) == 1);

// -- end test

