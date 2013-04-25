/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.7.3";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Numeric Literals";


    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(  "0",     0,      0 );
    array[item++] = Assert.expectEq(  "1",     1,      1 );
    array[item++] = Assert.expectEq(  "2",     2,      2 );
    array[item++] = Assert.expectEq(  "3",     3,      3 );
    array[item++] = Assert.expectEq(  "4",     4,      4 );
    array[item++] = Assert.expectEq(  "5",     5,      5 );
    array[item++] = Assert.expectEq(  "6",     6,      6 );
    array[item++] = Assert.expectEq(  "7",     7,      7 );
    array[item++] = Assert.expectEq(  "8",     8,      8 );
    array[item++] = Assert.expectEq(  "9",     9,      9 );

    array[item++] = Assert.expectEq(  "0.",     0,      0. );
    array[item++] = Assert.expectEq(  "1.",     1,      1. );
    array[item++] = Assert.expectEq(  "2.",     2,      2. );
    array[item++] = Assert.expectEq(  "3.",     3,      3. );
    array[item++] = Assert.expectEq(  "4.",     4,      4. );

    array[item++] = Assert.expectEq(  "0.e0",  0,      0.e0 );
    array[item++] = Assert.expectEq(  "1.e1",  10,     1.e1 );
    array[item++] = Assert.expectEq(  "2.e2",  200,    2.e2 );
    array[item++] = Assert.expectEq(  "3.e3",  3000,   3.e3 );
    array[item++] = Assert.expectEq(  "4.e4",  40000,  4.e4 );

    array[item++] = Assert.expectEq(  "0.1e0",  .1,    0.1e0 );
    array[item++] = Assert.expectEq(  "1.1e1",  11,    1.1e1 );
    array[item++] = Assert.expectEq(  "2.2e2",  220,   2.2e2 );
    array[item++] = Assert.expectEq(  "3.3e3",  3300,  3.3e3 );
    array[item++] = Assert.expectEq(  "4.4e4",  44000, 4.4e4 );

    array[item++] = Assert.expectEq(  ".1e0",  .1,   .1e0 );
    array[item++] = Assert.expectEq(  ".1e1",  1,    .1e1 );
    array[item++] = Assert.expectEq(  ".2e2",  20,   .2e2 );
    array[item++] = Assert.expectEq(  ".3e3",  300,  .3e3 );
    array[item++] = Assert.expectEq(  ".4e4",  4000, .4e4 );

    array[item++] = Assert.expectEq(  "0e0",  0,     0e0 );
    array[item++] = Assert.expectEq(  "1e1",  10,    1e1 );
    array[item++] = Assert.expectEq(  "2e2",  200,   2e2 );
    array[item++] = Assert.expectEq(  "3e3",  3000,  3e3 );
    array[item++] = Assert.expectEq(  "4e4",  40000, 4e4 );

    array[item++] = Assert.expectEq(  "0e0",  0,     0e0 );
    array[item++] = Assert.expectEq(  "1e1",  10,    1e1 );
    array[item++] = Assert.expectEq(  "2e2",  200,   2e2 );
    array[item++] = Assert.expectEq(  "3e3",  3000,  3e3 );
    array[item++] = Assert.expectEq(  "4e4",  40000, 4e4 );

    array[item++] = Assert.expectEq(  "0E0",  0,     0E0 );
    array[item++] = Assert.expectEq(  "1E1",  10,    1E1 );
    array[item++] = Assert.expectEq(  "2E2",  200,   2E2 );
    array[item++] = Assert.expectEq(  "3E3",  3000,  3E3 );
    array[item++] = Assert.expectEq(  "4E4",  40000, 4E4 );

    array[item++] = Assert.expectEq(  "1.e-1",  0.1,     1.e-1 );
    array[item++] = Assert.expectEq(  "2.e-2",  0.02,    2.e-2 );
    array[item++] = Assert.expectEq(  "3.e-3",  0.003,   3.e-3 );
    array[item++] = Assert.expectEq(  "4.e-4",  0.0004,  4.e-4 );

    array[item++] = Assert.expectEq(  "0.1e-0",  .1,     0.1e-0 );
    array[item++] = Assert.expectEq(  "1.1e-1",  0.11,   1.1e-1 );
    array[item++] = Assert.expectEq(  "2.2e-2",  .022,   2.2e-2 );
    array[item++] = Assert.expectEq(  "3.3e-3",  .0033,  3.3e-3 );
    array[item++] = Assert.expectEq(  "4.4e-4",  .00044, 4.4e-4 );

    array[item++] = Assert.expectEq(  ".1e-0",  .1,    .1e-0 );
    array[item++] = Assert.expectEq(  ".1e-1",  .01,    .1e-1 );
    array[item++] = Assert.expectEq(  ".2e-2",  .002,   .2e-2 );
    array[item++] = Assert.expectEq(  ".3e-3",  .0003,  .3e-3 );
    array[item++] = Assert.expectEq(  ".4e-4",  .00004, .4e-4 );

    array[item++] = Assert.expectEq(  "1.e+1",  10,     1.e+1 );
    array[item++] = Assert.expectEq(  "2.e+2",  200,    2.e+2 );
    array[item++] = Assert.expectEq(  "3.e+3",  3000,   3.e+3 );
    array[item++] = Assert.expectEq(  "4.e+4",  40000,  4.e+4 );

    array[item++] = Assert.expectEq(  "0.1e+0",  .1,    0.1e+0 );
    array[item++] = Assert.expectEq(  "1.1e+1",  11,    1.1e+1 );
    array[item++] = Assert.expectEq(  "2.2e+2",  220,   2.2e+2 );
    array[item++] = Assert.expectEq(  "3.3e+3",  3300,  3.3e+3 );
    array[item++] = Assert.expectEq(  "4.4e+4",  44000, 4.4e+4 );

    array[item++] = Assert.expectEq(  ".1e+0",  .1,   .1e+0 );
    array[item++] = Assert.expectEq(  ".1e+1",  1,    .1e+1 );
    array[item++] = Assert.expectEq(  ".2e+2",  20,   .2e+2 );
    array[item++] = Assert.expectEq(  ".3e+3",  300,  .3e+3 );
    array[item++] = Assert.expectEq(  ".4e+4",  4000, .4e+4 );

    array[item++] = Assert.expectEq(  "0x0",  0,   0x0 );
    array[item++] = Assert.expectEq(  "0x1",  1,   0x1 );
    array[item++] = Assert.expectEq(  "0x2",  2,   0x2 );
    array[item++] = Assert.expectEq(  "0x3",  3,   0x3 );
    array[item++] = Assert.expectEq(  "0x4",  4,   0x4 );
    array[item++] = Assert.expectEq(  "0x5",  5,   0x5 );
    array[item++] = Assert.expectEq(  "0x6",  6,   0x6 );
    array[item++] = Assert.expectEq(  "0x7",  7,   0x7 );
    array[item++] = Assert.expectEq(  "0x8",  8,   0x8 );
    array[item++] = Assert.expectEq(  "0x9",  9,   0x9 );
    array[item++] = Assert.expectEq(  "0xa",  10,  0xa );
    array[item++] = Assert.expectEq(  "0xb",  11,  0xb );
    array[item++] = Assert.expectEq(  "0xc",  12,  0xc );
    array[item++] = Assert.expectEq(  "0xd",  13,  0xd );
    array[item++] = Assert.expectEq(  "0xe",  14,  0xe );
    array[item++] = Assert.expectEq(  "0xf",  15,  0xf );

    array[item++] = Assert.expectEq(  "0X0",  0,   0X0 );
    array[item++] = Assert.expectEq(  "0X1",  1,   0X1 );
    array[item++] = Assert.expectEq(  "0X2",  2,   0X2 );
    array[item++] = Assert.expectEq(  "0X3",  3,   0X3 );
    array[item++] = Assert.expectEq(  "0X4",  4,   0X4 );
    array[item++] = Assert.expectEq(  "0X5",  5,   0X5 );
    array[item++] = Assert.expectEq(  "0X6",  6,   0X6 );
    array[item++] = Assert.expectEq(  "0X7",  7,   0X7 );
    array[item++] = Assert.expectEq(  "0X8",  8,   0X8 );
    array[item++] = Assert.expectEq(  "0X9",  9,   0X9 );
    array[item++] = Assert.expectEq(  "0Xa",  10,  0Xa );
    array[item++] = Assert.expectEq(  "0Xb",  11,  0Xb );
    array[item++] = Assert.expectEq(  "0Xc",  12,  0Xc );
    array[item++] = Assert.expectEq(  "0Xd",  13,  0Xd );
    array[item++] = Assert.expectEq(  "0Xe",  14,  0Xe );
    array[item++] = Assert.expectEq(  "0Xf",  15,  0Xf );

    array[item++] = Assert.expectEq(  "0x0",  0,   0x0 );
    array[item++] = Assert.expectEq(  "0x1",  1,   0x1 );
    array[item++] = Assert.expectEq(  "0x2",  2,   0x2 );
    array[item++] = Assert.expectEq(  "0x3",  3,   0x3 );
    array[item++] = Assert.expectEq(  "0x4",  4,   0x4 );
    array[item++] = Assert.expectEq(  "0x5",  5,   0x5 );
    array[item++] = Assert.expectEq(  "0x6",  6,   0x6 );
    array[item++] = Assert.expectEq(  "0x7",  7,   0x7 );
    array[item++] = Assert.expectEq(  "0x8",  8,   0x8 );
    array[item++] = Assert.expectEq(  "0x9",  9,   0x9 );
    array[item++] = Assert.expectEq(  "0xA",  10,  0xA );
    array[item++] = Assert.expectEq(  "0xB",  11,  0xB );
    array[item++] = Assert.expectEq(  "0xC",  12,  0xC );
    array[item++] = Assert.expectEq(  "0xD",  13,  0xD );
    array[item++] = Assert.expectEq(  "0xE",  14,  0xE );
    array[item++] = Assert.expectEq(  "0xF",  15,  0xF );

    array[item++] = Assert.expectEq(  "0X0",  0,   0X0 );
    array[item++] = Assert.expectEq(  "0X1",  1,   0X1 );
    array[item++] = Assert.expectEq(  "0X2",  2,   0X2 );
    array[item++] = Assert.expectEq(  "0X3",  3,   0X3 );
    array[item++] = Assert.expectEq(  "0X4",  4,   0X4 );
    array[item++] = Assert.expectEq(  "0X5",  5,   0X5 );
    array[item++] = Assert.expectEq(  "0X6",  6,   0X6 );
    array[item++] = Assert.expectEq(  "0X7",  7,   0X7 );
    array[item++] = Assert.expectEq(  "0X8",  8,   0X8 );
    array[item++] = Assert.expectEq(  "0X9",  9,   0X9 );
    array[item++] = Assert.expectEq(  "0XA",  10,  0XA );
    array[item++] = Assert.expectEq(  "0XB",  11,  0XB );
    array[item++] = Assert.expectEq(  "0XC",  12,  0XC );
    array[item++] = Assert.expectEq(  "0XD",  13,  0XD );
    array[item++] = Assert.expectEq(  "0XE",  14,  0XE );
    array[item++] = Assert.expectEq(  "0XF",  15,  0XF );


    // we will not support octal for flash 8
    array[item++] = Assert.expectEq(  "00",  0,   00 );
    array[item++] = Assert.expectEq(  "01",  1,   01 );
    array[item++] = Assert.expectEq(  "02",  2,   02 );
    array[item++] = Assert.expectEq(  "03",  3,   03 );
    array[item++] = Assert.expectEq(  "04",  4,   04 );
    array[item++] = Assert.expectEq(  "05",  5,   05 );
    array[item++] = Assert.expectEq(  "06",  6,   06 );
    array[item++] = Assert.expectEq(  "07",  7,   07 );

    array[item++] = Assert.expectEq(  "000",  0,   000 );
    array[item++] = Assert.expectEq(  "011",  11,   011 );
    array[item++] = Assert.expectEq(  "022",  22,  022 );
    array[item++] = Assert.expectEq(  "033",  33,  033 );
    array[item++] = Assert.expectEq(  "044",  44,  044 );
    array[item++] = Assert.expectEq(  "055",  55,  055 );
    array[item++] = Assert.expectEq(  "066",  66,  066 );
    array[item++] = Assert.expectEq(  "077",  77,   077 );

    array[item++] = Assert.expectEq(  "0.00000000001",  0.00000000001,  0.00000000001 );
    array[item++] = Assert.expectEq(  "0.00000000001e-2",  0.0000000000001,  0.00000000001e-2 );


    array[item++] = Assert.expectEq( 
                                  "123456789012345671.9999",
                                  "123456789012345700",
                                  123456789012345671.9999 +"");
    array[item++] = Assert.expectEq( 
                               "123456789012345672",
                               "123456789012345700",
                               123456789012345672 +"");

    array[item++] = Assert.expectEq(   
                                    "123456789012345672.000000000000000000000000000",
                                   "123456789012345700",
                                    123456789012345672.000000000000000000000000000 +"");

    array[item++] = Assert.expectEq( 
           "123456789012345672.01",
           "123456789012345700",
           123456789012345672.01 +"");

    array[item++] = Assert.expectEq( 
           "123456789012345672.000000000000000000000000001+'' == 123456789012345700 || 123456789012345660 ",
           true,
           ( 123456789012345672.00000000000000000000000000 +""  == 1234567890 * 100000000 + 12345700 )
           ||
           ( 123456789012345672.00000000000000000000000000 +""  == 1234567890 * 100000000 + 12345660) );

    array[item++] = Assert.expectEq( 
           "123456789012345673",
           "123456789012345700",
           123456789012345673 +"" );

    array[item++] = Assert.expectEq( 
           "-123456789012345671.9999",
           "-123456789012345700",
           -123456789012345671.9999 +"" );

    array[item++] = Assert.expectEq( 
           "-123456789012345672",
           "-123456789012345700",
           -123456789012345672+"");

    array[item++] = Assert.expectEq( 
           "-123456789012345672.000000000000000000000000000",
           "-123456789012345700",
           -123456789012345672.000000000000000000000000000 +"");

    array[item++] = Assert.expectEq( 
           "-123456789012345672.01",
           "-123456789012345700",
           -123456789012345672.01 +"" );

    array[item++] = Assert.expectEq( 
           "-123456789012345672.000000000000000000000000001 == -123456789012345680 or -123456789012345660",
           true,
           (-123456789012345672.000000000000000000000000001 == -1234567890 * 100000000 -12345680)
           ||
           (-123456789012345672.000000000000000000000000001 == -1234567890 * 100000000 -12345660));

    array[item++] = Assert.expectEq( 
           -123456789012345673,
           "-123456789012345700",
           -123456789012345673 +"");

    array[item++] = Assert.expectEq( 
           "12345678901234567890",
           "12345678901234570000",
           12345678901234567890 +"" );



  /*array[item++] = Assert.expectEq(  "12345678901234567",         "12345678901234567",        12345678901234567+"" );
    array[item++] = Assert.expectEq(  "123456789012345678",        "123456789012345678",       123456789012345678+"" );
    array[item++] = Assert.expectEq(  "1234567890123456789",       "1234567890123456789",      1234567890123456789+"" );
    array[item++] = Assert.expectEq(  "12345678901234567890",      "12345678901234567890",     12345678901234567890+"" );
    array[item++] = Assert.expectEq(  "123456789012345678900",     "123456789012345678900",    123456789012345678900+"" );
    array[item++] = Assert.expectEq(  "1234567890123456789000",    "1234567890123456789000",   1234567890123456789000+"" );*/

    array[item++] =  Assert.expectEq(  "0x1",          1,          0x1 );
    array[item++] =  Assert.expectEq(  "0x10",         16,         0x10 );
    array[item++] =  Assert.expectEq(  "0x100",        256,        0x100 );
    array[item++] =  Assert.expectEq(  "0x1000",       4096,       0x1000 );
    array[item++] =  Assert.expectEq(  "0x10000",      65536,      0x10000 );
    array[item++] =  Assert.expectEq(  "0x100000",     1048576,    0x100000 );
    array[item++] =  Assert.expectEq(  "0x1000000",    16777216,   0x1000000 );
    array[item++] =  Assert.expectEq(  "0x10000000",   268435456,  0x10000000 );

    array[item++] =  Assert.expectEq(  "0x100000000",          4294967296,      0x100000000 );
    array[item++] =  Assert.expectEq(  "0x1000000000",         68719476736,     0x1000000000 );
    array[item++] =  Assert.expectEq(  "0x10000000000",        1099511627776,     0x10000000000 );

    return ( array );
}
