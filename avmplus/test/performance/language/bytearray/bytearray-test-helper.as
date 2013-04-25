/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;
import avmplus.System;
import avmplus.File;

// We do not use Math.random because it is seeded differently on each
// run of AVM (based on the current time or machine uptime), and we
// want our test results to be time-invariant.

// Below pseudo-random number generator is from
//   http://en.wikipedia.org/wiki/Random_number_generation
//   citing
//   Marsaglia, George (1999-01-12). "sci.stat.math"
//   but as far as Felix can tell, the citation should be
//   Marsaglia, George (1997-09-29). "sci.stat.math"
//   http://groups.google.com/group/sci.stat.math/msg/87d2a5d269ae7744
//
// Felix adapted it to Actionscript style:
class Marsaglia {
    private var w:uint;
    private var z:uint;
    public function Marsaglia(w_:uint = 521288629, z_:uint = 362436069) {
        this.w = w_;
        this.z = z_;
    }
    public function random():uint {
        z = 36969 * (z & 65535) + (z >> 16);
        w = 18000 * (w & 65535) + (w >> 16);
        return (z << 16) + w;
    }
}

// prepareByteArrayDeterministically(L, M) returns a fresh ByteArray
// of filled with the first L bytes from a pseudo-random byte stream.
// Invocations with the same M will draw from the same stream of
// bytes (starting again from the beginning of the stream; thus
// we stress that this is "Deterministic" even though it is also
// random).
//
// The degree of randomness is controlled by the modulus argument M:
// making M smaller shrinks the domain so that compressible patterns
// are more likely to arise in the generated random data; the minimal
// "useful" M is 2 (making every 4-byte sequence either 0x00000000 or
// 0x00000001, and thus long runs of many of these sequences are very
// likely to occur multiple times in the output).
//
// For example, when LZMA-compressing a L = 1-megabyte bytearray that
// was filled via this function, Felix found the LZMA-compressed
// length for various values for M was as follows:
//
//              M    Compressed Length
// --------------    -----------------
//              2               40,507
//              3               58,677
//              4               76,124
//              5               88,051
//             10              122,997
//            100              241,816
//          1,000              354,365
//         10,000              455,036
//        100,000              581,743
//      1,000,000              670,124
//     10,000,000              782,022
//    100,000,000              972,763
//  1,000,000,000            1,043,710
// uint.MAX_VALUE            1,062,886
//
// (a smaller compressed length implies more patterns/redundancies in
//  the original 1-megabyte bytearray for LZMA to utilize.)
function prepareByteArrayDeterministically(length:uint, modulus:uint=uint.MAX_VALUE) : ByteArray
{
    var b:ByteArray = new ByteArray();
    var rng:Marsaglia = new Marsaglia();
    b.length = length;
    for (var i:uint = 0; i < length; i += 4) {
        var n = rng.random() % modulus;
        if (i+0 < length) b[i+0] = (n >> 24) & 0xFF;
        if (i+1 < length) b[i+1] = (n >> 16) & 0xFF;
        if (i+2 < length) b[i+2] = (n >>  8) & 0xFF;
        if (i+3 < length) b[i+3] = (n >>  0) & 0xFF;
    }
    return b;
}

function prepareByteArrayWithTextDeterministically(length : uint) : ByteArray
{
    var byteArrayWithText : ByteArray = new ByteArray();
    var iterations : uint = length / text.length;
    var remainder : uint = length % text.length;
    print(text.length);

    for (var iter : uint = 0; iter < iterations; iter++)
    {
        byteArrayWithText.writeUTFBytes(text);
    }

    byteArrayWithText.writeUTFBytes(text.substring(0, remainder));
    
    return byteArrayWithText;
}

function prepareByteArrayWithSizeAndData(testDataFilePath : String) : ByteArray
{
    var pathToTestData : String = testDataFilePath;
    
    if (System.argv.length > 0)
    {
        pathToTestData = System.argv[0] + '/' + pathToTestData;
    }
    
    return File.readByteArray(pathToTestData);
}

function TEST_TIME(run:Function, tag:String, iterations:uint = 1) 
{
    var start:uint = getTimer()

    for (var iter:int = 0; iter < iterations; iter++)
    {
        run();
    }
    
    print('name ' + tag);
    print('metric time ' + ((getTimer() - start) / 1000) / iterations);
}

function bytearray_compress(algorithm : String, x: ByteArray)
{
    var input:ByteArray = new ByteArray();

    input.clear();
    input.writeBytes(x);
    input.compress(algorithm);
    return input;
}

/**
 * The function takes two ByteArrays and create a new ByteArray with size targetSize. The content of ByteArray1 and ByteArray2 will used alternating to fill the new ByteArray.
 **/
function prepareCompressedTestData(algorithm : String, content1 : ByteArray, content2 : ByteArray, targetSize : uint) : ByteArray
{
    var result : ByteArray = new ByteArray();
    var content : Array = [content1, content2];
    
    var size : uint = 0;
    var i : uint = 0; // alternate content
    
    while(size <= targetSize)
    {
        var index : uint = i % 2;
        result.writeBytes(content[index]);
        size += content[index].length;

        i++;
    }

    result.compress(algorithm);

    return result;
}

var text : String = (<![CDATA[
I am leaving you and Dixie land tomorrow. It is customary perhaps to
say, "Dear Old Dixie" but, since I happen to be from that little place
off in the northwest, of which I have fondly told you, the _Rosebud
Country_, where I am returning at once, and which is the only place that
is dear to me, I could not conscientiously use the other term. Still, I
am grateful, and well I should be; for, had I not spent these eighteen
months down here, I could never have written _this_ story. No
imagination, positively not mine, could have created "Slim", "T. Toddy",
"Legs", "John Moore", et al. I really knew them. I haven't even changed
their names, since what's the use? They, unless by chance, will never
know, for, as I knew them, they never read. Only one of them I am sure
ever owned a book. That one did, however, and that I know, for he stole
my dictionary before I left the town. Whatever he expected to do with
it, is a puzzle to me, but since it was leather-bound, I think he
imagined it was a Bible. He was very fond of Bibles, and I recall that
was the only thing he read. He is in jail now, so I understand; which is
no surprise, since he visited there quite often in the six months I knew
him. As to "Legs", I have no word; but since summer time has come, I am
sure "Slim" has either gone into "business" or is "preaching." "T.
Toddy" was pretty shaky when I saw him last, and I wouldn't be surprised
if he were not now in Heaven. And still, with what he threatened to do
to me when he was informed that I had written of him in a book, he may
be in the other place, who knows! I recall it with a tremor. We were in
a restaurant some time after the first threat, but at that time, he
appeared to understand that I had written nothing bad concerning him,
and we were quite friendly. He told of himself and his travels, relating
a trip abroad, to Liverpool and London. In the course of his remarks, he
told that he used to run down from Liverpool to London every morning,
since it was just over the hill a mile, and could be seen from Liverpool
whenever the fog lifted. He advised me a bit remonstratingly, that,
since I had written of him in the book, if I had come to him in advance,
he would have told me something of himself to put into it that would
have interested the world. I suggested that it was not then too late,
and that he should make a copy of it. He intimated that it would be
worth something and I agreed with him, and told him I would give him
fifty cents. He said that would be satisfactory, but he wanted it then
in advance. I wouldn't agree to that, but told him that he would have to
give me a brief of his life, where and when he was born, if he had been,
also where and when he expected to die, etc. first. He got "mad" then
and threatened to do something "awful". Took himself outside and opened
a knife, the blade of which had been broken, and was then about a half
inch long, and told me to come out, whereupon he would show me my heart.
As he waited vainly for me, he took on an expression that made him
appear the worst man in all the world. I did not, of course go out, and
told him so--through the window.

That was the end of it--and of him, so far as I know. But you can
understand by this how near I have been to death in your Dixie Land.
When I come back it will not be for "color"; but--well, I guess you know.]]>).toString();
