/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "7.6";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Punctuators";


    var testcases = getTestCases();
    
    
function getTestCases() {
    var array = new Array();
    var item = 0;
    
    //original, made changes to make it run in fp7

    // ==
    var c:Number=1;
    var d:String=1;
    array[item++] = Assert.expectEq( 
                                    "var c,d;c==d",
                                    true,
                                    (c==d) );
    // ===
    var c;
    var d;
    array[item++] = Assert.expectEq( 
                                    "var c,d;c===d",
                                    false,
                                    (c===d) );

    // =
var a=true;
    array[item++] = Assert.expectEq( 
                                    "var a=true;a",
                                    true,
                                    (a) );

    // >
    var a=true;
    var b=false;
    array[item++] = Assert.expectEq( 
                                    "var a=true,b=false;a>b",
                                    true,
                                    (a>b) );

    // <
    var a=true;
    var b=false;
    array[item++] = Assert.expectEq( 
                                    "var a=true,b=false;a<b",
                                    false,
                                    (a<b) );

    // <=
    var a=0xFFFF;
    var b=0X0FFF;
    array[item++] = Assert.expectEq( 
                                    "var a=0xFFFF,b=0X0FFF;a<=b",
                                    false,
                                    a<=b );

    // >=
    var a=0xFFFF;
    var b=0XFFFE;
    array[item++] = Assert.expectEq( 
                                    "var a=0xFFFF,b=0XFFFE;a>=b",
                                    true,
                                    a>=b);
    //!=
    var a=true;
    var b=false;
    array[item++] = Assert.expectEq( 
                                    "var a=true,b=false;a!=b",
                                    true,
                                    (a!=b) );

var a=false;
var b=false;
    array[item++] = Assert.expectEq( 
                                    "var a=false,b=false;a!=b",
                                    false,
                                    (a!=b) );

       //!==
    var a=true;
    var b=false;
    array[item++] = Assert.expectEq( 
                                    "var a=true,b=false;a!==b",
                                    true,
                                    (a!==b) );

var a=false;
var b=false;
    array[item++] = Assert.expectEq( 
                                    "var a=false,b=false;a!==b",
                                    false,
                                    (a!==b) );

    var s1 = "5";
    var s2 = "5";

    array[item++] = Assert.expectEq( 
                                    "var s1='5',s2='5';s1!==s2",
                                    false,
                                    (s1!==s2) );
    var n:Number = 5;
    
    array[item++] = Assert.expectEq( 
                                    "var s1='5',n=5;s1!==n",
                                    true,
                                    (s1!==n) );

    var b:Boolean = true;
    
    array[item++] = Assert.expectEq( 
                                    "var s1='5',b=true;s1!==b",
                                    true,
                                    (s1!==b) );
    

    
    // ,
var a=true;
var b=false;
        array[item++] = Assert.expectEq( 
                                    "var a=true,b=false;a,b",
                                    false,
                                    (a,b) );
    // !
    var a=true;
    var b=false;
        array[item++] = Assert.expectEq( 
                                    "var a=true,b=false;!a",
                                    false,
                                    (!a) );

    // ~
    var a=true;
        array[item++] = Assert.expectEq( 
                                    "var a=true;~a",
                                    -2,
                                    (~a) );
    // ?
        array[item++] = Assert.expectEq( 
                                    "var a=true; (a ? 'PASS' : '')",
                                    "PASS",
                                    (a=true, (a ? 'PASS' : '') ) );

    // :

        array[item++] = Assert.expectEq( 
                                    "var a=false; (a ? 'FAIL' : 'PASS')",
                                    "PASS",
                                    (a=false, (a ? 'FAIL' : 'PASS') ) );
    // .

var a=Number;
        array[item++] = Assert.expectEq( 
                                        "var a=Number;a.NaN",
                                        NaN,
                                        (a.NaN) );

    // &&
     
        var a=true;
        var b=true;
        var mResult;
        if(a&&b){
         mResult = 'PASS'
        } else { mResult = 'FAIL'; }
        array[item++] = Assert.expectEq( 
                                        "var a=true,b=true;if(a&&b)'PASS';else'FAIL'",
                                        "PASS",
                                        mResult );

    // ||
        var a=true;
        var b=true;
        var mResult;
        if(a||b){
         mResult = 'PASS'
        } else { mResult = 'FAIL'; }
        array[item++] = Assert.expectEq( 
                                        "var a=false,b=false;if(a||b)'FAIL';else'PASS'",
                                        "PASS",
                                        mResult );
    // ++
    var a=false;
    var b=false;
        array[item++] = Assert.expectEq( 
                                        "var a=false,b=false;++a",
                                        1,
                                        (++a) );
    // --
    var a=true;
    var b=false;
        array[item++] = Assert.expectEq( 
                                        "var a=true,b=false--a",
                                        0,
                                        (--a) );
    // +
    var a=true;
        var b=true;
        array[item++] = Assert.expectEq( 
                                        "var a=true,b=true;a+b",
                                        2,
                                        (a+b) );
    // -
    var a=true;
    var b=true;
        array[item++] = Assert.expectEq( 
                                        "var a=true,b=true;a-b",
                                        0,
                                        (a-b) );
    // *
    var a=true;
    var b=true;
        array[item++] = Assert.expectEq( 
                                        "var a=true,b=true;a*b",
                                        1,
                                        (a*b) );
    // /
    var a=true;
    var b=true;
        array[item++] = Assert.expectEq( 
                                        "var a=true,b=true;a/b",
                                        1,
                                        (a/b) );
    // &
var a1=3;
var b1=2;
    array[item++] = Assert.expectEq( 
                                        "var a1=3,b1=2;a1&b1",
                                        2,
                                        ((a1)&(b1)) );
    // |
    var a2=4;
    var b2=3;
        array[item++] = Assert.expectEq( 
                                        "var a2=4,b2=3;a2|b2",
                                        7,
                                        ((a2)|(b2)) );

    // |
    var a3=4;
    var b3=3;
        array[item++] = Assert.expectEq( 
                                        "var a3=4,b3=3;a^b",
                                        7,
                                        ((a3)^(b3)) );

    // %
    var a4=4;
    var b4=3;
        array[item++] = Assert.expectEq( 
                                        "var a4=4,b4=3;a4%b4",
                                        1,
                                        ((a4)%(b4)) );

    // <<
    var a5=4;
    var b5=3;
        array[item++] = Assert.expectEq( 
                                        "var a5=4,b5=3;a5<<b5",
                                        32,
                                        ((a5)<<(b5)) );

    //  >>
    var a6=4;
    var b6=1;
        array[item++] = Assert.expectEq( 
                                        "var a6=4,b6=1;a6>>b6",
                                        2,
                                        ((a6)>>(b6)) );

    //  >>>
    var a7=1;
    var b7=1;
        array[item++] = Assert.expectEq( 
                                        "var a7=1,b7=1;a7>>>b7",
                                        0,
                                        ((a7)>>>(b7)) );
    //  +=
    var a8=4;
    var b8=3;
    a8+=b8;
        array[item++] = Assert.expectEq( 
                                        "var a8=4,b8=3;a8+=b8;a8",
                                        7,
                                        (a8) );

    //  -=
    var a9=4;
    var b9=3;
    a9-=b9;
        array[item++] = Assert.expectEq( 
                                        "var a9=4,b9=3;a9-=b9;a9",
                                        1,
                                        (a9) );
    //  *=
    var a10=4;
    var b10=3;
    a10*=b10;
        array[item++] = Assert.expectEq( 
                                        "var a10=4,b10=3;a10*=b10;a10",
                                        12,
                                        (a10) );
    //  +=
    var a11=4;
    var b11=3;
    a11+=b11;
        array[item++] = Assert.expectEq( 
                                        "var a11=4,b11=3;a11+=b11;a11",
                                        7,
                                        (a11) );
    //  /=
    var a12=12;
    var b12=3;
    a12/=b12;
        array[item++] = Assert.expectEq( 
                                        "var a12=12,b12=3;a12/=b12;a12",
                                        4,
                                        (a12) );

    //  &=
    var a12=4;
    var b12=5;a12&=b12;
        array[item++] = Assert.expectEq( 
                                        "var a12=4,b12=5;a12&=b12;a12",
                                        4,
                                        (a12) );

    // |=
    var a13=4;
    var b13=5;
    a13|=b13;
        array[item++] = Assert.expectEq( 
                                        "var a13=4,b13=5;a13&=b13;a13",
                                        5,
                                        (a13) );
    //  ^=
    var a14=4;
    var b14=5;
    a14^=b14;
        array[item++] = Assert.expectEq( 
                                        "var a14=4,b14=5;a14^=b14;a14",
                                        1,
                                        (a14) );
    // %=
    var a15=12;
    var b15=5;
    a15%=b15;
        array[item++] = Assert.expectEq( 
                                        "var a15=12,b15=5;a15%=b15;a15",
                                        2,
                                        (a15) );
    // <<=
    var a16=4;
    var b16=3;
    a16<<=b16;
        array[item++] = Assert.expectEq( 
                                        "var a16=4,b16=3;a16<<=b16;a16",
                                        32,
                                        (a16) );

    //  >>
    var a17=4;
    var b17=1;
    a17>>=b17;
        array[item++] = Assert.expectEq( 
                                        "var a17=4,b17=1;a17>>=b17;a17",
                                        2,
                                        (a17) );

    //  >>>
    var a18=1;
    var b18=1;
    a18>>>=b18;
        array[item++] = Assert.expectEq( 
                                        "var a18=1,b18=1;a18>>>=b18;a18",
                                        0,
                                        (a18) );

    // ()
    var a=4;
    var b=3;
        array[item++] = Assert.expectEq( 
                                        "var a=4,b=3;(a)",
                                        4,
                                        (a) );
    // {}
    var a19=4;
    var b19=3;
    
        array[item++] = Assert.expectEq( 
                                        "var a19=4,b19=3;{b19}",
                                        3,
                                        b19 );

    // []
    var a=new Array('hi');
        array[item++] = Assert.expectEq( 
                                        "var a=new Array('hi');a[0]",
                                        "hi",
                                        (a[0]) );

        array[item++] = Assert.expectEq( 
                                        ";",
                                        void 0,
                                        void 0 );
    return array;
}
