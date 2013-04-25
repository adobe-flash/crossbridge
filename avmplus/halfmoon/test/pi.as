/*-*-javascript-*-*/

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/*function isbothint(x,y):Boolean {
	return x is int && y is int;
}*/

function pi1(x,y,z) {
    var t1
//    if ( x is int && y is int ) { //short circuit cfg is a pain
//    if ( isbothint(x,y) ) { //no type expressions to examine
    //use bit & to side step short circuit cfg
    if ( x is int & y is int ) {
        var x1:int = x
        var y1:int = y
        t1 = x1 + y1
    } else {
        t1 = x + y
    }
    var t2
    if (t1 is int & z is int) {
        var x2:int = t1;
        var z2: int = z;
        t2 = x2 - z2;
    }
    else
        t2 = t1 - z
    return t2
}  
/*
function pi0(p, q, r) {
	return p+q-r;
}*/

var a = Math.random() * 100
var b = Math.random() * 100
var c = Math.random() * 100

trace( pi1(a,b,c) ) //run it
