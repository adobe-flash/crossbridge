/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'RegExp_lastIndex.js';

/**
   Filename:     RegExp_lastIndex.js
   Description:  'Tests RegExps lastIndex property'

   Author:       Nick Lerissa
   Date:         March 17, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: lastIndex';
var BUGNUMBER="123802";


// re=/x./g; re.lastIndex=4; re.exec('xyabcdxa');
re=/x./g;
re.lastIndex=4;
Assert.expectEq (  "re=/x./g; re.lastIndex=4; re.exec('xyabcdxa')",
           'xa', String(re.exec('xyabcdxa')));

// re.lastIndex
Assert.expectEq (  "re.lastIndex",
           8, re.lastIndex);

// re.exec('xyabcdef');
Assert.expectEq (  "re.exec('xyabcdef')",
           null, re.exec('xyabcdef'));

// re.lastIndex
Assert.expectEq (  "re.lastIndex",
           0, re.lastIndex);

// re.exec('xyabcdef');
Assert.expectEq (  "re.exec('xyabcdef')",
           'xy', String(re.exec('xyabcdef')));

// re.lastIndex=30; re.exec('123xaxbxc456');
re.lastIndex=30;
Assert.expectEq (  "re.lastIndex=30; re.exec('123xaxbxc456')",
           null, re.exec('123xaxbxc456'));

