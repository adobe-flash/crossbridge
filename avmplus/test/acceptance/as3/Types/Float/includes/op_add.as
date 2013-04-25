/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Typed: (vars and return type)
toplevel op toplevel
localvar op localvar
arg      op arg
arg      op toplevel
arg      op localvar
localvar op toplevel

Untyped: (vars and return type)
toplevel op toplevel
localvar op localvar
arg      op arg
arg      op toplevel
arg      op localvar
localvar op toplevel
 */

var flt1:float = 3.124f;
var flt2:float = 1.125f;
var any1:* = 3.124f;
var any2:* = 1.125f;

function runner():void
{
    AddStrictTestCase("typed: toplevel + toplevel",   4.249f, addTopLevelTyped());
    AddStrictTestCase("typed: local + local",         4.249f, addLocalsTyped());
    AddStrictTestCase("typed: arg + arg",             4.249f, addArgsTyped(flt1, flt2));
    AddStrictTestCase("typed: arg + toplevel",        4.249f, addArgTopLevelTyped(flt1));
    AddStrictTestCase("typed: arg + local",           4.249f, addArgLocalTyped(flt1));
    AddStrictTestCase("typed: local + toplevel",      4.249f, addLocalTopLevelTyped());

    AddStrictTestCase("untyped: toplevel + toplevel", 4.249f, addTopLevelUntyped());
    AddStrictTestCase("untyped: local + local",       4.249f, addLocalsUntyped());
    AddStrictTestCase("untyped: arg + arg",           4.249f, addArgsUntyped(any1, any2));
    AddStrictTestCase("untyped: arg + toplevel",      4.249f, addArgTopLevelUntyped(any1));
    AddStrictTestCase("untyped: arg + local",         4.249f, addArgLocalUntyped(any1));
    AddStrictTestCase("untyped: local + toplevel",    4.249f, addLocalTopLevelUntyped());
}

function addTopLevelTyped():float { return flt1 + flt2; }

function addLocalsTyped():float
{
    var f1:float = 3.124f;
    var f2:float = 1.125f;
    return f1 + f2;
}
function addArgsTyped(param1:float, param2:float):float
{
    return param1 + param2;
}

function addArgTopLevelTyped(param1:float):float
{
    return param1 + flt2;
}

function addArgLocalTyped(param1:float):float
{
    var f2:float = 1.125f;
    return param1 + f2;
}

function addLocalTopLevelTyped():float
{
    var f1:float = 3.124f;
    return f1 + flt2;
}

function addTopLevelUntyped():* { return any1 + any2; }

function addLocalsUntyped():*
{
    var f1:* = 3.124f;
    var f2:* = 1.125f;
    return f1 + f2;
}

function addArgsUntyped(param1:*, param2:*):*
{
    return param1 + param2;
}

function addArgTopLevelUntyped(param1:*):*
{
    return param1 + any2;
}

function addArgLocalUntyped(param1:*):*
{
    var f2:* = 1.125f;
    return param1 + f2;
}

function addLocalTopLevelUntyped():*
{
    var f1:* = 3.124f;
    return f1 + any2;
}

