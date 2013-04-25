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

package
{
    import sample.MyLib.ram;
    
    import MyLib;
    
    import sample.MyLib.CModule;
    
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.text.TextField;
    import flash.utils.ByteArray;
    
    public class PassData extends Sprite
    {
        public function callCFunctions():void
        {
            // 
            // This function walks through examples of how to pass different data types from
            // ActionScript to a C library that was built with flascc as a SWC.
            //
            
            // Note: If you are using Flash Builder to edit this code be careful about it changing
            // your import statements when something gets codehinted.  It will try to change import MyLib;
            // to import MyLib.*; which will result in a bunch of compiler errors.  This is currently 
            // filed as http://bugs.adobe.com/jira/browse/FB-33303
            
            //
            // Example 1: grab value of C constants
            //
            printLine("MyLib.A=" + MyLib.A);
            printLine("MyLib.B=" + MyLib.B);
            
            //
            // Example 2: pass an integer to C
            //
            var x:int = 1;
            printLine("MyLib.plus(x,x)=" + MyLib.plus(x, x));
            
            //
            // Example 3: pass a pointer to an integer to C
            //
            var xPtr:int = CModule.malloc(4); // create a pointer to 4 bytes of memory
            CModule.write32(xPtr, 5); // set the value of that pointer to 5
            printLine("MyLib.plusPtr(xPtr, xPtr)=" + MyLib.plusPtr(xPtr, xPtr));
            CModule.free(xPtr); // always remember to free() memory that you created with malloc()
            
            //
            // Example 4: pass a pointer to a struct to C
            //
            
            // Our C library has a struct named HockeyPlayer.  When you run the library through SWIG it will 
            // generate an ActionScript class for this struct that is called HockeyPlayerValue.  You can see
            // the source of this generated file by looking at the MyLib.as3 file that SWIG generates.
            // 
            // Any of these Value classes that SWIG generates will have a static size property that signifies
            // how much memory that class takes up.  Knowing this we can call the CModule.malloc() function
            // to return an int that is a pointer to a spot in the flascc ByteArray that represents main
            // memory and is accessible from both ActionScript and C.
            var myPlayerPtr:int = CModule.malloc(HockeyPlayerValue.size);
            // Now we can instantiate a HockeyPlayerValue class in ActionScript.  These Value classes take two
            // parameters for the constructor: a ByteArray and an offset into that ByteArray.
            // 
            // In this case we want to make it easy to create this value in ActionScript and make sure that if
            // we call any functions in C where it modifies this object that those modifications are reflected
            // in the ActionScript object.  To do this we pass the main memory as the first parameter (sample.MyLib.ram)
            // and the pointer to the memory we just malloc'd there (myPlayerPtr)
            var myPlayer:HockeyPlayerValue = new HockeyPlayerValue(sample.MyLib.ram, myPlayerPtr);
            
            // now we can modify this object easily in ActionScript
            myPlayer.goals = 1;
            myPlayer.jersey = 1;
            printLine("HockeyPlayer.goals " + myPlayer.goals);
            
            // and if we call a C function that modifies that object...
            MyLib.changeStats(myPlayerPtr);

            // those changes are reflected in the ActionScript object
            printLine("HockeyPlayer.goals " + myPlayer.goals);
            CModule.free(myPlayerPtr);
            
            // 
            // Example 5: pass a String to C (where C expects a char*)
            //
            
            // Notice that when the C function expects a char* parameter that you can just 
            // send an ActionScript string directly into it.  This is because SWIG maps char*
            // to the String class in ActionScript
            printLine("Length is " + MyLib.stringLength("ABCDE"));
            
            //
            // Example 6: pass a String to C (where C expects a char[])
            //

            // If C is expecting a char[] then this won't be treated as an ActionScript 
            // string, but rather as a pointer.  The CModule defines a mallocString() 
            // method that makes it really easy to create a pointer to a string.
            var string:String = "hello";
            var stringPtr:int = CModule.mallocString(string);
            printLine(string + " length is " + MyLib.stringLength2(stringPtr));
            CModule.free(stringPtr);
            // Note that even tho you have a pointer to a String here it acts a little different
            // than a struct.  You can't have C and AS operate on the same memory for a String,
            // instead Strings are passed around by value.  So if you have a function like 
            // void replace(inString, outString) where the value of inString is written to outString
            // you will have troubles.  You can probably work around this by defining some custom SWIG
            // directives, creating a simple struct that wraps a String member, or manually working
            // directly with a ByteArray.
            
            //
            // Example 7: pass an array of integers to C
            //
            var numbers:Array = [1, 2, 3, 4];
            var numbersPtr:int = CModule.malloc(4 * numbers.length);
            
            // write each value at an offset to numbersPtr
            for (var i:int = 0; i < numbers.length; i++)
                CModule.write32(numbersPtr + (i * 4), numbers[i]);
            
            printLine("Sum is " + MyLib.sumArray(numbersPtr));
            CModule.free(numbersPtr);
            // If you want to do something along these lines, but not necessarily with static array sizes
            // then check out Example 10 for an example using a pointer to a String of unknown length at
            // compile time.

            //
            // Example 8: pass a ByteArray to C
            // 
            
            // First lets fill a ByteArray like we normally would in ActionScript 
            var bytes:ByteArray = new ByteArray();
            bytes.endian = "littleEndian";
            for (var j:int = 0; j < 10; j++)
               bytes.writeInt(j);
            
            bytes.position = 0;
            
            // Now we want a pointer to that ByteArray
            var bytesPtr:int = CModule.malloc(bytes.length);
            
            // Use CModule.writeBytes() to write the ByteArray we created into flascc's
            // main memory.  The parameters of writeBytes() are first the pointer in flascc
            // memory, the length of the ByteArray, and the ByteArray itself
            CModule.writeBytes(bytesPtr, bytes.length, bytes);
            printLine("Checksum: " + MyLib.examineBytes(bytesPtr, bytes.length));
            CModule.free(bytesPtr);
            
            // 
            // Example 9: pass a 2 dimensional array to C
            //
            var row1:Array = [0,1,2];
            var row2:Array = [3,4,5];
            var row3:Array = [6,7,8];
            var rows:Array = [row1, row2, row3];
            var rowsLen:int = rows.length * row1.length;
            
            var rowsPtr:int = CModule.malloc(4 * rowsLen);
            
            for (var k:int = 0; k < rows.length; k++){
                var row:Array = rows[k];
                for (var kk:int = 0; kk < row.length; kk++){
                    var rowOffset:int    = k * 4 * row.length;
                    var columnOffset:int = kk * 4;
                    CModule.write32(rowsPtr + rowOffset + columnOffset, row[kk]);
                }
            }
            
            printLine("2D Sum: " + MyLib.sumGrid(rowsPtr));
            CModule.free(rowsPtr);
            
            // 
            // Example 10: Passing a pointer to a String to C which then modifies that string
            //
            var outputPtr:int = CModule.malloc(4);
            var outputLengthPtr:int = CModule.malloc(4);

            // the encode() function in C puts it output into the buffer and bufferLength pointers
            MyLib.encode(outputPtr, outputLengthPtr, "hello");
            
            // Getting the value that C stuffed into outputLengthPtr is pretty simple
            var outputLength:int = CModule.read32(outputLengthPtr);
            
            // Getting the value that C stuffed into outputPtr is a little bit more tricky.
            // Note that in C this is expecting a char** (a pointer to a char*).  In Actionscript
            // this means a pointer to a String.  So we need to dereference the outputPtr to find out
            // where the string is located in memory, and then use that address in readString() 
            var outputString:String = CModule.readString(CModule.read32(outputPtr), outputLength);
            printLine("encoded: " + outputString + " (length=" + outputLength + ")");
            
        }
        
        /** Constructor */
        public function PassData()
        {
            CModule.startAsync(this);
            
            // setup the output text area
            output = new TextField();
            output.multiline = true;
            output.width = stage.stageWidth;
            output.height = stage.stageHeight;
            addChild(output);
            
            // start interacting with the C library
            callCFunctions();
        }
        
        private var output:TextField;
        private function printLine(string:String):void 
        {
            output.appendText(string + "\n");
            trace(string);
        }
    }
}
