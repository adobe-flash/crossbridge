// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package com.adobe.flascc {
import flash.utils.ByteArray;

[ExcludeClass]
/**
 * ByteArray subclass that self-populates
 * based on class metadata
 * swfresolve will recognize this metadata and build
 * the correct definebinary tag
 *
 * [HexData("aabbccdd")]
 * public class D_2e_text extends BinaryData
 * {
* }
 *
 * @private
 */
public class BinaryData extends ByteArray {
    public function BinaryData() {
        // check for pre-populated via DefineBinary
        if (length) {
            return;
        }
        // parse hex data
        var md:XML = CModule.describeType(this);
        var bdList:XMLList = md..metadata.(@name == "HexData");

        for each(var bd:XML in bdList) {
            var argList:XMLList = bd..arg.(@key == "");

            for each(var arg:XML in argList) {
                var hex:String = arg.@value;
                var len:uint = hex.length;

                for (var i:uint = 0; i < len; i += 2) {
                    var c1:int = hex.charCodeAt(i);
                    var c2:int = hex.charCodeAt(i + 1);
                    var result:int = 0;

                    if (c1 < 58)
                        result = c1 - 48;
                    else if (c1 < 71)
                        result = 10 + (c1 - 65);
                    else if (c1 < 103)
                        result = 10 + (c1 - 97);

                    result *= 16;

                    if (c2 < 58)
                        result += c2 - 48;
                    else if (c2 < 71)
                        result += 10 + (c2 - 65);
                    else if (c2 < 103)
                        result += 10 + (c2 - 97);

                    writeByte(result);
                }
            }
        }
        position = 0;
        // free memory
        try
        {
            import flash.system.System;
            System.disposeXML(md);
        } catch(error:*) {
            // swallow #1065 flash.system::System is not defined, when using projector
        }
        md = null;
    }
}
}
