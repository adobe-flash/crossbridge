/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Vector initializer on Vector.<C> values, compare array-init-1.";
include "driver.as"

class C {
    public var c:C = null;
}

function vector_init_C(): *
{
    var t: Vector.<C>;
    var c: C = new C;
    for ( var i:int=0 ; i < 10000 ; i++ )
        t = new <C>[c, c, c];
    return t;
}

TEST(function () { vector_init_C(); }, "vector-init-1");
