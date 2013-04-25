/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'RegExp_multiline.js';

/**
   Filename:     RegExp_multiline.js
   Description:  'Tests RegExps multiline property'

   Author:       Nick Lerissa
   Date:         March 12, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: multiline';


// First we do a series of tests with RegExp.multiline set to false (default value)

// (multiline == false) '123\n456'.match(/^4../)
Assert.expectEq (  "(multiline == false) '123\\n456'.match(/^4../)",
           null, '123\n456'.match(/^4../));

// (multiline == false) 'a11\na22\na23\na24'.match(/^a../g)
Assert.expectEq (  "(multiline == false) 'a11\\na22\\na23\\na24'.match(/^a../g)",
           String(['a11']), String('a11\na22\na23\na24'.match(/^a../g)));

// (multiline == false) 'a11\na22'.match(/^.+^./)
Assert.expectEq (  "(multiline == false) 'a11\na22'.match(/^.+^./)",
           null, 'a11\na22'.match(/^.+^./));

// (multiline == false) '123\n456'.match(/.3$/)
Assert.expectEq (  "(multiline == false) '123\\n456'.match(/.3$/)",
           null, '123\n456'.match(/.3$/));

// (multiline == false) 'a11\na22\na23\na24'.match(/a..$/g)
Assert.expectEq (  "(multiline == false) 'a11\\na22\\na23\\na24'.match(/a..$/g)",
           String(['a24']), String('a11\na22\na23\na24'.match(/a..$/g)));

// (multiline == false) 'abc\ndef'.match(/c$...$/)
Assert.expectEq (  "(multiline == false) 'abc\ndef'.match(/c$...$/)",
           null, 'abc\ndef'.match(/c$...$/));

// (multiline == false) 'a11\na22\na23\na24'.match(new RegExp('a..$','g'))
Assert.expectEq (  "(multiline == false) 'a11\\na22\\na23\\na24'.match(new RegExp('a..$','g'))",
           String(['a24']), String('a11\na22\na23\na24'.match(new RegExp('a..$','g'))));

// (multiline == false) 'abc\ndef'.match(new RegExp('c$...$'))
Assert.expectEq (  "(multiline == false) 'abc\ndef'.match(new RegExp('c$...$'))",
           null, 'abc\ndef'.match(new RegExp('c$...$')));


// (multiline == true) '123\n456'.match(/^4../)
Assert.expectEq (  "(multiline == true) '123\\n456'.match(/^4../m)",
           String(['456']), String('123\n456'.match(/^4../m)));

// (multiline == true) 'a11\na22\na23\na24'.match(/^a../g)
Assert.expectEq (  "(multiline == true) 'a11\\na22\\na23\\na24'.match(/^a../mg)",
           String(['a11','a22','a23','a24']), String('a11\na22\na23\na24'.match(/^a../mg)));

// (multiline == true) 'a11\na22'.match(/^.+^./)
//Assert.expectEq (  "(multiline == true) 'a11\na22'.match(/^.+^./)",
//                                    String(['a11\na']), String('a11\na22'.match(/^.+^./)));

// (multiline == true) '123\n456'.match(/.3$/)
Assert.expectEq (  "(multiline == true) '123\\n456'.match(/.3$/m)",
           String(['23']), String('123\n456'.match(/.3$/m)));

// (multiline == true) 'a11\na22\na23\na24'.match(/a..$/g)
Assert.expectEq (  "(multiline == true) 'a11\\na22\\na23\\na24'.match(/a..$/mg)",
           String(['a11','a22','a23','a24']), String('a11\na22\na23\na24'.match(/a..$/mg)));

// (multiline == true) 'a11\na22\na23\na24'.match(new RegExp('a..$','g'))
Assert.expectEq (  "(multiline == true) 'a11\\na22\\na23\\na24'.match(new RegExp('a..$','mg'))",
           String(['a11','a22','a23','a24']), String('a11\na22\na23\na24'.match(new RegExp('a..$','mg'))));

// (multiline == true) 'abc\ndef'.match(/c$....$/)
//Assert.expectEq (  "(multiline == true) 'abc\ndef'.match(/c$.+$/)",
//                                    'c\ndef', String('abc\ndef'.match(/c$.+$/)));

