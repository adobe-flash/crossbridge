/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'special_characters.js';

/**
   Filename:     special_characters.js
   Description:  'Tests regular expressions containing special characters'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: special_charaters';



// testing backslash '\'
Assert.expectEq (  "'^abcdefghi'.match(/\^abc/)", String(["^abc"]), String('^abcdefghi'.match(/\^abc/)));

// testing beginning of line '^'
Assert.expectEq (  "'abcdefghi'.match(/^abc/)", String(["abc"]), String('abcdefghi'.match(/^abc/)));

// testing end of line '$'
Assert.expectEq (  "'abcdefghi'.match(/fghi$/)", String(["ghi"]), String('abcdefghi'.match(/ghi$/)));

// testing repeat '*'
Assert.expectEq (  "'eeeefghi'.match(/e*/)", String(["eeee"]), String('eeeefghi'.match(/e*/)));

// testing repeat 1 or more times '+'
Assert.expectEq (  "'abcdeeeefghi'.match(/e+/)", String(["eeee"]), String('abcdeeeefghi'.match(/e+/)));

// testing repeat 0 or 1 time '?'
Assert.expectEq (  "'abcdefghi'.match(/abc?de/)", String(["abcde"]), String('abcdefghi'.match(/abc?de/)));

// testing any character '.'
Assert.expectEq (  "'abcdefghi'.match(/c.e/)", String(["cde"]), String('abcdefghi'.match(/c.e/)));

// testing remembering ()
Assert.expectEq (  "'abcewirjskjdabciewjsdf'.match(/(abc).+\\1'/)",
           String(["abcewirjskjdabc","abc"]), String('abcewirjskjdabciewjsdf'.match(/(abc).+\1/)));

// testing or match '|'
Assert.expectEq (  "'abcdefghi'.match(/xyz|def/)", String(["def"]), String('abcdefghi'.match(/xyz|def/)));

// testing repeat n {n}
Assert.expectEq (  "'abcdeeeefghi'.match(/e{3}/)", String(["eee"]), String('abcdeeeefghi'.match(/e{3}/)));

// testing min repeat n {n,}
Assert.expectEq (  "'abcdeeeefghi'.match(/e{3,}/)", String(["eeee"]), String('abcdeeeefghi'.match(/e{3,}/)));

// testing min/max repeat {min, max}
Assert.expectEq (  "'abcdeeeefghi'.match(/e{2,8}/)", String(["eeee"]), String('abcdeeeefghi'.match(/e{2,8}/)));

// testing any in set [abc...]
Assert.expectEq (  "'abcdefghi'.match(/cd[xey]fgh/)", String(["cdefgh"]), String('abcdefghi'.match(/cd[xey]fgh/)));

// testing any in set [a-z]
Assert.expectEq (  "'netscape inc'.match(/t[r-v]ca/)", String(["tsca"]), String('netscape inc'.match(/t[r-v]ca/)));

// testing any not in set [^abc...]
Assert.expectEq (  "'abcdefghi'.match(/cd[^xy]fgh/)", String(["cdefgh"]), String('abcdefghi'.match(/cd[^xy]fgh/)));

// testing any not in set [^a-z]
Assert.expectEq (  "'netscape inc'.match(/t[^a-c]ca/)", String(["tsca"]), String('netscape inc'.match(/t[^a-c]ca/)));

// testing backspace [\b]
Assert.expectEq (  "'this is b\ba test'.match(/is b[\b]a test/)",
           String(["is b\ba test"]), String('this is b\ba test'.match(/is b[\b]a test/)));

// testing word boundary \b
Assert.expectEq (  "'today is now - day is not now'.match(/\bday.*now/)",
           String(["day is not now"]), String('today is now - day is not now'.match(/\bday.*now/)));

// control characters???

// testing any digit \d
Assert.expectEq (  "'a dog - 1 dog'.match(/\\d dog/)", String(["1 dog"]), String('a dog - 1 dog'.match(/\d dog/)));

// testing any non digit \d
Assert.expectEq (  "'a dog - 1 dog'.match(/\\D dog/)", String(["a dog"]), String('a dog - 1 dog'.match(/\D dog/)));

// testing form feed '\f'
Assert.expectEq (  "'a b a\\f b'.match(/a\\fb/)", String(["a\fb"]), String('a b a\fb'.match(/a\fb/)));

// testing line feed '\n'
Assert.expectEq (  "'a b a\\nb'.match(/a\\nb/)", String(["a\nb"]), String('a b a\nb'.match(/a\nb/)));

// testing carriage return '\r'
Assert.expectEq (  "'a b a\\rb'.match(/a\\rb/)", String(["a\rb"]), String('a b a\rb'.match(/a\rb/)));

// testing whitespace '\s'
Assert.expectEq (  "'xa\\f\\n\\r\\t\\vbz'.match(/a\\s+b/)", String(["a\f\n\r\t\vb"]), String('a\f\n\r\t\vbz'.match(/a\s+b/)));

// testing non whitespace '\S'
Assert.expectEq (  "'a\\tb a b a-b'.match(/a\\Sb/)", String(["a-b"]), String('a\tb a b a-b'.match(/a\Sb/)));

// testing tab '\t'
Assert.expectEq (  "'a\\t\\tb a  b'.match(/a\\t{2}/)", String(["a\t\t"]), String('a\t\tb a  b'.match(/a\t{2}/)));

// testing vertical tab '\v'
Assert.expectEq (  "'a\v\vb a  b'.match(/a\\v{2}/)", String(["a\v\v"]), String('a\v\vb a  b'.match(/a\v{2}/)));

// testing alphnumeric characters '\w'
Assert.expectEq (  "'%AZaz09_$'.match(/\\w+/)", String(["AZaz09_"]), String('%AZaz09_$'.match(/\w+/)));

// testing non alphnumeric characters '\W'
Assert.expectEq (  "'azx$%#@*4534'.match(/\\W+/)", String(["$%#@*"]), String('azx$%#@*4534'.match(/\W+/)));

// testing back references '\<number>'
Assert.expectEq (  "'test'.match(/(t)es\\1/)", String(["test","t"]), String('test'.match(/(t)es\1/)));

// testing hex excaping with '\'
Assert.expectEq (  "'abcdef'.match(/\x63\x64/)", String(["cd"]), String('abcdef'.match(/\x63\x64/)));

// testing oct excaping with '\'
Assert.expectEq (  "'abcdef'.match(/\\143\\144/)", String(["cd"]), String('abcdef'.match(/\143\144/)));


