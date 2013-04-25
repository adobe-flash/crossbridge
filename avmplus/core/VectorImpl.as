/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is the implementation of the common vector methods for each of the different vector classes.
// It is included in the body of the class of each of the different vector types.

public native function get length():uint;
public native function set length(value:uint);

public native function set fixed(f:Boolean);
public native function get fixed():Boolean;

AS3 function toString() : String {
    return this.AS3::join();
}

AS3 function toLocaleString() : String {
    var limit:uint = length;
    var separator:String = ",";
    var s:String = "";
    var i:uint = 0;

    if( limit > 0 ) {
        while (true) {
            var x = this[i];
            if (x !== undefined && x !== null)
                s += x.public::toLocaleString();
            if (++i == limit)
                break;
            s += separator;
        }
    }
    return s;
}

AS3 function join(separator: String=","): String {
    var limit:uint = length;
    var s:String = "";
    var i:int = 0;
    if( limit > 0 ) {
        while (true) {
            var y:uint=i;
            s += this[y];
            y = ++i;
            if (y == limit)
                break;
            s += separator;
        }
    }
    return s;
}

private function _concat(items:Array) {
    var v  = newThisType();
    v.private::_spliceHelper(0, length, 0, this, 0);
    const bug504525:Boolean = bugzilla(504525);
    for ( var j:uint=0, limit=items.length ; j < limit ; j++ ) {
        var item = castToThisType(items[j]);
        const insertPoint:uint = bug504525 ? v.length : length;
        v.private::_spliceHelper(insertPoint, item.length, 0, item, 0);
    }

    return v;
}

AS3 function every(checker:Function, thisObj: Object=null): Boolean {
    return _every(this, checker, thisObj is Object ? thisObj : null);
}

AS3 function forEach(eacher:Function, thisObj: Object=null): void {
    _forEach(this, eacher, (thisObj is Object ? thisObj : null));
}

AS3 function map(mapper:Function, thisObj:Object=null) {
    var result = this.private::_map(mapper, thisObj is Object ? thisObj : null);
    return bugzilla(574600) ? result : undefined;
}

AS3 native function push(...items:Array): uint;

private native function _reverse() : void;

AS3 function some(checker, thisObj: Object=null): Boolean {
    return _some(this, checker, thisObj is Object ? thisObj : null);
}

private function _slice(start: Number=0, end: Number=0x7fffffff) {
    var first:uint = clamp( start, length );
    var limit:uint = clamp( end, length );
    if (limit < first)
        limit = first;
    var result = newThisType();
    result.private::_spliceHelper(0, limit-first, 0, this, first);

    return result;
}

private function _splice(start, deleteCount, items : Array) {
    var first:uint  = clamp( start, length );
    var delcnt:uint = clamp( deleteCount, length-first );

    var result = newThisType();
    result.private::_spliceHelper(0, delcnt, 0, this, first);

    _spliceHelper(first, items.length, delcnt, items, 0);

    return result;
}

// First delete deleteCount entries, starting at insertpoint.
// Then insert insertcount entries starting at insertpoint;
// the insertcount entries are read from args object, starting at offset.
private native function _spliceHelper(insertpoint:uint, insertcount:uint, deleteCount:uint, args:Object, offset:uint):void;

AS3 native function unshift(...items) : uint;

// Prototype Methods
prototype.toString = function() {
    return castToThisType(this).AS3::toString();
}

prototype.toLocaleString = function() {
    return castToThisType(this).AS3::toLocaleString();
}

prototype.join = function(separator=void 0) {
    return castToThisType(this).AS3::join(separator == undefined ? "," : String(separator));
}

prototype.concat = function(...items) {
    return castToThisType(this)._concat(items);
}
private static native function _every(o, callback:Function, thisObject):Boolean;
prototype.every = function(checker, thisObj=void 0) : Boolean {
    return _every(castToThisType(this), checker, thisObj is Object ? thisObj : null);
}

private native function _filter(callback:Function, thisObject):*;
prototype.filter = function(checker, thisObj=void 0) {
    return castToThisType(this).private::_filter(checker, thisObj is Object ? thisObj : null);
}

private static native function _forEach(o, callback:Function, thisObject):void;
prototype.forEach = function(eacher, thisObj=void 0) {
    _forEach(castToThisType(this), eacher, (thisObj is Object ? thisObj : null));
}

prototype.indexOf = function(value, from=void 0) {
    return castToThisType(this).AS3::indexOf(value, Number(from));
}

prototype.lastIndexOf = function (value, from=void 0) {
    return castToThisType(this).AS3::lastIndexOf(value, from == undefined ? Infinity : Number(from));
}

private native function _map(callback:Function, thisObject):*;
prototype.map = function(mapper, thisObj=void 0) {
    return castToThisType(this).private::_map(mapper, thisObj is Object ? thisObj : null);
}

prototype.pop = function() {
    return castToThisType(this).AS3::pop();
}

// Bugzilla 573452, Type-specialized Vector methods, moved the prototype.push method to Vector.as
  
prototype.reverse = function() {
    return castToThisType(this).AS3::reverse();
}

prototype.shift = function() {
    return castToThisType(this).AS3::shift();
}

prototype.slice = function(start=void 0, end=void 0){
    return castToThisType(this)._slice(start == undefined ? 0 : Number(start),
                          end == undefined ? 0x7fffffff : Number(end));
}

private static native function _some(o, callback:Function, thisObject):Boolean;
prototype.some = function(checker, thisObj=void 0):Boolean{
    return _some(castToThisType(this), checker, thisObj is Object ? thisObj : null);
}

// Borrow the sort function from array
private static native function _sort(o, args:Array);
prototype.sort = function(comparefn){
    var a : Array = [comparefn];
    return _sort(castToThisType(this), a);
}

prototype.splice = function(start, deleteCount, ...items){
    return castToThisType(this)._splice(Number(start), Number(deleteCount), items);
}

prototype.unshift = function(...items){
    return castToThisType(this).AS3::unshift.AS3::apply(this, items);
}

private function clamp(val: Number, len: uint): uint {
    var result : uint;
    if (val < 0.0)
    {
        if (val + len < 0.0)
            result = 0;
        else
            result = uint(val + len);
    }
    else if (val > len) {
//ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=687817
        result = len;
//else
//      result = length;
//endif
    }
    else if (val != val) {
        result = 0;
    }
    else {
        result = uint(val);
    }
    return result;
}

_dontEnumPrototype(prototype);
