/* -*- indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Input is a bunch of file names and/or names of files from which to read file names.
//
// Output is a tree of classes derived from a root class (default GCFinalizedObject),
// with per-class annotations:
//
//   - a "D" if the class has an explicit Destructor
//   - a "E" if the class is Exactly traced
//
// Each class is printed with the file and line on which the definition starts.
//
// At the end some statistics are printed.
//
// Assumes names are globally unique (namespaces don't matter).  Is tricked by macro
// definitions like in the Error objects, and by the use of the bitwise not operator
// in some cases:   "exception->flags &= ~Exception::SEEN_BY_DEBUGGER;"
//
// Furtermore there will be generated destructors for classes that have substructures,
// and those destructors won't be visible here.
//
// Usage:
//
//    avmshell gcClasses.abc -- [options] file ...
//
// Options:
//
//    --base classname  Specify the base class name, default GCFinalizedObject.
//               Useful values are GCObject, GCTraceableObject, and RCObject.
//
// Example:
//
//    find . -name '*.cpp' -o -name '*.h' > allfiles.txt
//    avmshell gcClasses.abc -- @allfiles.txt
//
// Example: (for TR, though not the player)
//
//    avmshell utils/gcClasses.abc -- $(find . -name '*.h' -o -name '*.cpp')
//
// Note the list of "excluded" paths below; those could be command line arguments but
// that's for later.

import avmplus.*;

// The root class could be a command line argument.

var rootclass = "GCFinalizedObject";

// Classes defined in files whose names match these regexes will not
// be printed, but will participate in the computation.

const ignorePaths =
    [ new RegExp("^.*/shell/.*$"),
     new RegExp("^.*/extensions/ST_.*$"),
     new RegExp("^.*/eval/.*$"),
     new RegExp("^.*/MMgc/GCTests.*$")];

class LineInfo
{
    function LineInfo(_derived, _base, _isExact, _extraBases, _filename, _lineno) {
        derived = _derived;
        base = _base;
        isExact = _isExact;
        extraBases = _extraBases;
        filename = _filename;
        lineno = _lineno;
    }

    public function toString() {
        return "derived=" + derived + ", base=" + base + ", filename=" + filename + ", lineno=" + lineno + ", defcount=" + defcount;
    }

    var derived;            // name of the class being defined
    var base;               // name of its base class
    var isExact;            // true iff the class is exactly traced
    var extraBases;         // array of base classes beyond 'base' (usually []).
    var filename;           // file where we found the definition of 'derived'
    var lineno;             // line where we found the definition of 'derived'
    var baseref = null;     // reference to the LineInfo for the base class
    var subrefs = [];       // list of known subclasses
    var destructor = false; // true if we believe this has a destructor
    var defcount = 0;       // number of definitions
    var otherlines = [];    // for in-tree lines beyond the first: locations of definition: { filename:x, lineno:n }
    var exolines = [];      // for out-of-tree lines: locations of definition: { filename:x, lineno:n }
    var external = false;   // true if there are definitions of the 'derived' name not within the tree
}

main(System.argv);

function main(args) {
    if (args.length > 0 && args[0] == "--base") {
        if (args.length > 1) {
            rootclass = args[1];
            args.shift();
            args.shift();
        }
        else
            throw new Error("Missing class name to --base");
    }
    printClasses(processQueue(readFiles(args)));
}

function readFiles(argv) {
    var filenames_dict = {};
    var probable_destructors = {};
    var probable_tracers = {};
    var all_lines = [];
    // Hack, hack, hack
    // N.B.: ?<! is a negative-look-behind pattern, used here to avoid matching
    // friend declarations.
    var re1 = new RegExp("(?<!friend)\\s*class\\s+([A-Za-z_][A-Za-z0-9_]*)\\s*:(?!:)\\s*(?:(?:public|private|virtual)\\s)?\\s*(?:MMgc\\s*::\\s*)?([A-Za-z0-9_:]+)");
    var re2 = new RegExp("class\\s+GC_(?:MANUAL|AS3|CPP)_EXACT(?:[A-Z_]+)?\\(([^,\\)]+)\\s*,\\s*([^,\\)]+)");
    var re_inheritance_line =
        new RegExp("class\\s+([A-Za-z_][A-Za-z0-9_]*)\\s*:(.*)");
    var re_first_base_class =
        new RegExp("\\s*(?:(?:public|private|virtual)\\s)?\\s*(?:MMgc\\s*::\\s*)?([A-Za-z0-9_:]+)\\s*(.*)");
    var re_next_base_class =
        new RegExp("\\s*,\\s*(?:(?:public|private|virtual)\\s)?\\s*(?:MMgc\\s*::\\s*)?([A-Za-z0-9_:]+)\\s*(.*)");
    var re_no_next_base_class_on_this_line =
        new RegExp("\\s*,\\s*(?:(?:public|private|virtual)\\s)?\\s*$");
    var red = new RegExp("~\\s*([A-Za-z_][A-Za-z0-9_]*)\\s*\\(");
    var red2 = new RegExp("virtual\\s*~\\s*([A-Za-z_][A-Za-z0-9_]*)\\s*\\(.*\\)\\s*{\\s*}");
    var rtrace = new RegExp("([A-Za-z0-9_]+)\\s*::\\s*gcTrace");

    compute(argv,false);
    annotateDestructors();
    annotateTracers();
    return all_lines;

    function annotateDestructors() {
        for ( var i=0 ; i < all_lines.length ; i++ ) {
            var ln = all_lines[i];
            ln.destructor = probable_destructors.hasOwnProperty(ln.derived);
        }
    }

    function annotateTracers() {
        for ( var i=0 ; i < all_lines.length ; i++ ) {
            var ln = all_lines[i];
            ln.isExact = ln.isExact || probable_tracers.hasOwnProperty(ln.derived);
        }
    }

    function compute(argv) {
        for ( var i=0 ; i < argv.length ; i++ ) {
            var arg=argv[i];
            if (arg.charAt(0) == '@')
                compute(readFile(arg.substring(1)));
            else if (newFile(arg))
                filterFile(arg);
        }
    }

    function newFile(fn) {
        return File.exists(fn) && !filenames_dict.hasOwnProperty(fn);
    }

    function filterFile(fn) {
        var lines = readFile(fn);
        var ignore = false;
        var ignorelevel = 0;

        for ( var i=0 ; i < lines.length ; i++ ) {
            var l = lines[i];
            var result;
            // Faster to filter by indexOf and then do regex matching than just regex matching.
            if (l.indexOf("#") != -1) {
                // Preprocessor macro?
                // We need to know the regions commented out by #ifdef DRC_TRIVIAL_DESTRUCTOR .. #endif
                // but we must track nested regions too.
                if (l.match(/^\s*#\s*(?:ifdef|ifndef|if|elif)/)) {
                    if (l.match(/^\s*#\s*ifdef\s+DRC_TRIVIAL_DESTRUCTOR/))
                        ignore = true;
                    if (ignore)
                        ignorelevel++;
                }
                else if (l.match(/^\s*#\s*endif/)) {
                    if (ignore) {
                        --ignorelevel;
                        if (ignorelevel == 0)
                            ignore = false;
                    }
                }
            }

            if (l.indexOf("class") != -1) {
                var found = 0;
                // Order matters because we want commented-out "class GC_MANUAL_EXACT( ..." to take precedence
                // over the actual definition when we build the class tree.
                if ((result = l.match(re2)) != null)
                    found = 2;
                else if ((result = l.match(re1)) != null)
                    found = 1;
                if (found) {
                    var first_base;
                    var derived;
                    var extra_bases = [];
                    if (found == 2) {
                        derived = result[1];
                        first_base = result[2]
                    } else if (found == 1) {
                        result = l.match(re_inheritance_line);
                        derived = result[1];
                        var bases_text = result[2];
                        result = bases_text.match(re_first_base_class);
                        var first_base = result[1];
                        bases_text = result[2];
                        while (result = bases_text.match(re_next_base_class)) {
                            extra_bases.push(stripNamespaces(result[1]));
                            bases_text = result[2];
                        }
                        if (bases_text.match(re_no_next_base_class_on_this_line)) {
                            print(fn+":"+(i+1)+" WARNING: base class list of "+
                                  derived+" has trailing comma without name.");
                            // throw new Error("trailing comma without name.");
                        }
                    }
                    var base = stripNamespaces(first_base);
                    all_lines.push(new LineInfo(derived,base,found==2,extra_bases, fn,i+1));
                }
            }
            else if (!ignore && l.indexOf("~") != -1) {
                if (result = l.match(red)) {
                    if (!l.match(red2)) // filter out trivial destructors
                        probable_destructors[result[1]] = true;
                }
            }
            else if (!ignore && l.indexOf("gcTrace") != -1) {
                if (result = l.match(rtrace)) {
                    probable_tracers[result[1]] = true;
                }
            }
        }

        filenames_dict[fn] = true;
    }

    function stripNamespaces(name) {
        var x = name.lastIndexOf(":");
        return (x >= 0) ? name.substring(x+1) : name;
    }
}

function readFile(fn) {
    try {
        return File.read(fn).split("\n");
    }
    catch (e) {
        print("Can't open file \"" + "\"\n" + e);
        return [];
    }
}

function processQueue(lines) {
    var queue = [];     // Queue of classes to process
    var classes = {};   // Map of classes processed or in queue to their LineInfo

    queue.push(rootclass);
    classes[rootclass] = new LineInfo(rootclass, "", false, [], "(internal)", 0); // synthetic

    while (queue.length > 0) {
        var cls = queue.shift();
        for ( var l=0 ; l < lines.length ; l++ ) {
            var ln = lines[l];
            var baseNames = [ln.base];
            baseNames = baseNames.concat(ln.extraBases);
            for ( var b=0; b < baseNames.length ; b++ ) {
                var baseName = baseNames[b];
                if (classes.hasOwnProperty(baseName)) {
                    if (!classes.hasOwnProperty(ln.derived)) {
                        var basecls = classes[baseName];
                        ln.baseref = basecls;
                        basecls.subrefs.push(ln);
                        classes[ln.derived] = ln;
                        ln.defcount = 1;
                        queue.push(ln.derived);
                    }
                    else {
                        // Flag it as used, but it's not canonical because
                        // it's not in the classes table.
                        ln.defcount = 1;
                    }
                }
            }
        }
    }

    // Compute lines for multiple in-tree definitions, and attributes for not-within-tree definitions

    for ( var l=0 ; l < lines.length ; l++ ) {
        var ln = lines[l];
        if (ln.defcount > 0 && classes.hasOwnProperty(ln.derived)) {
            var c = classes[ln.derived];
            if (c !== ln) {
                c.defcount++;
                c.otherlines.push({filename: ln.filename, lineno: ln.lineno});
            }
        }                
        if (ln.defcount == 0 && classes.hasOwnProperty(ln.derived)) {
            var c = classes[ln.derived];
            c.external = true;
            c.exolines.push({filename: ln.filename, lineno: ln.lineno});
        }
    }

    return classes;
}

function printClasses(classes) {
    var destructors = 0;
    var exacts = 0;
    var classcount = 0;

    print("Legend:");
    print("  D - at least one of the definitions has an explicit destructor");
    print("  E - at least one of the definitions is exactly traced");
    print("  M - multiple definitions in the tree (in different namespaces or nested within classes)");
    print("  # - continuation line for M with additional locations");
    print("  m - at least one of the definitions has > 1 base.");
    print("  X - definitions of the same name exist outside the tree");
    print("  @ - continuation line for @ with locations");
    print("");

    printCls(classes[rootclass], 0);
    print("");
    print("Classes: " + classcount);
    print("Explicit destructors: " + destructors);
    print("Exactly traced: " + exacts + " (" + Math.round(exacts/classcount*100) + "%)");

    function printCls(cls, indent) {
        if (!ignoreIt(cls)) {
            classcount++;
            if (cls.destructor) destructors++;
            if (cls.isExact) exacts++;
            var d = " " + (cls.destructor ? "D" : " ") + (cls.isExact ? "E" : " ") + (cls.extraBases.length > 0 ? "m" : " ") + (cls.defcount > 1 ? "M" : " ") + (cls.external ? "X" : " ") + "  ";
            print(d + padTo50(spaces(indent) + cls.derived) + " " + cls.filename + ":" + cls.lineno);
            if (cls.defcount > 1) {
                var locs = cls.otherlines;
                for ( var l=0 ; l < locs.length ; l++ )
                    print("    #    " + padTo50("") + locs[l].filename + ":" + locs[l].lineno);
            }
            if (cls.external) {
                var locs = cls.exolines;
                for ( var l=0 ; l < locs.length ; l++ )
                    print("     @   " + padTo50("") + locs[l].filename + ":" + locs[l].lineno);
            }
        }
        cls.subrefs.sort(function (a,b) { return strcmp(a.derived, b.derived); })
        for ( var i=0 ; i < cls.subrefs.length ; i++ ) {
            var c = cls.subrefs[i];
            printCls(c, indent+2);
        }
    }

    function ignoreIt(cls) {
        for ( var i=0 ; i < ignorePaths.length ; i++ ) {
            if (cls.filename.match(ignorePaths[i]))
                return true;
        }
        return false;
    }

    function padTo50(s) {
        return (s.length >= 50) ? s : padTo50(s + " ");
    }

    function spaces(n) {
        return (n == 0) ? "" : " " + spaces(n-1);
    }

    function strcmp(a,b) {
        if (a < b) return -1;
        if (b < a) return 1;
        return 0;
    }
}

