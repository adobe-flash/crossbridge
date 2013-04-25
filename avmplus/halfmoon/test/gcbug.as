// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {
function Not(n) {
    n = ToInt32(n);
    n = ToInt32BitString(n);
    var r = ""
    for( var l = 0; l < n.length; l++  )
        r += ( n.charAt(l) == "0" ) ? "1" : "0";
    n = ToInt32Decimal(r);
    return n;
}

function ToInt32(n) {
    n = Number( n );
    var sign = ( n < 0 ) ? -1 : 1;
    if ( Math.abs( n ) == 0 || Math.abs( n ) == Number.POSITIVE_INFINITY)
        return 0;
    n = (sign * Math.floor( Math.abs(n) )) % Math.pow(2,32);
    n = ( n >= Math.pow(2,31) ) ? n - Math.pow(2,32) : n;
    return ( n );
}

function ToInt32BitString(n) {
    var b = "";
    var sign = ( n < 0 ) ? -1 : 1;
    b += ( sign == 1 ) ? "0" : "1";
    for ( p = 30; p >=0; p-- ) {
        if ( (sign == 1 ) ? sign * n >= Math.pow(2,p) : sign * n > Math.pow(2,p) ) {
            b += ( sign == 1 ) ? "1" : "0";
            n -= sign * Math.pow( 2, p );
        } else {
            b += ( sign == 1 ) ? "0" : "1";
        }
    }
    print('bitstring',b,b.length)
    return b;
}

function ToInt32Decimal(bin) {
	print('todecimal',bin,bin.length)
	print()
    var r = 0;
    var sign;
    if ( Number(bin.charAt(0)) == 0 ) {
        sign = 1;
        r = 0;
    } else {
        sign = -1;
        r = -(Math.pow(2,31));
    }
    for ( var j = 0; j < 31; j++ )
        r += Math.pow( 2, j ) * Number(bin.charAt(31-j));
    return r;
}
getTestCases();
 
function dump(title, v1, v2) {
    if (v1 !== v2)
	   print(title,v1,v2)
}

function getTestCases() {
    var array = new Array();
    var item = 0;
    for ( var i = 0; i < 35; i++ ) {
        var p = Math.pow(2,i);
		dump("~"+p, Not(p), ~p);
	}
}
}
