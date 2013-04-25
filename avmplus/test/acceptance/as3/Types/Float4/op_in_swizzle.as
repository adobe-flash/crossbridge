/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "4.6.3";
// var VERSION = "AS3";
// var TITLE   = "Component accesses";


var flt4:float4 = new float4(0f, 1f, 2f, 3f);


var component:Array = ['x','y','z','w'];
var input:String = "";
var output:String = "";
for (var levelA in component)
{
    for (var levelB in component)
    {
        for (var levelC in component)
        {
            for (var levelD in component)
            {
                input = component[levelA] + "" + component[levelB] + "" + component[levelC] + "" + component[levelD];
                AddStrictTestCase("'"+input+"'in float4()", true, input in flt4);
            }
        }
    }
}

