/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var x, y;

x = 10;  y = ++x;  print("PREFIX ++ " + x + " " + y);
x = 10;  y = x++;  print("POSTFIX ++ " + x + " " + y);

x = 10;  y = --x;  print("PREFIX -- " + x + " " + y);
x = 10;  y = x--;  print("POSTFIX -- " + x + " " + y);

x = { foo: 10 }
delete x.foo;
print( "DELPROP " + ("foo" in x) );

print( "DELVAR " + (delete x) );

function f() { print("VOID called") }
print( "VOID " + (void f()) );

print( "TYPEOF " + (typeof "foo"));
print( "TYPEOF " + (typeof null));
print( "TYPEOF " + (typeof zappa));

print( "UNOP + " + (+ "") );
print( "UNOP - " + (- 10) );
print( "UNOP ~ " + (~ 10) );
print( "UNOP ! " + (! true) );
print( "DONE" );
