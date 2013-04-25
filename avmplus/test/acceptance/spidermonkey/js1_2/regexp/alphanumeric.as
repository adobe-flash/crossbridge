/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'alphanumeric.js';

/**
   Filename:     alphanumeric.js
   Description:  'Tests regular expressions with \w and \W special characters'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: \\w and \\W';


var non_alphanumeric = "~`!@#$%^&*()-+={[}]|\\:;'<,>./?\f\n\r\t\v " + '"';
var alphanumeric     = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

// be sure all alphanumerics are matched by \w
Assert.expectEq ( 
           "'" + alphanumeric + "'.match(new RegExp('\\w+'))",
           String([alphanumeric]), String(alphanumeric.match(new RegExp('\\w+'))));

// be sure all non-alphanumerics are matched by \W
Assert.expectEq ( 
           "'" + non_alphanumeric + "'.match(new RegExp('\\W+'))",
           String([non_alphanumeric]), String(non_alphanumeric.match(new RegExp('\\W+'))));

// be sure all non-alphanumerics are not matched by \w
Assert.expectEq ( 
           "'" + non_alphanumeric + "'.match(new RegExp('\\w'))",
           null, non_alphanumeric.match(new RegExp('\\w')));

// be sure all alphanumerics are not matched by \W
Assert.expectEq ( 
           "'" + alphanumeric + "'.match(new RegExp('\\W'))",
           null, alphanumeric.match(new RegExp('\\W')));

var s = non_alphanumeric + alphanumeric;

// be sure all alphanumerics are matched by \w
Assert.expectEq ( 
           "'" + s + "'.match(new RegExp('\\w+'))",
           String([alphanumeric]), String(s.match(new RegExp('\\w+'))));

s = alphanumeric + non_alphanumeric;

// be sure all non-alphanumerics are matched by \W
Assert.expectEq ( 
           "'" + s + "'.match(new RegExp('\\W+'))",
           String([non_alphanumeric]), String(s.match(new RegExp('\\W+'))));

// be sure all alphanumerics are matched by \w (using literals)
Assert.expectEq ( 
           "'" + s + "'.match(/\w+/)",
           String([alphanumeric]), String(s.match(/\w+/)));

s = alphanumeric + non_alphanumeric;

// be sure all non-alphanumerics are matched by \W (using literals)
Assert.expectEq ( 
           "'" + s + "'.match(/\W+/)",
           String([non_alphanumeric]), String(s.match(/\W+/)));

s = 'abcd*&^%$$';
// be sure the following test behaves consistently
Assert.expectEq ( 
           "'" + s + "'.match(/(\w+)...(\W+)/)",
           String([s , 'abcd' , '%$$']), String(s.match(/(\w+)...(\W+)/)));

var i;

// be sure all alphanumeric characters match individually
for (i = 0; i < alphanumeric.length; ++i)
{
  s = '#$' + alphanumeric.charAt(i) + '%^';
  Assert.expectEq ( 
         "'" + s + "'.match(new RegExp('\\w'))",
         String([alphanumeric.charAt(i)]), String(s.match(new RegExp('\\w'))));
}
// be sure all non_alphanumeric characters match individually
for (i = 0; i < non_alphanumeric.length; ++i)
{
  s = 'sd' + non_alphanumeric.charAt(i) + String((i+10) * (i+10) - 2 * (i+10));
  Assert.expectEq ( 
         "'" + s + "'.match(new RegExp('\\W'))",
         String([non_alphanumeric.charAt(i)]), String(s.match(new RegExp('\\W'))));
}

