/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil;  -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Util,
    namespace Util;

function assert(cond) {
    if (!cond)
        throw new Error("Assertion failed!");
}

// Together these two ensure that all compiler errors are thrown as SyntaxErrors.
// This is for the benefit of ActionMonkey, we can clean it up later.

internal function formatMsg(file, line, msg) {
    if (line || file)
        msg = " " + msg;
    if (line)
        msg = line + ":" + msg;
    if (file)
        msg = file + ":" + msg;
    return msg;
}

function syntaxError(file, line, msg) {
    msg = formatMsg(file, line, msg);
    var obj = new SyntaxError(msg);
    if (line)
        obj.line = line;
    if (file)
        obj.file = file;
    throw obj;
}

function internalError(file, line, msg) {
    Util::syntaxError(file, line, "Internal: " + msg);
}

function warning(file, line, msg) {
    print(formatMsg(file, line, "Warning: " + msg));
}
    
function map(fn, a) {
    var b = [];
    for ( var i=0, limit=a.length ; i < limit ; i++ )
        if (i in a)
            b[i] = fn(a[i]);
    return b;
}

function forEach(fn, a) {
    for ( var i=0, limit=a.length ; i < limit ; i++ )
        if (i in a)
            fn(a[i]);
}

function memberOf(x, ys) {
    for ( var i=0, limit=ys.length ; i < limit ; i++ ) {
        if (ys[i] === x)
            return true;
    }
    return false;
}

function copyArray(c) {
    var a = new Array;
    for ( var i=0, limit=c.length ; i < limit ; i++ )
        a[i] = c[i];
    return a;
}

// push elements of a2 onto a1
function pushOnto(a1, a2) {
    for ( let i=0, limit=a2.length ; i < limit ; i++ )
        a1.push(a2[i]);
    return a1;
}

function toUint(x)
    uint(x);

function hash_number(n) {
    return uint(n);                       // Fairly arbitrary
}

// The type annotation and the explicit namespace has somewhat
// dramatic effects on performance.

function hash_string(s: String) {
    // See http://www.cse.yorku.ca/~oz/hash.html; this is the djb2 algorithm
    var h = 5381;
    for ( var i=0, limit=s.length ; i < limit ; i++ )
        h = ((h << 5) + h) + s."http://adobe.com/AS3/2006/builtin"::charCodeAt(i);
    return uint(h);
}

class Hashnode {
    var key = null;
    var value = null;
    var hashval = null;
    var link = null;

    function Hashnode(key, value, hashval) 
        : key     = key
        , value   = value
        , hashval = hashval
    {
    }
}

class Hashtable {
    var size = 10;
    var population = 0;
    var must_rehash = false;
    var tbl;
    var hashfn;
    var eqfn;
    var notfound;
    var debug;

    function Hashtable(hashfn, eqfn, notfound, debug=false)
        : hashfn   = hashfn
        , eqfn     = eqfn
        , notfound = notfound
        , debug    = debug
    {
        tbl = makeTable(size);
    }

    // Should be called "get" but ESC is not up to it
    function read(key) {
        var h = hashfn(key);
        assert( h >= 0 );
        var bucket = tbl[h % size];
        while (bucket != null) {
            if (bucket.hashval == h && eqfn(bucket.key, key))
                return bucket.value;
            bucket = bucket.link;
        }
        return notfound;
    }

    // Should be called "put" but called "write" to match the one above
    function write(key, value) {
        if (must_rehash)
            rehash();

        var h = hashfn(key);
        assert( h >= 0 );
        var b = h % size;
        var bucket = tbl[b];
        while (bucket != null) {
            if (bucket.hashval == h && eqfn(bucket.key, key)) {
                bucket.value = value;
                return;
            }
            bucket = bucket.link;
        }
        var node = new Hashnode(key, value, h);
        bucket = tbl[b];
        node.link = bucket;
        tbl[b] = node;

        ++population;
        if (population >= size)
            must_rehash = true;
    }

    function rehash() {
        if (debug)
            dump("BEFORE");
        var newsize = size*2;
        var newtbl = makeTable(newsize);
        for ( var i=0 ; i < size ; i++ ) {
            var bucket = tbl[i];
            while (bucket != null) {
                var node = bucket;
                bucket = bucket.link;
                var b = node.hashval % newsize;
                node.link = newtbl[b];
                newtbl[b] = node;
            }
        }
        size = newsize;
        tbl = newtbl;
        if (debug)
            dump("AFTER");
        must_rehash = false;
    }

    function toString()
        dumpToString("/*Hashtable*/", false);

    function dump(tag, withval=true) 
        print(dumpToString(tag, withval));

    function dumpToString(tag, withval=true) {
        s = tag + " [";
        for ( let i=0 ; i < size ; i++ ) {
            s += "[";
            let b = tbl[i];
            while (b != null) {
                s += "{key:" + b.key + ", value:" + b.value + (withval ? ", hashval:" + b.hashval : "") + "} ";
                b = b.link;
            }
            s += "] ";
        }
        s += "]";
        return s;
    }

    function makeTable(size) {
        var tbl = new Array(size);
        for ( var i=0 ; i < size ; i++ )
            tbl[i] = null;
        return tbl;
    }
}

class ENUM!
{
    const s;
    function ENUM(s) : s=s {}
    
    function toString() {
        return s;
    }
}

// FIXME!  Does not belong in this file, probably.
    
internal function printName() 
    "(" + this.ns + " :: " + this.val + ")";

class Names
{
    // Hashtable
    // Name -> [{namespace, val}]]
    // should it be to another hashtable?
    // What about get/set to different slots?
    const table;

    function Names() {
        table = new Hashtable((function (sym) sym.hash), (function (a,b) a === b), null);
    }
        
    function toString()
        table.toString();

    function getBinding(name: Ast::IDENT, ns: Ast::Namespace) {
        let o = table.read(name);
        let ret = null;
            
        if(o) {
            for( let i=0, limit=o.length; i<limit; ++i) {
                let entry = o[i];
                if(Ast::nsEquals(entry.namespace, ns))
                    ret = entry.val;
            } 
        }
        
        return ret;            
    }
        
    function putBinding(name, ns, value) {
        let o = table.read(name);
        if( o == null ) {
            o = [];
            table.write(name, o);
        }
        let index = 0;
        for( let i = 0, limit=o.length; i < limit; ++i ) {
            if( Ast::nsEquals(o[i].namespace, ns) ) {
                index = i;
                break;
            }
        }
        o[index] = {namespace:ns, val:value, toString: printName};
    }
}
