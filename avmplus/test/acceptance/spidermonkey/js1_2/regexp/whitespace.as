/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'whitespace.js';

/**
   Filename:     whitespace.js
   Description:  'Tests regular expressions containing \f\n\r\t\v\s\S\ '

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: \\f\\n\\r\\t\\v\\s\\S ';



var non_whitespace = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()-+={[}]|\\:;'<,>./?1234567890" + '"';

// Removed \v after \t
var whitespace     = "\f\n\r\t ";
var whitespace_desc = "\\f\\n\\r\\t ";

// be sure all whitespace is matched by \s
Assert.expectEq ( 
           "(whitespace).match(new RegExp('\\s+'))",
           String([whitespace]), String(whitespace.match(new RegExp('\\s+'))));

// be sure all non-whitespace is matched by \S
Assert.expectEq ( 
           "'" + non_whitespace + "'.match(new RegExp('\\S+'))",
           String([non_whitespace]), String(non_whitespace.match(new RegExp('\\S+'))));

// be sure all non-whitespace is not matched by \s
Assert.expectEq ( 
           "'" + non_whitespace + "'.match(new RegExp('\\s'))",
           null, non_whitespace.match(new RegExp('\\s')));

// be sure all whitespace is not matched by \S
Assert.expectEq ( 
           "(whitespace).match(new RegExp('\\S'))",
           null, whitespace.match(new RegExp('\\S')));

var s = non_whitespace + whitespace;

// be sure all digits are matched by \s
Assert.expectEq ( 
           "'" + non_whitespace + whitespace_desc + "'.match(new RegExp('\\s+'))",
           String([whitespace]), String(s.match(new RegExp('\\s+'))));

s = whitespace + non_whitespace;

// be sure all non-whitespace are matched by \S
Assert.expectEq ( 
           "'" + whitespace_desc + non_whitespace + "'.match(new RegExp('\\S+'))",
           String([non_whitespace]), String(s.match(new RegExp('\\S+'))));

// '1233345find me345'.match(new RegExp('[a-z\\s][a-z\\s]+'))
Assert.expectEq (  "'1233345find me345'.match(new RegExp('[a-z\\s][a-z\\s]+'))",
           String(["find me"]), String('1233345find me345'.match(new RegExp('[a-z\\s][a-z\\s]+'))));

var i;
whitespace_desc = ['\\f','\\n','\\r','\\t','\\v',' '];
// be sure all whitespace characters match individually
for (i = 0; i < whitespace.length; ++i)
{
  s = 'ab' + whitespace.charAt(i) + 'cd';
  s_desc = 'ab' + whitespace_desc[i] + 'cd';
  Assert.expectEq ( 
         "'" + s_desc + "'.match(new RegExp('\\\\s'))",
         String([whitespace.charAt(i)]), String(s.match(new RegExp('\\s'))));
  Assert.expectEq ( 
         "'" + s_desc + "'.match(/\s/)",
         String([whitespace.charAt(i)]), String(s.match(/\s/)));
}
// be sure all non_whitespace characters match individually
for (i = 0; i < non_whitespace.length; ++i)
{
  s = '  ' + non_whitespace.charAt(i) + '  ';
  Assert.expectEq ( 
         "'" + s + "'.match(new RegExp('\\\\S'))",
         String([non_whitespace.charAt(i)]), String(s.match(new RegExp('\\S'))));
  Assert.expectEq ( 
         "'" + s + "'.match(/\S/)",
         String([non_whitespace.charAt(i)]), String(s.match(/\S/)));
}


