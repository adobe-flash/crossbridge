// CODE FROM http://www.cat-language.com/interpreter.html.  Public domain.

function scheme() {

// BEGIN CODE FROM http://www.cat-language.com/interpreter.html

//=========================================================================
// Data

var stack = new Array(0);
var hist = new Array(0);
var words = { };
var debug = false;

//=========================================================================
// Misc. functions

function push(x) {
  stack.push(x);
}

function peek()  {
  return stack[stack.length - 1];
}

function push_fxn_string(s) {
  trace("pushing function");
  var f = new Function(s + "()");
}

function push_fxn(f) {
  var a = new Array(0);
  a.push(f);
  push(a);
}

//=========================================================================
// New Prototype Extensions

function asStackString(o) {
  if (o.toStackString != undefined) {
    return o.toStackString();
  }
  else {
    return o.toString();
  }
}

Function.prototype.toStackString = function () {
  return "[_function_]";
}

Array.prototype.toStackString = function () {
  var s = "(";
  for (var i = 0; i < this.length; ++i) {
    if (i > 0) s += ", ";
    if (i >= 5) return s + "...)";
    var x = this[this.length - i - 1];
    s += asStackString(x);
  }
  return s + ")";
}

String.prototype.toStackString = function () {
  return "\"" + this + "\"";
}

//=========================================================================
// Cat Primitive Instructions

function pop() {
  return stack.pop();
}

function swap() {
  var x = pop();
  var y = pop();
  push(x);
  push(y);
}

function dup() {
  var x = pop();
  push(x);
  push(x);
}

function peek() {
 var x = pop();
 push(x);
 return x;
}

function compose() {
  var x = pop();
  var y = pop();
  var f = function () {x(); y(); };
  push(f);
}

function quote() {
  var x = pop();
  var f = function () { push(x) };
  push(f);
}

function cat_if() {
  var f = pop();
  var t = pop();
  var c = pop();
  if (c) { t(); } else { f(); }
}

function cat_while()  {
  var b = pop();
  var f = pop();
  while (b(), pop()) {
    f();
  }
}

function list() {
  var f = pop();
  var old = stack;
  stack = new Array(0);
  f();
  old.push(stack);
  stack = old;
}

function fold() {
  var f = pop();
  var init = pop();
  var a = pop();
  for (var i = a.length - 1; i >= 0; --i)
  {
    push(init);
    var x = a[i];
    push(x);
    f();
    init = pop();
  }
  push(init);
}

// Cat primitives
words["add"] = function() { push(pop() + pop()); };
words["compose"] = compose;
words["cons"] = function() { var x = pop(); var xs = peek(); xs.push(x); }
words["div"] = function() { swap(); push(pop() / pop()); }
words["dup"] = dup;
words["empty"] = function() { var x = peek(); push(x.length == 0); }
words["eq"] = function() { push(pop() == pop()); }
words["false"] = function() { push(false); }
words["if"] = cat_if;
words["mod"] = function() { swap(); push(pop() % pop()); }
words["mul"] = function() { push(pop() * pop()); }
words["neg"] = function() { push(-pop()); };
words["nil"] = function() { push(nil); }
words["pop"] = pop;
words["quote"] = quote;
words["swap"] = swap;
words["true"] = function() { push(true); }
words["uncons"] = function() { var x = pop(); y = x.pop(); push(x); push(y); }
words["while"] = cat_while;
words["list"] = list;
words["nil"] = function() { push(new Array(0)); }
words["count"] = function() { push(peek().length); }
words["apply"] = function() { var f = pop(); f(); }
words["inc"] = function() { push(pop() + 1); };
words["dip"] = function() { var f = pop(); var x = pop(); f.call(); push(x); }
words["popd"] = function() { swap(); pop(); }
words["not"] = function() { push(false); words["eq"](); }
words["pair"] = function() { var x = pop(); var y = pop(); var xs = new Array(0); xs.push(y); xs.push(x); push(xs); }
words["and"] = function() { var b = pop(); var a = pop(); if (a) {push(b)} else {push(false)}; }
words["unit"] = function() { var xs = new Array(0); var x = pop(); xs.push(x); push(xs); }
words["fold"] = fold;
words["sub"] = function() { swap(); push(pop() - pop()); }
words["neq"] = function() { push(pop() !== pop()); }
words["lt"] = function() { push(pop() > pop()); }
words["lteq"] = function() { push(pop() >= pop()); }
words["gt"] = function() { push(pop() < pop()); }
words["gteq"] = function() { push(pop() <= pop()); }

// Symbolic primitives
words["+"] = words["add"];
words["-"] = words["sub"];
words["*"] = words["mul"];
words["/"] = words["div"];
words["%"] = words["mod"];
words["<"] = words["lt"];
words["<="] = words["lteq"];
words[">"] = words["gt"];
words[">="] = words["gteq"];
words["=="] = words["eq"];
words["!="] = words["neq"];
words["++"] = words["inc"];
words["--"] = words["dec"];

// Others
words["to_value"] = function() { push(peek().valueOf()); }
words["to_string"] = function() { push(peek().toString()); }
words["write"] = function() { write(pop().toString()); }
words["writeln"] = function() { writeln(pop().toString()); }
words["clear_output"] = function() { clear_output(); }
words["clear_stack"] = function() { clear_stack(); }
words["clear"] = function() { clear_output(); clear_stack(); }

//=========================================================================
// Unit tests
function run_test_case(string) {
  stack = new Array(0);
  parse_input(string);
  if (stack.length == 1 && stack[0] === true) {
    stack = new Array(0);
    return true;
  }
  throw new Error (0, "Test failed: " + string);
  return false;
}

//=============================================================================
// Parsing functions

function process_int(s) {
  var x = parseInt(s, 10);
  if (isNaN(x)) return false;
  stack.push(x);
  return true;
}

function process_float(s) {
  var x = parseFloat(s);
  if (isNaN(x)) return false;
  stack.push(x);
  return true;
}

function process_string(s) {
  if (!(s.length >= 2)) return false;
  if (s.charAt(0) != '"') return false;
  if (s.charAt(s.length - 1) != '"') return false;
  stack.push(s.slice(1, s.length - 1));
  return true;
}

function process_quote(s) {
  trace("processing quote");
  if (!(s.length >= 2)) return false;
  if (s.charAt(0) != '[') return false;
  if (s.charAt(s.length - 1) != ']') return false;
  var tmp = s.substring(1, s.length - 1);
  trace("creating function");
  var f = new Function("parse_input('" + tmp + "')");
  trace("created function");
  push(f);
  return true;
}

function add_word(name, body) {
  words[name] = new Function("parse_input('" + body + "')");
  display_words();
}

function trace(s) {
  if (debug)
    writeln("trace: " + s);
}

function fail(s) {
  writeln("error: " + s)
  return "";
}

function process(s) {
  var intRe = /^[+-]?[\d]+/;
  var floatRe = /^[+-]?[\d]+\.[\d]+/;
  var wsRe = /^\s+/;
  var fxnRe = /^\S+/;
  var defineRe = /^define\s+(\S+)\s+\{(.*)\}/

  if (wsRe.test(s))
  {
    trace("removed leading white space");
    var n = wsRe.exec(s)[0].length;
    return s.slice(n);
  }
  else if (defineRe.test(s))
  {
    var def = defineRe.exec(s);
    add_word(def[1], def[2]);
    return "";
  }
  else if (s.charAt(0) == '[')
  {
    trace("at a quote");
    var n = 1;
    var i = 1;
    while ((n > 0) && (i < s.length))
    {
      if (s.charAt(i) == ']') --n;
      if (s.charAt(i) == '[') ++n;
      ++i;
    }
    if (n > 0)
      return fail("unmatched [ ");
    var tmp = s.slice(0, i);
    if (!process_quote(tmp))
      return fail("unable to process quotation from \"" + tmp + "\"");
    return s.slice(i);
  }
  else if (s.charAt(0) == '"')
  {
    trace("at a string");
    var i = 1;
    while ((s.charAt(i) !== '"') && (i < s.length))
    {
      // advance over \" strings
      if (s.charAt(i++) == '\\') i++;
    }
    if (!process_string(s.slice(0, i + 1)))
      return fail("could not process string from \"" + s + "\"");
    return s.slice(i + 1);
  }
  else if (floatRe.test(s))
  {
    trace("at a float");
    var n = floatRe.exec(s)[0].length;
    if (!process_float(s.slice(0, n)))
      return fail("could not process a float from \"" + s + "\"");
    return s.slice(n);
  }
  else if (intRe.test(s))
  {
    trace("matched an integer");
    var n = intRe.exec(s)[0].length;
    if (!process_int(s.slice(0, n)))
      return fail("could not process an integer from \"" + s + "\"");
    return s.slice(n);
  }
  else if (fxnRe.test(s))
  {
    trace("matched a function");
    var n = fxnRe.exec(s)[0].length;
    var word = s.slice(0, n);
    var f = words[word];
    if (f == null)
      return fail(word + " is not a defined function");
    f.call();
    return s.slice(n);
  }
  else
  {
    return fail("could not process : " + s);
  }
}

function parse_input(s) {
  var rest = s;
  while (rest.length > 0)
  {
    trace("processing rest");
    rest = process(rest);
  }
}

function stack_as_string() {
  var result = "";
  for (var i=0; i < stack.length; ++i) {
    var x = stack[i];
    result = asStackString(x) + "\n" + result;
  }
  return result;
}

function write(s) {
  element = document.getElementById('out');
  if (element != null)
    element.value = element.value + s;
}

function writeln(s) {
  write(s + "\n");
}

function clear_output() {
  element = document.getElementById('out');
  element.value = "";
}

function clear_stack() {
  stack = new Array(0);
}

function update_stack() {
  stack_display = document.getElementById('stack');
  stack_display.value = stack_as_string()
}

function shortcut(s) {
  input = document.getElementById('in');
  input.value = s;
  exec();
}

function display_words() {
  var table = document.getElementById('words');
  while (table.rows.length > 0)
    table.deleteRow(0);
  var n = 0;
  var tr;
  var td;
  for (var word in words) {
    if (n % 5 == 0)
      tr = table.insertRow(table.rows.length);
    td = tr.insertCell(tr.cells.length);
    td.innerHTML = "<a href=\"javascript:shortcut('" + word + "')\">" + word + "</a>";
    ++n;
  }
}

//=========================================================================
// Main execution function

function exec() {
  trace("executing instruction");
  input = document.getElementById('in');
  try {
    trace("parsing text");
    hist.push(input.value);
    parse_input(input.value);
  }
  catch (e) {
    writeln("exception caught: " + e.name + ", " + e.message);
  }
  update_stack();
  input.value = "";
  return false;
}

// TODO: revive the test suites
//if (debug)
// run_test_suite();

//=========================================================================
// Data

var stack = new Array(0);
var hist = new Array(0);
var words = { };
var debug = false;

//=========================================================================
// Misc. functions

function push(x) {
  stack.push(x);
}

function peek()  {
  return stack[stack.length - 1];
}

function push_fxn_string(s) {
  trace("pushing function");
  var f = new Function(s + "()");
}

function push_fxn(f) {
  var a = new Array(0);
  a.push(f);
  push(a);
}

//=========================================================================
// New Prototype Extensions

function asStackString(o) {
  if (o.toStackString != undefined) {
    return o.toStackString();
  }
  else {
    return o.toString();
  }
}

Function.prototype.toStackString = function () {
  return "[_function_]";
}

Array.prototype.toStackString = function () {
  var s = "(";
  for (var i = 0; i < this.length; ++i) {
    if (i > 0) s += ", ";
    if (i >= 5) return s + "...)";
    var x = this[this.length - i - 1];
    s += asStackString(x);
  }
  return s + ")";
}

String.prototype.toStackString = function () {
  return "\"" + this + "\"";
}

//=========================================================================
// Cat Primitive Instructions

function pop() {
  return stack.pop();
}

function swap() {
  var x = pop();
  var y = pop();
  push(x);
  push(y);
}

function dup() {
  var x = pop();
  push(x);
  push(x);
}

function peek() {
 var x = pop();
 push(x);
 return x;
}

function compose() {
  var x = pop();
  var y = pop();
  var f = function () {x(); y(); };
  push(f);
}

function quote() {
  var x = pop();
  var f = function () { push(x) };
  push(f);
}

function cat_if() {
  var f = pop();
  var t = pop();
  var c = pop();
  if (c) { t(); } else { f(); }
}

function cat_while()  {
  var b = pop();
  var f = pop();
  while (b(), pop()) {
    f();
  }
}

function list() {
  var f = pop();
  var old = stack;
  stack = new Array(0);
  f();
  old.push(stack);
  stack = old;
}

function fold() {
  var f = pop();
  var init = pop();
  var a = pop();
  for (var i = a.length - 1; i >= 0; --i)
  {
    push(init);
    var x = a[i];
    push(x);
    f();
    init = pop();
  }
  push(init);
}

// Cat primitives
words["add"] = function() { push(pop() + pop()); };
words["compose"] = compose;
words["cons"] = function() { var x = pop(); var xs = peek(); xs.push(x); }
words["div"] = function() { swap(); push(pop() / pop()); }
words["dup"] = dup;
words["empty"] = function() { var x = peek(); push(x.length == 0); }
words["eq"] = function() { push(pop() == pop()); }
words["false"] = function() { push(false); }
words["if"] = cat_if;
words["mod"] = function() { swap(); push(pop() % pop()); }
words["mul"] = function() { push(pop() * pop()); }
words["neg"] = function() { push(-pop()); };
words["nil"] = function() { push(nil); }
words["pop"] = pop;
words["quote"] = quote;
words["swap"] = swap;
words["true"] = function() { push(true); }
words["uncons"] = function() { var x = pop(); y = x.pop(); push(x); push(y); }
words["while"] = cat_while;
words["list"] = list;
words["nil"] = function() { push(new Array(0)); }
words["count"] = function() { push(peek().length); }
words["apply"] = function() { var f = pop(); f(); }
words["inc"] = function() { push(pop() + 1); };
words["dip"] = function() { var f = pop(); var x = pop(); f.call(); push(x); }
words["popd"] = function() { swap(); pop(); }
words["not"] = function() { push(false); words["eq"](); }
words["pair"] = function() { var x = pop(); var y = pop(); var xs = new Array(0); xs.push(y); xs.push(x); push(xs); }
words["and"] = function() { var b = pop(); var a = pop(); if (a) {push(b)} else {push(false)}; }
words["unit"] = function() { var xs = new Array(0); var x = pop(); xs.push(x); push(xs); }
words["fold"] = fold;
words["sub"] = function() { swap(); push(pop() - pop()); }
words["neq"] = function() { push(pop() !== pop()); }
words["lt"] = function() { push(pop() > pop()); }
words["lteq"] = function() { push(pop() >= pop()); }
words["gt"] = function() { push(pop() < pop()); }
words["gteq"] = function() { push(pop() <= pop()); }

// Symbolic primitives
words["+"] = words["add"];
words["-"] = words["sub"];
words["*"] = words["mul"];
words["/"] = words["div"];
words["%"] = words["mod"];
words["<"] = words["lt"];
words["<="] = words["lteq"];
words[">"] = words["gt"];
words[">="] = words["gteq"];
words["=="] = words["eq"];
words["!="] = words["neq"];
words["++"] = words["inc"];
words["--"] = words["dec"];

// Others
words["to_value"] = function() { push(peek().valueOf()); }
words["to_string"] = function() { push(peek().toString()); }
words["write"] = function() { write(pop().toString()); }
words["writeln"] = function() { writeln(pop().toString()); }
words["clear_output"] = function() { clear_output(); }
words["clear_stack"] = function() { clear_stack(); }
words["clear"] = function() { clear_output(); clear_stack(); }

//=========================================================================
// Unit tests
function run_test_case(string) {
  stack = new Array(0);
  parse_input(string);
  if (stack.length == 1 && stack[0] === true) {
    stack = new Array(0);
    return true;
  }
  throw new Error (0, "Test failed: " + string);
  return false;
}

//=============================================================================
// Parsing functions

function process_int(s) {
  var x = parseInt(s, 10);
  if (isNaN(x)) return false;
  stack.push(x);
  return true;
}

function process_float(s) {
  var x = parseFloat(s);
  if (isNaN(x)) return false;
  stack.push(x);
  return true;
}

function process_string(s) {
  if (!(s.length >= 2)) return false;
  if (s.charAt(0) != '"') return false;
  if (s.charAt(s.length - 1) != '"') return false;
  stack.push(s.slice(1, s.length - 1));
  return true;
}

function process_quote(s) {
  trace("processing quote");
  if (!(s.length >= 2)) return false;
  if (s.charAt(0) != '[') return false;
  if (s.charAt(s.length - 1) != ']') return false;
  var tmp = s.substring(1, s.length - 1);
  trace("creating function");
  var f = new Function("parse_input('" + tmp + "')");
  trace("created function");
  push(f);
  return true;
}

function add_word(name, body) {
  words[name] = new Function("parse_input('" + body + "')");
  display_words();
}

function trace(s) {
  if (debug)
    writeln("trace: " + s);
}

function fail(s) {
  writeln("error: " + s)
  return "";
}

function process(s) {
  var intRe = /^[+-]?[\d]+/;
  var floatRe = /^[+-]?[\d]+\.[\d]+/;
  var wsRe = /^\s+/;
  var fxnRe = /^\S+/;
  var defineRe = /^define\s+(\S+)\s+\{(.*)\}/

  if (wsRe.test(s))
  {
    trace("removed leading white space");
    var n = wsRe.exec(s)[0].length;
    return s.slice(n);
  }
  else if (defineRe.test(s))
  {
    var def = defineRe.exec(s);
    add_word(def[1], def[2]);
    return "";
  }
  else if (s.charAt(0) == '[')
  {
    trace("at a quote");
    var n = 1;
    var i = 1;
    while ((n > 0) && (i < s.length))
    {
      if (s.charAt(i) == ']') --n;
      if (s.charAt(i) == '[') ++n;
      ++i;
    }
    if (n > 0)
      return fail("unmatched [ ");
    var tmp = s.slice(0, i);
    if (!process_quote(tmp))
      return fail("unable to process quotation from \"" + tmp + "\"");
    return s.slice(i);
  }
  else if (s.charAt(0) == '"')
  {
    trace("at a string");
    var i = 1;
    while ((s.charAt(i) !== '"') && (i < s.length))
    {
      // advance over \" strings
      if (s.charAt(i++) == '\\') i++;
    }
    if (!process_string(s.slice(0, i + 1)))
      return fail("could not process string from \"" + s + "\"");
    return s.slice(i + 1);
  }
  else if (floatRe.test(s))
  {
    trace("at a float");
    var n = floatRe.exec(s)[0].length;
    if (!process_float(s.slice(0, n)))
      return fail("could not process a float from \"" + s + "\"");
    return s.slice(n);
  }
  else if (intRe.test(s))
  {
    trace("matched an integer");
    var n = intRe.exec(s)[0].length;
    if (!process_int(s.slice(0, n)))
      return fail("could not process an integer from \"" + s + "\"");
    return s.slice(n);
  }
  else if (fxnRe.test(s))
  {
    trace("matched a function");
    var n = fxnRe.exec(s)[0].length;
    var word = s.slice(0, n);
    var f = words[word];
    if (f == null)
      return fail(word + " is not a defined function");
    f.call();
    return s.slice(n);
  }
  else
  {
    return fail("could not process : " + s);
  }
}

function parse_input(s) {
  var rest = s;
  while (rest.length > 0)
  {
    trace("processing rest");
    rest = process(rest);
  }
}

function stack_as_string() {
  var result = "";
  for (var i=0; i < stack.length; ++i) {
    var x = stack[i];
    result = asStackString(x) + "\n" + result;
  }
  return result;
}

function write(s) {
  element = document.getElementById('out');
  if (element != null)
    element.value = element.value + s;
}

function writeln(s) {
  write(s + "\n");
}

function clear_output() {
  element = document.getElementById('out');
  element.value = "";
}

function clear_stack() {
  stack = new Array(0);
}

function update_stack() {
  stack_display = document.getElementById('stack');
  stack_display.value = stack_as_string()
}

function shortcut(s) {
  input = document.getElementById('in');
  input.value = s;
  exec();
}

function display_words() {
  var table = document.getElementById('words');
  while (table.rows.length > 0)
    table.deleteRow(0);
  var n = 0;
  var tr;
  var td;
  for (var word in words) {
    if (n % 5 == 0)
      tr = table.insertRow(table.rows.length);
    td = tr.insertCell(tr.cells.length);
    td.innerHTML = "<a href=\"javascript:shortcut('" + word + "')\">" + word + "</a>";
    ++n;
  }
}

//=========================================================================
// Main execution function

function exec() {
  trace("executing instruction");
  input = document.getElementById('in');
  try {
    trace("parsing text");
    hist.push(input.value);
    parse_input(input.value);
  }
  catch (e) {
    writeln("exception caught: " + e.name + ", " + e.message);
  }
  update_stack();
  input.value = "";
  return false;
}

// TODO: revive the test suites
//if (debug)
// run_test_suite();

//=========================================================================
// Data

var stack = new Array(0);
var hist = new Array(0);
var words = { };
var debug = false;

//=========================================================================
// Misc. functions

function push(x) {
  stack.push(x);
}

function peek()  {
  return stack[stack.length - 1];
}

function push_fxn_string(s) {
  trace("pushing function");
  var f = new Function(s + "()");
}

function push_fxn(f) {
  var a = new Array(0);
  a.push(f);
  push(a);
}

//=========================================================================
// New Prototype Extensions

function asStackString(o) {
  if (o.toStackString != undefined) {
    return o.toStackString();
  }
  else {
    return o.toString();
  }
}

Function.prototype.toStackString = function () {
  return "[_function_]";
}

Array.prototype.toStackString = function () {
  var s = "(";
  for (var i = 0; i < this.length; ++i) {
    if (i > 0) s += ", ";
    if (i >= 5) return s + "...)";
    var x = this[this.length - i - 1];
    s += asStackString(x);
  }
  return s + ")";
}

String.prototype.toStackString = function () {
  return "\"" + this + "\"";
}

//=========================================================================
// Cat Primitive Instructions

function pop() {
  return stack.pop();
}

function swap() {
  var x = pop();
  var y = pop();
  push(x);
  push(y);
}

function dup() {
  var x = pop();
  push(x);
  push(x);
}

function peek() {
 var x = pop();
 push(x);
 return x;
}

function compose() {
  var x = pop();
  var y = pop();
  var f = function () {x(); y(); };
  push(f);
}

function quote() {
  var x = pop();
  var f = function () { push(x) };
  push(f);
}

function cat_if() {
  var f = pop();
  var t = pop();
  var c = pop();
  if (c) { t(); } else { f(); }
}

function cat_while()  {
  var b = pop();
  var f = pop();
  while (b(), pop()) {
    f();
  }
}

function list() {
  var f = pop();
  var old = stack;
  stack = new Array(0);
  f();
  old.push(stack);
  stack = old;
}

function fold() {
  var f = pop();
  var init = pop();
  var a = pop();
  for (var i = a.length - 1; i >= 0; --i)
  {
    push(init);
    var x = a[i];
    push(x);
    f();
    init = pop();
  }
  push(init);
}

// Cat primitives
words["add"] = function() { push(pop() + pop()); };
words["compose"] = compose;
words["cons"] = function() { var x = pop(); var xs = peek(); xs.push(x); }
words["div"] = function() { swap(); push(pop() / pop()); }
words["dup"] = dup;
words["empty"] = function() { var x = peek(); push(x.length == 0); }
words["eq"] = function() { push(pop() == pop()); }
words["false"] = function() { push(false); }
words["if"] = cat_if;
words["mod"] = function() { swap(); push(pop() % pop()); }
words["mul"] = function() { push(pop() * pop()); }
words["neg"] = function() { push(-pop()); };
words["nil"] = function() { push(nil); }
words["pop"] = pop;
words["quote"] = quote;
words["swap"] = swap;
words["true"] = function() { push(true); }
words["uncons"] = function() { var x = pop(); y = x.pop(); push(x); push(y); }
words["while"] = cat_while;
words["list"] = list;
words["nil"] = function() { push(new Array(0)); }
words["count"] = function() { push(peek().length); }
words["apply"] = function() { var f = pop(); f(); }
words["inc"] = function() { push(pop() + 1); };
words["dip"] = function() { var f = pop(); var x = pop(); f.call(); push(x); }
words["popd"] = function() { swap(); pop(); }
words["not"] = function() { push(false); words["eq"](); }
words["pair"] = function() { var x = pop(); var y = pop(); var xs = new Array(0); xs.push(y); xs.push(x); push(xs); }
words["and"] = function() { var b = pop(); var a = pop(); if (a) {push(b)} else {push(false)}; }
words["unit"] = function() { var xs = new Array(0); var x = pop(); xs.push(x); push(xs); }
words["fold"] = fold;
words["sub"] = function() { swap(); push(pop() - pop()); }
words["neq"] = function() { push(pop() !== pop()); }
words["lt"] = function() { push(pop() > pop()); }
words["lteq"] = function() { push(pop() >= pop()); }
words["gt"] = function() { push(pop() < pop()); }
words["gteq"] = function() { push(pop() <= pop()); }

// Symbolic primitives
words["+"] = words["add"];
words["-"] = words["sub"];
words["*"] = words["mul"];
words["/"] = words["div"];
words["%"] = words["mod"];
words["<"] = words["lt"];
words["<="] = words["lteq"];
words[">"] = words["gt"];
words[">="] = words["gteq"];
words["=="] = words["eq"];
words["!="] = words["neq"];
words["++"] = words["inc"];
words["--"] = words["dec"];

// Others
words["to_value"] = function() { push(peek().valueOf()); }
words["to_string"] = function() { push(peek().toString()); }
words["write"] = function() { write(pop().toString()); }
words["writeln"] = function() { writeln(pop().toString()); }
words["clear_output"] = function() { clear_output(); }
words["clear_stack"] = function() { clear_stack(); }
words["clear"] = function() { clear_output(); clear_stack(); }

//=========================================================================
// Unit tests
function run_test_case(string) {
  stack = new Array(0);
  parse_input(string);
  if (stack.length == 1 && stack[0] === true) {
    stack = new Array(0);
    return true;
  }
  throw new Error (0, "Test failed: " + string);
  return false;
}

//=============================================================================
// Parsing functions

function process_int(s) {
  var x = parseInt(s, 10);
  if (isNaN(x)) return false;
  stack.push(x);
  return true;
}

function process_float(s) {
  var x = parseFloat(s);
  if (isNaN(x)) return false;
  stack.push(x);
  return true;
}

function process_string(s) {
  if (!(s.length >= 2)) return false;
  if (s.charAt(0) != '"') return false;
  if (s.charAt(s.length - 1) != '"') return false;
  stack.push(s.slice(1, s.length - 1));
  return true;
}

function process_quote(s) {
  trace("processing quote");
  if (!(s.length >= 2)) return false;
  if (s.charAt(0) != '[') return false;
  if (s.charAt(s.length - 1) != ']') return false;
  var tmp = s.substring(1, s.length - 1);
  trace("creating function");
  var f = new Function("parse_input('" + tmp + "')");
  trace("created function");
  push(f);
  return true;
}

function add_word(name, body) {
  words[name] = new Function("parse_input('" + body + "')");
  display_words();
}

function trace(s) {
  if (debug)
    writeln("trace: " + s);
}

function fail(s) {
  writeln("error: " + s)
  return "";
}

function process(s) {
  var intRe = /^[+-]?[\d]+/;
  var floatRe = /^[+-]?[\d]+\.[\d]+/;
  var wsRe = /^\s+/;
  var fxnRe = /^\S+/;
  var defineRe = /^define\s+(\S+)\s+\{(.*)\}/

  if (wsRe.test(s))
  {
    trace("removed leading white space");
    var n = wsRe.exec(s)[0].length;
    return s.slice(n);
  }
  else if (defineRe.test(s))
  {
    var def = defineRe.exec(s);
    add_word(def[1], def[2]);
    return "";
  }
  else if (s.charAt(0) == '[')
  {
    trace("at a quote");
    var n = 1;
    var i = 1;
    while ((n > 0) && (i < s.length))
    {
      if (s.charAt(i) == ']') --n;
      if (s.charAt(i) == '[') ++n;
      ++i;
    }
    if (n > 0)
      return fail("unmatched [ ");
    var tmp = s.slice(0, i);
    if (!process_quote(tmp))
      return fail("unable to process quotation from \"" + tmp + "\"");
    return s.slice(i);
  }
  else if (s.charAt(0) == '"')
  {
    trace("at a string");
    var i = 1;
    while ((s.charAt(i) !== '"') && (i < s.length))
    {
      // advance over \" strings
      if (s.charAt(i++) == '\\') i++;
    }
    if (!process_string(s.slice(0, i + 1)))
      return fail("could not process string from \"" + s + "\"");
    return s.slice(i + 1);
  }
  else if (floatRe.test(s))
  {
    trace("at a float");
    var n = floatRe.exec(s)[0].length;
    if (!process_float(s.slice(0, n)))
      return fail("could not process a float from \"" + s + "\"");
    return s.slice(n);
  }
  else if (intRe.test(s))
  {
    trace("matched an integer");
    var n = intRe.exec(s)[0].length;
    if (!process_int(s.slice(0, n)))
      return fail("could not process an integer from \"" + s + "\"");
    return s.slice(n);
  }
  else if (fxnRe.test(s))
  {
    trace("matched a function");
    var n = fxnRe.exec(s)[0].length;
    var word = s.slice(0, n);
    var f = words[word];
    if (f == null)
      return fail(word + " is not a defined function");
    f.call();
    return s.slice(n);
  }
  else
  {
    return fail("could not process : " + s);
  }
}

function parse_input(s) {
  var rest = s;
  while (rest.length > 0)
  {
    trace("processing rest");
    rest = process(rest);
  }
}

function stack_as_string() {
  var result = "";
  for (var i=0; i < stack.length; ++i) {
    var x = stack[i];
    result = asStackString(x) + "\n" + result;
  }
  return result;
}

function write(s) {
  element = document.getElementById('out');
  if (element != null)
    element.value = element.value + s;
}

function writeln(s) {
  write(s + "\n");
}

function clear_output() {
  element = document.getElementById('out');
  element.value = "";
}

function clear_stack() {
  stack = new Array(0);
}

function update_stack() {
  stack_display = document.getElementById('stack');
  stack_display.value = stack_as_string()
}

function shortcut(s) {
  input = document.getElementById('in');
  input.value = s;
  exec();
}

function display_words() {
  var table = document.getElementById('words');
  while (table.rows.length > 0)
    table.deleteRow(0);
  var n = 0;
  var tr;
  var td;
  for (var word in words) {
    if (n % 5 == 0)
      tr = table.insertRow(table.rows.length);
    td = tr.insertCell(tr.cells.length);
    td.innerHTML = "<a href=\"javascript:shortcut('" + word + "')\">" + word + "</a>";
    ++n;
  }
}

//=========================================================================
// Main execution function

function exec() {
  trace("executing instruction");
  input = document.getElementById('in');
  try {
    trace("parsing text");
    hist.push(input.value);
    parse_input(input.value);
  }
  catch (e) {
    writeln("exception caught: " + e.name + ", " + e.message);
  }
  update_stack();
  input.value = "";
  return false;
}

// TODO: revive the test suites
//if (debug)
// run_test_suite();

// END CODE FROM http://www.cat-language.com/interpreter.html

}
