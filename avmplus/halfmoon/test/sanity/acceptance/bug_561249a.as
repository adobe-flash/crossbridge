// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function AddDoubleToDouble(a:Number, b:Number, c)
{
	print("DOUBLE TO DOUBLE");
}

function II(a:int, b:int, c)
{
    AddDoubleToDouble(a, b, c);
    AddDoubleToDouble(b, a, c);
}

function getTestCases()
{
var int29_maxm1 =   268435454;
var int29_max   =   268435455;
var int29_maxp1 =   268435456;

var int29_minm1 =  -268435457;
var int29_min   =  -268435456;
var int29_minp1 =  -268435455;

    // An integer representable in 29 bits will fit
    // in a kIntptrType atom on a 32-bit platform.
    II(0,    int29_max,    int29_max);
    II(0,    int29_maxp1,  int29_maxp1);
    II(0,    int29_min,    int29_min);
    II(0,    int29_minm1,  int29_minm1);

    II(1,    int29_maxm1,  int29_max);
    II(1,    int29_min,    int29_minp1);
}

getTestCases();
