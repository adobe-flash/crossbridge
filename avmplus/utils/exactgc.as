/* -*- indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// exactgc.as generates exact tracing code from annotated classes.  It
// is usually run by builtin-tracers.py, shell_toplevel-tracers.py,
// and similar scripts, and occasionally cooperates with nativegen.py.
//
// The <a href="doc/mmgc/exactgc-cookbook.html">cookbook</a> provides a gentle
// introduction to annotations; most people need look no further.
//
// The <a href="doc/mmgc/exactgc-manual.html">manual</a> provides a complete
// reference manual for exact GC and describes every facility provided
// by exactgc.as and more.
//
//
// Usage & options.
//
// exactgc.as extracts native attributes from ".as" and ".h" files and
// generates C++ code.  Little is assumed about the structure of the
// files; indeed the script looks only for annotations, never for C++
// or AS3 phrases, and you can blindly feed a lot of files to it and
// expect the right thing to happen.
//
// Typical usage:
//
//    avm exactgc.abc -- -b avmplus-as3-gc.h -n avmplus-cpp-gc.h -i avmplus-gc-interlock.h *.h *.as
//    
// Options:
//
//    filename       A file to process
//    @filename      A file from which to read more file names
//    -x filename    A file to process (useful if the filename starts with '@' or '-')
//    -n filename    Emit tracers for GC_CPP_EXACT ("natives") to this file
//    -b filename    Emit tracers for GC_AS3_EXACT ("builtins") to this file
//    -i filename    Emit interlock definitons to this file
//    -ns namespace  The C++ namespace to wrap around the output, default global
//
//
// Performance notes.
//
// The script has been tuned quite a bit, and there's a built-in profiling option
// to help us tune more (search for 'var profiling' below).  Even so, about
// 90% of the time is still spent in reading input and extracting annotations, so
// obvious things to do if further performance improvements are needed would be:
//
//   - cache intermediate data (eg, save extracted lines + file's path and mtime)
//   - don't split the file into individual lines (splitting takes some time).
//
// The design is currently line-based because I would like to expand it later to
// include various kinds of error checking that goes hand-in-hand with SafeGC, eg,
// if GCMember appears on a line then there should be a GC annotation too.  So
// it would be good to try caching first.

import avmplus.*;

// For [native] annotations on AS3 class definitions that have "gc",
// "clsgc", or "instancegc" annotations.
class AS3Class
{
    function AS3Class(ns, cls) {
        // This is for dealing with the "::avmshell::" prefix in some native annotations,
        // though there is probably a better way.
        var s = "::" + ns + "::";
        if (cls.indexOf(s) == 0)
            cls = cls.substring(s.length);
        this.cls = cls;
    }

    public function toString() { return printProps(this, ["cls"]) }

    const cls;
}


// For GC_AS3_EXACT and GC_CPP_EXACT annotations on C++ class
// definitions, also for the _WITH_HOOK variants.  Note
// _WITH_HOOK_{IF,IFDEF,IFNDEF} means we can have both hooks
// and conditional compilation.

class GCClass
{
    function GCClass(tag, attr, _cond) {
        cls = getAttr(attr, 0);
        base = getAttr(attr, 1);
        hook = tag.match("_WITH_HOOK") != null;
        ifdef = tag.match("_IFDEF") != null ? getAttr(attr,2) : false;
        ifndef = tag.match("_IFNDEF") != null ? getAttr(attr,2) : false;
        if_ = !ifdef && !ifndef && tag.match("_IF") != null ? getAttr(attr,2) : false;
        cond = _cond;
    }

    public function toString() { return printProps(this, ["cls","base","hook","ifdef"]) }

    public function fullName() { return fullClassPrefix + cls; }

    const cls;
    const base;
    const hook;
    const ifdef;
    const ifndef;
    const if_;
    const cond;

    var fullClassPrefix=""; // for nested classes
    var out = new Printer(1);
    var next = null;
    var fieldList = [];              // sorted by property name
    var fieldMap = {};
    var variable_length_field = null;  // does not appear in the 'fields' list
    var probablyLarge = false;
    var hint = null;
}

class GCCppExact extends GCClass { function GCCppExact(tag,attr,cond) { super(tag,attr,cond) } }
class GCAS3Exact extends GCClass { function GCAS3Exact(tag,attr,cond) { super(tag,attr,cond) } }


// For GC_DATA_BEGIN and GC_DATA_END annotations in C++ classes.
class GCDataSection
{
    function GCDataSection(tag, attr, cond) {
        // ignore cond
        cls = getAttr(attr, 0);
    }

    public function toString() { return printProps(this, ["cls"]) }

    const cls;
}

class GCDataBegin extends GCDataSection { function GCDataBegin(tag, attr, cond) { super(tag,attr,cond) } }
class GCDataEnd extends GCDataSection { function GCDataEnd(tag, attr, cond) { super(tag,attr,cond) } }
class GCNoData extends GCDataSection { function GCNoData(tag, attr, cond) { super(tag,attr,cond) } }

// For the various field annotations in C++ classes.
//
// The cls attribute is implicitly present in the C++ source code and
// is inserted at the beginning of the attribute array by the code
// that constructs GCField instances.
//
// Any condition on the field is considered part of the field's name,
// to support typical cases like these:
//
// #ifdef BLAH
//   Bletch* GC_POINTER_IFDEF(p, BLAH)
// #else
//   Blotch* GC_POINTER_IFNDEF(p, BLAH)
// #endif

class GCField 
{
    function GCField(tag, attr, _cond) {
        cls = getAttr(attr, 0);
        name = getAttr(attr, 1);
        ifdef = tag.match("_IFDEF") != null ? getAttr(attr,2) : false;
        ifndef = tag.match("_IFNDEF") != null ? getAttr(attr,2) : false;
        if_ = !ifdef && !ifndef && tag.match("_IF") != null ? getAttr(attr,2) : false;
        cond = _cond;
    }

    public function toString() { return printProps(this, ["cls","name","ifdef", "if_"]) }

    const cls;
    const name;
    const ifdef;
    const ifndef;
    const if_;
    const cond;
}

class GCPointer extends GCField { function GCPointer(tag, attr, cond) { super(tag, attr, cond) } }
class GCAtom extends GCField { function GCAtom(tag, attr, cond) { super(tag, attr, cond) } }
class GCStructure extends GCField { function GCStructure(tag, attr, cond) { super(tag, attr, cond) } }
class GCConservative extends GCField { function GCConservative(tag, attr, cond) { super(tag, attr, cond) } }

class GCPointers extends GCField
{
    function GCPointers(tag, attr, cond) {
        splitFieldAndSize(attr, 1);
        super(tag,attr,cond);
        declCount = getAttr(attr, 2);
        count = getAttr(attr, 3);
        hint = tag.match("_SMALL") != null ? "small" : null;
    }

    override public function toString() { return printProps(this, ["cls","name","ifdef","hint"]) }

    const declCount;
    const count;
    const hint;
}

class GCAtoms extends GCField
{
    function GCAtoms(tag, attr, cond) {
        splitFieldAndSize(attr, 1);
        super(tag,attr,cond);
        declCount = getAttr(attr, 2);
        count = getAttr(attr, 3);
        hint = tag.match("_SMALL") != null ? "small" : null;
    }

    override public function toString() { return printProps(this, ["cls","name","ifdef","hint"]) }

    const declCount;
    const count;
    const hint;
}

class GCStructures extends GCField
{
    function GCStructures(tag, attr, cond) {
        splitFieldAndSize(attr, 1);
        super(tag,attr,cond);
        declCount = getAttr(attr, 2);
        count = getAttr(attr, 3);
        hint = tag.match("_SMALL") != null ? "small" : null;
    }

    override public function toString() { return printProps(this, ["cls","name","ifdef","hint"]) }

    const declCount;
    const count;
    const hint;
}

class Printer
{
    function Printer(_indent=0) {
        for ( var i=0 ; i < _indent ; i++ )
            IN();
    }

    function IN() {
        indent+=4;
        if (indent > indentString.length)
            indentString += "    ";
        return this;
    }

    function OUT() {
        indent-=4;
        return this;
    }

    function PR(s) {
        if (s == null)
            return;
        if (s.charAt(0) != "#")
            output += indentString.substring(0,indent);
        while (s.length > 0 && s.charAt(s.length-1) == "\n")
            s = s.substring(0,s.length-1);
        output += s;
        output += "\n";
        return this;
    }

    function DO(f) {
        f(this);
        return this;
    }

    function NL() {
        return PR("");
    }

    function get() {
        return output;
    }

    var indent = 0;
    var indentString = "";
    var output = "";
}

class Condition
{
    static const IF = 0;
    static const IFDEF = 1;
    static const IFNDEF = 2;
    static const ELIF = 3;
    static const ELSE = 4;
    static const INERT = 5;

    function Condition(_type, _cond, _butnot=null) {
        type = _type;
        cond = _cond;
        butnot = _butnot;
    }

    public function toString() {
        return "Cond: " + typeName + ": " + cond;
    }

    function get typeName() {
        switch (type) {
        case IF: return "if";
        case IFDEF: return "ifdef";
        case IFNDEF: return "ifndef";
        case ELIF: return "elif";
        case ELSE: return "else";
        case INERT: return "inert";
        default: return "XXX";
        }
    }

    const type;
    const cond;
    const butnot;
}

const constructors =
{ "GC_CPP_EXACT":           GCCppExact,
  "GC_CPP_EXACT_IFDEF":     GCCppExact,
  "GC_CPP_EXACT_WITH_HOOK": GCCppExact,
  "GC_CPP_EXACT_WITH_HOOK_IFDEF": GCCppExact,
  "GC_CPP_EXACT_WITH_HOOK_IFNDEF": GCCppExact,
  "GC_CPP_EXACT_WITH_HOOK_IF": GCCppExact,
  "GC_AS3_EXACT":           GCAS3Exact,
  "GC_AS3_EXACT_IFDEF":     GCAS3Exact,
  "GC_AS3_EXACT_WITH_HOOK": GCAS3Exact,
  "GC_AS3_EXACT_WITH_HOOK_IFDEF": GCAS3Exact,
  "GC_AS3_EXACT_WITH_HOOK_IFNDEF": GCAS3Exact,
  "GC_AS3_EXACT_WITH_HOOK_IF": GCAS3Exact,
  "GC_DATA_BEGIN":          GCDataBegin,
  "GC_DATA_END":            GCDataEnd,
  "GC_NO_DATA":             GCNoData,
  "GC_ATOM":                GCAtom,
  "GC_POINTER":             GCPointer,
  "GC_CONSERVATIVE":        GCConservative,
  "GC_STRUCTURE":           GCStructure,
  "GC_ATOMS":               GCAtoms,
  "GC_ATOMS_SMALL":         GCAtoms,
  "GC_POINTERS":            GCPointers,
  "GC_POINTERS_SMALL":      GCPointers,
  "GC_STRUCTURES":          GCStructures,
  "GC_STRUCTURES_SMALL":    GCStructures
};

// Configuration etc
var debug = false;               // print useful debugging info
var profiling = false;           // profile at a function level for selected functions
var errorContext = "top level";  // for error messages, updated as we go
const largeObjectCutoff = 2000;  // more arbitrary than not, "close" to large object limit in MMgc

// Options
var builtinOutputFile = null;         // null == don't output, otherwise file name
var nativeOutputFile = null;          // null == don't output, otherwise file name
var interlockOutputFile = null;       // null == don't output (and don't do interlock checking), otherwise file name
var cppNamespace = "";                // wrap this namespace around the emitted code if not empty string

// Used during parsing and some initial processing
var specs = [];                // intermediate list of all metadata during parsing and field/class collection

// Created during processing
var cppClassList = [];         // all C++ classes ordered by class name
var as3ClassList = [];         // all AS3 classes ordered by class name
var cppClassMap = {};          // maps C++ class name to GCClass
var as3ClassMap = {};          // maps AS3 class name to GCClass

// Occasionally useful during debugging to print the attribute maps
Object.prototype.toString = 
function() { 
    var s=[];
    for ( var i in this ) 
        if (this.hasOwnProperty(i))
            s.push(i + ": " + this[i]); 
    return "{" + s.join(", ") + "}";
};

Array.prototype.toString = 
function() {
    return "[" + this.join(",") + "]";
};

function printProps(obj, attrs) {
    var s = [];
    for ( var i=0 ; i < attrs.length ; i++ )
        s.push(attrs[i] + ": " + obj[attrs[i]]);
    return "{" + s.join(", ") + "}";
}

const LICENSE = 
    ("/* This Source Code Form is subject to the terms of the Mozilla Public\n" +
     " * License, v. 2.0. If a copy of the MPL was not distributed with this\n" +
     " * file, You can obtain one at http://mozilla.org/MPL/2.0/. */\n");

// Read an attribute with range checking from the attribute array
function getAttr(attr, n)
{
    if (n >= attr.length)
        fail("Out-of-range attribute reference: " + attr + " " + n);
    return attr[n];
}

// attr[n] has text of the form "<something1>[<something2>]" with no embedded commas.
// Break them apart, and leave <something1> in position n and insert <something2> in
// position n+1.

function splitFieldAndSize(attr, n)
{
    if (n >= attr.length)
        fail("Out-of-range attribute reference: " + attr + " " + n);
    var v = attr[n];
    var result = (/^\s*([^\]]*)\s*\[([^\]]*)\]\s*$/).exec(v);
    if (result == null || result.length != 3)
        fail("Incorrect format for array field, expected name[size]: " + n);
    for ( var k=attr.length ; k > n+1 ; k-- )
        attr[k] = attr[k-1];
    attr[n] = result[1];
    attr[n+1] = result[2];
}

// Fail with an error message, never return
function fail(s) 
{
    throw new Error(errorContext + ": " + s);
}

// Compare two strings and return -1, 0, or 1
function strcmp(a,b)
{
    if (a < b) return -1;
    if (b < a) return 1;
    return 0;
}

// The following function is about 20x faster than the obvious one-liner
//    return File.read(filename).split(/\r\n|\r|\n/);
//
// Using single regular expressions in place of the disjunction gave us
// a factor of five; switching to strings for the splitting another
// factor of four.  (Reorganizing the code further, to use knowledge
// of the absence of \r to avoid scanning for \r\n, say, has yielded 
// nothing.)

function readLines(filename) {
    var text = File.read(filename);

    var crIndex = text.indexOf("\r");
    var lfIndex = text.indexOf("\n");

    // Be resilient to files with more than one line ending - provide a good error
    // message and fail.
    if (crIndex >= 0 && lfIndex >= 0)
    {
        // Must be crlf endings or error.  At this point we need to check every line
        // break, which sucks, but only on Windows...

        var oldCr = -1;
        var oldLf = -1;
        for (;;)
        {
            crIndex = text.indexOf("\r", oldCr+1);
            lfIndex = text.indexOf("\n", oldLf+1);
            if (crIndex == -1 && lfIndex == -1)
                break;
            if (lfIndex != crIndex+1)
                fail("Mixed line endings in file");
            oldCr = crIndex;
            oldLf = lfIndex;
        }

        return text.split("\r\n");
    }

    if (crIndex >= 0)
        return text.split("\r");

    return text.split("\n");
}

// Look for and record any options, return array of file names.  Skip
// files named 'GC.h' because MMgc/GC.h includes the macros for the
// annotations and cannot be processed.
function processOptionsAndFindFiles(args)
{
    var files = [];

    errorContext = "Processing options";
    processArgs(args);
    return files;

    function processArgs(args) {
        var i=0;
        var limit=args.length;

        function strip(s) {
            var result = (/^\s*(.*)\s*$/).exec(s);
            if (result == null) // shouldn't happen
                return s;
            return result[1];
        }

        function getArg(opt) {
            if (i == limit)
                fail("Missing argument for " + opt);
            return args[i++];
        }

        while (i < limit) {
            var s = args[i++];
            if (s == "-b") 
                builtinOutputFile = getArg(s);
            else if (s == "-n")
                nativeOutputFile = getArg(s);
            else if (s == "-i")
                interlockOutputFile = getArg(s);
            else if (s == "-ns") 
                cppNamespace = getArg(s);
            else if (s == "-x")
                files.push(getArg(s));
            else if (s.match(/GC\.h$/))
                ;
            else if (s.charAt(0) == "@") // indirection file
                files.push.apply(files, readLines(s.substring(1)).map(strip));
            else
                files.push(s);
        }
    }
}

// Read files, accumulate all metadata in order in 'specs'.
//
// Syntactic side conditions:
//
//   GC_DATA_BEGIN must follow a GC class spec with the same name.
//   GC_DATA_END pops both stacks, so there can only be one data
//   section per class.
//
//   At the end of a file, both stacks must be empty.

function readFiles(files)
{
    // For profiling
    var readingTime = 0;
    var processingTime = 0;
    var splittingTime = 0;

    var cppClassStack = [];       // tracks GC_CPP_EXACT, etc
    var cppDataStack = [];        // tracks GC_DATA_BEGIN / GC_DATA_END
    var cppIfStack = [];          // tracks #if, #ifdef, etc

    // TODO: factor regular expressions to avoid duplication?

    const attrStringRegex:RegExp = /^\s*\"([^\"]*)\"\s*$/;
    const attrMiscRegex:RegExp =   /^\s*((?:<\s+|\s+>|[a-zA-Z0-9_:<>])+)\s*$/;
    const attrNumberRegex:RegExp = /^\s*([0-9]+(?:\.[0-9]+)?)\s*$/;
    const attrArraydefRegex:RegExp = /^\s*([a-zA-Z0-9_]+\[[^\]]*\])\s*$/;

    function parseAttrValue(s) {
        var result;
        if ((result = attrStringRegex.exec(s)) != null ||
            (result = attrMiscRegex.exec(s)) != null ||
            (result = attrNumberRegex.exec(s)) != null ||
            (result = attrArraydefRegex.exec(s)) != null) {
            return result[1];
        }
        else
            fail("Bogus name-value pair: " + s);
    }

    // We can't simply split by "," here because the value can
    // contain a comma, for example, in "count" attributes the
    // expression frequently uses offsetof(a,b).

    const spacesStartRegex:RegExp =      /^(\s+)/;
    const spacesEndRegex:RegExp =        /(\s+)$/;
    const commaSpacesStartRegex:RegExp = /^(,\s*)/;
    const nameValuePairRegex:RegExp =    /^([a-zA-Z0-9_]+)\s*=\s*(\"[^\"]*\"|true|false)/;
    const valueRegex:RegExp =            /^\"[^\"]*\"|[a-zA-Z0-9_]+\[[^\]]*\]|(?:[0-9]+(?:\.[0-9]+)?)|(?:<\s+|\s+>|[a-zA-Z0-9_:<>])+/;

    function splitAttrs(s, paren=null) {
        var then = new Date();
        var xs = [];
        var result;
        for (;;) {
            // strip leading and trailing spaces
            if ((result = spacesStartRegex.exec(s)) != null)
                s = s.substring(result[1].length);
            if ((result = spacesEndRegex.exec(s)) != null)
                s = s.substring(0,s.length-result[1].length);

            if (paren != null) {
                if (s == "")
                    fail("Missing closing parenthesis: '" + paren + "'");
                if (s.charAt(0) == paren)
                    break;
            }
            else if (s == "")
                break;

            // strip leading comma and any spaces following it
            if (xs.length > 0) {
                if ((result = commaSpacesStartRegex.exec(s)) == null)
                    fail("Incorrect attribute string: missing comma in " + s);
                s = s.substring(result[1].length);
            }
            if (s == "")
                break;
            // simple-identifier=(string|boolean), string, number, qualified-identifier
            if ((result = nameValuePairRegex.exec(s)) != null)
                xs.push([result[1], parseAttrValue(result[2])]);
            else if ((result = valueRegex.exec(s)) != null)
                xs.push(parseAttrValue(result[0]));
            else 
                fail("Incorrect attribute string: bad value or name/value pair: " + s);
            s = s.substring(result[0].length)
        }
        if (debug)
            print(xs);
        splittingTime += (new Date() - then);
        return xs;
    }

    function copyIfStack() {
        var v = [];
        for ( var i=0 ; i < cppIfStack.length ; i++ )
            v[i] = cppIfStack[i];
        return v;
    }

    function pushGCmembers(list, cls) {
        var members = [];
        var result = list.split(/,|;/);
        for(var i = 0; i < result.length - 1; i++) {
            var attr = [cls];
            attr[1] = result[i].replace(/^\s*|\s*$/g, "");
            specs.push(new constructors["GC_POINTER"]("GC_POINTER",attr,copyIfStack()));
        }
    }

    function positionalAttrs(tag, s, paren, cls) {
        var attr = splitAttrs(s, paren);
        for ( var i=0 ; i < attr.length ; i++ ) {
            if (attr[i] is Array)
                fail("Named attributes not allowed here: " + attr[i]);
        }
        if (cls != null)
            attr.unshift(cls);
        return new constructors[tag](tag,attr,copyIfStack());
    }

    function parseNamedAttrs(s) {
        var attr = splitAttrs(s);
        for ( var i=0 ; i < attr.length ; i++ ) {
            if (!(attr[i] is Array || i ==0 && attr[i] is String && attr.length == 1))
                fail("Named attributes required here: " + attr[i]);
        }
        return attr;
    }

    function reportMatch(line) {
        if (debug)
            print(line);
    }

    function currentClassName() {
        if (cppDataStack.length == 0)
            fail("No active GC_DATA_BEGIN");
        return cppDataStack[cppDataStack.length-1];
    }

    // Does not match the trailing right paren.  $1 is the tag, $2 the
    // rest of the text starting just right of the left paren for the
    // argument list.

    const cppMetaTag = 
        new RegExp("^(" +
                   ["GC_CPP_EXACT_WITH_HOOK_IFDEF",
                    "GC_CPP_EXACT_WITH_HOOK_IFNDEF",
                    "GC_CPP_EXACT_WITH_HOOK_IF",
                    "GC_CPP_EXACT_WITH_HOOK",
                    "GC_CPP_EXACT_WITH_HOOK",
                    "GC_CPP_EXACT_WITH_HOOK",
                    "GC_CPP_EXACT_IFDEF",
                    "GC_CPP_EXACT_IFNDEF",
                    "GC_CPP_EXACT_IF",
                    "GC_CPP_EXACT",
                    "GC_AS3_EXACT_WITH_HOOK_IFDEF",
                    "GC_AS3_EXACT_WITH_HOOK_IFNDEF",
                    "GC_AS3_EXACT_WITH_HOOK_IF",
                    "GC_AS3_EXACT_WITH_HOOK",
                    "GC_AS3_EXACT_WITH_HOOK",
                    "GC_AS3_EXACT_WITH_HOOK",
                    "GC_AS3_EXACT_IFDEF",
                    "GC_AS3_EXACT_IFNDEF",
                    "GC_AS3_EXACT_IF",
                    "GC_AS3_EXACT",
                    "GC_NO_DATA",
                    "GC_DATA_BEGIN",
                    "GC_DATA_END"].join("|") +
                   ")\\s*\\((.*)");

    function matchCppMetaTag(line, where) {
        return cppMetaTag.exec(line.substring(where));
    }

    // Does not match the trailing right paren.  $1 is the tag, $2 the
    // rest of the text starting just right of the left paren for the
    // argument list.

    const cppFieldTag = 
        new RegExp("^(" +
                   ["GC_POINTERS",
                    "GC_POINTERS_SMALL",
                    "GC_STRUCTURES",
                    "GC_STRUCTURES_SMALL",
                    "GC_ATOMS",
                    "GC_ATOMS_SMALL",
                    "GC_STRUCTURE",
                    "GC_POINTER",
                    "GC_ATOM",
                    "GC_CONSERVATIVE"].join("|") +
                   ")\\s*\\((.*)");

    function matchCppFieldTag(line, where) {
        return cppFieldTag.exec(line.substring(where));
    }

    const deprecatedFieldTag =
        new RegExp("^(" +
                   ["GC_STRUCTURE_IFDEF",
                    "GC_STRUCTURE_IFNDEF",
                    "GC_STRUCTURE_IF",
                    "GC_POINTER_IFDEF",
                    "GC_POINTER_IFNDEF",
                    "GC_POINTER_IF",
                    "GC_ATOM_IFDEF",
                    "GC_ATOM_IFNDEF",
                    "GC_ATOM_IF",
                    "GC_CONSERVATIVE_IFDEF",
                    "GC_CONSERVATIVE_IFNDEF",
                    "GC_CONSERVATIVE_IF"].join("|") +
                    ")\\s*\\((.*)");

    function matchDeprecatedFieldTag(line, where) {
        return deprecatedFieldTag.exec(line.substring(where));
    }

    const gcmemberFieldTag = new RegExp("GCMember<.*>\\s*(.*;)");
    const gcPointerRegex = new RegExp("GC_POINTER\\((.*)");

    function matchGCMemberFields(line, where) {
        return gcmemberFieldTag.exec(line.substring(where));
    }

    function stackToCppPrefix() {
        var pfx="";
        for ( var i=0; i < cppClassStack.length; i++)
            pfx += cppClassStack[i] + "::";
        return pfx;
    }

    const nativeAnnotationRegex:RegExp = /^\[native\s*\((.*)\)\s*\]/;

    function sanitizeCondition(s) {
        s = s.replace(/^\s*|\s*$/g, "");
        s = s.replace(/\/\*.*\*\/\s*$/g, "");  // strip /* .. */ comments first in case it contains an embedded // 
        s = s.replace(/\/\/.*$/g, "");
        s = s.replace(/\s*$/g, "");
        return s;
    }

    // FIXME: Additional error checking we could add here:
    //  - only one data section per class, globally

    function processFile(filename) {
        cppDataStack.length = 0;
        cppClassStack.length = 0;
        cppIfStack.length = 0;

        errorContext = "On reading " + filename;

        const beforeReadLines = new Date();
        const text = readLines(filename);
        const afterReadLines = new Date();

        const beforeProcessing = new Date();
        const cppfile = Boolean(filename.match(/\.(h|cpp)$/));
        const hfile = Boolean(filename.match(/\.h$/));
        const as3file = Boolean(filename.match(/\.as$/));
        var lineno = 0;

        for ( var i=0 ; i < text.length ; i++ ) {
            var line = text[i];
            var result;

            lineno++;

            // Quick precomputation to filter out lines that we don't
            // need to examine any further with regular expressions.  The
            // regex search will start at the known good location.

            var gcIndex = -1;
            var nativeIndex = -1;
            var cppIndex = -1;
            var gcmemberIndex = -1;

            if (!as3file) {
                gcIndex = line.indexOf("GC_");
                cppIndex = line.indexOf("#"); // #ifdef ?
                gcmemberIndex = line.indexOf("GCMember"); // gcmember?
            }
            if (!cppfile)
                nativeIndex = line.indexOf("[native");
            if (gcIndex == -1 && cppIndex == -1 && nativeIndex == -1 && gcmemberIndex == -1)
                continue;

            // The start we found above may be inside a comment.  Here we handle
            // line comments; it would be better if we could handle block comments too.
            // Bugzilla 649333 tracks the block comment issue.

            if (line.match(/^\s*\/\//))
                continue;

            errorContext = "On " + filename + " line " + lineno;

            if (cppIndex >= 0) {
                // CPP macro?
                //
                // There are two options for handling elif / else:
                //
                //  - break them out as separate #ifdefs by accumulating (and negating) conditions
                //  - preserve the elif / else structure
                //
                // I've gone with the former because it fits the structure of existing code, but
                // if the resulting conditions end up being messy, or it becomes unreliable for
                // any reason, then we should reconsider. In practice most conditions are simple
                // and #if..elif chains are not long. The main liability is that we sort field names
                // lexicographically so clauses may become reorganized on output; this does not lead
                // to bugs but makes the code confusing to read.

                // Match the whole line to make it more likely to weed out lines inside block comments,
                // for example we'll catch the typical column of asterisks inside a block comment this way.
                // It's heuristic; bugzilla 649333 tracks the problem.

                if ((result = (/^\s*#\s*(ifdef|ifndef|if|elif|else|endif|define)/).exec(line)) != null) {
                    var rest = sanitizeCondition(line.substring(result[0].length));
                    var directive = result[1];
                    switch (directive) {
                    case "ifdef":
                        cppIfStack.push(new Condition(Condition.IFDEF, rest));
                        break;
                    case "ifndef":
                        cppIfStack.push(new Condition(Condition.IFNDEF, rest));
                        break;
                    case "define":
                        // This is a hack, but it's necessary: if this is a header file, and the top element 
                        // is #ifndef, and the condition for that element is the same name that is being 
                        // defined here, then we replace the top element with an inert element.  This takes
                        // care of the include-a-file-once idiom that would otherwise be hard to handle.
                        if (hfile && 
                            cppIfStack.length > 0 &&
                            rest == cppIfStack[cppIfStack.length-1].cond) {
                            cppIfStack.pop();
                            cppIfStack.push(new Condition(Condition.INERT, rest));
                        }
                        break;
                    case "if":
                        cppIfStack.push(new Condition(Condition.IF, rest));
                        break;
                    case "elif":
                        cppIfStack.length > 0 || fail("found #elif without matching #if, #ifdef, or #ifndef: " + line);
                        var top = cppIfStack.pop();
                        cppIfStack.push(new Condition(Condition.ELIF, rest, top));
                        break;
                    case "else": {
                        cppIfStack.length > 0 || fail("found #else without matching #if, #ifdef, or #ifndef: " + line);
                        var top = cppIfStack.pop();
                        cppIfStack.push(new Condition(Condition.ELSE, null, top));
                        break;
                    }
                    case "endif":
                        cppIfStack.length > 0 || fail("found #endif without matching #if, #ifdef, or #ifndef: " + line);
                        cppIfStack.pop();
                        break;
                    }
                }
            }

            // Filters for GCMember edge cases
            if(gcmemberIndex >= 0) {
                line = line.replace(/GCMember\s*</g, "GCMember<");
                var singlelineCommentIndex = line.indexOf("//");
                // GCMember inside a single line comment: " uint64_t* GC_POINTER( m_invocationCounts ); // GCMember<> does not work.... "
                if(singlelineCommentIndex >=0 && gcmemberIndex > singlelineCommentIndex)
                    gcmemberIndex = -1;

                var slashStarStartIndex = line.indexOf("/*");
                var slashStarEndIndex = line.indexOf("*/");
                // GCMember inside a single line comment: " uint64_t* GC_POINTER( m_invocationCounts ); /* GCMember<> does not work */ "
                if(gcmemberIndex > slashStarStartIndex && gcmemberIndex < slashStarEndIndex)
                    gcmemberIndex = -1;

                if(gcmemberIndex != -1) {
                    gcmemberIndex = line.indexOf("GCMember<");
                    if(gcmemberIndex == -1) {
                        print(line);
                        fail("GCMember incorrectly formatted. Must provide the member name on the same line!!");
                    }
                }
            }

            // For line matching of GC_ directives we match only at
            // the start of the line after taking the substring
            // starting at the known-good index.  This yields the same
            // performance as using a global regex and setting
            // lastIndex to indicate where we want to start matching.

            if (gcIndex >= 0) {
                // C++ annotations.

                if ((result = matchCppMetaTag(line, gcIndex)) != null) {
                    reportMatch(line);
                    var v = positionalAttrs(result[1], result[2], ")", null);
                    if (v is GCDataBegin || v is GCNoData) {
                        if (cppClassStack.length == 0 || cppClassStack[cppClassStack.length-1] != v.cls)
                            fail("Mismatched " + result[1] + " here: " + v.cls);
                        cppDataStack.push(v.cls);
                    }
                    if (v is GCDataEnd || v is GCNoData) {
                        var top = currentClassName();
                        if (v.cls != top)
                            fail(result[1] + " for " + v.cls + " but " + top + " is on the stack top");
                        cppDataStack.pop();
                        cppClassStack.pop();
                    }
                    if (!(v is GCDataSection)) {
                        if (v is GCClass) {
                            v.fullClassPrefix = stackToCppPrefix();
                            cppClassStack.push(v.cls);
                        }
                        specs.push(v);
                    }
                }
                else if ((result = matchCppFieldTag(line, gcIndex)) != null) {
                    reportMatch(line);
                    // If its a GCMember, lets just do the work below
                    if(gcmemberIndex == -1) {
                        var v = positionalAttrs(result[1], result[2], ")", currentClassName());
                        specs.push(v);
                    }
                }
                else {
                    if(line.indexOf("_IF") >= 0 && matchDeprecatedFieldTag(line, gcIndex) != null) {
                        print(line);
                        fail("The above annotation has _IF/_IFDEF/_IFNDEF!! _IF etc. are no longer required on member annotations and should be removed.");
                    }
                }
            }

            if(gcmemberIndex >= 0) {
                // Ignore GCMembers in non-exact classes
                if(cppClassStack.length == 0 || cppDataStack.length == 0) {
                    continue;
                }

                if((result = matchGCMemberFields(line, gcmemberIndex)) != null) {
                    reportMatch(line);

                    // Check if this already has a GC_POINTER annotation (for backward compatibility). This should ideally go away,
                    // Once there are no DWB etc, we can get rid of all GC_POINTER annotations altogether.
                    var gcIndexNow = line.indexOf("GC_");
                     if(gcIndexNow >= 0) {
                        // Remember we did not handle these above with GC_POINTER condition
                        // Need to get the member name. This doesn't take care of multiple gcmembers desclarations in a line
                        // like GCMember<SomeClass> GC_POINTER(x), y, z;
                        var new_result = gcPointerRegex.exec(result[1]);

                        var v = positionalAttrs("GC_POINTER", new_result[1], ")", currentClassName());
                        specs.push(v);
                    }
                    else {
                        // Handles multiple GCMember declaration of same class: GCMember<SomeClass> x, y, z;
                        pushGCmembers(result[1], currentClassName());
                    }
                }
                else {
                    print(line);
                    fail("GCMember incorrectly formatted. Must provide the member name on the same line!!");
                }
            }

            if (nativeIndex >= 0) {
                // AS3 annotations.
                //
                // For AS3 annotations we collect C++ class names if the [native] spec says
                // that the C++ class should be exactly traced.

                if ((result = nativeAnnotationRegex.exec(line.substring(nativeIndex))) != null) {
                    reportMatch(line);
                    var attr = parseNamedAttrs(result[1]);
                    var flags = {};
                    for ( var j=0 ; j < attr.length ; j++ ) {
                        if (attr[j] is Array)
                            flags[attr[j][0]] = attr[j][1];
                    }
                    if ("cls" in flags && ("classgc" in flags || "gc" in flags))
                        specs.push(new AS3Class(cppNamespace, flags["cls"]));
                    if ("instance" in flags && ("instancegc" in flags || "gc" in flags))
                        specs.push(new AS3Class(cppNamespace, flags["instance"]));
                }
            }
        }

        if (cppDataStack.length != 0)
            fail("Missing GC_DATA_END for " + currentClassName());

        if (cppClassStack.length != 0)
            fail("Missing GC_DATA_BEGIN/GC_DATA_END for these: " + cppClassStack);

        const afterProcessing = new Date();

        readingTime += (afterReadLines - beforeReadLines);
        processingTime += (afterProcessing - beforeProcessing);
    }

    for ( var i=0 ; i < files.length ; i++ )
        processFile(files[i]);

    if (profiling) {
        print("  reading time = " + readingTime/1000 + "s");
        print("  processing time = " + processingTime/1000 + "s");
        print("    splitting time = " + splittingTime/1000 + "s");
    }
}

function isVariableLength(t) {
    return t is GCPointers || t is GCAtoms || t is GCStructures;
}

// Populate as3ClassList, as3ClassMap, cppClassList, and cppClassMap.
// Sort the lists.

function collectClasses() 
{
    errorContext = "Collecting classes";

    for ( var i=0, limit=specs.length ; i < limit ; i++ ) {
        var s = specs[i];
        if (s is GCAS3Exact || s is GCCppExact) {
            var clsname = s.cls;
            if (cppClassMap.hasOwnProperty(clsname)) 
                fail("Duplicate " + (s is GCAS3Exact ? "GC_AS3_EXACT" : "GC_CPP_EXACT") + " spec: " + clsname);
            cppClassList.push(s);
            cppClassMap[clsname] = s;
        }
        else if (s is AS3Class) {
            var clsname = s.cls;
            if (as3ClassMap.hasOwnProperty(clsname)) {
                // Completely gross hack in the vector code - the instance for VectorClass and ObjectVectorClass
                // are both ObjectVectorObject.  Just work around it for now with this gross hack.
                if (clsname == "ObjectVectorObject")
                    continue;
                fail("Duplicate AS3 native spec: " + clsname);
            }
            as3ClassList.push(s);
            as3ClassMap[clsname] = s;
        }
    }

    as3ClassList.sort(function (a,b) { return strcmp(a.cls,b.cls) });
    cppClassList.sort(function (a,b) { return strcmp(a.cls,b.cls) });

    if (debug) {
        print("C++ class map: " + cppClassMap);
        print("AS3 class map: " + as3ClassMap);
    }
}

// Check that the sets of classes correspond: every AS3 class must be
// a C++ GC_AS3_EXACT class, and vice versa.  GC_CPP_EXACT classes must
// not have AS3 counterparts.

function checkClasses()
{
    errorContext = "Checking class correspondence";

    for ( var i=0 ; i < cppClassList.length ; i++ ) {
        var c = cppClassList[i];
        var n = c.cls;
        var probe = as3ClassMap.hasOwnProperty(n);
        if (c is GCCppExact) {
            if (probe)
                fail("AS3 side may not be defined for GC_CPP_EXACT " + n);
            continue;
        }
        if (!probe)
            fail("AS3 side is missing annotation for " + n);
    }

    for ( var i=0 ; i < as3ClassList.length ; i++ ) {
        var c = as3ClassList[i];
        var n = c.cls;
        if (!cppClassMap.hasOwnProperty(n))
            fail("C++ side is missing annotation for " + n);
        if (!(cppClassMap[n] is GCAS3Exact))
            fail("C++ side is not a GC_AS3_EXACT class for " + n);
    }
}

// Add fields to classes, check for errors in class names, duplicates, etc.
// Sort the fields by name.

function collectFields()
{
    errorContext = "Collecting fields";

    for ( var i=0 ; i < specs.length ; i++ ) {
        var s = specs[i];
        if (s is GCClass || s is AS3Class)
            continue;
        if (!cppClassMap.hasOwnProperty(s.cls))
            fail("Bad field annotation - unknown class: " + s.cls + " in " + s);
        var c = cppClassMap[s.cls];
        var fieldname = s.name;
        if (s.if_) fieldname += "!if!" + s.if_;
        if (s.ifdef) fieldname += "!ifdef!" + s.ifdef;
        if (s.ifndef) fieldname += "!ifndef!" + s.ifndef;
        if (s.cond) fieldname += "!cond!" + makeConditions(s.cond);
        if (isVariableLength(s)) {
            // FIXME: It would be good to loosen the following restriction up; it 
            // would be sufficient to decree that only one of the arrays can be 
            // large.  It is rarely the case that pointers or atom arrays are anything
            // but trailing (and then there's only one), but eg MethodInfo has 
            // an inline fixed-size pointer array for the lookup cache, and if it 
            // were to have a trailing array as well then we'd run into this restriction.
            if (c.variable_length_field != null)
                fail("Arbitrary restriction: More than one variable length field on " + c);
            if (c.fieldMap.hasOwnProperty(fieldname))
                fail("Duplicate field name: " + s.name + "; canonically " + fieldname);
            c.variable_length_field = s;
        }
        else {
            if (c.fieldMap.hasOwnProperty(fieldname) || (c.variable_length_field != null && c.variable_length_field.name == fieldname))
                fail("Duplicate field name: " + s.name + "; canonically " + fieldname);
            c.fieldMap[fieldname] = s;
            c.fieldList.push(s);
        }
    }

    for ( var i=0 ; i < cppClassList.length ; i++ )
        cppClassList[i].fieldList.sort(function (a,b) { return strcmp(a.name,b.name) });
}

// For each class compute whether it's likely to be large or small,
// this will influence how we generate code.  
// A manifestly large number of fixed fields overrides any "small" hint.
// A pointer array will make us assume the object is large unless
// the hint is that it's small.

function computeLargeOrSmall()
{
    for ( var i=0 ; i < cppClassList.length ; i++ ) {
        var c = cppClassList[i];
        if (c.fieldList.length >= largeObjectCutoff/4) // "4" is a proxy for word size, not correct on 64-bit systems but OK for this purpose
            c.probablyLarge = true;
        else if (c is GCCppExact && c.variable_length_field != null) {
            c.hint = c.variable_length_field.hint;
            c.probablyLarge = (c.hint != "small")
        }
    }
}

function noUsefulTracer(n)
{
    n = n.replace(/^MMgc::/,"");
    switch (n) {
    case "GCRoot":
    case "GCFinalizedObject":
    case "GCTraceableObject":
    case "RCObject":
    case "GCInlineObject":
    case "AvmPlusScriptableObject":
        return true;
    default:
        return false;
    }
}

// This stack is being used to filter our output so that we do not print
// redundant conditions.

const pendingConditions = [];

function matchesPendingCondition(c) {
    for ( var i=0 ; i < pendingConditions.length ; i++ ) {
        var x = pendingConditions[i];
        if (c.type == x.type && c.cond == x.cond)
            return true;
    }
    return false;
}

// Explicit _IFDEF etc annotations take precedence over computed annotations,
// this allows the explicit annotations to be used as an escape valve.

function ifdefOpen(out, fieldOrClass)
{
    if (fieldOrClass.ifdef || fieldOrClass.ifndef || fieldOrClass.if_) {
        if (fieldOrClass.ifdef)
            out.PR("#ifdef " + fieldOrClass.ifdef);
        if (fieldOrClass.ifndef)
            out.PR("#ifndef " + fieldOrClass.ifndef);
        if (fieldOrClass.if_)
            out.PR("#if " + fieldOrClass.if_);
    }
    else {
        var conds = fieldOrClass.cond;
        for ( var i=0 ; i < conds.length ; i++ ) {
            if (conds[i].type == Condition.INERT)
                continue;
            var skip = matchesPendingCondition(conds[i]);
            pendingConditions.push(conds[i]);
            if (skip)
                continue;
            out.PR("#if " + makeCondition(conds[i]));
        }
    }
}

function ifdefClose(out, fieldOrClass, withName = false)
{
    if (fieldOrClass.ifdef || fieldOrClass.ifndef || fieldOrClass.if_)
        out.PR("#endif // " + (fieldOrClass.ifdef || fieldOrClass.ifndef || fieldOrClass.if_));
    else {
        var conds = fieldOrClass.cond;
        for ( var i=0 ; i < conds.length ; i++ ) {
            if (conds[i].type == Condition.INERT)
                continue;
            var x = pendingConditions.pop();
            if (matchesPendingCondition(x))
                continue;
            out.PR("#endif" +( withName ? (" // " + makeCondition(x)) : ""));
        }
    }
}

// Returns something of this form:
//
//   C ::= defined( N )
//       | ( E )
//       | !C
//       | C && C
//
//   N ::= <some identifier>
//   E ::= <some expression>

function makeCondition(c, negate=false) {
    function neg(x) {
        if (!negate)
            return x;
        if (x.charCodeAt(0) == "!")
            return x.substring(1);
        return "!" + x;
    }
    switch (c.type) {
    case Condition.IF:
        return neg("(" + c.cond + ")");
    case Condition.IFDEF:
        return neg("defined(" + c.cond + ")");
    case Condition.IFNDEF:
        return neg("!defined(" + c.cond + ")");
    case Condition.ELIF:
        // Here we put the butnot condition first to make it more obvious what's going on,
        // though in practice it'll be pretty confusing to read the code since the field
        // names are sorted lexicographically (earlier in this script) and may be sorted
        // in some bizarre order at some future time in order to make multi-argument calls
        // to TraceLocation.  See comments in the parsing code above about the plusses
        // and minuses of breaking conditions apart rather than sticking with the if-elif-else
        // structure.
        return makeCondition(c.butnot, true) + " && " + neg("(" + c.cond + ")");
    case Condition.ELSE:
        return makeCondition(c.butnot, true);
    case Condition.INERT:
        return "1";
    default:
        fail("Internal error: makeCondition does not understand this condition : " + c);
    }
}

// A hack, used for generating unique names way up above.

function makeConditions(cs) {
    return cs.map(function (x) { return makeCondition(x,false) }).join("!");
}

// If a class is large it could be because it has a pointer array or
// because it has many fixed fields or both.  It is pretty much never
// going to be the case that it has many fixed fields so we always
// trace the fixed fields for cursor==0, technically that's suboptimal
// for incrementality but not a problem for correctness.

function constructTracerBodies()
{
    function traceField(out, field) {
        ifdefOpen(out, field);
        try {
            throw field;
        }
        catch (f: GCPointer)      { out.PR("gc->TraceLocation(&" + field.name + ");") }
        catch (f: GCConservative) { out.PR("gc->TraceConservativeLocation(&" + field.name + ");") }
        catch (f: GCStructure)    { out.PR(field.name + ".gcTrace(gc);") }
        catch (f: GCAtom)         { out.PR("gc->TraceAtom(&" + field.name + ");") }
        catch (f: *) {
            fail("Unknown type to trace: " + field);
        }
        ifdefClose(out, field);
    }

    // Here we can do better: we can collect the fields that have the
    // same pointer attribute and the same ifdef attribute, and then
    // emit each group in a chunk.  That will allow us to use the
    // multi-argument tracing functions when they're available.

    function emitFixedFields(c) {
        for ( var j=0 ; j < c.fieldList.length ; j++ ) {
            var f = c.fieldList[j];
            if (isVariableLength(f))
                fail("Invariant failure: there should be no variable length field among the fixed fields: " + f);
            traceField(c.out, f);
        }
    }

    function emitChunk(out, f, start, len) {
        try { 
            throw f;
        }
        catch (f:GCPointers)   { c.out.PR("gc->TraceLocations((" + f.name + "+" + start + "), " + len + ");"); }
        catch (f:GCAtoms)      { c.out.PR("gc->TraceAtoms((" + f.name + "+" + start + "), " + len + ");"); }
        catch (f:GCStructures) {
            c.out.
                PR("for ( size_t _xact_iter=0 ; _xact_iter < " + len + "; _xact_iter++ )").
                IN().
                PR(f.name + "[+_xact_iter+" + start + "].gcTrace(gc);").
                OUT();
        }
        catch (f:*) {
            fail("Unknown variable length field type: " + f);
        }
    }

    // Important that the names introduced here do not shadow the ones
    // in the class so we prefix locals with _xact_.

    function emitArrayChunked(c) {
        c.out.
            PR("const size_t _xact_work_increment = " + largeObjectCutoff + "/sizeof(void*);").
            PR("const size_t _xact_work_count = " + c.variable_length_field.count + ";").
            PR("if (_xact_cursor * _xact_work_increment >= _xact_work_count)").
            IN().
            PR("return false;").
            OUT().
            PR("size_t _xact_work = _xact_work_increment;").
            PR("bool _xact_more = true;").
            PR("if ((_xact_cursor + 1) * _xact_work_increment >= _xact_work_count)").
            PR("{").
            IN().
            PR("_xact_work = _xact_work_count - (_xact_cursor * _xact_work_increment);").
            PR("_xact_more = false;").
            OUT().
            PR("}");
        emitChunk(c.out, c.variable_length_field, "(_xact_cursor * _xact_work_increment)", "_xact_work");
        c.out.PR("return _xact_more;");
    }

    function emitArrayUnchunked(c) {
        emitChunk(c.out, c.variable_length_field, "0", c.variable_length_field.count);
    }

    // FIXME: Not currently using this, but we could use it - probably elsewhere - to flag
    // probably-incorrect code.

    function noCredibleTracer(n) {
        switch (n) {
        case "MMgc::GCObject":
        case "GCObject":
            return true;
        default:
            return false;
        }
    }

    function cleanupNs(name) {
        // If the name has a namespace, keep it
        // Otherwise prepend the current output namespace
        if (name.match("::"))
            name = name.replace(/::/g, "_");
        else
            name = cppNamespace + "_" + name;
        // Brockets appear in template expansions: Fnord<A>
        // Spaces appear in nested template expressions: Fnord< A<B> >
        return name.replace(/ /g, "").replace(/<|>/g, "X");
    }

    errorContext = "Accumulating bodies";

    // Open and close ifdefs for the class on a dummy printer to avoid redundant
    // ifdefs.  We need to do this because the tracer bodies are emitted separately
    // and pasted into functions later.

    var dummy = new Printer();

    for ( var i=0 ; i < cppClassList.length ; i++ ) {
        var c = cppClassList[i];

        ifdefOpen(dummy, c);

        // The interlock is just a #define with that name emitted at
        // the beginning of the output, we'll get a compilation error
        // if it's not defined.  This way we ensure that every exactly
        // traced class has an exactly traced base class.

        if (c.base != null && !noUsefulTracer(c.base)) {
            c.out.PR(c.base + "::gcTrace(gc, 0);");
            if (interlockOutputFile != null)
                c.out.PR("(void)(" + cleanupNs(c.base) + "_isExactInterlock != 0);");
        }

        if (c.hook)
            c.out.PR("gcTraceHook_" + c.cls + "(gc);");

        if (c.probablyLarge) {
            if (c.fieldList.length > 0) {
                c.out.
                    PR("if (_xact_cursor == 0) {").
                    IN();
                emitFixedFields(c);
                c.out.
                    OUT().
                    PR("}");
            }
            if (c is GCClass && c.variable_length_field != null)
                emitArrayChunked(c);
        }
        else {
            emitFixedFields(c);
            if (c is GCClass && c.variable_length_field != null)
                emitArrayUnchunked(c);
        }

        ifdefClose(dummy, c, true);
    }
}

function constructAndPrintTracers()
{
    var interlocks = interlockOutputFile ? new Printer() : null;
    var builtins = builtinOutputFile ? new Printer() : null;
    var natives = nativeOutputFile ? new Printer() : null;

    function emitInterlock(c) {
        if (interlocks)
            interlocks.PR("#define " + cppNamespace + "_" + c.cls + "_isExactInterlock 1");
    }

    function emitSafetyNet(c,output)
    {
        output.
            PR("#ifdef DEBUG");
        
        if(c.fieldList.length > 0) {
            output.
                PR("const uint32_t " + c.fullName() + "::gcTracePointerOffsets[] = {").
                IN();
            for ( var j=0 ; j < c.fieldList.length ; j++ ) {
                var f = c.fieldList[j];
                ifdefOpen(output, f);
                output.
                    PR("offsetof("+c.fullName()+", "+f.name+"),");
                ifdefClose(output, f);
            }
            output.
                PR("0};").
                OUT().
                NL();
        }
        output.
            PR("MMgc::GCTracerCheckResult " + c.fullName() + "::gcTraceOffsetIsTraced(uint32_t off) const").
                PR("{").
            IN().
            PR("MMgc::GCTracerCheckResult result;").
            PR("(void)off;").
            PR("(void)result;");
        
        // invoke GC_STRUCTURE checkers on types we've seen
        for ( var j=0 ; j < c.fieldList.length ; j++ ) {
            var f = c.fieldList[j];
            if (f is GCStructure) {
                ifdefOpen(output, f);
                output.
                    PR("if((result = " + f.name + ".gcTraceOffsetIsTraced(off - offsetof("+c.cls+","+f.name+"))) != MMgc::kOffsetNotFound) {").
                    IN().
                    //PR("GC::ExactTraceManualWarning(" + QUOT(c.cls) + ", " + f.name + ");").
                    PR("return result;").
                    OUT().
                    PR("}");
                ifdefClose(output, f);
            }
        }

        // invoke super class checker
        if (!noUsefulTracer(c.base)) {
            output.
                PR("if((result = " + c.base + "::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)").
                    IN().
                PR("return result;").
                OUT();
        }
        
        if(c.fieldList.length > 0)
            output.
                PR("return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,"+c.fieldList.length+");");
        else
            output.
                PR("return MMgc::kOffsetNotFound;");
        
        output.
            OUT().
            PR("}").
            PR("#endif // DEBUG").
            NL();
    }

    function emitTracers()
    {
        for ( var i=0 ; i < cppClassList.length ; i++ ) {
            var c = cppClassList[i];
            var output = null;
            if (c is GCCppExact) {
                if (!natives)
                    continue;
                output = natives;
            }
            else {
                if (!builtins)
                    continue;
                output = builtins;
            }

            emitInterlock(c);

            ifdefOpen(output, c);
            output.NL();
                     
            // Safety net stuff
            emitSafetyNet(c,output);

            // IMPORTANT NOTE: The code for emitting gcTrace() bodies is essentially
            // replicated in exactgc.as (for handwritten C++ classes) and in nativegen.py 
            // (for synthetically-generated C++ classes representing pure AS3 builtins).
            // This is a highly undesirable situation which is intended to be temporary;
            // we should really unify this code into a single place. At the time of
            // this writing, however (May 2011) it's an expediency we are willing to live with.
            // Until these are unified, please keep in mind that any changes to one may
            // necessitate similar changes to the other.
            if (c.probablyLarge) {
                output.
                    PR("bool " + c.fullName() + "::gcTrace(MMgc::GC* gc, size_t _xact_cursor)").
                    PR("{").
                    DO(function (output) {
                            if (output === builtins) {
                                output.
                                    IN().
                                    PR("#ifndef GC_TRIVIAL_TRACER_" + c.cls).
                                    PR("if (_xact_cursor == 0)").
                                    PR("{").
                                    IN().
                                    PR("m_slots_" + c.cls + ".gcTracePrivateProperties(gc);").
                                    OUT().
                                    PR("}").
                                    PR("#endif").
                                    OUT();
                            }
                        }).
                    PR(c.out.get()).
                    PR("}").
                    NL();
            }
            else {
                output.
                    PR("bool " + c.fullName() + "::gcTrace(MMgc::GC* gc, size_t _xact_cursor)").
                    PR("{").
                    IN().
                    PR("(void)gc;").
                    PR("(void)_xact_cursor;").
                    DO(function (output) {
                            if (output === builtins) {
                                output.
                                    PR("#ifndef GC_TRIVIAL_TRACER_" + c.cls).
                                    PR("m_slots_" + c.cls + ".gcTracePrivateProperties(gc);").
                                    PR("#endif");
                            }
                        }).
                    OUT().
                    PR(c.out.get()).
                    IN().
                    PR("return false;").
                    OUT().
                    PR("}").
                    NL();
            }
            ifdefClose(output,c, true);
            output.NL();
        }
    }

    function emitDelegates()
    {
        var output = "";
        for ( var i=0 ; i < cppClassList.length ; i++ ) {
            var c = cppClassList[i];
            if (c.base != null && c.base != "")
                output += ("#define GCDELEGATE_" + c.cls + " " + c.base + "\n");
        }
        return output;
    }

    function printToFile(fn, txt) {
        File.write(fn, txt);
    }

    function nsOpen()
    {
        if(cppNamespace == "")
            return "\n";
        else
            return "namespace " + cppNamespace +"\n{\n";
    }

    function nsClose()
    {
        if(cppNamespace == "")
            return "\n";
        else
            return "}\n";
    }

    errorContext = "Emitting code";

    emitTracers();

    const doNotEdit = "\n/* machine generated file via utils/exactgc.as -- do not edit */\n\n";

    if (builtinOutputFile) 
        printToFile(builtinOutputFile, 
                    (LICENSE + 
                     doNotEdit +
                     nsOpen() +
                     builtins.get() + 
                     (nativeOutputFile == builtinOutputFile ? natives.get() : "") +
                     nsClose()));

    if (nativeOutputFile && nativeOutputFile != builtinOutputFile)
        printToFile(nativeOutputFile,
                    (LICENSE + 
                     doNotEdit +
                     nsOpen() +
                     natives.get() + 
                     nsClose()));

    if (interlockOutputFile)
        printToFile(interlockOutputFile,
                    (LICENSE + 
                     doNotEdit +
                     interlocks.get() + "\n"));
}

function profile(what, thunk)
{
    var then = new Date();
    var result = thunk();
    var now = new Date();
    if (profiling)
        print(what + ": " + (now - then)/1000 + "s");
    return result;
}

function main()
{
    profile("readFiles", function() { readFiles(processOptionsAndFindFiles(System.argv)) });
    profile("collectClasses", collectClasses);
    profile("checkClasses", checkClasses);
    profile("collectFields", collectFields);
    profile("computeLargeOrSmall", computeLargeOrSmall);
    profile("constructTracerBodies",constructTracerBodies);
    profile("constructAndPrintTracers",constructAndPrintTracers);
}

profile("main", main);
