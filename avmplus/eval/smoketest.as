/* -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Smoketest for eval - at least one positive instance of every form
// in the language.  The results are not checked, but the test must
// run to completion and should print SMOKETEST DONE! at the end.

// Expression statement, global variable reference, string literal

print("hello, world");

// Other literal expressions

print(37);
print(31.4159e-1);
print(true);
print(false);
print(null);
print({ "foo": 10, bar: 20 });
print(["a","b",,"c",,]);
print(/a*b/g);
print(<!-- nothing to see -->);
print(<? nothing to see here either ?>);
print(<![CDATA[ and still nothing! ]]>);
print(<hello>xml world</hello>);

// Basic object references (more later)

print({ "foo": 10, bar: 20 }.foo);
print({ "foo": 10, bar: 20 }.bar);
print(["a","b",,"c",,].length); // should be 5
print(["a","b",,"c",,][0]); // should be "a"

// Binary operators (though not assignment operators)

print(9 && 4);
print(9 || 4);
print(9 ^ 4);
print(9 & 4);
print(9 | 4);
print(9 << 4);
print(9 >> 4);
print(9 >>> 4);
print({} instanceof Object);
print(9 is Number);
print(9 as Number);
print("toString" in Object);
print(9 == 4);
print(9 === 4);
print(9 != 4);
print(9 !== 4);
print(9 < 4);
print(9 <= 4);
print(9 > 4);
print(9 >= 4);
print(9 + 4);
print(9 - 4);
print(9 * 4);
print(9 % 4);
print(9 / 4);

// Conditional operator

print(37 ? "yes!" : "no");

// Comma expression, paren expression

print((37, 42));

// Global variable definition; global variable reference

var v = "v!";
print(v);
var vi:int = 37;
print(vi);

// Global const definition

const cnst = "cnst!";
print(cnst);
const cnstN: Number = 37.5;
print(cnstN);

// Assignment operators

var x = 10;
print(x++);
print(++x);
print(x--);
print(--x);
x = 10;
x += 1;
x -= 1;
x *= 1;
x /= 1;
x %= 1;
x ^= 1;
x &= 1;
x |= 1;
x <<= 1;
x >>= 1;
x >>>= 1;
/* FIXME - incorrect code is generated for these
x &&= 1;
x ||= 1;
*/
print(x);

// Other prefix (really an AS3 language bug that this is not simply unary)

var x;
print(delete x);

// Unary

print(!!!x);
print(~~~x);
print(- - -x);
print(+ + +x);
print(void void void x);
print(typeof typeof typeof x);

// Object construction, trickier initializer

print(new String("fnord"));


// FIXME - non-basic "property references" and qualifiers
//
// x..y
// ns::x
// ns::*
// *::x
// *::*
// x.@n
// x.@[e]
// x.*
// x.(e)
// Vector.<T>

x1 = 
<alpha attr1="value1" attr2="value2">
    <bravo>
        one
        <charlie>two</charlie>
    </bravo>
</alpha>;

print(x1..*);

// Vector syntax

var x = new <Vector.<int>> [new <int>[], null,null];
print(x);
print(x.length);
print(x[0]);
print(x[1]);
print(x[2]);
x[1] = new <int> [1,2,3];
print(x[1]);
x[2] = new <int> [4,5,6];
print(x[2]);
print(Vector.<int>);
print(Object);
print(new Vector.<int>(10));
var y : Vector.<int> = new <int> [1,2,3];
print(y);
var failed = false;
try {
    y = new <Number> [1,2,3]; // not compatible
}
catch (e) {
    failed = true;
}
if (!failed)
    print("Failed to perform type check");

// Global function definition; if statement; return statement.

function fib(n) {
    if (n < 2)
        return n;
    else
        return fib(n-1) + fib(n-2);
}
print(fib(7));

// 'this' expression

function f_this() { print(this); }
f_this();

// Various kinds of argument lists

function f1(a,b,c,d) { 
    return a+b+c+d
}
print(f1(1,2,3,4));

function f1_typed(a:int,b:Number,c:Number,d:uint):Number { 
    return a+b+c+d
}
print(f1_typed(1,2,3,4));

function f2(x, ...rest) {
   print(x);
   print(rest);
}
f2(1,2,3,4);

function f2_typed(x:Number, ...rest:Array) {
   print(x);
   print(rest);
}
f2_typed(1,2,3,4);

// Nested function definitions

function f() { 
     function g() { print("holy smokes, batman!"); }
     g();
     return "zappa";
}
print(f());

// for statement

for ( var i=0 ; i < 3 ; i++ )
    print(i);

var j=3;
for ( ; j > 0 ; )
    print(j--);

// for-in statement

for ( var i in [1,2,3])
    print(i);
for ( i in [37,42,60])
    print(i);

// for-each-in statement

for each ( var i in [1,2,3])
    print(i);
for each ( i in [37,42,60])
    print(i);

// do statement; block statement

var j=0;
do {
    print(j);
} while(++j < 3);

// while statement

var j=0;
while (j < 3) {
    print(j);
    j++;
}

// empty statement

var j=0;
while (j++ < 3)
    ;

// switch statement

var j=0;
switch (j) {
case 0: print("yes"); break;
case 1: print("no"); break;
default: print("maybe"); break;
}

// try-catch statement; throw statement

function thrower() { throw "Error"; }
try {
    thrower();
}
catch (e) {
    print("Caught: " + e);
}
finally {
    print("Done");
}

// break statement; labeled statement

var j=0;
while (true) {
    if (j > 3)
        break;
    j++;
}

var j=0;
w1loop:
while (true) {
    if (j > 3)
        break w1loop;
    j++;
}

// continue statement; labeled statement

var j=0;
w2loop:
while (true) {
    j++;
    if (j <= 3)
        continue w2loop;
    break;
}

// with statement

var x=5;
with ({x: 10})
    print(x);

// goto statement

function gotoTest()
{
    goto L1;
    throw "Should not happen: goto failed";
    L1: print("goto!");
}
gotoTest();
 
 // FIXME: super expression
 // FIXME: super statement
 
 // FIXME: import directive
 // FIXME: use directive
 
// include directive

include "smoketest2.as";
include "smoketest2.as"

// attributed definitions
//
// FIXME: namespace definition
// FIXME: class definition
// FIXME: interface definition
// FIXME: default xml namespace directive

/* FIXME: does not work
package p.q.r {
    internal function f() {}
    public function get x() { return 37 }
    public function set x(_x) {}
}
*/

print(w);
var w = "outer";
function g() { var w = "inner"; }
g();
print(w);
print(delete w);
print(typeof w);

function h() { print("first"); }
h();
function h() { print("second"); }
h();
function zappa(x, y) { return x + y; }
print(zappa("hello", "there"));
print(x);
print(x *= 5);
print(x);
var obj = ["a"];
print(obj.toString());
print(obj["toString"]());
print(new Array(1,2,3));
function ww() {
    return "" + this.x + "," + this.y
}
function c(x) {
    this.x = x;
    this.toString = ww;
}
c.prototype = { y: 20 };
print(new c(10));
var zz = function (x) { return x }
print(zz("hello"));
var uu = "nothing special"
var yy = function uu(x) { return x }
print(yy("hello"))
print(uu)

var obj = ["a","b","c"]
for ( i in obj)
    print(i);
for ( var i=17 in [] )
    print(i);
print(i);
for ( var i=17 in obj )
    print(i + "," + obj[i]);

try {
    obj = null;
    print("Should happen");
}
catch (e) {
      print("Should not happen");
}

try {
    throw "Foo";
    print("Should not happen");
}
catch (e) {
      print("Should happen with foo: " + e);
}
try {
    print("Number 1");
}
finally {
    print("Number 2");
}
try {
    throw "foo";
}
catch (e) {
    print(e);
}
finally {
    print("number 3");
}

print("Got to end");
function abba() {
     print(arguments);
}

abba(1,2,3,4)
for each ( var i in [1,2,3])
    print(i);

function typed1(x: int): void { 
    var v : String = "foo";
    return;
}

print("hello".split("ll"));

    x = 0;
loop:
    while(x < 10) {
        x++;
        continue loop;
        print("not here");
    }
    
    print(x);

for ( x=0 ; x < 1 ; x++ )
    for each ( i in [1,2,3] )
        print(i);

var object = { fun: function() { return this.valueOf(); }, valueOf: function () { return this.value }, value: 37 };
print(object.fun());
function t() {
    var a = 1;
    with ({a: 2}) {
         function f() { return a }
    }
    return f();
}
print(t());

print(1.7976931348623145e308);
print(Number.MAX_VALUE);

print(0x0);
print(0x12);

print(/\u0041/g);
print(/\u0041/.exec("A").length);
print("\u0041");
print(/\u004g/);

function p() {
         function g() { return this }
         var o = g() 
         return o == this
      }
print(p())
/*
try {
    print(eval("function () { broken broken broken"));
}
catch (e) {
    print(e);
}
*/
print(parseInt(341, 8).toString(16));
var f = function (x) { print(arguments); return arguments.callee; }
print(f(10));

function container() {
    var v = function () { print("hello"); }
    v.toString = Object.prototype.toString;
    print(v);
}
container();


order = 
<order>
    <customer>
        <name>John Smith</name>
    </customer>
    <item id="1">
        <description>Big Screen Television</description>
        <price>1299.99</price>
    </item>
    <item id="2">
        <description>DVD Player</description>
        <price>399.99</price>
    </item>
</order>;
print(order);

var xml7 = <><c q='1'>"quotes"</c><c q='2'>&#x7B;curly brackets&#x7D;</c></>;
print(xml7);

e = <employees>
    <employee id="0"><fname>John</fname><age>20</age></employee>
    <employee id="1"><fname>Sue</fname><age>30</age></employee>
    </employees>;


correct = <employee id="0"><fname>John</fname><age>20</age></employee>;

john = e.employee.(fname == "John");

print(john);

// fast switches - this code triggers fast switch generation on 2009-03-03, at least.
// would be good to verify that it continues to do so...

// output should be 5 4 4 again default 2 1 2 0

for ( var i=5 ; i >= 0 ; i-- ) {
    switch (i) {
    case 0: print("0"); break;
    case 1: print("1"); // no break
    case 2: print("2"); break;
    case 1: print("3"); break;
    case 4: print("4"); 
    print("4 again"); break;
    case 5: print("5"); break;
    default: print("default"); break;
    }
}

// -- Nothing new beyond this line! ----------------------------------------------------------

print("");
print("SMOKETEST DONE!");
