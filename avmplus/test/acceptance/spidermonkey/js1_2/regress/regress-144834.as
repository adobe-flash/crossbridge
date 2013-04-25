/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/*
 *
 * Date:    05 July 2002
 * SUMMARY: Testing local var having same name as switch label inside function
 *
 * The code below crashed while compiling in JS1.1 or JS1.2
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=144834
 *
 */
//-----------------------------------------------------------------------------
gTestfile = 'regress-144834.js';
var BUGNUMBER = 144834;
var summary = 'Local var having same name as switch label inside function';

print(BUGNUMBER);
print(summary);


function RedrawSched()
{
  var MinBound;

  switch (i)
  {
  case MinBound :
  }
}

Assert.expectEq('Do not crash', 'No Crash', 'No Crash');
