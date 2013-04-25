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
    import sample.IntArrayLib.ram;
    
    import IntArrayLib;
    
    import sample.IntArrayLib.CModule;
    
    import flash.display.Sprite;
    import flash.text.TextField;
    
    public class Main extends Sprite
    {
        public function callCFunctions():void
        {
            var v:Vector.<int> = new Vector.<int>();
            v.push(1);
            v.push(2);
            v.push(3);
            v.push(4);
            v.push(5);
            
            // pass an ActionScript object in
            printLine("Sum: " + IntArrayLib.sumArray(v) + " == 15");
            printLine("Product: " + IntArrayLib.dotProductArray(v) + " == 120");
            
            // pass an ActionScript object in and get an ActionScript object returned
            var reversed:Vector.<int> = IntArrayLib.reverseArray(v); 
            printLine("Reversed: " + reversed + " == 5,4,3,2,1");
            
            var incremented:Vector.<int> = IntArrayLib.incrementArray(v) 
            printLine("Incremented: " + incremented + " == 2,3,4,5,6");
        }
        
        /** Constructor */
        public function Main()
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
