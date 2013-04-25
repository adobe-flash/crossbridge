/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-329383.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 329383;
var summary = 'Math copysign issues';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

var inputs = [
  -Infinity,
  -10.01,
  -9.01,
  -8.01,
  -7.01,
  -6.01,
  -5.01,
  -4.01,
  -Math.PI,
  -3.01,
  -2.01,
  -1.01,
  -0.5,
  -0.49,
  -0.01,
  -0,
  0,
  +0,
  0.01,
  0.49,
  0.50,
  0,
  1.01,
  2.01,
  3.01,
  Math.PI,
  4.01,
  5.01,
  6.01,
  7.01,
  8.01,
  9.01,
  10.01,
  Infinity
  ];

var iinput;

for (iinput = 0; iinput < inputs.length; iinput++)
{
  var input = inputs[iinput];
  Assert.expectEq(summary + ': Math.round(' + input + ')',
                emulateRound(input),
                Math.round(input)
                );
}

Assert.expectEq(summary + ': Math.round(' + input + ')',
              isNaN(emulateRound(NaN)),
              isNaN(Math.round(NaN))
              );

function emulateRound(x)
{
  if (!isFinite(x) || x === 0) return x
    if (-0.5 <= x && x < 0) return -0
      return Math.floor(x + 0.5)
      }

var z;

z = Math.min(-0, 0);

Assert.expectEq(summary + ': Math.atan2(-0, -0)', -Math.PI, Math.atan2(z, z));
Assert.expectEq(summary + ': 1/-0', 1/z, -Infinity);

z = Math.max(-0, 0);

Assert.expectEq(summary + ': Math.atan2(0, 0)', 0 , Math.atan2(z, z));
Assert.expectEq(summary + ': 1/0', Infinity, 1/z );

